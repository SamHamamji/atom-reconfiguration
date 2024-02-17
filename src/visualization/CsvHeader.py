from enum import Enum


class RawCsvHeader(Enum):
    IMBALANCE_PERCENT = "imbalance_percentage"
    LENGTH = "length"
    LINEAR_SOLVER = "linear_solver"
    GRID_SOLVER = "grid_solver"
    TIME_TAKEN = "time_taken"
    WIDTH = "width"
    HEIGHT = "height"


class CsvHeader(Enum):
    IMBALANCE_PERCENT = "Imbalance percentage"
    LENGTH = "Length"
    LINEAR_SOLVER = "Linear solver"
    GRID_SOLVER = "Grid solver"
    TIME_TAKEN = "Time taken"
    WIDTH = "Width"
    HEIGHT = "Height"


raw_csv_to_parsed: dict[str, str] = {
    RawCsvHeader.IMBALANCE_PERCENT.value: CsvHeader.IMBALANCE_PERCENT.value,
    RawCsvHeader.LENGTH.value: CsvHeader.LENGTH.value,
    RawCsvHeader.LINEAR_SOLVER.value: CsvHeader.LINEAR_SOLVER.value,
    RawCsvHeader.GRID_SOLVER.value: CsvHeader.GRID_SOLVER.value,
    RawCsvHeader.TIME_TAKEN.value: CsvHeader.TIME_TAKEN.value,
    RawCsvHeader.WIDTH.value: CsvHeader.WIDTH.value,
    RawCsvHeader.HEIGHT.value: CsvHeader.HEIGHT.value,
}
