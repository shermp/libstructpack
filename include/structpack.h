/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 John Smith
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef STRUCTPACK_H
#define STRUCTPACK_H

#include <stddef.h>

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
