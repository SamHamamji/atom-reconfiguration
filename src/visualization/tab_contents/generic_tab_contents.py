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
    header: CsvHeader | None
    trendline: str = "lowess"
    trendline_options: dict = {"frac": 0.3}
    title_generator: typing.Callable[[str], str] = lambda x: x


class ScatterElement(html.Div):
    def __init__(
        self,
        df: pd.DataFrame,
        config: ScatterGridConfig,
        header_value: str,
    ):
        if config.header is not None:
            df = df[df[config.header.value] == header_value]
            title = f"{config.header.value}: {config.title_generator(header_value)}"
        else:
            title = header_value

        df = df.copy()
        df[config.color.value] = df[config.color.value].astype(str)
        df = compute_mean_for_duplicates(
            df, [config.x.value, config.color.value], config.y.value
        )

        figure = px.scatter(
            df,
            x=df[config.x.value],
            y=df[config.y.value],
            color=config.color.value,
            color_continuous_scale=px.colors.sequential.Turbo_r,
            trendline=config.trendline,
            trendline_options=config.trendline_options,
        )

        html.Div.__init__(
            self,
            [html.H3(title), dcc.Graph(figure=figure)],
            className="grid-item" if config.header is not None else "overview-item",
        )


class HistogramGridConfig(typing.NamedTuple):
    x: CsvHeader
    y: CsvHeader
    header: CsvHeader
    title_generator: typing.Callable[[str], str] = lambda x: x


class HistogramElement(html.Div):
    def __init__(
        self, df: pd.DataFrame, config: HistogramGridConfig, header_value: str
    ):
        value_data = df[df[config.header.value] == header_value]
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
