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


def init_tile_server(ss, load_state_fn, load_adjacent_fn, load_predecessors_fn):
    global _ss, _load_state, _load_adjacent, _load_predecessors
    _state_cache.clear()
    _neighbors_cache.clear()
    _ss               = ss
    _load_state       = load_state_fn
    _load_adjacent    = load_adjacent_fn
    _load_predecessors = load_predecessors_fn

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
        if state_id < 0 or state_id >= _ss.EtatsLength():
            return jsonify({"error": f"ID {state_id} hors bornes."}), 404
        if _ss.PositionsLength() == 0:
            return jsonify({"error": "Positions non calculées."}), 404
        pos = _ss.Positions(state_id)
        if pos is None:
            return jsonify({"error": "Position introuvable."}), 404
        return jsonify({"id": state_id, "x": pos.X(), "y": pos.Y()})
    except RuntimeError as e:
        return jsonify({"error": str(e)}), 503
    except Exception as e:
        return jsonify({"error": f"Erreur : {e}"}), 500

@flask_app.route("/static_positions_range/<int:start>/<int:end>")
def get_static_positions_range(start, end):
    try:
        _require_init()
        if _ss.PositionsLength() == 0:
            return jsonify({"error": "Positions non calculées."}), 404
        n   = _ss.EtatsLength()
        end = min(end, n)
        out = []
        for i in range(start, end):
            pos = _ss.Positions(i)
            if pos:
                out.append({"id": i, "x": pos.X(), "y": pos.Y()})
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