import pandas as pd

from CsvHeader import CsvHeader, raw_csv_to_parsed


def compute_mean_for_duplicates(
    dataframe: pd.DataFrame, grouped_column: str
) -> pd.DataFrame:
    return (
        dataframe.groupby(dataframe.columns.drop(grouped_column).to_list(), sort=False)
        .mean()
        .reset_index()
    )


def read_data(file_path: str):
    file_content = pd.read_csv(file_path)
    file_content = file_content.rename(columns=raw_csv_to_parsed)
    return compute_mean_for_duplicates(file_content, CsvHeader.TIME_TAKEN.value)
