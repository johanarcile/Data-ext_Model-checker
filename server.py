import threading
from flask import Flask, jsonify, send_from_directory
import os

flask_app = Flask(__name__, static_folder=os.path.join(os.path.dirname(__file__), "static"))

@flask_app.after_request
def add_cors(response):
    response.headers["Access-Control-Allow-Origin"] = "*"
    return response

_load_state       = None
_load_adjacent    = None
_load_predecessors = None
_ss               = None

def init_tile_server(ss, load_state_fn, load_adjacent_fn, load_predecessors_fn):
    global _ss, _load_state, _load_adjacent, _load_predecessors
    _ss               = ss
    _load_state       = load_state_fn
    _load_adjacent    = load_adjacent_fn
    _load_predecessors = load_predecessors_fn

@flask_app.route("/state/<int:state_id>")
def get_state(state_id):
    node, _ = _load_state(_ss, state_id)
    return jsonify(node["data"])

@flask_app.route("/neighbors/<int:state_id>")
def get_neighbors(state_id):
    adj_ids,  adj_edges  = _load_adjacent(_ss, state_id)
    pred_ids, pred_edges = _load_predecessors(_ss, state_id)

    neighbors = []
    for nid, edge in zip(adj_ids, adj_edges):
        node, _ = _load_state(_ss, nid)
        neighbors.append({
            "node":      node["data"],
            "edge":      edge["data"],
            "direction": "successor",
        })
    for nid, edge in zip(pred_ids, pred_edges):
        node, _ = _load_state(_ss, nid)
        neighbors.append({
            "node":      node["data"],
            "edge":      edge["data"],
            "direction": "predecessor",
        })
    return jsonify(neighbors)

@flask_app.route("/")
def serve_index():
    return send_from_directory(flask_app.static_folder, "index.html")

def start_flask(port=8051):
    thread = threading.Thread(
        target=lambda: flask_app.run(
            host="127.0.0.1", port=port, debug=False, use_reloader=False
        ),
        daemon=True,
    )
    thread.start()
    print(f"[server] Flask démarré sur http://127.0.0.1:{port}", flush=True)