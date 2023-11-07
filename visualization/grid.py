import typing
import pandas as pd
import plotly.express as px

from dash import dcc, html
from CsvHeader import CsvHeader
from data_processing import compute_mean_for_duplicates

class GraphConfig(typing.NamedTuple):
    x: CsvHeader
    y: CsvHeader
    color: CsvHeader
    header: CsvHeader
    trendline: str | None = "lowess"
    title_generator: typing.Callable[[str], str] = lambda x: x

class GridElement(html.Div):
    def __init__(self, dataframe: pd.DataFrame, config: GraphConfig, header_value: str):
        value_data = dataframe[dataframe[config.header.value] == header_value]
        figure = px.scatter(
            value_data,
            x = config.x.value,
            y = config.y.value,
            color = value_data[config.color.value],
            color_continuous_scale=px.colors.sequential.Turbo_r,
            trendline=config.trendline,
            trendline_options={"frac":0.3},
        )
        html.Div.__init__(self, [
            html.H3(f"{config.header.value}: {config.title_generator(header_value)}"),
            dcc.Graph(figure=figure),
        ], className="grid-item")

class SolverGrid(html.Div):
    graphConfig= GraphConfig(
        x=CsvHeader.IMBALANCE_PERCENT,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.SIZE,
        header=CsvHeader.SOLVER,
    )
    def __init__(self, dataframe: pd.DataFrame):
        solver_names = dataframe[CsvHeader.SOLVER.value].sort_values().unique()

        new_dataframe = dataframe.copy()
        new_dataframe.sort_values(by=CsvHeader.SIZE.value, ascending=False, inplace=True)
        new_dataframe[CsvHeader.SIZE.value] = new_dataframe[CsvHeader.SIZE.value].astype(str)

        grid = html.Div(
            [GridElement(new_dataframe, self.graphConfig, solver_name) for solver_name in solver_names],
            className="grid"
        )
        grid_title = html.H3("Performance by solver")
        html.Div.__init__(self, [grid_title, grid])

class HeatMapGrid(html.Div):
    graphConfig= GraphConfig(
        x=CsvHeader.SIZE,
        y=CsvHeader.IMBALANCE_PERCENT,
        color=CsvHeader.TIME_TAKEN,
        header=CsvHeader.SOLVER,
        trendline=None,
    )
    def __init__(self, dataframe: pd.DataFrame):
        solver_names = dataframe[CsvHeader.SOLVER.value].sort_values().unique()

        grid = html.Div(
            [GridElement(dataframe, self.graphConfig, solver_name) for solver_name in solver_names],
            className="grid"
        )
        grid_title = html.H3("Performance map by solver")
        html.Div.__init__(self, [grid_title, grid])

class SizeGrid(html.Div):
    graphConfig= GraphConfig(
        x=CsvHeader.IMBALANCE_PERCENT,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.SOLVER,
        header=CsvHeader.SIZE,
    )
    def __init__(self, dataframe: pd.DataFrame):
        sizes = dataframe[CsvHeader.SIZE.value].sort_values(key=lambda x: x.astype(int)).unique()

        grid = html.Div(
            [GridElement(dataframe, self.graphConfig, size) for size in sizes],
            className="grid"
        )
        grid_title = html.H3("Performance by size")
        html.Div.__init__(self, [grid_title, grid])

class ImbalanceGrid(html.Div):
    graphConfig = GraphConfig(
        x=CsvHeader.SIZE,
        y=CsvHeader.TIME_TAKEN,
        color=CsvHeader.SOLVER,
        header=CsvHeader.IMBALANCE_PERCENT,
        title_generator=lambda x: f"{x}%",
    )
    def __init__(self, dataframe: pd.DataFrame):
        dataframe[CsvHeader.IMBALANCE_PERCENT.value] = dataframe[CsvHeader.IMBALANCE_PERCENT.value].apply(round) # type: ignore
        dataframe = compute_mean_for_duplicates(dataframe)
        imbalance_percentages = dataframe[CsvHeader.IMBALANCE_PERCENT.value].unique()

        grid = html.Div(
            [GridElement(dataframe, self.graphConfig, imbalance) for imbalance in imbalance_percentages],
            className="grid"
        )
        html.Div.__init__(self, [html.H3("Performance by imbalance:"), grid])

Grid = typing.Union[HeatMapGrid, ImbalanceGrid, SolverGrid, SizeGrid]