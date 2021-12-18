/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 Sherman Perry
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SP_STRUCT_OFFSETS_H
#define SP_STRUCT_OFFSETS_H

#include <stddef.h>

/* The following macro help make populating an offset list less tedious */

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

#define SP_OFFSET11(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11) \
    SP_OFFSET10((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10); \
    SP_OFFSET1((arr), (index) + 10, type, m11)

#define SP_OFFSET12(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12) \
    SP_OFFSET11((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11); \
    SP_OFFSET1((arr), (index) + 11, type, m12)

#define SP_OFFSET13(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13) \
    SP_OFFSET12((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12); \
    SP_OFFSET1((arr), (index) + 12, type, m13)

#define SP_OFFSET14(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14) \
    SP_OFFSET13((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13); \
    SP_OFFSET1((arr), (index) + 13, type, m14)

#define SP_OFFSET15(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15) \
    SP_OFFSET14((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14); \
    SP_OFFSET1((arr), (index) + 14, type, m15)

#define SP_OFFSET16(arr, index, type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16) \
    SP_OFFSET15((arr), (index), type, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15); \
    SP_OFFSET1((arr), (index) + 15, type, m16)

// Based on the example provided at https://stackoverflow.com/a/69945225/16776990
#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, NAME, ...) NAME

/*!
 * \brief Assign struct member offset(s) to an offset array
 *
 * This variadic macro use offsetof to add the offsets of up to 16 struct members
 * to an offset array for use with sp_unpack_bin_offset and sp_pack_bin_offset.
 * 
 * \param arr : array of type size_t arr[]
 * \param index : start index in arr to use
 * \param type : struct type, eg: 'struct somestruct'
 * \param ... : Struct members to obtain offset for. There can be up to 16
 */
#define SP_ADD_STRUCT_OFFSET(arr, index, type, ...) EXPAND(GET_MACRO(__VA_ARGS__, SP_OFFSET16, SP_OFFSET15, SP_OFFSET14, SP_OFFSET13, SP_OFFSET12, SP_OFFSET11, SP_OFFSET10, SP_OFFSET9, SP_OFFSET8, SP_OFFSET7, SP_OFFSET6, SP_OFFSET5, SP_OFFSET4, SP_OFFSET3, SP_OFFSET2, SP_OFFSET1)(arr, index, type, __VA_ARGS__))

#endif // SP_STRUCT_OFFSETS_H
