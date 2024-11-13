import requests
import zipfile
import os.path

def download_zipped_dataset(data_url, zip_file_path, local_path):
  if os.path.isfile(zip_file_path):
    return
  
  resp = requests.get(data_url)

  with open(zip_file_path, "wb") as f:
    f.write(resp.content)

  with zipfile.ZipFile(zip_file_path, 'r') as zip_ref:
    zip_ref.extractall(local_path)
