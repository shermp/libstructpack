#include <stdint.h>
#include <string.h>

#include <structpack.h>
#include <sp_struct_offsets.h>
#include "sp_test.h"

#define ARR_LEN(arr) sizeof arr / sizeof arr[0]

struct unpack {
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
};
const char unpack_fmt_be[] = ">12s IqiQhH 3(qI) [5]h b";
const char unpack_fmt_le[] = "<12s IqiQhH 3(qI) [5]h b";

/* The following byte sequence was generated with
 * struct.pack('>12sIqiQhHqIqIqI5hc', b'Hello World!', 100000, -8000000000, -100000, 8000000000, -30000, 60000, -7000000000, 200000, 
 *   -8000000000, 300000, -9000000000, 400000, 1000, 2000, 3000, 4000, 5000, b's')
 */

uint8_t bytes_be[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x00, 0x01, 0x86, 0xa0, 0xff, 0xff, 0xff, 0xfe, 0x23, 0x29, 0xb0, 0x00, 0xff, 0xfe, 0x79, 0x60, 0x00, 0x00, 0x00, 0x01, 0xdc, 0xd6, 0x50, 0x00, 0x8a, 0xd0, 0xea, 0x60, 0xff, 0xff, 0xff, 0xfe, 0x5e, 0xc4, 0x7a, 0x00, 0x00, 0x03, 0x0d, 0x40, 0xff, 0xff, 0xff, 0xfe, 0x23, 0x29, 0xb0, 0x00, 0x00, 0x04, 0x93, 0xe0, 0xff, 0xff, 0xff, 0xfd, 0xe7, 0x8e, 0xe6, 0x00, 0x00, 0x06, 0x1a, 0x80, 0x03, 0xe8, 0x07, 0xd0, 0x0b, 0xb8, 0x0f, 0xa0, 0x13, 0x88, 0x73};
uint8_t bytes_le[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0xa0, 0x86, 0x01, 0x00, 0x00, 0xb0, 0x29, 0x23, 0xfe, 0xff, 0xff, 0xff, 0x60, 0x79, 0xfe, 0xff, 0x00, 0x50, 0xd6, 0xdc, 0x01, 0x00, 0x00, 0x00, 0xd0, 0x8a, 0x60, 0xea, 0x00, 0x7a, 0xc4, 0x5e, 0xfe, 0xff, 0xff, 0xff, 0x40, 0x0d, 0x03, 0x00, 0x00, 0xb0, 0x29, 0x23, 0xfe, 0xff, 0xff, 0xff, 0xe0, 0x93, 0x04, 0x00, 0x00, 0xe6, 0x8e, 0xe7, 0xfd, 0xff, 0xff, 0xff, 0x80, 0x1a, 0x06, 0x00, 0xe8, 0x03, 0xd0, 0x07, 0xb8, 0x0b, 0xa0, 0x0f, 0x88, 0x13, 0x73};

static int rv = 0;

int main(void) {
    size_t offsets[15] = {0};
    SP_ADD_STRUCT_OFFSET_7(offsets, 0, struct unpack, hello, spu32, spi64, spi32, spu64, spi16, spu16);
    for (int i = 0; i < 3; ++i) {
        SP_ADD_STRUCT_OFFSET_2(offsets, 7 + (2 * i), struct unpack, s_arr[i].spsti64, s_arr[i].spstu32);
    }
    SP_ADD_STRUCT_OFFSET_2(offsets, 13, struct unpack, spi16arr, spchar);

    const char *fmt = unpack_fmt_be;
    uint8_t *bytes = bytes_be;
    int b_sz = (int)(sizeof bytes_be);
    const char *status;
    status = "\nTesting big endian data";
    for (int i = 0; i < 2; ++i) {
        printf("%s\n", status);
        struct unpack up = {0};
        SP_TEST_ASSERT(rv, sp_unpack_bin_offset(fmt, ARR_LEN(offsets), offsets, &up, bytes, b_sz) == SP_OK, "unpack");
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
            SP_TEST_ASSERT(rv, up.spi16arr[j] == (j + 1) * 1000, "spi16arr")
        }
        SP_TEST_ASSERT(rv, up.spchar == 's', "spchar");

        fmt = unpack_fmt_le;
        bytes = bytes_le;
        b_sz = (int)(sizeof bytes_le);

        status = "\nTesting little endian data";
    }
    return rv;
}
