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

#ifndef SAMPLES_UDA_H
#define SAMPLES_UDA_H

#include "impala_udf/udf.h"

using namespace impala_udf;

void ApproxPercentileInit(FunctionContext* context, StringVal* val);
void ApproxPercentileUpdate(FunctionContext* context, const DoubleVal& input, const DoubleVal& quantile, StringVal* val);
void ApproxPercentileMerge(FunctionContext* context, const StringVal& src, StringVal* dst);
StringVal ApproxPercentileSerialize(FunctionContext* context, const StringVal& src);
DoubleVal ApproxPercentileFinalize(FunctionContext* context, const StringVal& val);

#endif
