file(GLOB InternedSymbols_SRC *.cpp *.c)

include_directories( . ${InternedSymbols_SOURCE_DIR}/include )
add_library( libinternedsymbols SHARED ${InternedSymbols_SRC} )
