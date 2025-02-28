import os
import subprocess       
import time
import shutil

benchmark_path = '..//benchmarks//EX1//bench-regression//'
result_file = 'EX1-result-yogar-cbmc-inc.txt'
yogar_cbmc = '..//bin//Yogar-CBMC-Inc'
timeoutT = 600


def init_tmp_files(base_path):
	if os.path.exists(base_path + result_file):
		os.unlink(base_path + result_file)
	folder_path = base_path + 'kernel-source-tmp/'
	for filename in os.listdir(folder_path):
		file_path = os.path.join(folder_path, filename)
		if os.path.isfile(file_path):
			os.unlink(file_path)


def run_benchmark(base_path):
    for root, dirs, files in os.walk(base_path):
        count = 0
        for file in files:
            if file.endswith('.i') or file.endswith('.c'):
                filepath = os.path.join(root, file)
                
                count = count + 1
                with open(result_file, 'a') as f:
                    f.write(filepath + ' ')

                new_filepath = filepath[:-5] + '.i' if file.endswith('.i') else filepath[:-5] + '.c'

                shutil.copy(filepath, new_filepath)
                
                try:    
                    result = subprocess.run([yogar_cbmc, new_filepath], timeout=timeoutT)            
                    
                except subprocess.TimeoutExpired:
                    with open(result_file, 'a') as f:
                        f.write('Timeout')
                except Exception as e:
                    with open(result_file, 'a') as f:
                        f.write('Error')

                with open(result_file, 'a') as f:
                        f.write('\n')

                os.unlink(new_filepath)

def main():   

    init_tmp_files("./")

    run_benchmark(benchmark_path)

if __name__ == "__main__":

    main()
