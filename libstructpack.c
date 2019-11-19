#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "libstructpack.h"

enum sp_endian {SP_BIG_ENDIAN, SP_LITTLE_ENDIAN};
enum sp_action {SP_PACK, SP_UNPACK};

struct fmt_str_parser {
    const char* fmt_str;
    const char* curr_pos;
    enum sp_endian endian;
    /* Make a little array based stack to handle nested groups */
    struct {
        int depth;
        int repeat[11];
        const char* start[11];
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

static void reset_parser(struct fmt_str_parser* parser) {
    parser->curr_pos = parser->fmt_str;
    parser->endian = SP_LITTLE_ENDIAN;
    if (parser->curr_pos[0] == '>') {
        parser->endian = SP_BIG_ENDIAN;
        parser->curr_pos += 1;
    } else if (parser->curr_pos[0] == '<') {
        parser->endian = SP_LITTLE_ENDIAN;
        parser->curr_pos += 1;
    }
    parser->current.type = '\0';
    parser->current.arr_len = 0;
}

static struct fmt_str_parser new_parser(const char* fmt_str, SPResult* err) {
    *err = SP_OK;
    struct fmt_str_parser parser = {};
    if (fmt_str == NULL || fmt_str[0] == '\0') {
        *err = SP_NULL_CHAR;
        return parser;
    }
    parser.fmt_str = fmt_str;
    reset_parser(&parser);
    return parser;
}

static bool is_fmt_char(char fmt) {
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
        return true;
    }
    return false;
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
static SPResult check_format_str(const char* format_str) {
    size_t fmt_len = strlen(format_str);
    /* Check for invalid characters */
    char fmt;
    for (int i = 0; i < fmt_len; i++) {
        fmt = format_str[i];
        if (!(is_fmt_char(fmt) || is_endian_char(fmt) || is_arr_char(fmt) || is_group_char(fmt) || is_digit_char(fmt) || is_whitespace_char(fmt))) {
            return SP_ERR_INVALID_FMT_CHAR;
        }
    }
    /* Check that endian character only at beginning */
    for (int i = 0; i < fmt_len; i++) {
        if (is_endian_char(format_str[i]) && i > 0) {
            return SP_ERR_INVALID_ENDIAN_LOC;
        }
    }
    /* Check the string doesn't start with a '(' */
    if (format_str[0] == '(') {
        return SP_ERR_INVALID_GRP_LOC;
    }
    /* Make sure the format string ends with a valid character */
    fmt = format_str[fmt_len - 1];
    if (format_str[fmt_len - 1] != ')' && !is_fmt_char(format_str[fmt_len - 1]) && !is_whitespace_char(format_str[fmt_len - 1])) {
        return SP_ERR_INVALID_END_CHAR;
    }
    return SP_OK;
}

static SPResult parse_next(struct fmt_str_parser* parser) {
    char *end_pos;
    long num;
    while (parser->curr_pos[0] == ' ' || parser->curr_pos[0] == '\t') {
        parser->curr_pos++;
    }
    if (parser->curr_pos[0] == '\0') {
        return SP_NULL_CHAR;
    }
    if (parser->current.repeat > 0) {
        parser->current.repeat--;
    } else if (is_fmt_char(parser->curr_pos[0])) {
        parser->current.type = parser->curr_pos[0];
        parser->current.arr_len = 0;
        parser->current.repeat = 0;
        parser->curr_pos++;
    } else if (parser->curr_pos[0] == '[') {
        num = strtol(parser->curr_pos + 1, &end_pos, 10);
        if (num > INT_MAX || *end_pos != ']' || !is_fmt_char(end_pos[1])) {
            return SP_ERR_INVALID_ARR;
        }
        parser->curr_pos = (const char*)end_pos + 1;
        parser->current.arr_len = (int)num;
        parser->current.repeat = 0;
        parser->current.type = parser->curr_pos[0];
        parser->curr_pos++;
    } else if (is_digit_char(parser->curr_pos[0])) {
        num = strtol(parser->curr_pos, &end_pos, 10);
        if (num > INT_MAX) {
            return SP_ERR_INT_OVERFLOW;
        }
        /* Strings are special. They are always treated as arrays */
        if (*end_pos == 's') {
            parser->curr_pos = (const char*)end_pos;
            parser->current.repeat = 0;
            parser->current.arr_len = (int)num;
            parser->current.type = parser->curr_pos[0];
            parser->curr_pos++;
        } else if (is_fmt_char(*end_pos)) {
            parser->curr_pos = (const char*)end_pos;
            parser->current.repeat = (int)num - 1;
            parser->current.type = parser->curr_pos[0];
            parser->current.arr_len = 0;
            parser->curr_pos++;
        } else if (*end_pos == '(') {
            parser->curr_pos = end_pos + 1;
            parser->groups.depth++;
            parser->groups.start[parser->groups.depth] = parser->curr_pos;
            parser->groups.repeat[parser->groups.depth] = (int)num - 1;
            return parse_next(parser);
        }
    } else if (parser->curr_pos[0] == ')') {
        if (parser->groups.repeat[parser->groups.depth] == 0) {
            parser->groups.start[parser->groups.depth] = NULL;
            parser->groups.depth--;
            parser->curr_pos++;
            return parse_next(parser);
        } else {
            parser->groups.repeat[parser->groups.depth]--;
            parser->curr_pos = parser->groups.start[parser->groups.depth];
            return parse_next(parser);
        }
    }
    return SP_OK;
}

static SPResult sp_unpack(struct fmt_str_parser* p, SPStructDef* sd, void* src_buff, int buff_len) {
    uint8_t* buff_ptr = (uint8_t*)src_buff;
    uint8_t* struct_ptr;
    int i, j;
    int len;
    i = 0;
    while (parse_next(p) == 0) {
        struct_ptr = (uint8_t*)(sd->struct_ptr + sd->field_offsets[i]);
        len = 1;
        if (p->current.arr_len > 0) {
            len = p->current.arr_len;
        }
        switch (p->current.type) {
            case 'x':
                buff_ptr += len;
                break;
            case 'b':
            case 'B':
                memcpy(struct_ptr, buff_ptr, len);
                buff_ptr += len;
                break;
            case 'h':
            case 'H': ;
                uint16_t tmp16;
                for (j = 0; j < len; j++) {
                    memcpy(&tmp16, buff_ptr, sizeof tmp16);
                    tmp16 = (p->endian == SP_BIG_ENDIAN) ? sp_from_be16(tmp16) : sp_from_le16(tmp16);
                    memcpy(struct_ptr + (sizeof tmp16 * j), &tmp16, sizeof tmp16);
                    buff_ptr += sizeof tmp16;
                }
                break;
            case 'i':
            case 'I': ;
                uint32_t tmp32;
                for (j = 0; j < len; j++) {
                    memcpy(&tmp32, buff_ptr, sizeof tmp32);
                    tmp32 = (p->endian == SP_BIG_ENDIAN) ? sp_from_be32(tmp32) : sp_from_le32(tmp32);
                    memcpy(struct_ptr + (sizeof tmp32 * j), &tmp32, sizeof tmp32);
                    buff_ptr += sizeof tmp32;
                }
                break;
            case 'q':
            case 'Q': ;
                uint64_t tmp64;
                for (j = 0; j < len; j++) {
                    memcpy(&tmp64, buff_ptr, sizeof tmp64);
                    tmp64 = (p->endian == SP_BIG_ENDIAN) ? sp_from_be64(tmp64) : sp_from_le64(tmp64);
                    memcpy(struct_ptr + (sizeof tmp64 * j), &tmp64, sizeof tmp64);
                    buff_ptr += sizeof tmp64;
                }
                break;
            case 's':
                strncpy((char*)struct_ptr, (const char*)buff_ptr, len);
                struct_ptr[len] = '\0';
                buff_ptr += len;
                break;
        }
        i++;
    }
    return SP_OK;
}

static SPResult sp_pack(struct fmt_str_parser* p, SPStructDef* sd, void* dst_buff, int buff_len) {
    uint8_t* buff_ptr = (uint8_t*)dst_buff;
    uint8_t* struct_ptr;
    int i, j;
    int len;
    i = 0;
    while (parse_next(p) == 0) {
        struct_ptr = (uint8_t*)(sd->struct_ptr + sd->field_offsets[i]);
        len = 1;
        if (p->current.arr_len > 0) {
            len = p->current.arr_len;
        }
        switch (p->current.type) {
            case 'x':
                buff_ptr += len;
                break;
            case 'b':
            case 'B':
                memcpy(buff_ptr, struct_ptr, len);
                buff_ptr += len;
                break;
            case 'h':
            case 'H': ;
                uint16_t tmp16;
                for (j = 0; j < len; j++) {
                    memcpy(&tmp16, struct_ptr + (sizeof tmp16 * j), sizeof tmp16);
                    tmp16 = (p->endian == SP_BIG_ENDIAN) ? sp_to_be16(tmp16) : sp_to_le16(tmp16);
                    memcpy(buff_ptr, &tmp16, sizeof tmp16);
                    buff_ptr += sizeof tmp16;
                }
                break;
            case 'i':
            case 'I': ;
                uint32_t tmp32;
                for (j = 0; j < len; j++) {
                    memcpy(&tmp32, struct_ptr + (sizeof tmp32 * j), sizeof tmp32);
                    tmp32 = (p->endian == SP_BIG_ENDIAN) ? sp_to_be32(tmp32) : sp_to_le32(tmp32);
                    memcpy(buff_ptr, &tmp32, sizeof tmp32);
                    buff_ptr += sizeof tmp32;
                }
                break;
            case 'q':
            case 'Q': ;
                uint64_t tmp64;
                for (j = 0; j < len; j++) {
                    memcpy(&tmp64, struct_ptr + (sizeof tmp64 * j), sizeof tmp64);
                    tmp64 = (p->endian == SP_BIG_ENDIAN) ? sp_to_be64(tmp64) : sp_to_le64(tmp64);
                    memcpy(buff_ptr, &tmp64, sizeof tmp64);
                    buff_ptr += sizeof tmp64;
                }
                break;
            case 's':
                strncpy((char*)buff_ptr, (const char*)struct_ptr, len);
                buff_ptr += len;
                break;
        }
        i++;
    }
    return SP_OK;
}

static SPResult sp_pack_unpack_bin(enum sp_action action, SPStructDef* sd, void* buff, int buff_len) {
    if (sd == NULL || buff == NULL || buff_len == 0) {
        return SP_ERR_MISSING_PARAMS;
    }
    if (sd->struct_ptr == NULL || sd->field_offsets == NULL || sd->fmt_Str == NULL || sd->num_fields == 0) {
        return SP_ERR_MISSING_PARAMS;
    }
    SPResult err;
    err = check_format_str(sd->fmt_Str);
    if (err != SP_OK) {
        return err;
    }
    struct fmt_str_parser p = new_parser(sd->fmt_Str, &err);
    if (err != SP_OK) {
        return err;
    }
    int parsed_count = 0;
    while (parse_next(&p) == 0) {
        if (p.current.type != 'x') {
            parsed_count++;
        }
    }
    if (parsed_count != sd->num_fields) {
        return SP_ERR_FIELD_CNT;
    }
    reset_parser(&p);
    SPResult res;
    if (action == SP_UNPACK) {
        res = sp_unpack(&p, sd, buff, buff_len);
    } else {
        res = sp_pack(&p, sd, buff, buff_len);
    }
    return res;
}

SPResult sp_unpack_bin(SPStructDef* sd, void* src_buff, int buff_len) {
    return sp_pack_unpack_bin(SP_UNPACK, sd, src_buff, buff_len);
}

SPResult sp_pack_bin(SPStructDef* sd, void* dest_buff, int buff_len) {
    return sp_pack_unpack_bin(SP_PACK, sd, dest_buff, buff_len);
}