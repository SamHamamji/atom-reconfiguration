import pandas as pd
from dash import html

from CsvHeader import CsvHeader
from tab_contents.generic_tab_contents import (
    ScatterConfig,
    ScatterElement,
    HistogramConfig,
    HistogramElement,
)


class OverviewElement(html.Div):
    def __init__(self, dataframe: pd.DataFrame):
        html.Div.__init__(
            self,
            [
                html.H1("Performance overview"),
                ScatterElement(
                    dataframe,
                    ScatterConfig(
                        x=CsvHeader.GRID_SIZE,
                        y=CsvHeader.TIME_TAKEN,
                        color=CsvHeader.GRID_SOLVER,
                        header=None,
                    ),
                ),
            ],
        )


class DimensionsGrid(html.Div):
    width_by_height_config = ScatterConfig(
        x=CsvHeader.WIDTH,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.HEIGHT,
        header=CsvHeader.GRID_SOLVER,
    )

    height_by_width_config = ScatterConfig(
        x=CsvHeader.HEIGHT,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.WIDTH,
        header=CsvHeader.GRID_SOLVER,
    )

    def __init__(self, dataframe: pd.DataFrame):
        grid_solvers = dataframe[CsvHeader.GRID_SOLVER.value].unique()

        grid = html.Div(
            [
                ScatterElement(dataframe, config, grid_solver)
                for grid_solver in grid_solvers
                for config in (
                    DimensionsGrid.width_by_height_config,
                    DimensionsGrid.height_by_width_config,
                )
            ],
            className="grid",
        )
        grid_title = html.H1("Performance by width and height")
        html.Div.__init__(self, [grid_title, grid])


class SizeGrid(html.Div):
    config = HistogramConfig(
        x=CsvHeader.GRID_SOLVER,
        y=CsvHeader.TIME_TAKEN,
        header=CsvHeader.GRID_SIZE,
    )

    def __init__(self, dataframe: pd.DataFrame):
        sizes = (
            dataframe[CsvHeader.GRID_SIZE.value]
            .sort_values(key=lambda x: x.astype(int))
            .unique()
        )

        grid = html.Div(
            [HistogramElement(dataframe, self.config, size) for size in sizes],
            className="grid",
        )
        grid_title = html.H1("Performance by grid size")
        html.Div.__init__(self, [grid_title, grid])
