#ifndef STRUCTPACK_H
#define STRUCTPACK_H

#include <stddef.h>

/*******************************************************************
 *                   Format String Details                         *
 *******************************************************************
 * The format string is used to determine how bytes of data should 
 * be interpreted, including size, endianess and layout. It hass 
 * been inspired by the Python struct library. The behaviour is
 * NOT identical however.
 * 
 * Details:
 * 1. The format string may begin with an endian indicator '<' or '>'. 
 *    Where '<' = little endian, and '>' = big endian.
 *    If no such identifier is found, little endian is assumed.
 * 2. The format string contains one or more format charachters.
 *    These characters determine the data type, and whether to
 *    perform endian handling.
 *    The currently supported format characters are as follows:
 *      FMT_CHAR            C_TYPE
 *      'x'                 none (ignore byte)
 *      'b'                 int8_t
 *      'B'                 uint8_t
 *      'h'                 int16_t
 *      'H'                 uint16_t
 *      'i'                 int32_t
 *      'I'                 uint32_t
 *      'q'                 int64_t
 *      'Q'                 uint64_t
 *      's'                 char[]
 * 3. A format character may be preceeded by an integer, which will
 *    repeat that data type n times. Eg: 'III' and '3I' are treated
 *    the same.
 * 4. A format character preceeded by an integer in square brackets
 *    [n] will be treated as an n-long array of that type.
 *    Eg: '[10]I' is treated as a single array of 10 uint32_t's
 * 5. Fields may be grouped and repeated using the following syntax:
 *    'n('sub_format_str')', where the elements of 'sub_format_str'
 *    will be repeated n times.
 * 6. 'x' will skip reading/writing bytes to/from the buffer at that
 *    offset.
 * 7. <n>s and [<n>]s are treated the same. They are both treated as
 *    a char array. Null termination is guaranteed, and so the 
 *    dest. struct field MUST be one element longer than the source.
 * 8. Whitespace is (mostly) ignored
 *******************************************************************/

typedef enum {
    SP_OK,
    SP_NULL_CHAR,
    SP_ERR_NULL_FMT,
    SP_ERR_MISSING_PARAMS,
    SP_ERR_INVALID_PARAMS,
    SP_ERR_INVALID_ARR,
    SP_ERR_INVALID_FMT_STR,
    SP_ERR_INT,
    SP_ERR_FIELD_CNT
} SPResult;

SPResult sp_unpack_bin_ptr(
    const char* fmt_str, 
    int num_fields, 
    void** ptr_list, 
    void* src_buff, 
    int buff_len
);
SPResult sp_pack_bin_ptr(
    const char* fmt_str, 
    int num_fields, 
    void** ptr_list, 
    void* dest_buff, 
    int buff_len
);

SPResult sp_unpack_bin_offset(
    const char* fmt_str, 
    int num_fields, 
    size_t* offset_list, 
    void* offset_base,
    void* src_buff, 
    int buff_len
);
SPResult sp_pack_bin_offset(
    const char* fmt_str, 
    int num_fields, 
    size_t* offset_list, 
    void* offset_base,
    void* dest_buff, 
    int buff_len
);

#endif // STRUCTPACK_H
