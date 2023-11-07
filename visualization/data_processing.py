import pandas as pd

from CsvHeader import CsvHeader, raw_csv_to_parsed

def compute_mean_for_duplicates(dataframe: pd.DataFrame) -> pd.DataFrame:
    return dataframe.groupby(
        [CsvHeader.IMBALANCE_PERCENT.value, CsvHeader.SOLVER.value, CsvHeader.SIZE.value],
    ).mean().reset_index()

def convert_imbalance_to_percentage(dataframe: pd.DataFrame):
    dataframe[CsvHeader.IMBALANCE_PERCENT.value] = (
        100 * dataframe[CsvHeader.IMBALANCE_PERCENT.value] / dataframe[CsvHeader.SIZE.value]
    )

def read_data(file_path: str):
    file_content = pd.read_csv(file_path)
    file_content = file_content.rename(columns=raw_csv_to_parsed)
    convert_imbalance_to_percentage(file_content)
    return compute_mean_for_duplicates(file_content)
