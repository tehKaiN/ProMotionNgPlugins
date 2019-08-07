cmake_minimum_required(VERSION 2.8.5)
project(promotionPlugins)

set(CMAKE_C_STANDARD 11)
file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE HEADERS src/*.h)
include_directories(
	${PROJECT_SOURCE_DIR}/src
)

add_executable(ehb ${SOURCES} ${HEADERS})
