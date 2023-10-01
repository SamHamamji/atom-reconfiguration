from enum import Enum

class CsvHeader(Enum):
    IMBALANCE = "imbalance"
    SIZE = "size"
    SOLVER = "solver"
    TIME_TAKEN = "time_taken"

header_to_string: dict[CsvHeader, str] = {
    CsvHeader.IMBALANCE: "Imbalance",
    CsvHeader.SIZE: "Size",
    CsvHeader.SOLVER: "Solver",
    CsvHeader.TIME_TAKEN: "Time taken",
}