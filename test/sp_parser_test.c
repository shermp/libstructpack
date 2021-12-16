/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 John Smith
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

#include "../src/sp_parser.h"

static int rv = 0;

#define SP_TEST_ASSERT(cond, str) if ((cond)) {        \
        printf("[PASS] %s\n", (str));                  \
    } else {                                           \
        printf("[FAIL] %s\n", (str));                  \
        rv = 1;                                        \
    }                                                  

int main(void) {
    const char *fmt_str;
    /* Good format string */
    fmt_str = ">4s h i 4(4B Q)";
    SP_TEST_ASSERT(validate_format_str(fmt_str) == SP_OK, "parse good format string");

    /* Invalid chars */
    fmt_str = ">4s h i 4(4P Q)";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "parse invalid character");
    
    /* Check invalid endian char location */
    fmt_str = "4s >h i 4(4B Q)";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "parse invalid endian location");
    
    /* Test beginning with '(' */
    fmt_str = "(4s) h i 4(4B Q)";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "parse invalid opening '('");
    
    /* Test invalid ending character */
    fmt_str = ">4s h i 4(4B Q)]";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "pare invalid end character");
    
    /* Test max depth detection */
    fmt_str = ">4s h i 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q 4(4B Q))))))))))";
    SP_TEST_ASSERT(validate_format_str(fmt_str) != SP_OK, "parse beyond max group depth");
    
    return rv;
}
