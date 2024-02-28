import pandas as pd
from dash import html

from .app import App
from tab_contents.linear_solver_tabs import (
    OverviewElement,
    ImbalanceGrid,
    LinearSolverGrid,
    LengthGrid,
)


class LinearSolverApp(App):
    tab_content_types: dict[str, type[html.Div]] = {
        "Overview": OverviewElement,
        "Imbalance Grid": ImbalanceGrid,
        "Linear Solver Grid": LinearSolverGrid,
        "Length Grid": LengthGrid,
    }

    def __init__(self, dataframe: pd.DataFrame):
        super().__init__(dataframe, self.tab_content_types)
