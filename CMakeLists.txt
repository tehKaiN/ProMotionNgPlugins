cmake_minimum_required(VERSION 2.8.5)
project(promotionPlugins)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES src/*.c src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

include_directories(
	${PROJECT_SOURCE_DIR}/src
)

add_executable(ehb ${SOURCES} ${HEADERS})
target_link_libraries(ehb -static)
