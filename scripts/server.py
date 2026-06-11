import threading
from flask import Flask, jsonify, send_from_directory
import os

flask_app = Flask(__name__, static_folder=os.path.join(os.path.dirname(__file__), "static"))

@flask_app.after_request
def add_cors(response):
    response.headers["Access-Control-Allow-Origin"] = "*"
    return response

_load_state        = None
_load_adjacent     = None
_load_predecessors = None
_ss                = None

_state_cache: dict     = {}
_neighbors_cache: dict = {}
_static_positions: dict = {}

def _get_state_cached(state_id: int):
    if state_id not in _state_cache:
        _state_cache[state_id] = _load_state(_ss, state_id)
    return _state_cache[state_id]

def _get_neighbors_cached(state_id: int):
    if state_id not in _neighbors_cache:
        adj_ids,  adj_edges  = _load_adjacent(_ss, state_id)
        pred_ids, pred_edges = _load_predecessors(_ss, state_id)

        neighbors = []
        for nid, edge in zip(adj_ids, adj_edges):
            node, _ = _get_state_cached(nid)
            neighbors.append({
                "node":      node["data"],
                "edge":      edge["data"],
                "direction": "successor",
            })
        for nid, edge in zip(pred_ids, pred_edges):
            node, _ = _get_state_cached(nid)
            neighbors.append({
                "node":      node["data"],
                "edge":      edge["data"],
                "direction": "predecessor",
            })
        _neighbors_cache[state_id] = neighbors
    return _neighbors_cache[state_id]


def _compute_bfs_layout(root_id: int = 0, x_spacing: float = 150.0, y_spacing: float = 100.0):
    n = _ss.EtatsLength()
    depth = [-1] * n
    bfs_queue = []

    depth[root_id] = 0
    bfs_queue.append(root_id)
    head = 0

    while head < len(bfs_queue):
        cur = bfs_queue[head]
        head += 1
        for nb in _neighbors_cache.get(cur, []):
            if nb["direction"] != "successor":
                continue
            nxt = int(nb["node"]["id"])
            if depth[nxt] == -1:
                depth[nxt] = depth[cur] + 1
                bfs_queue.append(nxt)

    for i in range(n):
        if depth[i] == -1:
            depth[i] = 9999999

    max_depth = max((d for d in depth if d != 9999999), default=0)
    overflow = max_depth + 1

    layer_count = [0] * (overflow + 1)
    for i in range(n):
        d = depth[i] if depth[i] <= overflow else overflow
        layer_count[d] += 1

    layer_cur = [0] * (overflow + 1)

    for nid in bfs_queue:
        d = depth[nid] if depth[nid] <= overflow else overflow
        slot = layer_cur[d]
        layer_cur[d] += 1
        total = layer_count[d]
        _static_positions[nid] = {
            "x": d * x_spacing,
            "y": (slot - (total - 1) / 2.0) * y_spacing,
        }

    for i in range(n):
        if depth[i] == 9999999:
            slot = layer_cur[overflow]
            layer_cur[overflow] += 1
            total = layer_count[overflow]
            _static_positions[i] = {
                "x": overflow * x_spacing,
                "y": (slot - (total - 1) / 2.0) * y_spacing,
            }


def init_tile_server(ss, load_state_fn, load_adjacent_fn, load_predecessors_fn):
    global _ss, _load_state, _load_adjacent, _load_predecessors
    _state_cache.clear()
    _neighbors_cache.clear()
    _static_positions.clear()
    _ss                = ss
    _load_state        = load_state_fn
    _load_adjacent     = load_adjacent_fn
    _load_predecessors = load_predecessors_fn

    n = ss.EtatsLength()
    print(f"[server] Pré-chargement de {n} états...", flush=True)
    for i in range(n):
        _get_state_cached(i)
    print(f"[server] États chargés, pré-chargement des voisins...", flush=True)
    for i in range(n):
        _get_neighbors_cached(i)
    print(f"[server] Pré-chargement terminé.", flush=True)
    print(f"[server] Calcul du layout BFS...", flush=True)
    _compute_bfs_layout()
    print(f"[server] Layout BFS terminé ({len(_static_positions)} noeuds).", flush=True)


def _require_init():
    if _ss is None or _load_state is None:
        raise RuntimeError("init_tile_server() doit être appelé avant de démarrer Flask.")


@flask_app.route("/info")
def get_info():
    try:
        _require_init()
        return jsonify({"n_states": _ss.EtatsLength()})
    except RuntimeError as e:
        return jsonify({"error": str(e)}), 503
    except Exception as e:
        return jsonify({"error": f"Erreur interne : {e}"}), 500

@flask_app.route("/state/<int:state_id>")
def get_state(state_id):
    try:
        _require_init()
        if state_id < 0 or state_id >= _ss.EtatsLength():
            return jsonify({"error": f"ID {state_id} hors bornes (0–{_ss.EtatsLength() - 1})."}), 404
        node, _ = _get_state_cached(state_id)
        return jsonify(node["data"])
    except RuntimeError as e:
        return jsonify({"error": str(e)}), 503
    except Exception as e:
        return jsonify({"error": f"Erreur lors du chargement de l'état {state_id} : {e}"}), 500

@flask_app.route("/neighbors/<int:state_id>")
def get_neighbors(state_id):
    try:
        _require_init()
        if state_id < 0 or state_id >= _ss.EtatsLength():
            return jsonify({"error": f"ID {state_id} hors bornes (0–{_ss.EtatsLength() - 1})."}), 404
        neighbors = _get_neighbors_cached(state_id)
        return jsonify(neighbors)
    except RuntimeError as e:
        return jsonify({"error": str(e)}), 503
    except Exception as e:
        return jsonify({"error": f"Erreur lors du chargement des voisins de {state_id} : {e}"}), 500

@flask_app.route("/nb_states")
def get_nb_states():
    try:
        _require_init()
        return jsonify({"nb": _ss.EtatsLength()})
    except RuntimeError as e:
        return jsonify({"error": str(e)}), 503
    except Exception as e:
        return jsonify({"error": f"Erreur interne : {e}"}), 500

@flask_app.route("/static_position/<int:state_id>")
def get_static_position(state_id):
    try:
        _require_init()
        if state_id not in _static_positions:
            return jsonify({"error": "Position introuvable."}), 404
        pos = _static_positions[state_id]
        return jsonify({"id": state_id, "x": pos["x"], "y": pos["y"]})
    except RuntimeError as e:
        return jsonify({"error": str(e)}), 503
    except Exception as e:
        return jsonify({"error": f"Erreur : {e}"}), 500

@flask_app.route("/static_positions_range/<int:start>/<int:end>")
def get_static_positions_range(start, end):
    try:
        _require_init()
        n = _ss.EtatsLength()
        end = min(end, n)
        out = [
            {"id": i, "x": _static_positions[i]["x"], "y": _static_positions[i]["y"]}
            for i in range(start, end)
            if i in _static_positions
        ]
        return jsonify(out)
    except RuntimeError as e:
        return jsonify({"error": str(e)}), 503
    except Exception as e:
        return jsonify({"error": f"Erreur : {e}"}), 500

@flask_app.route("/")
def serve_index():
    try:
        return send_from_directory(flask_app.static_folder, "index.html")
    except Exception:
        return jsonify({"error": "index.html introuvable dans le dossier static."}), 404


def start_flask(port=8051):
    thread = threading.Thread(
        target=lambda: flask_app.run(
            host="127.0.0.1", port=port, debug=False, use_reloader=False
        ),
        daemon=True,
    )
    thread.start()
    print(f"[server] Flask démarré sur http://127.0.0.1:{port}", flush=True)