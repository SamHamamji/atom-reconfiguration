import argparse

from app import PerformanceVisualizationApp
from data_processing import read_data
from grid import HeatMapGrid, ImbalanceGrid, SolverGrid, SizeGrid

parser = argparse.ArgumentParser(
    description="Visualize linear assignment solvers performance results",
    prog="visualize_performance"
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

if __name__== "__main__":
    args = parser.parse_args()
    dataframe = read_data(args.file)
    app = PerformanceVisualizationApp(dataframe, [
        # HeatMapGrid,
        # ImbalanceGrid,
        SolverGrid,
        # SizeGrid,
    ])
    app.run(debug=True, port=args.port)
