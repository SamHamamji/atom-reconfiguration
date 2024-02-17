import pandas as pd

from CsvHeader import raw_csv_to_parsed


def compute_mean_for_duplicates(
    df: pd.DataFrame, kept_columns: list[str], target_column: str
) -> pd.DataFrame:
    for column in df.columns.drop([*kept_columns, target_column]):
        df = df.drop(column, axis=1)
    return df.groupby(kept_columns, sort=False).mean().reset_index()


def read_data(file_path: str):
    file_content = pd.read_csv(file_path)
    file_content = file_content.rename(columns=raw_csv_to_parsed)
    return file_content
