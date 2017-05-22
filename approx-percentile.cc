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

#include "approx-percentile.h"
#include "TDigest.h"

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>

using namespace impala_udf;

class Intermediate {
public:
  tdigest::TDigest digest {1000};
  double targetQuantile = -1.0;

  std::string serialize() {
    std::stringstream stream;
    {
      cereal::BinaryOutputArchive oa(stream);
      oa << *this;
    }
    return stream.str();
  }

  static Intermediate deserialize(std::string s) {
    Intermediate result;
    {
      std::stringstream stream(s);
      cereal::BinaryInputArchive ia(stream);
      ia >> result;
    }
    return result;
  }

private:
  friend class cereal::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & digest;
    ar & targetQuantile;
  }
};

double checkQuantileArg(FunctionContext* context, const DoubleVal& quantileArg) {
  if (quantileArg.is_null) {
    context->SetError("The percentile argument must not be null");
    return -1.0;
  }
  double targetQuantile = quantileArg.val;
  if (targetQuantile < 0.0 || targetQuantile > 1.0) {
    context->SetError("The percentile argument must be between 0.0 and 1.0");
    return -1.0;
  }
  return targetQuantile;
}

void ApproxPercentileInit(FunctionContext* context, StringVal* val) {
  // setup TDigest instance into aggregation function intermediate store
  auto intermediate = new Intermediate();

  val->is_null = false;
  val->len = sizeof(Intermediate*);
  val->ptr = (unsigned char *)intermediate;
}

void ApproxPercentileUpdate(FunctionContext* context, const DoubleVal& input, const DoubleVal& quantile, StringVal* val) {
  if (input.is_null) return;
  // add value to TDigest
  auto intermediate = reinterpret_cast<Intermediate*>(val->ptr);
  intermediate->digest.add(input.val);

  if (intermediate->targetQuantile == -1.0) {
    intermediate->targetQuantile = checkQuantileArg(context, quantile);
  }
}

void ApproxPercentileMerge(FunctionContext* context, const StringVal& src, StringVal* dst) {
  if (src.is_null) return;

  // deserialize data passed in by Impala to Intermediate
  std::string s(src.ptr, src.ptr+src.len);
  auto src_intermediate = Intermediate::deserialize(s);

  // merge into destination Intermediate
  auto dst_intermediate = reinterpret_cast<Intermediate*>(dst->ptr);
  dst_intermediate->digest.merge(&src_intermediate.digest);
  dst_intermediate->targetQuantile = src_intermediate.targetQuantile;
}

StringVal ApproxPercentileSerialize(FunctionContext* context, const StringVal& src) {
  // serialize TDigest into std string, and delete it afterwards
  auto src_intermediate = reinterpret_cast<Intermediate*>(src.ptr);
  src_intermediate->digest.compress();
  auto serialized = src_intermediate->serialize();
  delete src_intermediate;

  // create Impala StringVal from std string
  auto data = reinterpret_cast<const unsigned char*>(serialized.data());
  return StringVal::CopyFrom(context, data, serialized.length());
}

DoubleVal ApproxPercentileFinalize(FunctionContext* context, const StringVal& val) {
  // get user-provided quantile param: would be the way to go, but does not work in Finalize for some reason
  //  auto quantileParam = reinterpret_cast<DoubleVal*>(context->GetConstantArg(1));
  // retrieve approx percentile from Intermediate
  auto result_intermediate = reinterpret_cast<Intermediate*>(val.ptr);
  double quantile = result_intermediate->targetQuantile;
  double result = result_intermediate->digest.quantile(quantile);

  // delete the tdigest and return result
  delete result_intermediate;
  return DoubleVal(result);
}