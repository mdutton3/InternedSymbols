
include_directories( . ${InternedSymbols_SOURCE_DIR}/include )

add_library( unittest_helper SHARED helper/helper.cpp )

file(GLOB SRC *.cpp)
add_executable( unittest ${SRC} )
target_link_libraries( unittest unittest_helper )
