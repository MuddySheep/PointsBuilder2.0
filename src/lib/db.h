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

#ifndef POINTS_BUILDER_DB_H
#define POINTS_BUILDER_DB_H

#include "../common_def.h"

int db_open(
    const char * dbName
);

int db_close();

void db_insert_result(
    U64 keyOffset,
    const U64 * x,
    U8 yParity
);

int db_lookup_scalar(
    const U8 * x,
    U64 * scalar
);

#endif // POINTS_BUILDER_DB_H
