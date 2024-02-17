import argparse

from apps.linear_solver_app import LinearSolverApp
from apps.grid_solver_app import GridSolverApp
from data_processing import read_data
from DataFrameType import DataFrameType, get_dataframe_type

parser = argparse.ArgumentParser(
    description="Visualize linear solvers performance results",
    prog="visualize_performance",
)
parser.add_argument(
    "--file",
    type=str,
    help="Performance results file",
)
parser.add_argument(
    "-p",
    "--port",
    type=int,
    help="Port to run the server on",
    default=8050,
)

if __name__ == "__main__":
    args = parser.parse_args()
    dataframe = read_data(args.file)
    dataframe_type = get_dataframe_type(dataframe)

    if dataframe_type == DataFrameType.LINEAR_SOLVER:
        app = LinearSolverApp(dataframe)
    elif dataframe_type == DataFrameType.GRID_SOLVER:
        app = GridSolverApp(dataframe)

    app.run(debug=True, port=args.port)
