/*
 * SPDX-FileCopyrightText: Copyright 2019-2021 Sherman Perry
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>
#include <string.h>

#include <structpack.h>
#include "sp_test.h"

#define ARR_LEN(arr) sizeof arr / sizeof arr[0]

struct sp_pack_unpack {
    char hello[13];
    uint32_t spu32;
    int64_t spi64;
    int32_t spi32;
    uint64_t spu64;
    int16_t spi16;
    uint16_t spu16;
    struct {
        int64_t spsti64;
        uint32_t spstu32;
    }s_arr[3];
    int16_t spi16arr[5];
    char spchar;
    uint16_t helloW[6];
    uint32_t worldU[6];
};
const char fmt_str_be[] = ">12s IqiQhH 3(qI) [5]h b 5w 5u";
const char fmt_str_le[] = "<12s IqiQhH 3(qI) [5]h b 5w 5u";

/* The following byte sequence was generated with
 * struct.pack('>12sIqiQhHqIqIqI5hc5h5i', b'Hello World!', 100000, -8000000000, -100000, 8000000000, -30000, 60000, 
 *   -7000000000, 200000, -8000000000, 300000, -9000000000, 400000, 1000, 2000, 3000, 4000, 5000, b's', 
 *   *[ord(c) for c in "Hello"], *[ord(c) for c in "World"])
 */

uint8_t bytes_be[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x00, 0x01, 0x86, 0xa0, 0xff, 0xff, 0xff, 0xfe, 0x23, 0x29, 0xb0, 0x00, 0xff, 0xfe, 0x79, 0x60, 0x00, 0x00, 0x00, 0x01, 0xdc, 0xd6, 0x50, 0x00, 0x8a, 0xd0, 0xea, 0x60, 0xff, 0xff, 0xff, 0xfe, 0x5e, 0xc4, 0x7a, 0x00, 0x00, 0x03, 0x0d, 0x40, 0xff, 0xff, 0xff, 0xfe, 0x23, 0x29, 0xb0, 0x00, 0x00, 0x04, 0x93, 0xe0, 0xff, 0xff, 0xff, 0xfd, 0xe7, 0x8e, 0xe6, 0x00, 0x00, 0x06, 0x1a, 0x80, 0x03, 0xe8, 0x07, 0xd0, 0x0b, 0xb8, 0x0f, 0xa0, 0x13, 0x88, 0x73, 0x00, 0x48, 0x00, 0x65, 0x00, 0x6c, 0x00, 0x6c, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x57, 0x00, 0x00, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00, 0x64};
uint8_t bytes_le[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0xa0, 0x86, 0x01, 0x00, 0x00, 0xb0, 0x29, 0x23, 0xfe, 0xff, 0xff, 0xff, 0x60, 0x79, 0xfe, 0xff, 0x00, 0x50, 0xd6, 0xdc, 0x01, 0x00, 0x00, 0x00, 0xd0, 0x8a, 0x60, 0xea, 0x00, 0x7a, 0xc4, 0x5e, 0xfe, 0xff, 0xff, 0xff, 0x40, 0x0d, 0x03, 0x00, 0x00, 0xb0, 0x29, 0x23, 0xfe, 0xff, 0xff, 0xff, 0xe0, 0x93, 0x04, 0x00, 0x00, 0xe6, 0x8e, 0xe7, 0xfd, 0xff, 0xff, 0xff, 0x80, 0x1a, 0x06, 0x00, 0xe8, 0x03, 0xd0, 0x07, 0xb8, 0x0b, 0xa0, 0x0f, 0x88, 0x13, 0x73, 0x48, 0x00, 0x65, 0x00, 0x6c, 0x00, 0x6c, 0x00, 0x6f, 0x00, 0x57, 0x00, 0x00, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00};

static int rv = 0;

int main(void) {
    size_t offsets[17] = {0};
    SP_ADD_STRUCT_OFFSET(offsets, 0, struct sp_pack_unpack, hello, spu32, spi64, spi32, spu64, spi16, spu16);
    for (int i = 0; i < 3; ++i) {
        SP_ADD_STRUCT_OFFSET(offsets, 7 + (2 * i), struct sp_pack_unpack, s_arr[i].spsti64, s_arr[i].spstu32);
    }
    SP_ADD_STRUCT_OFFSET(offsets, 13, struct sp_pack_unpack, spi16arr, spchar, helloW, worldU);

    /* Test offset unpacking */
    uint16_t hW[] = {'H', 'e', 'l', 'l', 'o', '\0'};
    uint32_t wU[] = {'W', 'o', 'r', 'l', 'd', '\0'};
    const char *fmt = fmt_str_be;
    uint8_t *bytes = bytes_be;
    int b_sz = (int)(sizeof bytes_be);
    const char *status;
    status = "\nTesting big endian data";
    for (int i = 0; i < 2; ++i) {
        printf("%s\n", status);
        struct sp_pack_unpack up = {0};
        SP_TEST_ASSERT(rv, sp_unpack_bin_offset(fmt, ARR_LEN(offsets), offsets, &up, bytes, b_sz) == SP_OK, "sp_pack_unpack");
        SP_TEST_ASSERT(rv, strcmp((const char*)up.hello, "Hello World!") == 0, "compare hello world");
        SP_TEST_ASSERT(rv, up.spu32 == 100000, "spu32");
        SP_TEST_ASSERT(rv, up.spi64 == -8000000000, "spi64");
        SP_TEST_ASSERT(rv, up.spi32 == -100000, "spi32");
        SP_TEST_ASSERT(rv, up.spu64 == 8000000000, "spu64");
        SP_TEST_ASSERT(rv, up.spi16 == -30000, "spi16");
        SP_TEST_ASSERT(rv, up.spu16 == 60000, "spu16");

        SP_TEST_ASSERT(rv, up.s_arr[0].spsti64 == -7000000000, "s_arr[0].spsti64");
        SP_TEST_ASSERT(rv, up.s_arr[0].spstu32 == 200000, "s_arr[0].spstu32");
        SP_TEST_ASSERT(rv, up.s_arr[1].spsti64 == -8000000000, "s_arr[1].spsti64");
        SP_TEST_ASSERT(rv, up.s_arr[1].spstu32 == 300000, "s_arr[1].spstu32");
        SP_TEST_ASSERT(rv, up.s_arr[2].spsti64 == -9000000000, "s_arr[2].spsti64");
        SP_TEST_ASSERT(rv, up.s_arr[2].spstu32 == 400000, "s_arr[2].spstu32");

        for (int j = 0; j < 5; ++j) {
            SP_TEST_ASSERT(rv, up.spi16arr[j] == (j + 1) * 1000, "spi16arr");
        }
        SP_TEST_ASSERT(rv, up.spchar == 's', "spchar");
        SP_TEST_ASSERT(rv, memcmp(up.helloW, hW, sizeof hW) == 0, "helloW");
        SP_TEST_ASSERT(rv, memcmp(up.worldU, wU, sizeof wU) == 0, "worldU");

        fmt = fmt_str_le;
        bytes = bytes_le;

        status = "\nTesting little endian data";
    }

    /* Test offset packing */
    struct sp_pack_unpack pack = {
        .hello = "Hello World!",
        .spu32 = 100000,
        .spi64 = -8000000000,
        .spi32 = -100000,
        .spu64 = 8000000000,
        .spi16 = -30000,
        .spu16 = 60000,
        .s_arr[0] = {.spsti64 = -7000000000, .spstu32 = 200000},
        .s_arr[1] = {.spsti64 = -8000000000, .spstu32 = 300000},
        .s_arr[2] = {.spsti64 = -9000000000, .spstu32 = 400000},
        .spi16arr = {1000, 2000, 3000, 4000, 5000},
        .spchar = 's',
        .helloW = {'H', 'e', 'l', 'l', 'o', '\0'},
        .worldU = {'W', 'o', 'r', 'l', 'd', '\0'}
    };

    uint8_t pack_buff[sizeof bytes_be] = {0};
    SP_TEST_ASSERT(rv, sp_pack_bin_offset(fmt_str_be, ARR_LEN(offsets), offsets, &pack, pack_buff, b_sz) == SP_OK, "pack BE");
    SP_TEST_ASSERT(rv, memcmp(pack_buff, bytes_be, b_sz) == 0, "compare BE buffer");
    memset(pack_buff, 0, sizeof pack_buff);

    SP_TEST_ASSERT(rv, sp_pack_bin_offset(fmt_str_le, ARR_LEN(offsets), offsets, &pack, pack_buff, b_sz) == SP_OK, "pack LE");
    SP_TEST_ASSERT(rv, memcmp(pack_buff, bytes_le, sizeof pack_buff) == 0, "compare LE buffer");

    return rv;
}
