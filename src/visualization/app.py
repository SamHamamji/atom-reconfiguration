import pandas as pd
from dash import Dash, html, Output, Input, dcc

from tab_contents import HeatMapGrid, ImbalanceGrid, SolverGrid, SizeGrid, OverviewElement

class PerformanceVisualizationApp(Dash):
    tab_content_types: dict[str, type[html.Div]] = {
        "Overview": OverviewElement,
        "Imbalance Grid": ImbalanceGrid,
        "Solver Grid": SolverGrid,
        "HeatMap Grid": HeatMapGrid,
        "Size Grid": SizeGrid,
    }
    tabs : list[dcc.Tab] = list(map(lambda name: dcc.Tab(label=name, value=name), tab_content_types.keys()))

    def __init__(self, dataframe: pd.DataFrame):
        Dash.__init__(self, __name__, external_stylesheets=["./style.css"])
        self.layout = html.Div([
            dcc.Tabs(
                id='tabs',
                value=PerformanceVisualizationApp.tab_content_types.keys().__iter__().__next__(),
                children=PerformanceVisualizationApp.tabs,
            ),
            html.Div(id='tab-content'),
        ], className="app")

        self.title = "Performance results"

        @self.callback(
            Output('tab-content', 'children'),
            Input('tabs', 'value'),
        )
        def switch_tab(tab_content_type):
            return [
                PerformanceVisualizationApp.tab_content_types[tab_content_type](dataframe),
            ]
