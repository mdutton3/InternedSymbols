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
        SymbolRef const empty;
        SymbolRef const cstr    (  "char-string" );
        SymbolRef const cstrlen (  "char-string~~~", 11 );
        SymbolRef const wcstr   ( L"wchar-string" );
        SymbolRef const wcstrlen( L"wchar-string~~~", 12 );
        SymbolRef const str     ( std::string(  "char-string") );
        SymbolRef const strlen  ( std::string(  "char-string~~~"), 11 );
        SymbolRef const wstr    ( std::wstring(L"wchar-string") );
        SymbolRef const wstrlen ( std::wstring(L"wchar-string~~~"), 12 );
        SymbolRef const copy_empty( empty );
        SymbolRef const copy_char ( cstr  );
        SymbolRef const copy_wchar( wcstr );

        // Ensure that they constructed something that makes sense
        CPPUNIT_ASSERT( empty.empty() );
        CPPUNIT_ASSERT( 0 == empty.length() );
        CPPUNIT_ASSERT( empty == SymbolRef( L"" ) );
        
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
        CPPUNIT_ASSERT( &SymbolRef::Empty() == &SymbolRef::Empty() );

        CPPUNIT_ASSERT( 0 != SymbolRef::Empty( ).handle( ) );
        CPPUNIT_ASSERT( 0 == InternedSymbol_GetLengthW( SymbolRef::Empty().handle() ) );
        CPPUNIT_ASSERT( 0 == SymbolRef::Empty().length() );
        CPPUNIT_ASSERT( SymbolRef::Empty().empty() );

        CPPUNIT_ASSERT( SymbolRef::Empty() == SymbolRef() );
        CPPUNIT_ASSERT( SymbolRef::Empty() == SymbolRef("") );
        CPPUNIT_ASSERT( SymbolRef::Empty() == SymbolRef(L"") );
        CPPUNIT_ASSERT( SymbolRef::Empty() != SymbolRef("NotEmpty") );
    }

    void testEmptyQuery( )
    {
        CPPUNIT_ASSERT( SymbolRef::Empty().empty() );
        CPPUNIT_ASSERT( SymbolRef().empty() );
        CPPUNIT_ASSERT( SymbolRef("").empty() );
        CPPUNIT_ASSERT( SymbolRef(L"").empty() );
        CPPUNIT_ASSERT( SymbolRef( std::string() ).empty() );
        CPPUNIT_ASSERT( SymbolRef( std::wstring() ).empty() );

        CPPUNIT_ASSERT( ! SymbolRef("NotEmpty").empty() );

        CPPUNIT_ASSERT( ! SymbolRef("\x00InnerNull", 10).empty() );
    }

    void testLength( )
    {
        CPPUNIT_ASSERT(  0 == SymbolRef::Empty().length() );
        CPPUNIT_ASSERT(  0 == SymbolRef().length() );
        CPPUNIT_ASSERT(  0 == SymbolRef("").length() );
        CPPUNIT_ASSERT(  0 == SymbolRef(L"").length() );
        CPPUNIT_ASSERT(  0 == SymbolRef( std::string() ).length() );
        CPPUNIT_ASSERT(  0 == SymbolRef( std::wstring() ).length() );

        CPPUNIT_ASSERT(  1 == SymbolRef("1").length() );
        CPPUNIT_ASSERT(  6 == SymbolRef("FooBar").length() );

        CPPUNIT_ASSERT( 10 == SymbolRef("\x00InnerNull", 10).length() );
    }
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CPPUNIT_TEST_NAME );

