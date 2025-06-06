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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../inc/points_builder.h"
#include "../common_def.h"
#include "../lib/db.h"
#include <inttypes.h>

/**
 * Entry point.
 *
 * @param argc      Number of program arguments.
 * @param argv      Array of program arguments.
 *
 * @return          Program exit code.
 */
int main(int argc, char **argv) {
    const char * baseKey = NULL;
    U64 rangeSize = 0;
    U32 numLoopsPerLaunch = 1;
    U16 numThreads = 1;
    U32 progressMinInterval = 3;
    char *dbName = NULL;
    char *lookupX = NULL;
    int prime_only = 0;
    int triangular_only = 0;
    U64 moduloN = 0;
    int use_modulo = 0;

    int arg_idx = 1; // skip arg 0 = binary path

    // parse arguments
    while (arg_idx < argc) {
        char * arg = argv[arg_idx];

        if (arg[0] == '-' && arg[1] == '-') {
            if (strcmp(arg, "--prime-only") == 0) {
                prime_only = 1;
            } else if (strcmp(arg, "--triangular-only") == 0) {
                triangular_only = 1;
            } else if (strcmp(arg, "--modulo") == 0) {
                if (argc > arg_idx + 1) {
                    use_modulo = 1;
                    moduloN = strtoull(argv[++arg_idx], NULL, 10);
                } else {
                    fprintf(stderr, "--modulo requires a value\n");
                    return EXIT_FAILURE;
                }
            } else {
                fprintf(stderr, "Unknown argument %s\n", arg);
                return EXIT_FAILURE;
            }
        } else if (strlen(arg) == 2 && '-' == arg[0]) {
            switch (arg[1]) {
            case 'b':
                baseKey = argc > ++arg_idx ? argv[arg_idx] : NULL;
                break;
            case 's':
                rangeSize = argc > ++arg_idx ? strtoull(argv[arg_idx], NULL, 10) : 0;
                break;
            case 't':
                numThreads = argc > ++arg_idx ? strtoul(argv[arg_idx], NULL, 10) : 0;
                break;
            case 'p':
                progressMinInterval = argc > ++arg_idx ? strtoul(argv[arg_idx], NULL, 10) : 0;
                break;
            case 'n':
                numLoopsPerLaunch = argc > ++arg_idx ? strtoul(argv[arg_idx], NULL, 10) : 0;
                break;
            case 'o':
                dbName = argc > ++arg_idx ? argv[arg_idx] : NULL;
                break;
            case 'L':
                lookupX = argc > ++arg_idx ? argv[arg_idx] : NULL;
                break;
            default:
                fprintf(stderr, "Unknown argument %s\n", argv[arg_idx]);
                return EXIT_FAILURE;
            }
        } else {
            fprintf(stderr, "Unknown argument %s\n", arg);
            return EXIT_FAILURE;
        }

        ++arg_idx;
    }

    if (lookupX) {
        if (NULL == dbName) {
            fprintf(stderr, "DB name required for lookup\n");
            return EXIT_FAILURE;
        }
        U8 xbuf[32];
        if (strlen(lookupX) != 64) {
            fprintf(stderr, "X must be 32 bytes hex\n");
            return EXIT_FAILURE;
        }
        for (int i = 0; i < 32; i++) {
            sscanf(lookupX + i * 2, "%2hhx", &xbuf[i]);
        }
        db_open(dbName);
        U64 scalar;
        int err = db_lookup_scalar(xbuf, &scalar);
        db_close();
        if (err) {
            printf("Not found\n");
        } else {
            printf("Scalar: %" PRIu64 "\n", scalar);
        }
        return err ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    if (NULL == baseKey) {
        fprintf(stderr, "Base key is required.\n");
        return EXIT_FAILURE;
    }

    if (0 == rangeSize) {
        fprintf(stderr, "Range size cannot be 0\n");
        return EXIT_FAILURE;
    }

    if (0 == numThreads) {
        fprintf(stderr, "Num threads cannot be 0\n");
        return EXIT_FAILURE;
    }

    if (0 == numLoopsPerLaunch) {
        fprintf(stderr, "Loop count cannot be 0\n");
        return EXIT_FAILURE;
    }

    if (NULL == dbName) {
        printf("No DB name given - compute only mode\n");
    }

    scalar_filter_t filter = { prime_only, triangular_only, moduloN, use_modulo };

    int err = pointsBuilderGenerate(
        baseKey, rangeSize,
        numLoopsPerLaunch, numThreads, progressMinInterval,
        dbName, &filter
    );

    if (err) {
        fprintf(
            stderr,
            "[%d] Failed\n",
            err
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
