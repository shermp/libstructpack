/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 Sherman Perry
 *
 * SPDX-License-Identifier: MIT
 */

#include <structpack.h>
#include "sp_test.h"

struct s_test {
    int mem1;
    int mem2;
    int mem3;
    int mem4;
    int mem5;
    int mem6;
    int mem7;
    int mem8;
    int mem9;
    int mem10;
    int mem11;
    int mem12;
    int mem13;
    int mem14;
    int mem15;
    int mem16;
};

static int rv = 0;

int main(void) {
    size_t offset_arr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    SP_ADD_STRUCT_OFFSET(offset_arr, 0, struct s_test, mem1, mem2, mem3, mem4, mem5, mem6, mem7, mem8, mem9, mem10, mem11, mem12, mem13, mem14, mem15, mem16);
    for (size_t i = 0; i < 16; ++i) {
        SP_TEST_ASSERT(rv, offset_arr[i] == (sizeof(int) * i), "test SP_ADD_STRUCT_OFFSET macro with 16 args");
    }

    return rv;
}
