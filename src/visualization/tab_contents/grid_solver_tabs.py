import pandas as pd
from dash import html

from CsvHeader import CsvHeader
from tab_contents.generic_tab_contents import (
    ScatterGridConfig,
    ScatterElement,
)


class OverviewElement(html.Div):
    def __init__(self, dataframe: pd.DataFrame):
        dataframe[CsvHeader.LENGTH.value] = (
            dataframe[CsvHeader.WIDTH.value] * dataframe[CsvHeader.HEIGHT.value]
        )
        html.Div.__init__(
            self,
            [
                html.H1("Performance overview"),
                ScatterElement(
                    dataframe,
                    ScatterGridConfig(
                        x=CsvHeader.LENGTH,
                        y=CsvHeader.TIME_TAKEN,
                        color=CsvHeader.GRID_SOLVER,
                        header=None,
                    ),
                    "Overview by length",
                ),
                ScatterElement(
                    dataframe,
                    ScatterGridConfig(
                        x=CsvHeader.HEIGHT,
                        y=CsvHeader.TIME_TAKEN,
                        color=CsvHeader.WIDTH,
                        header=None,
                    ),
                    "Overview by width and height",
                ),
                ScatterElement(
                    dataframe,
                    ScatterGridConfig(
                        x=CsvHeader.WIDTH,
                        y=CsvHeader.TIME_TAKEN,
                        color=CsvHeader.HEIGHT,
                        header=None,
                    ),
                    "Overview by width and height",
                ),
            ],
        )
