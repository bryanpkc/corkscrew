cmake_minimum_required(VERSION 3.20)

project(corkscrew LANGUAGES C)

set(VERSION "2.0.win32")

option(ANSI_FUNC "Use ANSI-C functions" 0)

if(MSVC)
	add_compile_options(/W4)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_WINSOCK_DEPRECATED_NO_WARNINGS)
else()
	add_compile_options(-Wall -Werror -pedantic)
endif()

configure_file(config.h.in config.h)
include_directories(${CMAKE_BINARY_DIR})

add_executable(corkscrew corkscrew.c)

install(TARGETS corkscrew)

if(MSVC)
	target_link_libraries(corkscrew Ws2_32)
endif()
