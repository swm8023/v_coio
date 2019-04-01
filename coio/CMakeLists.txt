project(coiolib)


macro(add_vg_source_dir name)
    file(GLOB SRC_FILE ${name}/*.h ${name}/*.cc)
	if (WIN32)
		list(FILTER SRC_FILE EXCLUDE  REGEX _posix.cc)
	else()
		list(FILTER SRC_FILE EXCLUDE  REGEX _win.cc)
	endif()
    list(APPEND ALL_SRC ${SRC_FILE})
    string(REPLACE "/" "\\" groupname ${name})
    source_group(${groupname} FILES ${SRC_FILE})
endmacro()


add_vg_source_dir(cosys)
add_vg_source_dir(common)
add_vg_source_dir(net)

add_library(vcoio STATIC ${ALL_SRC})

