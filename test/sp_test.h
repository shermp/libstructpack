/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 Sherman Perry
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SP_TEST_H
#define SP_TEST_H

#include <stdio.h>

#define SP_TEST_ASSERT(ret, cond, str) if ((cond)) {        \
        printf("[PASS] %s\n", (str));                  \
    } else {                                           \
        printf("[FAIL] %s\n", (str));                  \
        rv = 1;                                        \
    }             

#endif
