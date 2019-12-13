#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "libstructpack.h"

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

struct HeadPtr create_head_arr(MVHDSparseHeader* header) {
    struct HeadPtr ptr_arr = {};
    ptr_arr.arr[0] = &header->cookie;
    ptr_arr.arr[1] = &header->data_offset;
    ptr_arr.arr[2] = &header->bat_offset;
    ptr_arr.arr[3] = &header->head_vers;
    ptr_arr.arr[4] = &header->max_bat_ent;
    ptr_arr.arr[5] = &header->block_sz;
    ptr_arr.arr[6] = &header->checksum;
    ptr_arr.arr[7] = &header->par_uuid;
    ptr_arr.arr[8] = &header->par_timestamp;
    ptr_arr.arr[9] = &header->reserved_1;
    ptr_arr.arr[10] = &header->par_utf16_name;
    int j = 11;
    for (int i = 0; i < 8; i++) {
        ptr_arr.arr[j] = &header->par_loc_entry[i].plat_code;
        ptr_arr.arr[j + 1] = &header->par_loc_entry[i].plat_data_space;
        ptr_arr.arr[j + 2] = &header->par_loc_entry[i].plat_data_len;
        ptr_arr.arr[j + 3] = &header->par_loc_entry[i].reserved;
        ptr_arr.arr[j + 4] = &header->par_loc_entry[i].plat_data_offset;
        j += 5;
    }
    ptr_arr.arr[51] = &header->reserved_2;
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
    SPStructDef sd_footer = {};
    MVHDFooter footer = {};
    struct FooterPtr fp = create_foot_arr(&footer);
    sd_footer.field_ptr = fp.arr;
    sd_footer.num_fields = sizeof fp.arr / sizeof *fp.arr;
    sd_footer.fmt_str = "> [8]s 2I Q I [4]s I [4]s 2Q HBB 2I [16]B B [427]B";

    fseek(f, -512, SEEK_END);
    uint8_t foot_buff[512];
    fread(foot_buff, 512, 1, f);

    SPResult res = sp_unpack_bin(&sd_footer, foot_buff, sizeof foot_buff);
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
        SPStructDef sd_header = {};
        struct HeadPtr hp = create_head_arr(&header);
        sd_header.field_ptr = hp.arr;
        sd_header.num_fields = sizeof hp.arr / sizeof *hp.arr;
        sd_header.fmt_str = "> 8s 2Q 4I [16]B 2I [512]B 8(4s 3I Q) [256]B";

        fseek(f, footer.data_offset, SEEK_SET);
        fread(header_buff, sizeof header_buff, 1, f);
        res = sp_unpack_bin(&sd_header, header_buff, sizeof header_buff);
        if (res != SP_OK) {
            printf("Oops, something went wrong getting header...\n");
            rv = EXIT_FAILURE;
            goto cleanup;
        }
        memset(header_buff, 0, sizeof header_buff);
        res = sp_pack_bin(&sd_header, header_buff, sizeof header_buff);
        if (res != SP_OK) {
            printf("Oops, something went wrong setting header...\n");
            rv = EXIT_FAILURE;
            goto cleanup;
        }
        fseek(f, footer.data_offset, SEEK_SET);
        fwrite(header_buff, sizeof header_buff, 1, f);
    }
    memset(foot_buff, 0, sizeof foot_buff);
    res = sp_pack_bin(&sd_footer, foot_buff, sizeof foot_buff);
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