__author__ = 'dimaxx'

import os

gas_num = 2
params = ["conc", "temp", "pressure"]
#params = ["conc"]

home_dir = '../build/out/'

for param in params:
  for gas in range(gas_num):
    data_folder = home_dir + 'gas' + '%i' % gas + '/' + param
    folder = data_folder + '/pic'

    for the_file in os.listdir(folder):
      file_path = os.path.join(folder, the_file)
      try:
          if os.path.isfile(file_path):
              os.unlink(file_path)
          elif os.path.isdir(file_path): shutil.rmtree(file_path)
      except Exception(e):
          print(e)