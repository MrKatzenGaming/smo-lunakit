cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/sys/cmake/toolchain.cmake")
project(${PROJNAME} VERSION 1.0 LANGUAGES ASM C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(config/config.cmake)

file(GLOB_RECURSE SOURCES_ASM ${PROJECT_SOURCE_DIR}/src/*.S)
file(GLOB_RECURSE SOURCES_C ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE SOURCES_CXX ${PROJECT_SOURCE_DIR}/src/*.cpp)
set(SOURCES ${SOURCES_ASM} ${SOURCES_C} ${SOURCES_CXX})
add_executable(${PROJECT_NAME} ${SOURCES})

# add_subdirectory(lib/custom)
# target_link_libraries(${PROJECT_NAME} PUBLIC CustomHeaders)
include_directories(lib/OdysseyHeaders)
add_subdirectory(lib/OdysseyHeaders)
target_link_libraries(${PROJECT_NAME} PUBLIC OdysseyHeaders)

include_directories(src)


include(sys/cmake/module.cmake)
include(sys/cmake/deploy.cmake)
include(sys/cmake/sail.cmake)
usesail(${PROJECT_NAME})

# SMOVER ?= 100
# BUILDVER ?= 101
# BUILDVERSTR ?= $(GIT_REV)
# DEBUGLOG ?= 0 # defaults to disable debug logger
# SERVERIP ?= 0.0.0.0 # put debug logger server IP here
execute_process(
  COMMAND git describe --tags --abbrev=0
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  OUTPUT_VARIABLE GIT_VER
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUGLOG=${DEBUGLOG})
add_compile_definitions(GIT_VER="${GIT_VER}")
add_compile_definitions(SWITCH=1)

if(DEBUG)
  target_compile_definitions(${PROJECT_NAME} PRIVATE SMOOHKDBG)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/include/imconfig.h")
