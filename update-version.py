#
# This script updates the version number and dates in the source code.
# Arguments:
#   --version x.y.z  The new version number
#   --date DD.MM.YY  The new date
#

import re
import pathlib
import argparse

# Parse command line arguments
parser = argparse.ArgumentParser()
parser.add_argument('--version', help='The new version number', required=True)
parser.add_argument('--date', help='The new date', required=True)
args = parser.parse_args()
# Get the current directory
current_dir = pathlib.Path(__file__).parent.absolute()

def replace_in_file(filepath: pathlib.Path, frompattern: str, topattern: str, regex_flags=0):
    regex = re.compile(frompattern, regex_flags)
    with open(filepath, 'r+', newline='\n') as file:
        file_lines = file.readlines()
        for i in range(len(file_lines)):
            result_line = regex.sub(topattern, file_lines[i])
            if (result_line != file_lines[i]):
                file_lines[i] = result_line
                print(f'line {i+1} in {filepath.relative_to(current_dir)}')
        file.seek(0)
        file.writelines(file_lines)
        file.truncate()

# update the version number and date in *.h, *.cpp, *.skeleton source files
files = []
files += list(current_dir.glob('**/*.h'))
files += list(current_dir.glob('**/*.cpp'))
files += list(current_dir.glob('**/*.skeleton'))
for file in files:
    replace_in_file(file, r'(?<=\*\*  Copyright \(C\) 2011-20)\d\d', args.date[-2:])
for file in files:
    replace_in_file(file, r'(?<=\*\*             Date: )\d+\.\d+\.\d+', args.date)
for file in files:
    replace_in_file(file, r'(?<=\*\*          Version: )\d+\.\d+\.\d+', args.version)

# update the copyright year in the documentation HTML header
replace_in_file(current_dir/'documentation'/'header.html', r'(?<=<meta name="copyright" content="\(C\) 2011-20)\d\d', args.date[-2:])

# update the version in the main documentation file:
replace_in_file(current_dir/'src'/'doc-mainpage.dox', r'(?<=\\mainpage %QCustomPlot )\d+.\d+.\d+', args.version)

# update the version defines:
replace_in_file(current_dir/'src'/'global.h', r'(?<=QCUSTOMPLOT_VERSION_STR ")\d+.\d+.\d+', args.version)
v_nums = args.version.split('.')
replace_in_file(current_dir/'src'/'global.h', r'(?<=QCUSTOMPLOT_VERSION 0x)\d{6}', f'{int(v_nums[0]):02}{int(v_nums[1]):02}{int(v_nums[2]):02}')

# update .pro file versions:
replace_in_file(current_dir/'src'/'qcp-staticlib.pro', r'(?<=VERSION = )\d+.\d+.\d+', args.version)
replace_in_file(current_dir/'sharedlib'/'sharedlib-compilation'/'sharedlib-compilation.pro', r'(?<=VERSION = )\d+.\d+.\d+', args.version)
replace_in_file(current_dir/'sharedlib'/'sharedlib-usage'/'sharedlib-usage.pro', r'(?<=QCPLIB = qcustomplotd)\d', v_nums[0])
replace_in_file(current_dir/'sharedlib'/'sharedlib-usage'/'sharedlib-usage.pro', r'(?<=QCPLIB = qcustomplot)\d', v_nums[0])

