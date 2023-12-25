from enum import Enum


class RawCsvHeader(Enum):
    IMBALANCE = "imbalance_percentage"
    LENGTH = "length"
    SOLVER = "solver"
    TIME_TAKEN = "time_taken"


class CsvHeader(Enum):
    IMBALANCE_PERCENT = "Imbalance percentage"
    LENGTH = "Length"
    SOLVER = "Solver"
    TIME_TAKEN = "Time taken"


raw_csv_to_parsed: dict[str, str] = {
    RawCsvHeader.IMBALANCE.value: CsvHeader.IMBALANCE_PERCENT.value,
    RawCsvHeader.LENGTH.value: CsvHeader.LENGTH.value,
    RawCsvHeader.SOLVER.value: CsvHeader.SOLVER.value,
    RawCsvHeader.TIME_TAKEN.value: CsvHeader.TIME_TAKEN.value,
}
