/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 Sherman Perry
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef STRUCTPACK_H
#define STRUCTPACK_H

#include <stddef.h>
#include "sp_struct_offsets.h"

#if defined(_MSC_VER) 
#define SP_API __declspec(dllexport)
#else
#define SP_API
#endif

typedef enum {
    SP_OK,
    SP_NULL_CHAR,
    SP_ERR_NULL_FMT,
    SP_ERR_MISSING_PARAMS,
    SP_ERR_INVALID_PARAMS,
    SP_ERR_INVALID_ARR,
    SP_ERR_INVALID_FMT_STR,
    SP_ERR_INT,
    SP_ERR_FIELD_CNT,
    SP_ERR_BUFF_OVERRUN
} SPResult;

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

/*!
 * \brief Unpack binary data to a struct using pointers to struct members
 * 
 * \param fmt_str : format string to use to interpret incoming data structure
 * \param num_fields : Number of fields to write to. Must match what fmt_str parses
 * \param ptr_list : List of struct member pointers to write into
 * \param src_buff : Source buffer to read from
 * \param buff_len : Source buffer length in bytes
 * \return SPResult : Result will be 'SP_OK' if unpacking was successful
 */
SP_API SPResult sp_unpack_bin_ptr(
    const char* fmt_str, 
    int num_fields, 
    void** ptr_list, 
    void* src_buff, 
    int buff_len
);

/*!
 * \brief Pack binary data to a struct using pointers to struct members
 * 
 * \param fmt_str : format string to use to format outgoing data
 * \param num_fields  : Number of fields to read from. Must match what fmt_str parses
 * \param ptr_list : List of struct member pointers to read from
 * \param dest_buff : Destination buffer to write to
 * \param buff_len : Destination buffer length in bytes
 * \return SPResult : Result will be 'SP_OK' if unpacking was successful
 */
SP_API SPResult sp_pack_bin_ptr(
    const char* fmt_str, 
    int num_fields, 
    void** ptr_list, 
    void* dest_buff, 
    int buff_len
);

/*!
 * \brief Unpack binary data to a struct using struct member offsets
 * 
 * \param fmt_str : format string to use to interpret incoming data structure
 *                  refer to README.md for format string documentation
 * \param num_fields : Number of fields to write to. Must match what fmt_str parses
 * \param offset_list : List of struct member offsets to write to
 * \param offset_base : Address of structure to write into
 * \param src_buff : Source buffer to read from
 * \param buff_len : Source buffer length in bytes
 * \return SPResult : Result will be 'SP_OK' if unpacking was successful
 */
SP_API SPResult sp_unpack_bin_offset(
    const char* fmt_str, 
    int num_fields, 
    size_t* offset_list, 
    void* offset_base,
    void* src_buff, 
    int buff_len
);

/*!
 * \brief Pack binary data to a struct using struct member offsets
 * 
 * \param fmt_str : format string to use to format outgoing data
 * \param num_fields  : Number of fields to read from. Must match what fmt_str parses
 * \param offset_list : List of struct member offsets to read from
 * \param offset_base : Address of structure to read from
 * \param dest_buff : Destination buffer to write to
 * \param buff_len : Destination buffer length in bytes
 * \return SPResult : Result will be 'SP_OK' if unpacking was successful
 */
SP_API SPResult sp_pack_bin_offset(
    const char* fmt_str, 
    int num_fields, 
    size_t* offset_list, 
    void* offset_base,
    void* dest_buff, 
    int buff_len
);

#endif // STRUCTPACK_H
