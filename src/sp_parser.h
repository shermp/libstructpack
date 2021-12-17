/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 Sherman Perry
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SP_PARSER_H
#define SP_PARSER_H

#include <structpack.h>
#include "sp_internal.h"

struct fmt_str_parser {
    const char* fmt_str;
    const char* curr_pos;
    enum sp_endian endian;
    /* Make a little array based stack to handle nested groups */
    struct {
        int depth;
        int repeat[SP_MAX_GRP_DEPTH];
        const char* start[SP_MAX_GRP_DEPTH];
    } groups;
    struct {
        int arr_len;
        int repeat;
        char type;
    } current;
};

struct fmt_str_parser new_parser(const char* fmt_str, SPResult* err);
void reset_parser(struct fmt_str_parser* parser);
SPResult validate_format_str(const char* format_str);
SPResult parse_next(struct fmt_str_parser* parser);

#endif // SP_PARSER_H
