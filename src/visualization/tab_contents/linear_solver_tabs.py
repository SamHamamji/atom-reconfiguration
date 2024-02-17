import pandas as pd
from dash import html

from CsvHeader import CsvHeader
from tab_contents.generic_tab_contents import (
    ScatterGridConfig,
    ScatterElement,
    HistogramGridConfig,
    HistogramElement,
)


class OverviewElement(html.Div):
    config = ScatterGridConfig(
        x=CsvHeader.LENGTH,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.LINEAR_SOLVER,
        header=None,
    )

    def __init__(self, dataframe: pd.DataFrame):
        html.Div.__init__(
            self,
            [
                html.H1("Performance overview"),
                ScatterElement(dataframe, self.config, ""),
            ],
        )


class LinearSolverGrid(html.Div):
    config = ScatterGridConfig(
        x=CsvHeader.LENGTH,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.IMBALANCE_PERCENT,
        header=CsvHeader.LINEAR_SOLVER,
    )

    def __init__(self, dataframe: pd.DataFrame):
        sorted_dataframe = dataframe.sort_values(
            by=self.config.color.value, ascending=False, inplace=False
        )

        grid = html.Div(
            [
                ScatterElement(sorted_dataframe, self.config, linear_solver)
                for linear_solver in dataframe[CsvHeader.LINEAR_SOLVER.value].unique()
            ],
            className="grid",
        )
        grid_title = html.H1("Performance by linear solver")
        html.Div.__init__(self, [grid_title, grid])


class ImbalanceGrid(html.Div):
    config = ScatterGridConfig(
        x=CsvHeader.LENGTH,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.LINEAR_SOLVER,
        header=CsvHeader.IMBALANCE_PERCENT,
        title_generator=lambda x: f"{x}%",
    )

    def __init__(self, dataframe: pd.DataFrame):
        grid = html.Div(
            [
                ScatterElement(dataframe, self.config, imbalance)
                for imbalance in dataframe[CsvHeader.IMBALANCE_PERCENT.value].unique()
            ],
            className="grid",
        )
        grid_title = html.H1("Performance by imbalance")
        html.Div.__init__(self, [grid_title, grid])


class LengthGrid(html.Div):
    config = HistogramGridConfig(
        x=CsvHeader.LINEAR_SOLVER,
        y=CsvHeader.TIME_TAKEN,
        header=CsvHeader.LENGTH,
    )

    def __init__(self, dataframe: pd.DataFrame):
        lengths = (
            dataframe[CsvHeader.LENGTH.value]
            .sort_values(key=lambda x: x.astype(int))
            .unique()
        )

        grid = html.Div(
            [HistogramElement(dataframe, self.config, length) for length in lengths],
            className="grid",
        )
        grid_title = html.H1("Performance by length")
        html.Div.__init__(self, [grid_title, grid])
