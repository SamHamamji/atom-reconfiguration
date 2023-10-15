import argparse
import pandas as pd

from CsvHeader import CsvHeader
from app import PerformanceVisualizationApp

def read_data(file_path: str):
    file_content = pd.read_csv(file_path).groupby(
        [CsvHeader.IMBALANCE.value, CsvHeader.SOLVER.value, CsvHeader.SIZE.value]
    ).mean().reset_index()
    return file_content

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
    app = PerformanceVisualizationApp(dataframe)
    app.run(debug=True, port=args.port)
