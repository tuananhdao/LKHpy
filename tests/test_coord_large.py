import numpy as np
import json
import LKHpy as LK
import pytest
from download_dataset import download_zipped_dataset

DATA_URL = "https://huggingface.co/datasets/tuananhdao/lkhpy-test-data/resolve/main/dataset1.zip?download=true"
ZIP_FILE_PATH = "data/dataset1.zip"
LOCAL_PATH = "data"
TEST_FILES = [
  ("nodes_3000_0.json", 1),
  ("nodes_4000_0.json", 7),
  ("nodes_3000_1.json", 1),
  ("nodes_2000_0.json", 4),
  ("nodes_4000_1.json", 5),
]
SHOW_OUTPUT = False

def test_coord_large():
  # Download dataset if not exists
  download_zipped_dataset(DATA_URL, ZIP_FILE_PATH, LOCAL_PATH)

  # The bug usually appears in a loop
  for test_data, salesmen in TEST_FILES:    
    with open(LOCAL_PATH + "/" + test_data, 'r') as json_file:
        data_list = json.load(json_file)
    data_array = np.array(data_list)

    params = {
      '#SHOW_OUTPUT': SHOW_OUTPUT,
      'SPECIAL': '',
      'MTSP_OBJECTIVE': 'MINMAX',
      'SALESMEN': salesmen,
      'RUNS': 1,
      'TOTAL_TIME_LIMIT': 7,
      'MAX_CANDIDATES': 6,
      'MAX_TRIALS': 10000,
      'PRECISION': 100
    }

    # test geom()
    solution = LK.geom(data_array, params)
    assert len(solution) > 0

    # test geo()
    solution = LK.geo(data_array, params)
    assert len(solution) > 0

if __name__ == "__main__":
  SHOW_OUTPUT = True
  test_coord_large()
