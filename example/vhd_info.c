
#if defined(_WIN32)
    #include <windows.h>
#else
    #define _POSIX_C_SOURCE 200808L
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <structpack.h>

#if defined(_WIN32)
    #define vhd_main() wmain(int argc, wchar_t *argv[])
    #define vhd_printf wprintf
    #define fopen _wfopen
    #define _L(str) L ## str
    #define vhd_fseek _fseeki64
#else
    #define vhd_main() main(int argc, char *argv[])
    #define vhd_printf printf
    #define _L(str) str
    #define vhd_fseek fseeko
#endif

#define VHD_FAIL(str, file) fclose(file); vhd_printf(str); return EXIT_FAILURE;

struct vhd_footer {
    char cookie[9];
    int32_t features;
    int32_t fi_fmt_vers;
    int64_t data_offset;
    uint32_t timestamp;
    char cr_app[5];
    uint32_t cr_vers;
    char cr_host_os[5];
    int64_t orig_sz;
    int64_t curr_sz;
    struct {
        uint16_t cyl;
        uint8_t heads;
        uint8_t spt;
    } geom;
    int32_t disk_type;
    uint32_t checksum;
    uint8_t uuid[16];
    uint8_t saved_st;
    uint8_t reserved[427];
};
const char* foot_fmt_str = "> [8]s 2i q I [4]s I [4]s 2q HBB iI [16]B B [427]B";

struct vhd_sparse {
    char cookie[9];
    int64_t data_offset;
    int64_t bat_offset;
    int32_t head_vers;
    int32_t max_bat_ent;
    int32_t block_sz;
    uint32_t checksum;
    uint8_t par_uuid[16];
    uint32_t par_timestamp;
    int32_t reserved_1;
    uint16_t par_utf16_name[257];
    struct {
        char plat_code[5];
        int32_t plat_data_space;
        int32_t plat_data_len;
        int32_t reserved;
        int64_t plat_data_offset;
    } par_loc_entry[8];
    uint8_t reserved_2[256];
};
const char* sparse_fmt_str = "> 8s 2q 3i I [16]B Ii [256]u 8(4s 3i q) [256]B";

#define VHD_PRINT_ROW(label, val, ...) vhd_printf(_L("%-29s : "), label); vhd_printf(val, __VA_ARGS__); vhd_printf(_L("\n"));
#define VHD_PRINT_ROW_A(label, val) vhd_printf(_L("%-29s : "), label); printf("%s", val); vhd_printf(_L("\n"));

const char* disk_type(int32_t type) {
    switch (type) {
        case 2:
            return "fixed";
            break;
        case 3:
            return "sparse";
            break;
        case 4:
            return "differencing";
            break;
        default:
            return "unknown";
    }
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning( disable : 4996)
#endif
int vhd_main() {
    char footer_buf[512] = {0};
    //char sparse_buf[1024] = {0};
    if (argc != 2) {
        vhd_printf(_L("Expected one argument: path to a VHD file.\n"));
        return EXIT_FAILURE;
    }
    vhd_printf(_L("Getting info for VHD file: %s"), argv[1]);
    FILE* vhd_file = fopen(argv[1], _L("rb"));
    if (!vhd_file) {
        vhd_printf(_L("error opening VHD file"));
        return EXIT_FAILURE;
    }
    vhd_fseek(vhd_file, -512ll, SEEK_END);
    fread(footer_buf, 512, 1, vhd_file);
    if (strncmp(footer_buf, "conectix", 8) != 0) {
        VHD_FAIL(_L("cookie string not found. Is file a VHD?"), vhd_file);
    }
    size_t footer_offsets[18] = {0};
    SP_ADD_STRUCT_OFFSET(footer_offsets, 0, struct vhd_footer, cookie, \
        features, fi_fmt_vers, data_offset, timestamp, cr_app, cr_vers, cr_host_os, orig_sz, curr_sz);
    SP_ADD_STRUCT_OFFSET(footer_offsets, 10, struct vhd_footer, geom.cyl, geom.heads, geom.spt, disk_type, \
        checksum, uuid, saved_st, reserved);

    struct vhd_footer footer = {0};
    if (sp_unpack_bin_offset(foot_fmt_str, 18, footer_offsets, &footer, footer_buf, (int)sizeof footer_buf) != SP_OK) {
        VHD_FAIL(_L("failure to unpack vhd"), vhd_file);
    }
    vhd_printf(_L("\n"));
    VHD_PRINT_ROW_A(_L("Cookie"), footer.cookie);
    VHD_PRINT_ROW(_L("Features"), _L("%d"), footer.features);
    VHD_PRINT_ROW(_L("File Format Vers."), _L("%d"), footer.fi_fmt_vers);
    VHD_PRINT_ROW(_L("Sparse Header Offset"), _L("%lld"), (long long)footer.data_offset);
    VHD_PRINT_ROW(_L("Timestamp"), _L("%u"), footer.timestamp);
    VHD_PRINT_ROW_A(_L("Creator App"), footer.cr_app);
    VHD_PRINT_ROW(_L("Creator Vers."), _L("%u"), footer.cr_vers);
    VHD_PRINT_ROW_A(_L("Creator Host OS"), footer.cr_host_os);
    VHD_PRINT_ROW(_L("Original Size"), _L("%lld"), (long long)footer.orig_sz);
    VHD_PRINT_ROW(_L("Current Size"), _L("%lld"), (long long)footer.curr_sz);
    VHD_PRINT_ROW(_L("[Geom] cyl"), _L("%hd"), footer.geom.cyl);
    VHD_PRINT_ROW(_L("[Geom] heads"), _L("%hhu"), footer.geom.heads);
    VHD_PRINT_ROW(_L("[Geom] spt"), _L("%hhu"), footer.geom.spt);
    VHD_PRINT_ROW_A(_L("VHD type"), disk_type(footer.disk_type));
    VHD_PRINT_ROW(_L("Checksum"), _L("%u"), footer.checksum);

    fclose(vhd_file);

    //VHD_PRINT_ROW_A(_L("Cookie"), f)
    return EXIT_SUCCESS;
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
