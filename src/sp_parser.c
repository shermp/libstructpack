/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 Sherman Perry
 *
 * SPDX-License-Identifier: MIT
 */

#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "sp_parser.h"

static const char* advance_fmt_str(const char** c);

static bool is_fmt_char(char fmt) {
    switch (fmt) {
        case 'x':
        case 'b':
        case 'B':
        case 'h':
        case 'H':
        case 'i':
        case 'I':
        case 'q':
        case 'Q':
        case 's':
        case 'w':
        case 'u':
            return true;
        default:
            return false;
    }
}

static bool is_endian_char(char end) {
    if (end == '<' || end == '>') {
        return true;
    }
    return false;
}

static bool is_arr_char(char arr) {
    if (arr == '[' || arr == ']') {
        return true;
    }
    return false;
}

static bool is_group_char(char grp) {
    if (grp == '(' || grp == ')') {
        return true;
    }
    return false;
}

static bool is_digit_char(char digit) {
    if (digit >= '0' && digit <= '9') {
        return true;
    }
    return false;
}

static bool is_whitespace_char(char ws) {
    if (ws == ' ' || ws == '\t') {
        return true;
    }
    return false;
}

const char* advance_fmt_str(const char** c) {
    *c += 1;
    while (is_whitespace_char(**c)) {
        *c += 1;
    }
    return *c;
}

struct fmt_str_parser new_parser(const char* fmt_str, SPResult* err) {
    *err = SP_OK;
    struct fmt_str_parser parser = {0};
    if (!fmt_str || fmt_str[0] == '\0') {
        *err = SP_NULL_CHAR;
        return parser;
    }
    parser.fmt_str = fmt_str;
    reset_parser(&parser);
    return parser;
}

void reset_parser(struct fmt_str_parser* parser) {
    parser->curr_pos = parser->fmt_str;
    parser->endian = SP_BIG_ENDIAN;
    if (parser->curr_pos[0] == '>') {
        parser->endian = SP_BIG_ENDIAN;
        advance_fmt_str(&parser->curr_pos);
    } else if (parser->curr_pos[0] == '<') {
        parser->endian = SP_LITTLE_ENDIAN;
        advance_fmt_str(&parser->curr_pos);
    }
    parser->current.type = '\0';
    parser->current.arr_len = 0;
}

SPResult validate_format_str(const char* format_str) {
    size_t fmt_len = strlen(format_str);
    size_t i;
    /* Check for invalid characters */
    char fmt;
    for (i = 0; i < fmt_len; i++) {
        fmt = format_str[i];
        if (!(is_fmt_char(fmt) || is_endian_char(fmt) || is_arr_char(fmt) || is_group_char(fmt) || is_digit_char(fmt) || is_whitespace_char(fmt))) {
            return SP_ERR_INVALID_FMT_STR;
        }
    }
    /* Check that endian character only at beginning */
    for (i = 0; i < fmt_len; i++) {
        if (is_endian_char(format_str[i]) && i > 0) {
            return SP_ERR_INVALID_FMT_STR;
        }
    }
    /* Check the string doesn't start with a '(' */
    if (format_str[0] == '(') {
        return SP_ERR_INVALID_FMT_STR;
    }
    /* Make sure the format string ends with a valid character */
    fmt = format_str[fmt_len - 1];
    if (fmt != ')' && !is_fmt_char(fmt) && !is_whitespace_char(fmt)) {
        return SP_ERR_INVALID_FMT_STR;
    }
    /* Ensure groups are balanced and do not exceed maximum depth */
    int depth = 0;
    for (i = 0; i < fmt_len; i++) {
        fmt = format_str[i];
        if (depth >= SP_MAX_GRP_DEPTH) {
            return SP_ERR_INVALID_FMT_STR;
        }
        if (fmt == '(') {
            depth++;
        } else if (fmt == ')') {
            depth--;
        }
        /* if depth < 0, it means we started with a closing parenthesis */
        if (depth < 0) {
            return SP_ERR_INVALID_FMT_STR;
        }
    }
    if (depth != 0) {
        return SP_ERR_INVALID_FMT_STR;
    }
    /* Ensure correct array syntax */
    bool arr_open = false;
    for (i = 0; i < fmt_len; i++) {
        fmt = format_str[i];
        if ((fmt == ']' && !arr_open) || (fmt == '[' && arr_open)) {
            return SP_ERR_INVALID_FMT_STR;
        }
        if (fmt == '[') {
            arr_open = true;
        }
        if (fmt == ']') {
            arr_open = false;
        }
    }
    /* Ensure no spaces between digits */
    const char *tmp_fmt;
    for (i = 0; i < fmt_len; i++) {
        fmt = format_str[i];
        if (is_digit_char(fmt) && is_whitespace_char(format_str[i + 1])) {
            tmp_fmt = (format_str + i);
            advance_fmt_str(&tmp_fmt);
            if (is_digit_char(*tmp_fmt)) {
                return SP_ERR_INVALID_FMT_STR;
            }
        }
    }

    return SP_OK;
}

SPResult parse_next(struct fmt_str_parser* parser) {
    char *end_pos;
    long num;
    if (parser->curr_pos[0] == '\0') {
        return SP_NULL_CHAR;
    }
    if (parser->current.repeat > 0) {
        parser->current.repeat--;
    } else if (is_fmt_char(parser->curr_pos[0])) {
        parser->current.type = parser->curr_pos[0];
        parser->current.arr_len = 0;
        parser->current.repeat = 0;
        advance_fmt_str(&parser->curr_pos);
    } else if (parser->curr_pos[0] == '[') {
        num = strtol(advance_fmt_str(&parser->curr_pos), &end_pos, 10);
        if (is_whitespace_char(*end_pos)) {
            advance_fmt_str((const char**)&end_pos);
        }
        if (num > INT_MAX || *end_pos != ']' || !is_fmt_char(*advance_fmt_str((const char**)&end_pos))) {
            return SP_ERR_INVALID_ARR;
        }
        parser->curr_pos = (const char*)end_pos;
        parser->current.arr_len = (int)num;
        parser->current.repeat = 0;
        parser->current.type = parser->curr_pos[0];
        advance_fmt_str(&parser->curr_pos);
    } else if (is_digit_char(parser->curr_pos[0])) {
        num = strtol(parser->curr_pos, &end_pos, 10);
        if (num > INT_MAX) {
            return SP_ERR_INT;
        }
        if (is_whitespace_char(*end_pos)) {
            advance_fmt_str((const char**)&end_pos);
        }
        /* Strings are special. They are always treated as arrays */
        if (*end_pos == 's' || *end_pos == 'w' || *end_pos == 'u') {
            parser->curr_pos = (const char*)end_pos;
            parser->current.repeat = 0;
            parser->current.arr_len = (int)num;
            parser->current.type = parser->curr_pos[0];
            advance_fmt_str(&parser->curr_pos);
        } else if (is_fmt_char(*end_pos)) {
            parser->curr_pos = (const char*)end_pos;
            parser->current.repeat = (int)num - 1;
            parser->current.type = parser->curr_pos[0];
            parser->current.arr_len = 0;
            advance_fmt_str(&parser->curr_pos);
        } else if (*end_pos == '(') {
            parser->curr_pos = advance_fmt_str((const char**)&end_pos);
            parser->groups.depth++;
            parser->groups.start[parser->groups.depth] = parser->curr_pos;
            parser->groups.repeat[parser->groups.depth] = (int)num - 1;
            return parse_next(parser);
        }
    } else if (parser->curr_pos[0] == ')') {
        if (parser->groups.repeat[parser->groups.depth] == 0) {
            parser->groups.start[parser->groups.depth] = NULL;
            parser->groups.depth--;
            if (parser->groups.depth < 0) {
                return SP_ERR_INVALID_FMT_STR;
            }
            advance_fmt_str(&parser->curr_pos);
            return parse_next(parser);
        } else {
            parser->groups.repeat[parser->groups.depth]--;
            parser->curr_pos = parser->groups.start[parser->groups.depth];
            return parse_next(parser);
        }
    }
    return SP_OK;
}
