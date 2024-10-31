import os
import random
import re
import shutil
import time
from openai import OpenAI

def generate_random_number():
    return random.randint(3, 40)

def read_nth_last_line(file_path, n):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        if n <= len(lines):
            return lines[-n].strip(), len(lines) - n
        return None, None

def is_non_substantial_code(line):
    line = line.strip()
    if not line or line == '{' or line == '}':
        return True
    
    if re.match(r"^[\w\s\*]+\s+\w+\s*\(.*\)\s*{", line):
        return True
    
    if re.match(r"^\s*(if|for|while|switch)\s*\(.*\)\s*{?", line):
        return True

    if re.match(r"^\s*\w+\s*\(.*\)\s*;$", line):
        return True

    if line.startswith("extern") or line.startswith("typedef"):
        return True

    if not line.endswith(";"):
        return True

    return False

def code_generation_client(input_code, client, model_name):
    print(f"--{model_name} start code generation")

    prompt = """Make a small change to the following single line of C code by modifying or adding part of the line to alter its logic or behavior. Ensure that the modified line still compiles and behaves correctly. Provide only the modified line of code, with a brief comment explaining the change, and enclose the code in a ```c``` code block:\n\n"""

    for i in range(5):
        try:
            response = client.chat.completions.create(
                model=model_name,
                messages=[
                    {"role": "system", "content": "You are a smart programming assistant capable of generating code accurately based on user prompts."},
                    {"role": "user", "content": prompt + input_code},
                ],
                stream=False
            )
            
            content = response.choices[0].message.content
            pattern = r'```c(.*?)```'
            match = re.search(pattern, content, re.DOTALL)

            print(f"##{model_name} finished code generation")

            if match:
                return match.group(1).strip()
            else:
                print(f"Could not find a valid code block in {model_name}'s response.")
                return None

        except Exception as e:
            print(f"{model_name} code generation failed, error: {e}")
            print("Retrying after 2 seconds")
            time.sleep(2)

def code_generation_deepseek(problem):
    client = OpenAI(api_key="sk-af0f584d9df14565becade6c8999e4a1", base_url="https://api.deepseek.com")
    return code_generation_client(problem, client, "deepseek-chat")

def code_generation_qwen(problem):
    client = OpenAI(
        api_key="sk-93a1db5eacb543688353d9c03ba32170", 
        base_url="https://dashscope.aliyuncs.com/compatible-mode/v1"
    )
    return code_generation_client(problem, client, "qwen-coder-turbo-latest")

def modify_code_with_model(line):
    print(line)
    modified_line = code_generation_deepseek(line)
    return modified_line

def replace_line_in_file(file_path, line_number, modified_line):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    
    if 0 <= line_number < len(lines):
        lines[line_number] = modified_line + "\n"
    
    with open(file_path, 'w') as file:
        file.writelines(lines)

def create_modified_file(base_file, version, output_folder):
    base_name = re.sub(r"_v\d+$", "", os.path.basename(base_file).split('.')[0])
    new_file = os.path.join(output_folder, f"{base_name}_v{version}.i")
    shutil.copy(base_file, new_file)
    return new_file

def process_file(file_path, output_folder):
    base_file = file_path
    for version in range(1, 4):
        modified_file = create_modified_file(base_file, version, output_folder)
        replaced_lines = set()
        attempts = 0

        while len(replaced_lines) < 1 and attempts < 10:  
            n = generate_random_number()
            line, line_number = read_nth_last_line(modified_file, n)

            if line_number in replaced_lines:
                continue

            while line is not None and is_non_substantial_code(line):
                n -= 1
                line, line_number = read_nth_last_line(modified
