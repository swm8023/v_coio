project(coio_example)


macro(target_link_public_lib name)
	target_link_libraries(${name} vcoio vco ev)
	if (WIN32)
		target_link_libraries(${name} Ws2_32)
	else()
		target_link_libraries(${name} pthread)
	endif()
endmacro()

# http server
add_executable(http_server http/http_server.cc)
target_link_public_lib(http_server)
target_link_libraries(http_server http_parser)

# http client
add_executable(http_client http/http_client.cc)
target_link_public_lib(http_client)
target_link_libraries(http_client http_parser)