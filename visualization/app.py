import pandas as pd
import typing
from dash import Dash, html

from grid import Grid

grid_type = typing.Type[Grid]

class PerformanceVisualizationApp(Dash):
    def __init__(self, dataframe: pd.DataFrame, grid_types: list[grid_type]):
        Dash.__init__(self, __name__, external_stylesheets=["./style.css"])
        self.layout = html.Div([
            html.H1("Performance results"),
            *map(lambda grid: grid(dataframe), grid_types),
        ], className="app")
