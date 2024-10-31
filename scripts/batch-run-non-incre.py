import os
import subprocess       # 调用系统命令
import time
import shutil

benchmark_path = 'bench-regression//'
result_file = 'result-yogar-cbmc.txt'
yogar_cbmc = 'bin/yogar-cbmc'
timeoutT = 600

# 初始化临时文件夹
def init_tmp_files(base_path):
	if os.path.exists(base_path + result_file):
		os.unlink(base_path + result_file)
	folder_path = base_path + 'kernel-source-tmp/'
	for filename in os.listdir(folder_path):
		file_path = os.path.join(folder_path, filename)
		if os.path.isfile(file_path):
			os.unlink(file_path)

# 增量式运行base_path下的所有.i文件
def run_benchmark(base_path):
    for root, dirs, files in os.walk(base_path):
        # 遍历文件夹下所有.i文件
        count = 0
        for file in files:
            if file.endswith('.i') or file.endswith('.c'):
                filepath = os.path.join(root, file)
                
                # 将测试用例名称和运行结果输出到result.txt文件中
                count = count + 1
                with open(result_file, 'a') as f:
                    f.write(filepath + ' ')

                # filepath是xx_v1.cpp，将其改名为xx.cpp
                new_filepath = filepath[:-5] + '.i' if file.endswith('.i') else filepath[:-5] + '.c'

                # 复制 filepath 到 new_filepath
                shutil.copy(filepath, new_filepath)
                
                try:
                    # 运行测试用例，设置超时时间为600秒
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

    # 初始化临时文件夹
    init_tmp_files("./")

    # 增量式运行base_path下的所有.i文件
    run_benchmark(benchmark_path)

if __name__ == "__main__":

    main()
