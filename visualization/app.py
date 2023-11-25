import pandas as pd
from dash import Dash, html

class PerformanceVisualizationApp(Dash):
    def __init__(self, dataframe: pd.DataFrame, grid_types: list[html.Div]):
        Dash.__init__(self, __name__, external_stylesheets=["./style.css"])
        self.layout = html.Div([
            html.H1("Performance results"),
            *map(lambda grid: grid(dataframe), grid_types),
        ], className="app")
