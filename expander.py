import os
import sys
import re

processed_files = set()
collected_system_includes = set()

include_regex = re.compile(r'^\s*#\s*include\s+<([^>]+)>')
user_include_regex = re.compile(r'^\s*#\s*include\s+"([^"]+)"')

def strip_comments_and_pragmas(code: str) -> str:
    code = re.sub(r'//.*', '', code)
    code = re.sub(r'/\*.*?\*/', '', code, flags=re.DOTALL)
    code = re.sub(r'^\s*#\s*pragma\s+once\s*$', '', code, flags=re.MULTILINE)
    code = re.sub(r'\n\s*\n+', '\n', code)
    return code.strip() + '\n'

def extract_system_includes(lines):
    remaining = []
    for line in lines:
        match = include_regex.match(line)
        if match:
            collected_system_includes.add(match.group(1))
        else:
            remaining.append(line)
    return remaining

def expand_file(path: str, is_root: bool = False) -> str:
    abs_path = os.path.abspath(path)
    if not is_root and abs_path in processed_files:
        return ''

    if not is_root:
        processed_files.add(abs_path)

    with open(path, 'r') as f:
        lines = f.readlines()

    # Extract system includes
    lines = extract_system_includes(lines)

    output = []
    for line in lines:
        match = user_include_regex.match(line)
        if match:
            include_path = match.group(1)
            include_full = os.path.join(os.path.dirname(path), include_path)
            if os.path.exists(include_full):
                output.append(expand_file(include_full, is_root=False))
        else:
            output.append(line)

    code = ''.join(output)
    return code if is_root else strip_comments_and_pragmas(code)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python expander.py <input_file.cpp> [output_file.cpp]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) >= 3 else "expanded.cpp"

    expanded = expand_file(input_file, is_root=True)

    # Final formatting: deduplicated includes at the top
    includes_block = ''.join(sorted(f'#include <{h}>\n' for h in collected_system_includes))
    final_output = includes_block + '\n' + expanded

    with open(output_file, 'w') as out:
        out.write(final_output)

    print(f"Expanded and saved to {output_file}")
