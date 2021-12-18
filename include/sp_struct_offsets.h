/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 Sherman Perry
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SP_STRUCT_OFFSETS_H
#define SP_STRUCT_OFFSETS_H

#include <stddef.h>

/* The following macros help make populating an offset list less tedious */

/*!
 * \brief Assign struct member offset(s) to an offset array
 *
 * The following macros use offsetof to add the offsets of <n> struct members
 * to an offset array for use with sp_unpack_bin_offset and sp_pack_bin_offset.
 * 
 * \param arr : array of type size_t arr[]
 * \param index : start index in arr to use
 * \param type : struct type, eg: 'struct somestruct'
 * \param mn : Struct member to obtain offset for. There can be up to 10, depending on macro
 */
#define SP_OFFSET1(arr, index, type, m1) (arr)[(index)] = offsetof(type, m1)

#define SP_OFFSET2(arr, index, type, m1, m2) \
    SP_OFFSET1((arr), (index), type, m1); \
    SP_OFFSET1((arr), (index) + 1, type, m2)

#define SP_OFFSET3(arr, index, type, m1, m2, m3) \
    SP_OFFSET2((arr), (index), type, m1, m2); \
    SP_OFFSET1((arr), (index) + 2, type, m3)

#define SP_OFFSET4(arr, index, type, m1, m2, m3, m4) \
    SP_OFFSET3((arr), (index), type, m1, m2, m3); \
    SP_OFFSET1((arr), (index) + 3, type, m4)

#define SP_OFFSET5(arr, index, type, m1, m2, m3, m4, m5) \
    SP_OFFSET4((arr), (index), type, m1, m2, m3, m4); \
    SP_OFFSET1((arr), (index) + 4, type, m5)

#define SP_OFFSET6(arr, index, type, m1, m2, m3, m4, m5, m6) \
    SP_OFFSET5((arr), (index), type, m1, m2, m3, m4, m5); \
    SP_OFFSET1((arr), (index) + 5, type, m6)

#define SP_OFFSET7(arr, index, type, m1, m2, m3, m4, m5, m6, m7) \
    SP_OFFSET6((arr), (index), type, m1, m2, m3, m4, m5, m6); \
    SP_OFFSET1((arr), (index) + 6, type, m7)

#define SP_OFFSET8(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8) \
    SP_OFFSET7((arr), (index), type, m1, m2, m3, m4, m5, m6, m7); \
    SP_OFFSET1((arr), (index) + 7, type, m8)

#define SP_OFFSET9(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9) \
    SP_OFFSET8((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8); \
    SP_OFFSET1((arr), (index) + 8, type, m9)

#define SP_OFFSET10(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10) \
    SP_OFFSET9((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8, m9); \
    SP_OFFSET1((arr), (index) + 9, type, m10)

// Based on the example provided at https://stackoverflow.com/a/69945225/16776990
#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, NAME, ...) NAME
#define SP_ADD_STRUCT_OFFSET(arr, index, type, ...) EXPAND(GET_MACRO(__VA_ARGS__, SP_OFFSET10, SP_OFFSET9, SP_OFFSET8, SP_OFFSET7, SP_OFFSET6, SP_OFFSET5, SP_OFFSET4, SP_OFFSET3, SP_OFFSET2, SP_OFFSET1)(arr, index, type, __VA_ARGS__))

#endif // SP_STRUCT_OFFSETS_H
