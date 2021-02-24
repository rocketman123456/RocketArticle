#!/bin/bash
find . -path ./Thirdparty -prune -o \( -name "*.cpp" -o -name "*.c" -o -name "*.mm" -o -name "*.h" -o -name "*.hpp" \) -exec clang-format -i --style=file {} +;