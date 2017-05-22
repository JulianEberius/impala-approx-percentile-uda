/*
 * Copyright 2017 Julian Eberius
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "impala_udf/uda-test-harness.h"
#include "impala_udf/udf-test-harness.h"
#include "approx-percentile.h"

using namespace std;

int main(int argc, char** argv) {
  typedef UdaTestHarness2<DoubleVal, StringVal, DoubleVal, DoubleVal> TestHarness;
  // Note: reinterpret_cast is required because pre-2.9 UDF headers had a spurious "const"
  // specifier in the return type for SerializeFn. It is unnecessary for 2.9+ headers.
  TestHarness test(ApproxPercentileInit, ApproxPercentileUpdate, ApproxPercentileMerge,
      reinterpret_cast<TestHarness::SerializeFn>(ApproxPercentileSerialize), ApproxPercentileFinalize);

  vector<DoubleVal> vals;
  vector<DoubleVal> quantile;

  // Test values
  for (int i = 0; i < 1001; ++i) {
    vals.push_back(DoubleVal(i));
    quantile.push_back(DoubleVal(0.5));
  }
  if (!test.Execute(vals, quantile, DoubleVal(500.0))) {
    cerr << "ApproxPercentile: " << test.GetErrorMsg() << endl;
  }
  cout << "Done." << endl;
}