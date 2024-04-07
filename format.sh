#!/bin/bash
folder="src tests"
exclude_folder=./submodule
format_files=`find ${folder} -type f -name "*.cpp" -o -name "*.h"`

for file in $format_files
do
  clang-format -i "$file"
done
