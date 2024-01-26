import typing
import pandas as pd
import plotly.express as px
from dash import dcc, html

from CsvHeader import CsvHeader
from data_processing import compute_mean_for_duplicates


class ScatterGridConfig(typing.NamedTuple):
    x: CsvHeader
    y: CsvHeader
    color: CsvHeader
    header: CsvHeader
    trendline: str = "lowess"
    trendline_options: dict = {"frac": 0.3}
    title_generator: typing.Callable[[str], str] = lambda x: x


class ScatterElement(html.Div):
    def __init__(
        self, dataframe: pd.DataFrame, config: ScatterGridConfig, header_value: str
    ):
        value_data = dataframe[dataframe[config.header.value] == header_value]
        figure = px.scatter(
            value_data,
            x=config.x.value,
            y=config.y.value,
            color=value_data[config.color.value] if config.color else None,
            color_continuous_scale=px.colors.sequential.Turbo_r
            if config.color
            else None,
            trendline=config.trendline,
            trendline_options=config.trendline_options,
        )
        html.Div.__init__(
            self,
            [
                html.H3(
                    f"{config.header.value}: {config.title_generator(header_value)}"
                ),
                dcc.Graph(figure=figure),
            ],
            className="grid-item",
        )


class HistogramGridConfig(typing.NamedTuple):
    x: CsvHeader
    y: CsvHeader
    header: CsvHeader
    title_generator: typing.Callable[[str], str] = lambda x: x


class HistogramElement(html.Div):
    def __init__(
        self, dataframe: pd.DataFrame, config: ScatterGridConfig, header_value: str
    ):
        value_data = dataframe[dataframe[config.header.value] == header_value]
        figure = px.histogram(
            value_data,
            x=config.x.value,
            y=config.y.value,
            histfunc="avg",
        )
        html.Div.__init__(
            self,
            [
                html.H3(
                    f"{config.header.value}: {config.title_generator(header_value)}"
                ),
                dcc.Graph(figure=figure),
            ],
            className="grid-item",
        )


class LinearSolverGrid(html.Div):
    config = ScatterGridConfig(
        x=CsvHeader.LENGTH,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.IMBALANCE_PERCENT,
        header=CsvHeader.LINEAR_SOLVER,
    )

    def __init__(self, dataframe: pd.DataFrame):
        new_dataframe = dataframe.sort_values(
            by=self.config.color.value, ascending=False, inplace=False
        )
        new_dataframe[self.config.color.value] = new_dataframe[
            self.config.color.value
        ].astype(str)

        grid = html.Div(
            [
                ScatterElement(new_dataframe, self.config, linear_solver)
                for linear_solver in dataframe[CsvHeader.LINEAR_SOLVER.value].unique()
            ],
            className="grid",
        )
        grid_title = html.H1("Performance by linear solver")
        html.Div.__init__(self, [grid_title, grid])


class HeatMapGrid(html.Div):
    config = ScatterGridConfig(
        x=CsvHeader.LENGTH,
        y=CsvHeader.IMBALANCE_PERCENT,
        color=CsvHeader.TIME_TAKEN,
        header=CsvHeader.LINEAR_SOLVER,
        trendline=None,
    )

    def __init__(self, dataframe: pd.DataFrame):
        grid = html.Div(
            [
                ScatterElement(dataframe, self.config, linear_solver)
                for linear_solver in dataframe[CsvHeader.LINEAR_SOLVER.value].unique()
            ],
            className="grid",
        )
        grid_title = html.H1("Performance heatmap by linear solver")
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


class OverviewElement(html.Div):
    def __init__(self, dataframe: pd.DataFrame):
        new_dataframe = compute_mean_for_duplicates(
            dataframe.drop(CsvHeader.IMBALANCE_PERCENT.value, axis=1),
            CsvHeader.TIME_TAKEN.value,
        )
        figure = px.scatter(
            new_dataframe,
            x=CsvHeader.LENGTH.value,
            y=CsvHeader.TIME_TAKEN.value,
            color=new_dataframe[CsvHeader.LINEAR_SOLVER.value],
            color_continuous_scale=px.colors.sequential.Turbo_r,
            trendline="lowess",
            trendline_options={"frac": 0.3},
        )
        html.Div.__init__(
            self,
            [
                html.H1("Performance overview"),
                dcc.Graph(figure=figure, className="overview-graph"),
            ],
        )
