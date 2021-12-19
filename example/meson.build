link_args = []
if host_machine.system() == 'windows' and cc.get_id() != 'msvc'
    link_args += ['-municode']
endif

vhd_info_bin = executable('vhd_info', 'vhd_info.c', 
    objects : sp_obj,
    include_directories : inc,
    link_args : link_args)
