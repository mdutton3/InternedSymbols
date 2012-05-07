// Copyright 2012

#define _CRT_SECURE_NO_WARNINGS

#include <cppunit/extensions/HelperMacros.h>

#include "internedsymbols/SymbolRef.hpp"

#include <cstdlib>
#include <cstring>

#define CPPUNIT_TEST_NAME   SymbolRef_Test

using namespace InternedSymbols;

class CPPUNIT_TEST_NAME : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CPPUNIT_TEST_NAME );
        CPPUNIT_TEST( testConstructors );
        CPPUNIT_TEST( testEmptyInstance );
        CPPUNIT_TEST( testEmptyQuery );
        CPPUNIT_TEST( testLength );
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() { }
    void tearDown( ) { }

    void testConstructors( )
    {
        // Exercise each constructor to catch compile errors
        Symbol const empty;
        Symbol const cstr    (  "char-string" );
        Symbol const cstrlen (  "char-string~~~", 11 );
        Symbol const wcstr   ( L"wchar-string" );
        Symbol const wcstrlen( L"wchar-string~~~", 12 );
        Symbol const str     ( std::string(  "char-string") );
        Symbol const strlen  ( std::string(  "char-string~~~"), 11 );
        Symbol const wstr    ( std::wstring(L"wchar-string") );
        Symbol const wstrlen ( std::wstring(L"wchar-string~~~"), 12 );
        Symbol const copy_empty( empty );
        Symbol const copy_char ( cstr  );
        Symbol const copy_wchar( wcstr );

        // Ensure that they constructed something that makes sense
        CPPUNIT_ASSERT( empty.empty() );
        CPPUNIT_ASSERT( 0 == empty.length() );
        CPPUNIT_ASSERT( empty == Symbol( L"" ) );
        
        CPPUNIT_ASSERT( !cstr.empty() );
        CPPUNIT_ASSERT( 11 == cstr.length() );
        CPPUNIT_ASSERT( !cstrlen.empty() );
        CPPUNIT_ASSERT( 11 == cstrlen.length() );
        CPPUNIT_ASSERT( cstr == cstrlen );

        CPPUNIT_ASSERT( !wcstr.empty() );
        CPPUNIT_ASSERT( 12 == wcstr.length() );
        CPPUNIT_ASSERT( !wcstrlen.empty() );
        CPPUNIT_ASSERT( 12 == wcstrlen.length() );
        CPPUNIT_ASSERT( wcstr == wcstrlen );

        CPPUNIT_ASSERT( !str.empty() );
        CPPUNIT_ASSERT( 11 == str.length() );
        CPPUNIT_ASSERT( !strlen.empty() );
        CPPUNIT_ASSERT( 11 == strlen.length() );
        CPPUNIT_ASSERT( str == strlen );
        CPPUNIT_ASSERT( cstr == str );

        CPPUNIT_ASSERT( !wstr.empty() );
        CPPUNIT_ASSERT( 12 == wstr.length() );
        CPPUNIT_ASSERT( !wstrlen.empty() );
        CPPUNIT_ASSERT( 12 == wstrlen.length() );
        CPPUNIT_ASSERT( wstr == wstrlen );
        CPPUNIT_ASSERT( wcstr == wstr );

        CPPUNIT_ASSERT( empty == copy_empty );
        CPPUNIT_ASSERT( cstr  == copy_char  );
        CPPUNIT_ASSERT( wcstr == copy_wchar );
    }

    void testEmptyInstance( )
    {
        CPPUNIT_ASSERT( &Symbol::Empty() == &Symbol::Empty() );

        CPPUNIT_ASSERT( 0 != Symbol::Empty( ).handle( ) );
        CPPUNIT_ASSERT( 0 == InternedSymbol_GetLengthW( Symbol::Empty().handle() ) );
        CPPUNIT_ASSERT( 0 == Symbol::Empty().length() );
        CPPUNIT_ASSERT( Symbol::Empty().empty() );

        CPPUNIT_ASSERT( Symbol::Empty() == Symbol() );
        CPPUNIT_ASSERT( Symbol::Empty() == Symbol("") );
        CPPUNIT_ASSERT( Symbol::Empty() == Symbol(L"") );
        CPPUNIT_ASSERT( Symbol::Empty() != Symbol("NotEmpty") );
    }

    void testEmptyQuery( )
    {
        CPPUNIT_ASSERT( Symbol::Empty().empty() );
        CPPUNIT_ASSERT( Symbol().empty() );
        CPPUNIT_ASSERT( Symbol("").empty() );
        CPPUNIT_ASSERT( Symbol(L"").empty() );
        CPPUNIT_ASSERT( Symbol( std::string() ).empty() );
        CPPUNIT_ASSERT( Symbol( std::wstring() ).empty() );

        CPPUNIT_ASSERT( ! Symbol("NotEmpty").empty() );

        CPPUNIT_ASSERT( ! Symbol("\x00InnerNull", 10).empty() );
    }

    void testLength( )
    {
        CPPUNIT_ASSERT(  0 == Symbol::Empty().length() );
        CPPUNIT_ASSERT(  0 == Symbol().length() );
        CPPUNIT_ASSERT(  0 == Symbol("").length() );
        CPPUNIT_ASSERT(  0 == Symbol(L"").length() );
        CPPUNIT_ASSERT(  0 == Symbol( std::string() ).length() );
        CPPUNIT_ASSERT(  0 == Symbol( std::wstring() ).length() );

        CPPUNIT_ASSERT(  1 == Symbol("1").length() );
        CPPUNIT_ASSERT(  6 == Symbol("FooBar").length() );

        CPPUNIT_ASSERT( 10 == Symbol("\x00InnerNull", 10).length() );
    }
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CPPUNIT_TEST_NAME );

