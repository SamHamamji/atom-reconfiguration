import pandas as pd
from dash import html

from apps.app import App

from tab_contents.grid_solver_tabs import (
    OverviewElement,
)


class GridSolverApp(App):
    tab_content_types: dict[str, type[html.Div]] = {
        "Overview": OverviewElement,
    }

    def __init__(self, dataframe: pd.DataFrame):
        super().__init__(dataframe, self.tab_content_types)
