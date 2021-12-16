/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 John Smith
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <structpack.h>
#include "sp_parser.h"
#include "sp_internal.h"

/* endian conversion functions */
static uint16_t sp_from_be16(uint16_t val) {
    uint8_t *tmp = (uint8_t*)&val;
    uint16_t ret = 0;
    ret |= (uint16_t)tmp[0] << 8;
    ret |= (uint16_t)tmp[1] << 0;
    return ret;
}

static uint16_t sp_from_le16(uint16_t val) {
    uint8_t *tmp = (uint8_t*)&val;
    uint16_t ret = 0;
    ret |= (uint16_t)tmp[1] << 8;
    ret |= (uint16_t)tmp[0] << 0;
    return ret;
}

static uint32_t sp_from_be32(uint32_t val) {
    uint8_t *tmp = (uint8_t*)&val;
    uint32_t ret = 0;
    ret =  (uint32_t)tmp[0] << 24;
    ret |= (uint32_t)tmp[1] << 16;
    ret |= (uint32_t)tmp[2] << 8;
    ret |= (uint32_t)tmp[3] << 0;
    return ret;
}

static uint32_t sp_from_le32(uint32_t val) {
    uint8_t *tmp = (uint8_t*)&val;
    uint32_t ret = 0;
    ret =  (uint32_t)tmp[3] << 24;
    ret |= (uint32_t)tmp[2] << 16;
    ret |= (uint32_t)tmp[1] << 8;
    ret |= (uint32_t)tmp[0] << 0;
    return ret;
}

static uint64_t sp_from_be64(uint64_t val) {
    uint8_t *tmp = (uint8_t*)&val;
    uint64_t ret = 0;
    ret =  (uint64_t)tmp[0] << 56;
    ret |= (uint64_t)tmp[1] << 48;
    ret |= (uint64_t)tmp[2] << 40;
    ret |= (uint64_t)tmp[3] << 32;
    ret |= (uint64_t)tmp[4] << 24;
    ret |= (uint64_t)tmp[5] << 16;
    ret |= (uint64_t)tmp[6] << 8;
    ret |= (uint64_t)tmp[7] << 0;
    return ret;
}

static uint64_t sp_from_le64(uint64_t val) {
    uint8_t *tmp = (uint8_t*)&val;
    uint64_t ret = 0;
    ret =  (uint64_t)tmp[7] << 56;
    ret |= (uint64_t)tmp[6] << 48;
    ret |= (uint64_t)tmp[5] << 40;
    ret |= (uint64_t)tmp[4] << 32;
    ret |= (uint64_t)tmp[3] << 24;
    ret |= (uint64_t)tmp[2] << 16;
    ret |= (uint64_t)tmp[1] << 8;
    ret |= (uint64_t)tmp[0] << 0;
    return ret;
}

static uint16_t sp_to_be16(uint16_t val) {
    uint16_t ret = 0;
    uint8_t *tmp = (uint8_t*)&ret;
    tmp[0] = (val & 0xff00) >> 8;
    tmp[1] = (val & 0x00ff) >> 0;
    return ret;
}

static uint16_t sp_to_le16(uint16_t val) {
    uint16_t ret = 0;
    uint8_t *tmp = (uint8_t*)&ret;
    tmp[1] = (val & 0xff00) >> 8;
    tmp[0] = (val & 0x00ff) >> 0;
    return ret;
}

static uint32_t sp_to_be32(uint32_t val) {
    uint32_t ret = 0;
    uint8_t *tmp = (uint8_t*)&ret;
    tmp[0] = (val & 0xff000000) >> 24;
    tmp[1] = (val & 0x00ff0000) >> 16;
    tmp[2] = (val & 0x0000ff00) >> 8;
    tmp[3] = (val & 0x000000ff) >> 0;
    return ret;
}

static uint32_t sp_to_le32(uint32_t val) {
    uint32_t ret = 0;
    uint8_t *tmp = (uint8_t*)&ret;
    tmp[3] = (val & 0xff000000) >> 24;
    tmp[2] = (val & 0x00ff0000) >> 16;
    tmp[1] = (val & 0x0000ff00) >> 8;
    tmp[0] = (val & 0x000000ff) >> 0;
    return ret;
}

static uint64_t sp_to_be64(uint64_t val) {
    uint64_t ret = 0;
    uint8_t *tmp = (uint8_t*)&ret;
    tmp[0] = (val & 0xff00000000000000) >> 56;
    tmp[1] = (val & 0x00ff000000000000) >> 48;
    tmp[2] = (val & 0x0000ff0000000000) >> 40;
    tmp[3] = (val & 0x000000ff00000000) >> 32;
    tmp[4] = (val & 0x00000000ff000000) >> 24;
    tmp[5] = (val & 0x0000000000ff0000) >> 16;
    tmp[6] = (val & 0x000000000000ff00) >> 8;
    tmp[7] = (val & 0x00000000000000ff) >> 0;
    return ret;
}

static uint64_t sp_to_le64(uint64_t val) {
    uint64_t ret = 0;
    uint8_t *tmp = (uint8_t*)&ret;
    tmp[7] = (val & 0xff00000000000000) >> 56;
    tmp[6] = (val & 0x00ff000000000000) >> 48;
    tmp[5] = (val & 0x0000ff0000000000) >> 40;
    tmp[4] = (val & 0x000000ff00000000) >> 32;
    tmp[3] = (val & 0x00000000ff000000) >> 24;
    tmp[2] = (val & 0x0000000000ff0000) >> 16;
    tmp[1] = (val & 0x000000000000ff00) >> 8;
    tmp[0] = (val & 0x00000000000000ff) >> 0;
    return ret;
}

static void sp_copy_8(void* struct_ptr, void* buff_ptr, int len, enum sp_action action, bool is_str) {
    if (action == SP_UNPACK) {
        memcpy(struct_ptr, buff_ptr, len);
        if (is_str) {
            /* Note, it is made clear in the format string docs that the dest char array MUST be
               at least one character longer than the provided length. */
            ((char*)struct_ptr)[len] = '\0';
        }
    } else {
        memcpy(buff_ptr, struct_ptr, len);
    }
}

static void sp_copy_16(void* struct_ptr, void* buff_ptr, int len, enum sp_endian endian, enum sp_action action) {
    uint16_t tmp16;
    int j;
    void *src = (action == SP_UNPACK) ? buff_ptr : struct_ptr;
    void *dst = (action == SP_UNPACK) ? struct_ptr : buff_ptr;
    for (j = 0; j < len; j++) {
        memcpy(&tmp16, src, sizeof tmp16);
        if (action == SP_UNPACK) {
            tmp16 = (endian == SP_BIG_ENDIAN) ? sp_from_be16(tmp16) : sp_from_le16(tmp16);
        } else {
            tmp16 = (endian == SP_BIG_ENDIAN) ? sp_to_be16(tmp16) : sp_to_le16(tmp16);
        }
        memcpy(dst, &tmp16, sizeof tmp16);
        src = (char*)src + sizeof tmp16;
        dst = (char*)dst + sizeof tmp16;
    }
}

static void sp_copy_32(void* struct_ptr, void* buff_ptr, int len, enum sp_endian endian, enum sp_action action) {
    uint32_t tmp32;
    int j;
    void *src = (action == SP_UNPACK) ? buff_ptr : struct_ptr;
    void *dst = (action == SP_UNPACK) ? struct_ptr : buff_ptr;
    for (j = 0; j < len; j++) {
        memcpy(&tmp32, src, sizeof tmp32);
        if (action == SP_UNPACK) {
            tmp32 = (endian == SP_BIG_ENDIAN) ? sp_from_be32(tmp32) : sp_from_le32(tmp32);
        } else {
            tmp32 = (endian == SP_BIG_ENDIAN) ? sp_to_be32(tmp32) : sp_to_le32(tmp32);
        }
        memcpy(dst, &tmp32, sizeof tmp32);
        src = (char*)src + sizeof tmp32;
        dst = (char*)dst + sizeof tmp32;
    }
}

static void sp_copy_64(void* struct_ptr, void* buff_ptr, int len, enum sp_endian endian, enum sp_action action) {
    uint64_t tmp64;
    int j;
    void *src = (action == SP_UNPACK) ? buff_ptr : struct_ptr;
    void *dst = (action == SP_UNPACK) ? struct_ptr : buff_ptr;
    for (j = 0; j < len; j++) {
        memcpy(&tmp64, src, sizeof tmp64);
        if (action == SP_UNPACK) {
            tmp64 = (endian == SP_BIG_ENDIAN) ? sp_from_be64(tmp64) : sp_from_le64(tmp64);
        } else {
            tmp64 = (endian == SP_BIG_ENDIAN) ? sp_to_be64(tmp64) : sp_to_le64(tmp64);
        }
        memcpy(dst, &tmp64, sizeof tmp64);
        src = (char*)src + sizeof tmp64;
        dst = (char*)dst + sizeof tmp64;
    }
}

static SPResult sp_pack_unpack_bin( enum sp_action action, 
                                    const char* fmt_str, 
                                    int num_fields,
                                    void** ptr_list, 
                                    size_t* offset_list, 
                                    void* offset_base,
                                    void* buff, 
                                    int buff_len) 
{

    SPResult err;
    err = validate_format_str(fmt_str);
    if (err != SP_OK) {
        return err;
    }
    struct fmt_str_parser p = new_parser(fmt_str, &err);
    if (err != SP_OK) {
        return err;
    }
    int parsed_count = 0;
    while (parse_next(&p) == 0) {
        if (p.current.type != 'x') {
            parsed_count++;
        }
    }
    if (parsed_count != num_fields) {
        return SP_ERR_FIELD_CNT;
    }
    reset_parser(&p);
    SPResult res = SP_OK;
    uint8_t* buff_ptr = (uint8_t*)buff;
    uint8_t* struct_ptr;
    int off_index = 0;
    int len;
    while ((res = parse_next(&p)) == SP_OK) {
        if (offset_list) {
            struct_ptr = (uint8_t*)((char*)offset_base + offset_list[off_index]);
        } else {
            struct_ptr = (uint8_t*)ptr_list[off_index];
        }
        len = 1;
        if (p.current.arr_len > 0) {
            len = p.current.arr_len;
        }
        switch (p.current.type) {
            case 'x':
                off_index--;
                buff_ptr += len;
                break;
            case 'b':
            case 'B':
                sp_copy_8(struct_ptr, buff_ptr, len, action, false);
                buff_ptr += len;
                break;
            case 'h':
            case 'H':
                sp_copy_16(struct_ptr, buff_ptr, len, p.endian, action);
                buff_ptr += len * 2;
                break;
            case 'i':
            case 'I':
                sp_copy_32(struct_ptr, buff_ptr, len, p.endian, action);
                buff_ptr += len * 4; 
                break;
            case 'q':
            case 'Q':
                sp_copy_64(struct_ptr, buff_ptr, len, p.endian, action);
                buff_ptr += len * 8;
                break;
            case 's':
                sp_copy_8(struct_ptr, buff_ptr, len, action, true);
                buff_ptr += len;
                break;
        }
        off_index++;
    }
    if (res == SP_NULL_CHAR) {
        res = SP_OK;
    }
    return res;
}

SPResult sp_unpack_bin_ptr( const char* fmt_str, 
                            int num_fields, 
                            void** ptr_list, 
                            void* src_buff, 
                            int buff_len ) 
{
    if (!fmt_str || num_fields <= 0 || !ptr_list || !src_buff || buff_len <= 0) {
        return SP_ERR_MISSING_PARAMS;
    }
    return sp_pack_unpack_bin(SP_UNPACK, fmt_str, num_fields, ptr_list, NULL, NULL, src_buff, buff_len);
}

SPResult sp_pack_bin_ptr(const char* fmt_str, 
                         int num_fields, 
                         void** ptr_list, 
                         void* dest_buff, 
                         int buff_len) 
{
    if (!fmt_str || num_fields <= 0 || !ptr_list || !dest_buff || buff_len <= 0) {
        return SP_ERR_MISSING_PARAMS;
    }
    return sp_pack_unpack_bin(SP_PACK, fmt_str, num_fields, ptr_list, NULL, NULL, dest_buff, buff_len);
}

SPResult sp_unpack_bin_offset(  const char* fmt_str, 
                                int num_fields, 
                                size_t* offset_list, 
                                void* offset_base,
                                void* src_buff, 
                                int buff_len ) 
{
    if (!fmt_str || num_fields <= 0 || !offset_list || !offset_base || !src_buff || buff_len <= 0) {
        return SP_ERR_MISSING_PARAMS;
    }
    return sp_pack_unpack_bin(SP_UNPACK, fmt_str, num_fields, NULL, offset_list, offset_base, src_buff, buff_len);
}

SPResult sp_pack_bin_offset(const char* fmt_str, 
                            int num_fields, 
                            size_t* offset_list, 
                            void* offset_base,
                            void* dest_buff, 
                            int buff_len) 
{
    if (!fmt_str || num_fields <= 0 || !offset_list || !offset_base || !dest_buff || buff_len <= 0) {
        return SP_ERR_MISSING_PARAMS;
    }
    return sp_pack_unpack_bin(SP_PACK, fmt_str, num_fields, NULL, offset_list, offset_base, dest_buff, buff_len);
}
