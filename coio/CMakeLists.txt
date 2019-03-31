project(libcoio)


macro(add_vg_source_dir name)
    message(${name})
    file(GLOB SRC_FILE ${name}/*.h ${name}/*.cc)
	if (WIN32)
		message("win32 ${SRC_FILE}")
		list(FILTER SRC_FILE EXCLUDE  REGEX _posix.cc)
		message("win32 ${SRC_FILE}")
	else()
		list(FILTER SRC_FILE EXCLUDE  REGEX _win.cc)
	endif()
	message("${SRC_FILE}")
    list(APPEND ALL_SRC ${SRC_FILE})
    string(REPLACE "/" "\\" groupname ${name})
    source_group(${groupname} FILES ${SRC_FILE})
endmacro()


add_vg_source_dir(cosys)
add_vg_source_dir(common)
add_vg_source_dir(net)
add_vg_source_dir(codec)

add_library(vcoio STATIC ${ALL_SRC})

