cmake_minimum_required(VERSION 3.0)
project(cmath VERSION "0.1.0" LANGUAGES CXX C)

include(CheckIncludeFile)
include(CheckIncludeFile)

check_include_file(editline/readline.h HAVE_EDITLINE_READLINE_H)
check_include_file(readline/readline.h HAVE_READLINE_READLINE_H)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(cmath STATIC
	src/cmath/expr.cc
	src/cmath/expr_parser.cc
)
set_target_properties(cmath PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

add_executable(cm
	src/cm/console.cc
	src/cm/main.cc
)
set_target_properties(cm PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(cm PRIVATE cmath)
if(HAVE_EDITLINE_READLINE_H)
	target_link_libraries(cm PRIVATE editline)
elseif(HAVE_READLINE_READLINE_H)
	target_link_libraries(cm PRIVATE readline)
else()
	message("*** OOPS: compile w/o such support")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/cmath/sysconfig.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/src/cmath/sysconfig.h)
