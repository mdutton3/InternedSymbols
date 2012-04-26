// Copyright 2012

#include <cppunit/extensions/HelperMacros.h>

#include "internedsymbols/InternedSymbols_API.h"

#include <ctime>


#define CPPUNIT_TEST_NAME   InternedSymbol_UnitTest

// Gets handles for the two symbols interned during static initialization
InternHandle_t DLLSYMBOL_IMPORT Helper_GetHelper1( );
InternHandle_t DLLSYMBOL_IMPORT Helper_GetHelper2( );

// Interns a symbol through the helper DLL to test interning across DLLs
InternHandle_t DLLSYMBOL_IMPORT Helper_GetSymbol( std::wstring const & name );

//! @brief Get the length of the string in characters, excluding the NULL terminator
template<typename CHAR, unsigned int N>
unsigned int StrLength( CHAR const (&str)[N] )
{
    return N - 1;
}

//! @brief Get a handle for the literal string
template<unsigned int N>
InternHandle_t GetHandle( wchar_t const (&str)[N] )
{
    return InternedSymbol_AcquireHandleW( str, StrLength(str) );
}


class CPPUNIT_TEST_NAME : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CPPUNIT_TEST_NAME );
        CPPUNIT_TEST( testOtherDLL );
        CPPUNIT_TEST( testSharedSymbols );
        CPPUNIT_TEST( testCaseSensitivity );
        CPPUNIT_TEST( testResolve );
        CPPUNIT_TEST( testResolveLen );
        CPPUNIT_TEST( testResolveCallback );
        CPPUNIT_TEST( testNoncontentiousSpeed );
        CPPUNIT_TEST( testManyNames );
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() { }
    void tearDown( ) { }

    void testOtherDLL()
    {
        InternHandle_t const helper1 = Helper_GetHelper1();
        CPPUNIT_ASSERT( 0 != helper1 );
        CPPUNIT_ASSERT( helper1 == Helper_GetHelper1() );

        InternHandle_t const helper2 = Helper_GetHelper2();
        CPPUNIT_ASSERT( 0 != helper2 );
        CPPUNIT_ASSERT( helper1 != helper2 );

        InternHandle_t const helper2manual = Helper_GetSymbol( L"helper2" );
        CPPUNIT_ASSERT( helper2 == helper2manual );
    }

    void testSharedSymbols( )
    {
        InternHandle_t const helper1 = Helper_GetHelper1();
        InternHandle_t const local1 = GetHandle( L"helper1" );
        CPPUNIT_ASSERT( 0 != local1 );
        CPPUNIT_ASSERT( helper1 == local1 );

        InternHandle_t const newSym1 = GetHandle( L"newSym1" );
        CPPUNIT_ASSERT( 0 != newSym1 );
        CPPUNIT_ASSERT( newSym1 != helper1 );
        CPPUNIT_ASSERT( newSym1 != local1 );

        InternHandle_t const newSym2 = GetHandle( L"newSym2" );
        CPPUNIT_ASSERT( 0 != newSym2 );
        CPPUNIT_ASSERT( newSym1 != newSym2 );

        InternHandle_t const newSym2_again = GetHandle( L"newSym2" );
        CPPUNIT_ASSERT( 0 != newSym2_again );
        CPPUNIT_ASSERT( newSym2 == newSym2_again );
    }

    void testCaseSensitivity( )
    {
        InternHandle_t const lower = GetHandle( L"case" );
        InternHandle_t const upper = GetHandle( L"CASE" );
        CPPUNIT_ASSERT( 0 != lower );
        CPPUNIT_ASSERT( 0 != upper );
        CPPUNIT_ASSERT( lower != upper );
    }

    template<int N>
    void testResolveName( wchar_t const (&str)[N] )
    {
        InternHandle_t const handle = GetHandle( str );
        CPPUNIT_ASSERT( 0 != handle );

        enum { BUF_LEN = 100 };
        wchar_t buffer[BUF_LEN];
        unsigned int len = BUF_LEN;

        InternedSymbol_ResolveHandleW( handle, buffer, &len );
        CPPUNIT_ASSERT( StrLength(str) == len );
        CPPUNIT_ASSERT( 0 == buffer[len] );
        CPPUNIT_ASSERT( wcscmp( str, buffer ) == 0 );
    }

    void testResolve( )
    {
        testResolveName( L"helper1" );
        testResolveName( L"local1" );
    }

    void testResolveLen( )
    {
        wchar_t const symbolName[] = L"symbolName";
        unsigned int const nameLen = StrLength( symbolName );

        InternHandle_t const handle = GetHandle( symbolName );

        enum { BUF_LEN = 100 };
        wchar_t buffer[BUF_LEN];
        unsigned int len = BUF_LEN;

        len = BUF_LEN;
        InternedSymbol_ResolveHandleW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[nameLen] );

        len = nameLen;
        InternedSymbol_ResolveHandleW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[nameLen - 1] );

        len = nameLen - 1;
        InternedSymbol_ResolveHandleW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[nameLen - 1 - 1] );

        len = nameLen - 2;
        InternedSymbol_ResolveHandleW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[nameLen - 2 - 1] );

        len = 1;
        InternedSymbol_ResolveHandleW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[1 - 1] );
    }

    void testResolveCallback( )
    {
        wchar_t const symbolName[] = L"symbolName";
        unsigned int const nameLen = StrLength( symbolName );

        InternHandle_t const handle = GetHandle( symbolName );

        struct StrSetter
        {
            wchar_t const * const str;
            unsigned int const len;

            static void INTERNEDSYMBOLS_API Callback( void * pUserData,
                                                      wchar_t const * const cbStr,
                                                      uint32_t const cbLen )
            {
                CPPUNIT_ASSERT( 0 != pUserData );
                StrSetter * const pThis = reinterpret_cast<StrSetter *>( pUserData );

                CPPUNIT_ASSERT( (void*)pThis->str != (void*)cbStr );
                CPPUNIT_ASSERT( pThis->len == cbLen );
                CPPUNIT_ASSERT( 0 == pThis->str[ pThis->len ] );
                CPPUNIT_ASSERT( 0 == cbStr[cbLen] );
                CPPUNIT_ASSERT( 0 == wcscmp( pThis->str, cbStr ) );
            }
        } setter = { symbolName, nameLen };

        InternedSymbol_ResolveHandleCallbackW( handle, &StrSetter::Callback, &setter );
    }

    void testNoncontentiousSpeed( )
    {
        typedef wchar_t const Name[7];
        static Name const names[100] = {
            L"test00", L"test01", L"test02", L"test03", L"test04", L"test05", L"test06", L"test07", L"test08", L"test09",
            L"test10", L"test11", L"test12", L"test13", L"test14", L"test15", L"test16", L"test17", L"test18", L"test19",
            L"test20", L"test21", L"test22", L"test23", L"test24", L"test25", L"test26", L"test27", L"test28", L"test29",
            L"test30", L"test31", L"test32", L"test33", L"test34", L"test35", L"test36", L"test37", L"test38", L"test39",
            L"test40", L"test41", L"test42", L"test43", L"test44", L"test45", L"test46", L"test47", L"test48", L"test49",
            L"test50", L"test51", L"test52", L"test53", L"test54", L"test55", L"test56", L"test57", L"test58", L"test59",
            L"test60", L"test61", L"test62", L"test63", L"test64", L"test65", L"test66", L"test67", L"test68", L"test69",
            L"test70", L"test71", L"test72", L"test73", L"test74", L"test75", L"test76", L"test77", L"test78", L"test79",
            L"test80", L"test81", L"test82", L"test83", L"test84", L"test85", L"test86", L"test87", L"test88", L"test89",
            L"test90", L"test91", L"test92", L"test93", L"test94", L"test95", L"test96", L"test97", L"test98", L"test99"
        };

        clock_t const startTime = clock( );
        enum { NUM_ROUND = 10000 };
        for( unsigned int round = 0; round < NUM_ROUND; ++round )
        {
            for( unsigned int iName = 0; iName < 100; ++iName )
                GetHandle( names[iName] );
        }
        clock_t const endTime = clock( );
        clock_t const deltaCold = endTime - startTime;
        double const msCold = deltaCold * 1000.0 / CLOCKS_PER_SEC;

        std::wcout << std::endl;
        std::wcout << L"It took " << msCold << L" ms to do " << NUM_ROUND << L" rounds of 100 names" << std::endl;
    }

    InternHandle_t manyNames_handles[100000];

    void testManyNames_acquire( )
    {
        wchar_t name[] = L"manyname00000";
        unsigned int const nameLen = StrLength( name );

        int count = 0;
        for( int a = 0; a < 10; ++a )
        {
            for( int b = 0; b < 10; ++b )
            {
                for( int c = 0; c < 10; ++c )
                {
                    for( int d = 0; d < 10; ++d )
                    {
                        for( int e = 0; e < 10; ++e )
                        {
                            name[ 8] = L'0' + a;
                            name[ 9] = L'0' + b;
                            name[10] = L'0' + c;
                            name[11] = L'0' + d;
                            name[12] = L'0' + e;
                            manyNames_handles[count++] = GetHandle( name );
                        }
                    }
                }
            }
        }
    } // end testManyNames_acquire    

    void testManyNames_duplicate( )
    {
        int count = 0;
        for( int count = 0; count < 100000; ++count )
        {
            InternHandle_t const handle = manyNames_handles[count];
            InternedSymbol_DuplicateHandle( handle );
        }
    } // end testManyNames_duplicate

    void testManyNames_release( )
    {
        int count = 0;
        for( int count = 0; count < 100000; ++count )
        {
            InternHandle_t const handle = manyNames_handles[count];
            InternedSymbol_ReleaseHandle( handle );
        }
    } // end testManyNames_release


    typedef void (CPPUNIT_TEST_NAME::* TestFnPtr)(void);

    double timeTest( TestFnPtr pTest )
    {
        clock_t const startTime = clock( );
            ((*this).*(pTest))( );
        clock_t const endTime = clock( );
        clock_t const delta = endTime - startTime;
        double const ms = delta * 1000.0 / CLOCKS_PER_SEC;
        return ms;
    }

    void testManyNames( )
    {
        double const msCold = timeTest( &CPPUNIT_TEST_NAME::testManyNames_acquire );
        double const msHot = timeTest( &CPPUNIT_TEST_NAME::testManyNames_acquire );
        double const msReacquire = timeTest( &CPPUNIT_TEST_NAME::testManyNames_duplicate );
        timeTest( &CPPUNIT_TEST_NAME::testManyNames_release );
        double const msRelease = timeTest( &CPPUNIT_TEST_NAME::testManyNames_release );       
        double const msDealloc = timeTest( &CPPUNIT_TEST_NAME::testManyNames_release );

        std::wcout << std::endl;
        std::wcout << L"It took " << msCold      << L" ms to acquire 100000 new names" << std::endl;
        std::wcout << L"It took " << msHot       << L" ms to acquire 100000 existing names" << std::endl;
        std::wcout << L"It took " << msReacquire << L" ms to reacquire 100000 existing names" << std::endl;
        std::wcout << L"It took " << msRelease   << L" ms to release 100000 existing names" << std::endl;
        std::wcout << L"It took " << msDealloc   << L" ms to dealloc 100000 existing names" << std::endl;
    }
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CPPUNIT_TEST_NAME );

