from enum import Enum

import pandas as pd

from CsvHeader import CsvHeader


class DataFrameType(Enum):
    LINEAR_SOLVER = "linear_solver"
    GRID_SOLVER = "grid_solver"


columns_per_dataframe_type: dict[DataFrameType, list[str]] = {
    DataFrameType.LINEAR_SOLVER: [
        CsvHeader.LINEAR_SOLVER.value,
        CsvHeader.INTERVAL_LENGTH.value,
        CsvHeader.IMBALANCE_PERCENT.value,
        CsvHeader.TIME_TAKEN.value,
    ],
    DataFrameType.GRID_SOLVER: [
        CsvHeader.GRID_SOLVER.value,
        CsvHeader.WIDTH.value,
        CsvHeader.HEIGHT.value,
        CsvHeader.IMBALANCE_PERCENT.value,
        CsvHeader.TIME_TAKEN.value,
    ],
}


def get_dataframe_type(dataframe: pd.DataFrame) -> DataFrameType:
    if dataframe.columns.equals(
        pd.Index(columns_per_dataframe_type[DataFrameType.LINEAR_SOLVER])
    ):
        return DataFrameType.LINEAR_SOLVER

    if dataframe.columns.equals(
        pd.Index(columns_per_dataframe_type[DataFrameType.GRID_SOLVER])
    ):
        return DataFrameType.GRID_SOLVER

    raise ValueError("Dataframe does not match any known type")
