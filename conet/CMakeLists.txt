project(conetlib)

enable_language(C ASM)

# select asm file
message(" --- select asm ---")
if (WIN32)
    enable_language(ASM_MASM)
    if (MSVC)
        if (CMAKE_CL_64)
            set(ASM_SUFFIX "x86_64_ms_pe_masm.asm")
        else()
            set(ASM_SUFFIX "i386_ms_pe_masm.asm")
        endif()
    endif()
elseif(UNIX)

endif()

list(APPEND CONET_ASM_FILE "coroutine/asm/make_${ASM_SUFFIX}")
list(APPEND CONET_ASM_FILE "coroutine/asm/jump_${ASM_SUFFIX}")
list(APPEND CONET_ASM_FILE "coroutine/asm/ontop_${ASM_SUFFIX}")
source_group(coroutine\\asm FILES ${CONET_ASM_FILE}})
message("${CONET_ASM_FILE}")

list(APPEND CONET_SRC_LIST ${CONET_ASM_FILE})


# src files
message(" --- add source ---")
macro(add_vg_source_dir name)
    message(${name})
    file(GLOB SRC_FILE ${name}/*.h ${name}/*.cc)
    list(APPEND CONET_SRC_LIST ${SRC_FILE})
    string(REPLACE "/" "\\" groupname ${name})
    source_group(${groupname} FILES ${SRC_FILE})
endmacro()

add_vg_source_dir(common)
add_vg_source_dir(coroutine)
add_vg_source_dir(net)

if (WIN32)
    add_vg_source_dir(coroutine/win)
else()
    add_vg_source_dir(coroutine/posix)
endif()


# 


add_library(conet STATIC ${CONET_SRC_LIST})
