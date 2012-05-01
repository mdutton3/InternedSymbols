
find_package( CppUnit REQUIRED )

include_directories( . ${CPPUNIT_INCLUDE_DIR} ${InternedSymbols_SOURCE_DIR}/include )

add_library( unittest_helper SHARED helper/helper.cpp )
target_link_libraries( unittest_helper libinternedsymbols )

file(GLOB SRC *.cpp)
add_executable( unittest ${SRC} )
target_link_libraries( unittest unittest_helper libinternedsymbols ${CPPUNIT_LIBRARY} )

set(unittest_stamp "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/unittest.stamp")
add_custom_command(OUTPUT "${unittest_stamp}"
                    COMMAND unittest
                    COMMAND ${CMAKE_COMMAND} -E touch "${unittest_stamp}"
                    COMMENT "Running unit test"
                    DEPENDS unittest)
add_custom_target(unittest_run ALL DEPENDS "${unittest_stamp}")