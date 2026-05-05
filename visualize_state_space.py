"""Simple Dash + Cytoscape visualisation for the FlatBuffers `state_space` binary.

Usage: python scripts/visualize_state_space.py [path/to/state_space]
"""

import os
import dash
import dash_cytoscape as cyto
from dash import dcc, html, no_update
from dash.dependencies import Input, Output, State
from State_space_ta.State_space_ta import State_space_ta

LOCATIONS = ["l0l3", "l0l4", "l1l3", "l1l4", "l2l3", "l2l4"]
ACTIONS = ["a", "b", "c"]


def open_flatbuffers(path: str):
    print(f"[open_flatbuffers] opening path={path}", flush=True)
    with open(path, "rb") as f:
        data = bytearray(f.read())
    print(f"[open_flatbuffers] read {len(data)} bytes", flush=True)
    ss = State_space_ta.GetRootAs(data, 0)
    print(f"[open_flatbuffers] n_states={ss.EtatsLength()}", flush=True)
    return ss


def load_state(ss, state_id: int):
    s = ss.Etats(state_id)
    loc = s.Location()
    loc_name = LOCATIONS[loc] if loc < len(LOCATIONS) else str(loc)
    dbm = s.ClockZone()
    if dbm is not None:
        dbm_dim = dbm.Dim()
        ma_values = [dbm.MatriceApplatie(j) for j in range(dbm.MatriceApplatieLength())]
        dbm_matrix = [ma_values[r * dbm_dim:(r + 1) * dbm_dim] for r in range(dbm_dim)]
    else:
        dbm_dim = None
        dbm_matrix = []

    var = s.Var()
    var_v = var.V() if var is not None else None
    constraints = convert_to_constraint_string(dbm_matrix)
    constraints_str = "\n".join(constraints) if constraints else ""
    label = f"{loc_name}, v={var_v}\n{constraints_str}"
    node = {
        "data": {
            "id": str(state_id),
            "label": label,
            "loc": loc,
            "loc_name": loc_name,
            "var_v": var_v,
        }
    }
    info = {
        "index": state_id,
        "location": loc_name,
        "variable_v": var_v,
        "dbm_dim": dbm_dim,
        "dbm_matrix": dbm_matrix,
    }
    return node, info


def load_adjacent(ss, state_id: int):
    st = ss.StateTransitions(state_id)
    if st is None:
        return [], []
    
    adjacent_ids = []
    edges = []
    
    for j in range(st.ItemsLength()):
        tr = st.Items(j)
        cible = tr.Cible()
        action_id = tr.ActionId()
        action_name = ACTIONS[action_id] if action_id < len(ACTIONS) else str(action_id)
        
        adjacent_ids.append(cible)
        edges.append({
            "data": {
                "source": str(state_id),
                "target": str(cible),
                "label": action_name,
            }
        })
    
    return adjacent_ids, edges


def load_predecessors(ss, state_id: int):
    st = ss.StatePredecessors(state_id)
    if st is None:
        return [], []
    
    predecessor_ids = []
    edges = []
    
    for j in range(st.ItemsLength()):
        tr = st.Items(j)
        pred_id = tr.Cible()
        action_id = tr.ActionId()
        action_name = ACTIONS[action_id] if action_id < len(ACTIONS) else str(action_id)
        
        predecessor_ids.append(pred_id)
        edges.append({
            "data": {
                "source": str(pred_id),
                "target": str(state_id),
                "label": action_name,
            }
        })
    
    return predecessor_ids, edges


def load_subgraph_succ(ss, root_id: int, depth: int):
    visited_ids = {root_id}
    all_nodes = {}
    all_edges = []
    all_info = {}

    root_node, root_info = load_state(ss, root_id)
    all_nodes[root_id] = root_node
    all_info[str(root_id)] = root_info

    current_level = {root_id}

    for _ in range(depth):
        next_level = set()
        for state_id in current_level:
            adjacent_ids, edges = load_adjacent(ss, state_id)
            all_edges.extend(edges)
            for nid in adjacent_ids:
                if nid not in visited_ids:
                    visited_ids.add(nid)
                    next_level.add(nid)
                    node, info = load_state(ss, nid)
                    all_nodes[nid] = node
                    all_info[str(nid)] = info
        current_level = next_level
        if not current_level:
            break

    elements = list(all_nodes.values()) + all_edges
    return elements, all_info



def load_subgraph_pred(ss, root_id: int, depth: int):
    visited_ids = {root_id}
    all_nodes = {}
    all_edges = []
    all_info = {}

    root_node, root_info = load_state(ss, root_id)
    all_nodes[root_id] = root_node
    all_info[str(root_id)] = root_info

    current_level = {root_id}

    for _ in range(depth):
        next_level = set()
        for state_id in current_level:
            predecessor_ids, edges = load_predecessors(ss, state_id)
            all_edges.extend(edges)
            for nid in predecessor_ids:
                if nid not in visited_ids:
                    visited_ids.add(nid)
                    next_level.add(nid)
                    node, info = load_state(ss, nid)
                    all_nodes[nid] = node
                    all_info[str(nid)] = info
        current_level = next_level
        if not current_level:
            break

    elements = list(all_nodes.values()) + all_edges
    return elements, all_info


def load_subgraph(ss, root_id: int, depth: int):
    visited_ids = {root_id}
    all_nodes = {}
    all_edges = []
    all_info = {}

    root_node, root_info = load_state(ss, root_id)
    all_nodes[root_id] = root_node
    all_info[str(root_id)] = root_info

    current_level = {root_id}

    for _ in range(depth):
        next_level = set()
        for state_id in current_level:
            predecessor_ids, pred_edges = load_predecessors(ss, state_id)
            adjacent_ids, edges = load_adjacent(ss, state_id)
            all_edges.extend(edges)
            all_edges.extend(pred_edges)
            for nid in predecessor_ids + adjacent_ids:
                if nid not in visited_ids:
                    visited_ids.add(nid)
                    next_level.add(nid)
                    node, info = load_state(ss, nid)
                    all_nodes[nid] = node
                    all_info[str(nid)] = info
        current_level = next_level
        if not current_level:
            break

    elements = list(all_nodes.values()) + all_edges
    return elements, all_info


def convert_to_constraint_string(dbm_matrix):
    constraints = []
    for i in range(len(dbm_matrix)):
        for j in range(i+1, len(dbm_matrix[i])):
            if (dbm_matrix[i][j] == float('inf') or dbm_matrix[i][j] == float('-inf')) and (dbm_matrix[j][i] == float('inf') or dbm_matrix[j][i] == float('-inf')):
                continue
            if i == 0:
                if dbm_matrix[i][j] == float('inf') or dbm_matrix[i][j] == float('-inf'):
                    constraints.append(f"x{j} <= {dbm_matrix[j][i]}")
                elif dbm_matrix[j][i] == float('inf') or dbm_matrix[j][i] == float('-inf'):
                    constraints.append(f"x{j} >= {-dbm_matrix[i][j]}")
                elif dbm_matrix[i][j] == -dbm_matrix[j][i]:
                    constraints.append(f"x{j} = {-dbm_matrix[i][j]}")
                else:
                    if dbm_matrix[i][j] == 0:
                        constraints.append(f"x{j} <= {dbm_matrix[j][i]}")
                    elif dbm_matrix[j][i] == 0:
                        constraints.append(f"x{j} >= {-dbm_matrix[i][j]}")
                    else:
                        constraints.append(f"{-dbm_matrix[i][j]} <= x{j} <= {dbm_matrix[j][i]}")
            else:
                if dbm_matrix[i][j] == 0 and dbm_matrix[j][i] == 0:
                    constraints.append(f"x{i} = x{j}")
                elif dbm_matrix[i][j] == float('inf') or dbm_matrix[i][j] == float('-inf'):
                    constraints.append(f"x{j} - x{i} <= {dbm_matrix[j][i]}")
                elif dbm_matrix[j][i] == float('inf') or dbm_matrix[j][i] == float('-inf'):
                    constraints.append(f"x{i} - x{j} <= {dbm_matrix[i][j]}")
                elif dbm_matrix[i][j] == -dbm_matrix[j][i]:
                    constraints.append(f"x{i} - x{j} = {dbm_matrix[i][j]}")
                else:
                    constraints.append(f"{-dbm_matrix[j][i]} <= x{i} - x{j} <= {dbm_matrix[i][j]}")
    return constraints


def serve(ss):
    app = dash.Dash(__name__, suppress_callback_exceptions=True, serve_locally=True)
    try:
        cyto.load_extra_layouts()
    except Exception:
        pass

    print("[serve] entering serve()", flush=True)

    stylesheet = [
        {
            "selector": "node",
            "style": {
                "shape": "rectangle",
                "content": "data(label)",
                "text-valign": "center",
                "text-halign": "center",
                "color": "#fff",
                "background-color": "#6EA8FE",
                "width": 60,
                "height": 40,
                "font-size": 8,
                "text-wrap": "wrap"
            },
        },
        {
            "selector": "node:selected",
            "style": {
                "border-width": 3,
                "border-color": "#000",
            },
        },
        {
            "selector": "edge",
            "style": {
                "curve-style": "bezier",
                "target-arrow-shape": "triangle",
                "label": "data(label)",
                "font-size": 10,
                "line-color": "#888",
                "target-arrow-color": "#888",
            },
        },
        {
            "selector": "edge:selected",
            "style": {
                "line-color": "#e24a4a",
                "target-arrow-color": "#e24a4a",
            },
        },
    ]

    app.layout = html.Div([
        html.H4("State Space Visualization", style={"margin": "10px"}),
        html.Div([
            html.Span("Entrez l'ID d'un état pour démarrer : ", style={"marginRight": "8px"}),
            dcc.Input(
                id="start-state-id",
                type="number",
                placeholder="ID état",
                style={"width": "120px", "marginRight": "8px"},
            ),
            html.Button("Afficher", id="start-state-btn", n_clicks=0),
            html.Span(id="start-state-message", style={"marginLeft": "12px", "color": "#444"}),
        ], style={"margin": "10px"}),
        html.Div([
            cyto.Cytoscape(
                id="graph",
                elements=[],
                layout={
                    "name": "breadthfirst",
                    "animate": False,
                    "fit": True,
                    "directed": True,
                    "padding": 40,
                },
                    style={"width": "100%", "height": "90vh", "display": "block", "border": "1px solid #ddd"},
                stylesheet=stylesheet,
                minZoom=0.05,
                maxZoom=5,
                autoRefreshLayout=True,
            ),
        ], style={"display": "flex"}),
        dcc.Store(id="visible-subgraph", data={"root_id": None, "depth": 0, "started": False}),
        dcc.Store(id="state-info-store", data={}),
    ])

    @app.callback(
        Output("graph", "elements"),
        Output("graph", "layout"),
        Output("start-state-message", "children"),
        Output("visible-subgraph", "data"),
        Output("state-info-store", "data"),
        Input("start-state-btn", "n_clicks"),
        Input("graph", "tapNodeData"),
        State("start-state-id", "value"),
        State("visible-subgraph", "data"),
        State("state-info-store", "data"),
    )
    def update_graph(start_clicks, tap_data, start_id, subgraph_state, state_info):
        ctx = dash.callback_context
        trigger = ctx.triggered[0]["prop_id"].split(".")[0] if ctx.triggered else None

        if not subgraph_state:
            subgraph_state = {"root_id": None, "depth": 0, "started": False}

        if trigger == "start-state-btn":
            if start_id is None or str(start_id).strip() == "":
                return no_update, no_update, "Entrez un ID valide.", subgraph_state, state_info

            root_id = int(start_id)
            if root_id < 0 or root_id >= ss.EtatsLength():
                return no_update, no_update, f"ID {root_id} introuvable.", subgraph_state, state_info

            root_node, root_info = load_state(ss, root_id)
            state_info.update({str(root_id): root_info})
            subgraph_state = {"root_id": root_id, "depth": 0, "started": True, "elements": [root_node]}
            return (
                [root_node],
                {"name": "breadthfirst", "animate": False, "fit": True, "directed": True, "padding": 40},
                f"État {root_id} affiché. Cliquez sur un noeud pour afficher ses prédécesseurs.",
                subgraph_state,
                state_info,
            )

        if trigger == "graph" and tap_data and subgraph_state.get("started"):
            clicked_id = int(tap_data.get("id"))

            if clicked_id < 0 or clicked_id >= ss.EtatsLength():
                return no_update, no_update, no_update, subgraph_state, state_info

            new_elements, new_info = load_subgraph(ss, clicked_id, 1)
            state_info.update(new_info)
            existing_elements = subgraph_state.get("elements", [])
            all_ids = set()
            merged = []
            for el in existing_elements + new_elements:
                eid = el["data"].get("id") or f"{el['data'].get('source')}-{el['data'].get('target')}"
                if eid not in all_ids:
                    all_ids.add(eid)
                    merged.append(el)

            subgraph_state["elements"] = merged
            return (
                merged,
                {"name": "breadthfirst", "animate": False, "fit": True, "directed": True, "padding": 40},
                f"Prédécesseurs et successeurs de l'état {clicked_id} affichés.",
                subgraph_state,
                state_info,
            )

        return [], {"name": "breadthfirst", "animate": False, "fit": True, "directed": True, "padding": 40}, no_update, subgraph_state, state_info

    run = getattr(app, "run", None)
    if callable(run):
        run(host="127.0.0.1", port=8050, debug=False, use_reloader=False)
    else:
        app.run_server(host="127.0.0.1", port=8050, debug=False, use_reloader=False)


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Visualize state_space FlatBuffers")
    parser.add_argument("path", nargs="?", default=os.path.join(os.getcwd(), "state_space"), help="Path to state_space file")
    args = parser.parse_args()

    path = args.path
    if not os.path.exists(path):
        print(f"Fichier non trouvé : {path}")
        raise SystemExit(1)

    print(f"[main] loading path={path}", flush=True)
    ss = open_flatbuffers(path)
    print(f"[main] ready, n_states={ss.EtatsLength()}", flush=True)
    serve(ss)


if __name__ == "__main__":
    main()
