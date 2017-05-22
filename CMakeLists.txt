# Copyright 2017 Julian Eberius
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 2.6)

# where to put generated libraries
set(LIBRARY_OUTPUT_PATH "build")
# where to put generated binaries
set(EXECUTABLE_OUTPUT_PATH "build")

find_program(CLANG_EXECUTABLE clang++)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb -std=c++11")

include_directories(cereal/include tdigest)

# Build the UDA/UDFs into a shared library.
add_library(approxpercentile SHARED approx-percentile.cc)

# Build test executable, links Impala UDF lib for test harness
add_executable(approx-percentile-test approx-percentile-test.cc approx-percentile.cc)
target_link_libraries(approx-percentile-test ImpalaUdf)
