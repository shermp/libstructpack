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

SP_API SPResult sp_unpack_bin_ptr(
    const char* fmt_str, 
    int num_fields, 
    void** ptr_list, 
    void* src_buff, 
    int buff_len
);

SP_API SPResult sp_pack_bin_ptr(
    const char* fmt_str, 
    int num_fields, 
    void** ptr_list, 
    void* dest_buff, 
    int buff_len
);

SP_API SPResult sp_unpack_bin_offset(
    const char* fmt_str, 
    int num_fields, 
    size_t* offset_list, 
    void* offset_base,
    void* src_buff, 
    int buff_len
);

SP_API SPResult sp_pack_bin_offset(
    const char* fmt_str, 
    int num_fields, 
    size_t* offset_list, 
    void* offset_base,
    void* dest_buff, 
    int buff_len
);

#endif // STRUCTPACK_H
