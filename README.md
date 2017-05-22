# impala-approx-percentile-uda
An approximative percentile aggregation function for [Cloudera Impala](https://en.wikipedia.org/wiki/Cloudera_Impala) based on T-Digest

Cloudera Impala currently only includes an appx_median() aggregation function, but no general approximate percentile function. However, it exposes an C++ API for user-defined aggregates (UDAs). This project takes an existing [open source C++ implementation](https://github.com/tdunning/t-digest/) of the [T-Digest data structure](https://github.com/tdunning/t-digest), and fits it into the Impala UDA framework. It also adds [Cereal](https://github.com/USCiLab/cereal) for the serialization functionality required to pass partial results between nodes.
