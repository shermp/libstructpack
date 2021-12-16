# libstructpack - Python inspired C struct (de)serialization

Inspired by Python's `struct` module, this library aims to make it easier to (de)serialize data from files or network to or from C structs, without using packed structs.

## Features

- Use a Python `struct` inspired format string to describe binary data
- 16, 32, 64 bit integers are converted to host byte order depending
- Strings can be null terminated if desired

## Current limitations

- Bitfields are not (directly) supported
- libstructpack works at byte level. It's not possible to copy an arbitrary number of bits
- Floating point numbers are currently not supported
- Member pointers are not followed. Separate calls are required to populate pointers to structures etc

## Building

libstructpack uses the Meson build system. The library itself requires no external dependencies, or compiler flags, so directly including the files in `src` directory, and `include/structpack.h` in your source tree is possible.

To build with meson perform the following:

```
meson setup build
meson compile -C build
```

You can run tests by invoking:
```
meson test -C build
```

It should be compatible on most modern compilers. Basic C99 support is required. Currently MSYS2 (Clang 64) and MSVC 2019 have been tested.

## Usage

libstructpack requires using a format string to describe how to unpack or pack data to or from structs. A detailed description of this is given below.

As well as the format string, a list of struct member offsets, or pointers or pointers is required. libstructpack will use the format string to copy data from a provided buffer to these members, or vice versa.

The following four functions are available:

```c
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
```

For the offset function variants, you only need to create an `offset_list` once per struct definition, along with its format string.

### Format string

The format string is used to determine how bytes of data should 
be interpreted, including size, endianess and layout. It hass 
been inspired by the Python struct library. The behaviour is
NOT identical however.

#### Details

1. The format string may begin with an endian indicator `<` or `>`. 
   Where `<` = little endian, and `>` = big endian.
   If no such identifier is found, little endian is assumed.
2. The format string contains one or more format charachters.
   These characters determine the data type, and whether to
   perform endian handling.
   The currently supported format characters are as follows:

   | FMT_CHAR | C_TYPE             |
   |----------|--------------------|
   | `x`      | none (ignore byte) |
   | `b`      | int8_t             |
   | `B`      | uint8_t            |
   | `h`      | int16_t            |
   | `H`      | uint16_t           |
   | `i`      | int32_t            |
   | `I`      | uint32_t           |
   | `q`      | int64_t            |
   | `Q`      | uint64_t           |
   | `s`      | char[]             |

3. A format character may be preceeded by an integer, which will
   repeat that data type n times. Eg: `III` and `3I` are treated
   the same.
4. A format character preceeded by an integer in square brackets
   `[n]` will be treated as an n-long array of that type.
   Eg: `[10]I` is treated as a single array of 10 uint32_t's
5. Fields may be grouped and repeated using the following syntax:
   `n('sub_format_str')`, where the elements of `sub_format_str`
   will be repeated `n` times.
6. `x` will skip reading/writing bytes to/from the buffer at that
   offset.
7. `<n>s` and `[<n>]s` are treated the same. They are both treated as
   a char array. Null termination is guaranteed, and so the 
   dest. struct field **MUST** be one element longer than the source.
8. Whitespace is (mostly) ignored
