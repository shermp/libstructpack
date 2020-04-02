#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "libstructpack.h"

#define SP_MAX_GRP_DEPTH 11

enum sp_endian {SP_BIG_ENDIAN, SP_LITTLE_ENDIAN};
enum sp_action {SP_PACK, SP_UNPACK};
typedef enum { SP_FALSE, SP_TRUE } sp_bool;

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
static const char* advance_fmt_str(const char** c);
static void reset_parser(struct fmt_str_parser* parser) {
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

static struct fmt_str_parser new_parser(const char* fmt_str, SPResult* err) {
    *err = SP_OK;
    struct fmt_str_parser parser = {};
    if (!fmt_str || fmt_str[0] == '\0') {
        *err = SP_NULL_CHAR;
        return parser;
    }
    parser.fmt_str = fmt_str;
    reset_parser(&parser);
    return parser;
}

static sp_bool is_fmt_char(char fmt) {
    if (
        fmt == 'x' || 
        fmt == 'b' || 
        fmt == 'B' || 
        fmt == 'h' || 
        fmt == 'H' || 
        fmt == 'i' || 
        fmt == 'I' || 
        fmt == 'q' || 
        fmt == 'Q' ||
        fmt == 's'
    ) {
        return SP_TRUE;
    }
    return SP_FALSE;
}
static sp_bool is_endian_char(char end) {
    if (end == '<' || end == '>') {
        return SP_TRUE;
    }
    return SP_FALSE;
}
static sp_bool is_arr_char(char arr) {
    if (arr == '[' || arr == ']') {
        return SP_TRUE;
    }
    return SP_FALSE;
}
static sp_bool is_group_char(char grp) {
    if (grp == '(' || grp == ')') {
        return SP_TRUE;
    }
    return SP_FALSE;
}
static sp_bool is_digit_char(char digit) {
    if (digit >= '0' && digit <= '9') {
        return SP_TRUE;
    }
    return SP_FALSE;
}
static sp_bool is_whitespace_char(char ws) {
    if (ws == ' ' || ws == '\t') {
        return SP_TRUE;
    }
    return SP_FALSE;
}
const char* advance_fmt_str(const char** c) {
    *c += 1;
    while (is_whitespace_char(**c)) {
        *c += 1;
    }
    return *c;
}
static SPResult validate_format_str(const char* format_str) {
    size_t fmt_len = strlen(format_str);
    int i;
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
    sp_bool arr_open = SP_FALSE;
    for (i = 0; i < fmt_len; i++) {
        fmt = format_str[i];
        if ((fmt == ']' && !arr_open) || (fmt == '[' && arr_open)) {
            return SP_ERR_INVALID_FMT_STR;
        }
        if (fmt == '[') {
            arr_open = SP_TRUE;
        }
        if (fmt == ']') {
            arr_open = SP_FALSE;
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

static SPResult parse_next(struct fmt_str_parser* parser) {
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
        if (*end_pos == 's') {
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
static void sp_copy_8(void* struct_ptr, void* buff_ptr, int len, enum sp_action action, sp_bool is_str) {
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
        src += sizeof tmp16;
        dst += sizeof tmp16;
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
        src += sizeof tmp32;
        dst += sizeof tmp32;
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
        src += sizeof tmp64;
        dst += sizeof tmp64;
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
            struct_ptr = (uint8_t*)(offset_base + offset_list[off_index]);
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
                sp_copy_8(struct_ptr, buff_ptr, len, action, SP_FALSE);
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
                sp_copy_8(struct_ptr, buff_ptr, len, action, SP_TRUE);
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