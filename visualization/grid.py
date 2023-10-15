import typing
import pandas as pd
import plotly.express as px

from dash import dcc, html
from CsvHeader import CsvHeader, header_to_string

class GraphConfig(typing.NamedTuple):
    x: CsvHeader
    y: CsvHeader
    color: CsvHeader
    header: CsvHeader

class GridElement(html.Div):
    def __init__(self, dataframe: pd.DataFrame, config: GraphConfig, header_value: str):
        value_data = dataframe[dataframe[config.header.value] == header_value].sort_values(by=config.color.value, ascending=False)
        figure = px.scatter(
            value_data,
            x = config.x.value,
            y = config.y.value,
            color = value_data[config.color.value].apply(str),
            color_continuous_scale=px.colors.sequential.Turbo_r,
            trendline="lowess",
            trendline_options={"frac":0.3},
        )
        html.Div.__init__(self, [
            html.H3(f"{header_to_string[config.header]}: {header_value}"),
            dcc.Graph(figure=figure),
        ], className="grid-item")


class SolverGrid(html.Div):
    graphConfig= GraphConfig(
        x=CsvHeader.IMBALANCE,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.SIZE,
        header=CsvHeader.SOLVER,
    )
    def __init__(self, dataframe: pd.DataFrame):
        solver_names = dataframe[CsvHeader.SOLVER.value].sort_values().unique()
        grid = html.Div(
            [GridElement(dataframe, config=self.graphConfig, header_value=solver_name) for solver_name in solver_names],
            className="grid"
        )
        grid_title = html.H3("Performance by solver")
        html.Div.__init__(self, [grid_title, grid])

class SizeGrid(html.Div):
    graphConfig= GraphConfig(
        x=CsvHeader.IMBALANCE,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.SOLVER,
        header=CsvHeader.SIZE,
    )
    def __init__(self, dataframe: pd.DataFrame):
        sizes = dataframe[CsvHeader.SIZE.value].sort_values(key=lambda x: x.astype(int)).unique()
        grid = html.Div(
            [GridElement(dataframe, config=self.graphConfig, header_value=size) for size in sizes],
            className="grid"
        )
        grid_title = html.H3("Performance by size")
        html.Div.__init__(self, [grid_title, grid])
