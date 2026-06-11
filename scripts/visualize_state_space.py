import os
import dash
from dash import ctx, dcc, html, no_update
from dash.dependencies import Input, Output, State
from State_space_ta.State_space_ta import State_space_ta
from server import init_tile_server, start_flask


def open_flatbuffers(path: str):
    print(f"[open_flatbuffers] opening path={path}", flush=True)
    with open(path, "rb") as f:
        data = bytearray(f.read())
    print(f"[open_flatbuffers] read {len(data)} bytes", flush=True)
    ss = State_space_ta.GetRootAs(data, 0)
    print(f"[open_flatbuffers] n_states={ss.EtatsLength()}", flush=True)
    return ss


def load_state(ss, state_id: int):
    locations = [ss.Locations(i).decode() for i in range(ss.LocationsLength())]
    s = ss.Etats(state_id)
    loc = s.Location()
    loc_name = locations[loc] if loc < len(locations) else str(loc)
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
    constraints_str = " ,".join(constraints) if constraints else ""
    label = f"ID: {state_id}, {loc_name}\n {constraints_str}\n v= {var_v}"
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
    actions = [ss.Actions(i).decode() for i in range(ss.ActionsLength())]
    st = ss.StateSuccessors(state_id)
    if st is None:
        return [], []
    adjacent_ids = []
    edges = []
    for j in range(st.ItemsLength()):
        tr = st.Items(j)
        succ_id = tr.Cible()
        action_id = tr.ActionId()
        action_name = f"{actions[action_id]}" if action_id < len(actions) else str(action_id)
        adjacent_ids.append(succ_id)
        edges.append({
            "data": {
                "source": str(state_id),
                "target": str(succ_id),
                "label": action_name,
            }
        })
    return adjacent_ids, edges


def load_predecessors(ss, state_id: int):
    actions = [ss.Actions(i).decode() for i in range(ss.ActionsLength())]
    st = ss.StatePredecessors(state_id)
    if st is None:
        return [], []
    predecessor_ids = []
    edges = []
    for j in range(st.ItemsLength()):
        tr = st.Items(j)
        pred_id = tr.Cible()
        action_id = tr.ActionId()
        action_name = f"{actions[action_id]}" if action_id < len(actions) else str(action_id)
        predecessor_ids.append(pred_id)
        edges.append({
            "data": {
                "source": str(pred_id),
                "target": str(state_id),
                "label": action_name,
            }
        })
    return predecessor_ids, edges


def convert_to_constraint_string(dbm_matrix):
    constraints = []
    for i in range(len(dbm_matrix)):
        for j in range(i + 1, len(dbm_matrix[i])):
            if (dbm_matrix[i][j] == float('inf') or dbm_matrix[i][j] == float('-inf')) and \
               (dbm_matrix[j][i] == float('inf') or dbm_matrix[j][i] == float('-inf')):
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

    btn_style = {
        "marginLeft": "8px",
        "padding": "4px 12px",
        "cursor": "pointer",
        "border": "1px solid #ccc",
        "borderRadius": "4px",
        "background": "#fff",
    }

    btn_nav_base = {
        "padding": "4px 12px",
        "cursor": "pointer",
        "border": "1px solid #ccc",
        "borderRadius": "4px",
        "background": "#fff",
        "marginLeft": "4px",
    }

    btn_nav_active = {
        **btn_nav_base,
        "background": "#7ea3f3",
        "color": "#fff",
        "borderColor": "#556b9b",
    }

    app.layout = html.Div([
        html.H4("State Space Visualization", style={"margin": "10px"}),
        html.Div([
            html.Div([
                html.Button("Navigation dynamique", id="btn-nav-dynamic", n_clicks=0,
                            style=btn_nav_active),
                html.Button("Navigation statique", id="btn-nav-static", n_clicks=0,
                            style={**btn_nav_base, "marginLeft": "4px"}),
            ], style={"display": "flex", "alignItems": "center"}),
            html.Div([
                html.Button("Prédecesseurs", id="btn-pred", n_clicks=0, style=btn_nav_base),
                html.Button("Successeurs", id="btn-succ", n_clicks=0, style=btn_nav_base),
                html.Button("Préd + Succ", id="btn-both", n_clicks=0, style={**btn_nav_active, "marginLeft": "4px"}),
            ], style={"display": "flex", "alignItems": "center"}),
            html.Div([
                html.Span("Entrez l'ID d'un état pour démarrer : ",
                          style={"marginRight": "8px"}),
                dcc.Input(
                    id="start-state-id",
                    type="number",
                    placeholder="ID état",
                    style={"width": "120px", "marginRight": "8px"},
                ),
                html.Button("Afficher", id="start-state-btn", n_clicks=0, style=btn_style),
                html.Span(id="start-state-message",
                          style={"marginLeft": "12px", "color": "#444"}),
            ], style={"display": "flex", "alignItems": "center"}),
        ], style={"margin": "10px", "display": "flex", "justifyContent": "flex-start", "alignItems": "center", "gap": "40px"}),

        dcc.Store(id="current-nav", data="dynamic"),
        dcc.Store(id="current-mode", data="both"),
        dcc.Store(id="current-start", data=None),

        html.Iframe(
            id="graph-iframe",
            src="http://127.0.0.1:8051/?start=0&nav=dynamic&mode=both",
            style={
                "width": "100%",
                "height": "90vh",
                "border": "1px solid #ddd",
                "display": "block",
            },
        ),
    ], style={"position": "relative"})

    @app.callback(
        Output("current-start", "data"),
        Output("start-state-message", "children"),
        Input("start-state-btn", "n_clicks"),
        State("start-state-id", "value"),
        prevent_initial_call=True,
    )
    def update_start(n_clicks, start_id):
        if start_id is None:
            return no_update, "Entrez un ID valide."
        root_id = int(start_id)
        if root_id < 0 or root_id >= ss.EtatsLength():
            return no_update, f"ID {root_id} introuvable."
        return root_id, f"État {root_id} affiché."

    @app.callback(
        Output("current-nav", "data"),
        Output("btn-nav-dynamic", "style"),
        Output("btn-nav-static", "style"),
        Input("btn-nav-dynamic", "n_clicks"),
        Input("btn-nav-static", "n_clicks"),
        State("current-nav", "data"),
        prevent_initial_call=True,
    )
    def update_nav(n_dyn, n_sta, current_nav):
        ctx = dash.callback_context
        if not ctx.triggered:
            raise dash.exceptions.PreventUpdate
        triggered = ctx.triggered[0]["prop_id"].split(".")[0]
        nav = "dynamic" if triggered == "btn-nav-dynamic" else "static"
        style_dyn = {**btn_nav_active} if nav == "dynamic" else {**btn_nav_base, "marginLeft": "24px"}
        style_sta = {**btn_nav_active} if nav == "static"  else {**btn_nav_base}
        return nav, style_dyn, style_sta
    
    @app.callback(
    Output("current-mode", "data"),
    Input("btn-pred", "n_clicks"),
    Input("btn-succ", "n_clicks"),
    Input("btn-both", "n_clicks"),
    State("current-mode", "data"),
    prevent_initial_call=True,
    )
    def update_mode(n_pred, n_succ, n_both, current):
        ctx = dash.callback_context
        if not ctx.triggered:
            raise dash.exceptions.PreventUpdate

        trigger = ctx.triggered[0]["prop_id"].split(".")[0]
        mode = "both" if trigger == "btn-both" else ("pred" if trigger == "btn-pred" else "succ")
        style_pred = {**btn_nav_active} if mode =="pred" else {**btn_nav_base}
        style_succ = {**btn_nav_active} if mode == "succ" else {**btn_nav_base}
        style_both = {**btn_nav_active} if mode == "both" else {**btn_nav_base, "marginLeft": "24px"}
        return mode, style_pred, style_succ, style_both

    @app.callback(
        Output("graph-iframe", "src"),
        Input("current-start", "data"),
        Input("current-nav", "data"),
        Input("current-mode", "data"),
        State("start-state-btn", "n_clicks"),
        prevent_initial_call=True,
    )
    def update_iframe(start_id, nav, mode, n_clicks):
        sid = start_id if start_id is not None else 0
        return f"http://127.0.0.1:8051/?start={sid}&nav={nav}&mode={mode}&t={n_clicks}"

    run = getattr(app, "run", None)
    if callable(run):
        run(host="127.0.0.1", port=8050, debug=False, use_reloader=False)
    else:
        app.run_server(host="127.0.0.1", port=8050, debug=False, use_reloader=False)


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Visualize state_space FlatBuffers")
    parser.add_argument(
        "path",
        nargs="?",
        default=os.path.join(os.getcwd(), "state_space"),
        help="Path to state_space file",
    )
    args = parser.parse_args()

    if not os.path.exists(args.path):
        print(f"Fichier non trouvé : {args.path}")
        raise SystemExit(1)

    ss = open_flatbuffers(args.path)

    init_tile_server(ss, load_state, load_adjacent, load_predecessors)
    start_flask(port=8051)
    serve(ss)


if __name__ == "__main__":
    main()