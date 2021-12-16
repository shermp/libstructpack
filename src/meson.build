sp_sources = [
    'sp_parser.c',
    'structpack.c'
]

sp_lib = library('structpack', 
                 sp_sources,
                 include_directories : inc,
                 install : true)

sp_obj = sp_lib.extract_objects(sp_sources)
