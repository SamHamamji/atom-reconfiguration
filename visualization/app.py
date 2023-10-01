import pandas as pd
from dash import Dash, html

from grid import SolverGrid, SizeGrid

class PerformanceVisualizationApp(Dash):
    def __init__(self, dataframe: pd.DataFrame):
        Dash.__init__(self, __name__, external_stylesheets=["./style.css"])
        self.layout = html.Div([
            html.H1("Performance results"),
            SolverGrid(dataframe),
            SizeGrid(dataframe)
        ], className="app")
