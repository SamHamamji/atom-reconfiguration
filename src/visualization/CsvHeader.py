from enum import Enum


class RawCsvHeader(Enum):
    IMBALANCE_PERCENT = "imbalance_percentage"
    LENGTH = "length"
    LINEAR_SOLVER = "linear_solver"
    TIME_TAKEN = "time_taken"


class CsvHeader(Enum):
    IMBALANCE_PERCENT = "Imbalance percentage"
    LENGTH = "Length"
    LINEAR_SOLVER = "Linear solver"
    TIME_TAKEN = "Time taken"


raw_csv_to_parsed: dict[str, str] = {
    RawCsvHeader.IMBALANCE_PERCENT.value: CsvHeader.IMBALANCE_PERCENT.value,
    RawCsvHeader.LENGTH.value: CsvHeader.LENGTH.value,
    RawCsvHeader.LINEAR_SOLVER.value: CsvHeader.LINEAR_SOLVER.value,
    RawCsvHeader.TIME_TAKEN.value: CsvHeader.TIME_TAKEN.value,
}
