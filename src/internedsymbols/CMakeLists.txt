
file(GLOB SRC *.cpp *.c)
file(GLOB INC *.hpp *.h ${InternedSymbols_SOURCE_DIR}/include/*.h )

include_directories( . ${InternedSymbols_SOURCE_DIR}/include )
add_library( libinternedsymbols SHARED ${SRC} ${INC} ${InternedSymbols_SOURCE_DIR}/README.md )
