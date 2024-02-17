import pandas as pd
from dash import Dash, html, Output, Input, dcc


class App(Dash):

    def __init__(
        self, dataframe: pd.DataFrame, tab_content_types: dict[str, type[html.Div]]
    ):
        self.tab_content_types = tab_content_types
        self.tabs = dcc.Tabs(
            id="tabs",
            value=self.tab_content_types.keys().__iter__().__next__(),
            children=list(
                map(
                    lambda name: dcc.Tab(label=name, value=name),
                    self.tab_content_types.keys(),
                )
            ),
        )

        Dash.__init__(self, __name__, assets_folder="../assets")
        self.layout = html.Div(
            [self.tabs, html.Div(id="tab-content")],
            className="app",
        )

        self.title = "Performance results"

        @self.callback(
            Output("tab-content", "children"),
            Input("tabs", "value"),
        )
        def switch_tab(tab_content_type):
            return [
                self.tab_content_types[tab_content_type](dataframe),
            ]
