import pandas as pd
from dash import html

from .app import App
from CsvHeader import CsvHeader
from tab_contents.grid_solver_tabs import OverviewElement, DimensionsGrid, SizeGrid


class GridSolverApp(App):
    tab_content_types: dict[str, type[html.Div]] = {
        "Overview": OverviewElement,
        "Dimensions Grid": DimensionsGrid,
        "Size Grid": SizeGrid,
    }

    def __init__(self, dataframe: pd.DataFrame):
        dataframe[CsvHeader.GRID_SIZE.value] = (
            dataframe[CsvHeader.WIDTH.value] * dataframe[CsvHeader.HEIGHT.value]
        )

        super().__init__(dataframe, self.tab_content_types)
