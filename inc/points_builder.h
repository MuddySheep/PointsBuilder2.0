/* Copyright 2025 kTimesG

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef POINTS_BUILDER_H
#define POINTS_BUILDER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Public API
// Don't include or use other lib headers here.

typedef struct scalar_filter_s {
    int prime_only;
    int triangular_only;
    U64 modulo;
    int use_modulo;
} scalar_filter_t;

int pointsBuilderGenerate(
    const char * baseKey,
    uint64_t rangeSize,
    uint32_t numLoopsPerThread,
    uint16_t numThreads,
    uint32_t progressMinInterval,
    const char * dbName,
    const scalar_filter_t * filter
);

#ifdef __cplusplus
}
#endif
#endif // POINTS_BUILDER_H
