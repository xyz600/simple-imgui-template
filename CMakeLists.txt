cmake_minimum_required(VERSION 3.10.0 FATAL_ERROR)

#
# PROJECT
#

project(cmake_sample_all CXX)

#
# STATIC VARIABLES
#

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "CMAKE_BUILD_TYPE not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # EXPORT COMPILE COMMANDS

#
# OUTPUTS
#

set(OUTPUT_DEBUG Debug/bin)
set(OUTPUT_RELEASE Release/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE
    "${CMAKE_BINARY_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE
    "${CMAKE_BINARY_DIR}/${OUTPUT_RELEASE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE
    "${CMAKE_BINARY_DIR}/${OUTPUT_RELEASE}")

#
# COMPILER/LINKER FLAGS
#

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DNDEBUG)
endif()

if(MSVC)
  set(CMAKE_MSVC_PARALLEL_ENABLE ON)
  set(
    CMAKE_CXX_FLAGS
    "/source-charset:utf-8 /permissive- /MP /GS /Zc:rvalueCast /W4 /Zc:wchar_t /sdl /Zc:inline /fp:precise /errorReport:prompt /WX- /Zc:forScope /Gd /FC /EHsc /diagnostics:classic"
    )
  # replace /MT and /MTd by /MDd /MDd for dynamic linking, respectively
  set(CMAKE_CXX_FLAGS_DEBUG "/JMC /ZI /Od /RTC1 /FAcs /MTd")
  set(CMAKE_CXX_FLAGS_RELEASE "/GL /Gy /Zi /Gm- /O2 /Oi /MT")
  add_definitions(/D_CONSOLE /DUNICODE /D_UNICODE)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(/D_DEBUG)
  endif()
  set(CMAKE_EXE_LINKER_FLAGS
      "/NXCOMPAT /DYNAMICBASE /DEBUG /ERRORREPORT:PROMPT")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/INCREMENTAL")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE
      "/LTCG:incremental /INCREMENTAL:NO /OPT:REF /OPT:ICF")
elseif(UNIX AND NOT APPLE)
  if(NOT EXISTS ${CMAKE_CXX_COMPILER})
    set(CMAKE_CXX_COMPILER "clang++")
    message(
      STATUS "CMAKE_CXX_COMPILER is not specified: Use clang++ by default.")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
  # set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native")
  set(
    CMAKE_CXX_FLAGS
    "-Wall -Wextra -Wcast-align -Wcast-qual -Wc++17-compat -Wdelete-non-virtual-dtor -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wpointer-arith -Wfloat-equal -Winit-self -Wredundant-decls -Wconversion -Wshadow -Wsign-promo -Wswitch-default -Wswitch-enum -Wvariadic-macros -Wwrite-strings -Wno-unknown-pragmas"
    )
endif()

#
# TIDY
#

option(CLANG_TIDY "Perform ClangTidy" OFF)
option(CLANG_TIDY_FIX "Perform fixes for ClangTidy" OFF)

if(CLANG_TIDY)
  # Add clang-tidy if available
  find_program(CLANG_TIDY_EXE
               NAMES "clang-tidy"
               DOC "Path to clang-tidy executable")
  if(CLANG_TIDY_EXE)
    if(CLANG_TIDY_FIX)
      set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}" "-fix")
    else()
      set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
  endif()
else()
  unset(CLANG_TIDY_EXE CACHE)
  unset(CMAKE_CXX_CLANG_TIDY CACHE)
endif()

#
# EXPORT COMPILE COMMANDS
#
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(${CMAKE_EXPORT_COMPILE_COMMANDS} STREQUAL ON)
  add_custom_target(gen_compile_commands ALL
                    COMMENT "Generating compile_commands.json")
  add_custom_command(TARGET gen_compile_commands PRE_BUILD
                     COMMAND ${CMAKE_COMMAND}
                             -E
                             remove
                             ${CMAKE_SOURCE_DIR}/compile_commands.json)
  find_program(SED sed)
  if(SED)
    add_custom_command(TARGET gen_compile_commands PRE_BUILD
                       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                       COMMAND sed
                               -i
                               -e
                               's/nvcc/clang++/g'
                               -e
                               's/-x cuda/-x cu/g'
                               -e
                               's/-x cu/-x cuda/g'
                               ${CMAKE_BINARY_DIR}/compile_commands.json
                       COMMENT "Replacing strings")
  endif()
  find_program(COMPDB compdb)
  if(COMPDB)
    add_custom_command(TARGET gen_compile_commands PRE_BUILD
                       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                       COMMAND ${COMPDB}
                               list
                               >
                               ${CMAKE_SOURCE_DIR}/compile_commands.json
                       COMMENT "Running compdb")
  else()
    message(AUTHOR_WARNING "'compdb' not found")
    add_custom_command(TARGET gen_compile_commands PRE_BUILD
                       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                       COMMAND ${CMAKE_COMMAND}
                               -E
                               create_symlink
                               ${CMAKE_BINARY_DIR}/compile_commands.json
                               ${CMAKE_SOURCE_DIR}/compile_commands.json
                       COMMENT "Creating symbolic link")
  endif()
endif()

#
# SUBDIRECTORIES
#

# main project
add_subdirectory(src)

#
# CTest
#

enable_testing()
# add_subdirectory(test)
