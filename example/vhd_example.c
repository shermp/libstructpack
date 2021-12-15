#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../src/libstructpack.h"

/* The following structs were adapted from my MiniVHD library to be used
   as an example... */
typedef struct MVHDFooter {
    char cookie[9];
    uint32_t features;
    uint32_t fi_fmt_vers;
    uint64_t data_offset;
    uint32_t timestamp;
    char cr_app[5];
    uint32_t cr_vers;
    char cr_host_os[5];
    uint64_t orig_sz;
    uint64_t curr_sz;
    struct {
        uint16_t cyl;
        uint8_t heads;
        uint8_t spt;
    } geom;
    uint32_t disk_type;
    uint32_t checksum;
    uint8_t uuid[16];
    uint8_t saved_st;
    uint8_t reserved[427];
} MVHDFooter;

typedef struct MVHDSparseHeader {
    char cookie[9];
    uint64_t data_offset;
    uint64_t bat_offset;
    uint32_t head_vers;
    uint32_t max_bat_ent;
    uint32_t block_sz;
    uint32_t checksum;
    uint8_t par_uuid[16];
    uint32_t par_timestamp;
    uint32_t reserved_1;
    uint8_t par_utf16_name[512];
    struct {
        char plat_code[5];
        uint32_t plat_data_space;
        uint32_t plat_data_len;
        uint32_t reserved;
        uint64_t plat_data_offset;
    } par_loc_entry[8];
    uint8_t reserved_2[256];
} MVHDSparseHeader;

struct FooterPtr {
    void* arr[18];
};
struct HeadPtr {
    void* arr[52];
};

struct FooterPtr create_foot_arr(MVHDFooter* footer) {
    struct FooterPtr ptr_arr = {};
    ptr_arr.arr[0] = &footer->cookie;
    ptr_arr.arr[1] = &footer->features;
    ptr_arr.arr[2] = &footer->fi_fmt_vers;
    ptr_arr.arr[3] = &footer->data_offset;
    ptr_arr.arr[4] = &footer->timestamp;
    ptr_arr.arr[5] = &footer->cr_app;
    ptr_arr.arr[6] = &footer->cr_vers;
    ptr_arr.arr[7] = &footer->cr_host_os;
    ptr_arr.arr[8] = &footer->orig_sz;
    ptr_arr.arr[9] = &footer->curr_sz;
    ptr_arr.arr[10] = &footer->geom.cyl;
    ptr_arr.arr[11] = &footer->geom.heads;
    ptr_arr.arr[12] = &footer->geom.spt;
    ptr_arr.arr[13] = &footer->disk_type;
    ptr_arr.arr[14] = &footer->checksum;
    ptr_arr.arr[15] = &footer->uuid;
    ptr_arr.arr[16] = &footer->saved_st;
    ptr_arr.arr[17] = &footer->reserved;
    return ptr_arr;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Expected One argument: path to a sparse or differencing VHD file.\n");
        return EXIT_FAILURE;
    }
    char* vhd_path = argv[1];
    FILE* f = fopen(vhd_path, "rb+");
    if (f == NULL) {
        printf("Could not open %s\n", vhd_path);
        return EXIT_FAILURE;
    }
    int rv = EXIT_SUCCESS;
    MVHDFooter footer = {};
    struct FooterPtr fp = create_foot_arr(&footer);
    void** foot_field_ptr = fp.arr;
    int foot_num_fields = (int)(sizeof fp.arr / sizeof *fp.arr);
    const char* foot_fmt_str = "> [8]s 2I Q I [4]s I [4]s 2Q HBB 2I [16]B B [427]B";

    fseek(f, -512, SEEK_END);
    uint8_t foot_buff[512];
    fread(foot_buff, 512, 1, f);

    SPResult res = sp_unpack_bin_ptr(foot_fmt_str, foot_num_fields, foot_field_ptr, foot_buff, sizeof foot_buff);
    if (res != SP_OK) {
        printf("Oops, something went wrong getting footer...\n");
        rv = EXIT_FAILURE;
        goto cleanup;
    }
    if (strncmp(footer.cookie, "conectix", 8) != 0) {
        printf("The file does not appear to be a VHD image. Aborting.\n");
        rv = EXIT_FAILURE;
        goto cleanup;
    }
    if (footer.data_offset != 0xffffffffffffffff) {
        uint8_t header_buff[1024];
        MVHDSparseHeader header = {};
        size_t head_offset[52];
        head_offset[0] = offsetof(MVHDSparseHeader, cookie);
        head_offset[1] = offsetof(MVHDSparseHeader, data_offset);
        head_offset[2] = offsetof(MVHDSparseHeader, bat_offset);
        head_offset[3] = offsetof(MVHDSparseHeader, head_vers);
        head_offset[4] = offsetof(MVHDSparseHeader, max_bat_ent);
        head_offset[5] = offsetof(MVHDSparseHeader, block_sz);
        head_offset[6] = offsetof(MVHDSparseHeader, checksum);
        head_offset[7] = offsetof(MVHDSparseHeader, par_uuid);
        head_offset[8] = offsetof(MVHDSparseHeader, par_timestamp);
        head_offset[9] = offsetof(MVHDSparseHeader, reserved_1);
        head_offset[10] = offsetof(MVHDSparseHeader, par_utf16_name);
        int j = 11;
        for (int i = 0; i < 8; i++) {
            head_offset[j] = offsetof(MVHDSparseHeader, par_loc_entry[i].plat_code);
            head_offset[j + 1] = offsetof(MVHDSparseHeader, par_loc_entry[i].plat_data_space);
            head_offset[j + 2] = offsetof(MVHDSparseHeader, par_loc_entry[i].plat_data_len);
            head_offset[j + 3] = offsetof(MVHDSparseHeader, par_loc_entry[i].reserved);
            head_offset[j + 4] = offsetof(MVHDSparseHeader, par_loc_entry[i].plat_data_offset);
            j += 5;
        }
        head_offset[51] = offsetof(MVHDSparseHeader, reserved_2);
        int header_num_fields = (int)(sizeof head_offset / sizeof *head_offset);
        const char* header_fmt_str = "> 8s 2Q 4I [16]B 2I [512]B 8(4s 3I Q) [256]B";

        fseek(f, footer.data_offset, SEEK_SET);
        fread(header_buff, sizeof header_buff, 1, f);
        res = sp_unpack_bin_offset(header_fmt_str, header_num_fields, head_offset, &header, header_buff, sizeof header_buff);
        if (res != SP_OK) {
            printf("Oops, something went wrong getting header...\n");
            rv = EXIT_FAILURE;
            goto cleanup;
        }
        memset(header_buff, 0, sizeof header_buff);
        res = sp_pack_bin_offset(header_fmt_str, header_num_fields, head_offset, &header, header_buff, sizeof header_buff);
        if (res != SP_OK) {
            printf("Oops, something went wrong setting header...\n");
            rv = EXIT_FAILURE;
            goto cleanup;
        }
        fseek(f, footer.data_offset, SEEK_SET);
        fwrite(header_buff, sizeof header_buff, 1, f);
    }
    memset(foot_buff, 0, sizeof foot_buff);
    res = sp_pack_bin_ptr(foot_fmt_str, foot_num_fields, foot_field_ptr, foot_buff, sizeof foot_buff);
    if (res != SP_OK) {
        printf("Oops, something went wrong setting footer...\n");
        rv = EXIT_FAILURE;
        goto cleanup;
    }
    fseek(f, -512, SEEK_END);
    fwrite(foot_buff, sizeof foot_buff, 1, f);
cleanup:
    fclose(f);
    return rv;
}
