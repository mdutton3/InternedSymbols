// Copyright 2012

#define _CRT_SECURE_NO_WARNINGS

#include <cppunit/extensions/HelperMacros.h>

#include "internedsymbols/CExports.h"

#include <ctime>
#include <cstdlib>
#include <cstring>

#define CPPUNIT_TEST_NAME   CExports_Test

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
        CPPUNIT_TEST( testNullTerminator );
        CPPUNIT_TEST( testCaseSensitivity );
        CPPUNIT_TEST( testResolve );
        CPPUNIT_TEST( testResolveLen );
        CPPUNIT_TEST( testResolveCallback );
        CPPUNIT_TEST( testGetLength );
        CPPUNIT_TEST( testCompare );
        CPPUNIT_TEST( testNoncontentiousSpeed );
        CPPUNIT_TEST( testManyNames );
        CPPUNIT_TEST( testStringEncoding );
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

    void testNullTerminator( )
    {
        InternHandle_t const len5Null = GetHandle( L"12345" );
        InternHandle_t const len5NoNull = InternedSymbol_AcquireHandleW( L"12345x", 5 );
        InternHandle_t const embeddedNull = GetHandle( L"yyyy\x0000yyyy" );

        CPPUNIT_ASSERT( 0 != len5Null   );
        CPPUNIT_ASSERT( 0 != len5NoNull );
        CPPUNIT_ASSERT( len5Null == len5NoNull );

        CPPUNIT_ASSERT( 5 == InternedSymbol_GetLength( len5Null ) );
        CPPUNIT_ASSERT( 5 == InternedSymbol_GetLength( len5NoNull ) );

        CPPUNIT_ASSERT( 0 != embeddedNull );
        CPPUNIT_ASSERT( 9 == InternedSymbol_GetLength( embeddedNull ) );

        InternedSymbol_ReleaseHandle( len5Null );
        InternedSymbol_ReleaseHandle( len5NoNull );
        InternedSymbol_ReleaseHandle( embeddedNull );
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

        InternedSymbol_CopyToW( handle, buffer, &len );
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
        InternedSymbol_CopyToW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[nameLen] );

        len = nameLen;
        InternedSymbol_CopyToW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[nameLen - 1] );

        len = nameLen - 1;
        InternedSymbol_CopyToW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[nameLen - 1 - 1] );

        len = nameLen - 2;
        InternedSymbol_CopyToW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[nameLen - 2 - 1] );

        len = 1;
        InternedSymbol_CopyToW( handle, buffer, &len );
        CPPUNIT_ASSERT( nameLen == len );
        CPPUNIT_ASSERT( 0 == buffer[1 - 1] );
    }

    void testGetLength( )
    {
        enum { BUF_LEN = 1024 };
        wchar_t buffer[BUF_LEN+1];
        for( unsigned int i = 0; i < BUF_LEN; ++i )
            buffer[ i ] = L'x';
        buffer[ BUF_LEN ] = 0;

        InternHandle_t const null      = 0;
        InternHandle_t const empty     = GetHandle( L"" );
        InternHandle_t const len1      = GetHandle( L"1" );
        InternHandle_t const len10     = GetHandle( L"123456789A" );
        InternHandle_t const len256    = InternedSymbol_AcquireHandleW( buffer, 256 );
        InternHandle_t const lenBufLen = InternedSymbol_AcquireHandleW( buffer, BUF_LEN );

        CPPUNIT_ASSERT(       0 == InternedSymbol_GetLength( null   ) );
        CPPUNIT_ASSERT(       0 == InternedSymbol_GetLength( empty  ) );
        CPPUNIT_ASSERT(       1 == InternedSymbol_GetLength( len1   ) );
        CPPUNIT_ASSERT(      10 == InternedSymbol_GetLength( len10  ) );
        CPPUNIT_ASSERT(     256 == InternedSymbol_GetLength( len256 ) );
        CPPUNIT_ASSERT( BUF_LEN == InternedSymbol_GetLength( lenBufLen ) );

        InternedSymbol_ReleaseHandle( null );
        InternedSymbol_ReleaseHandle( empty );
        InternedSymbol_ReleaseHandle( len1 );
        InternedSymbol_ReleaseHandle( len10 );
        InternedSymbol_ReleaseHandle( len256 );
        InternedSymbol_ReleaseHandle( lenBufLen );
    }

    void testCompare( )
    {
        InternHandle_t const handleA  = GetHandle( L"aFoo" );
        InternHandle_t const handleB  = GetHandle( L"bFoo" );
        InternHandle_t const handleB2 = GetHandle( L"bFoo" );
        InternHandle_t const handleC  = GetHandle( L"cFoo" );

        CPPUNIT_ASSERT( 0 != handleA );
        CPPUNIT_ASSERT( 0 != handleB );
        CPPUNIT_ASSERT( 0 != handleB2 );
        CPPUNIT_ASSERT( 0 != handleC );

        CPPUNIT_ASSERT( handleA  != handleB  );
        CPPUNIT_ASSERT( handleA  != handleB2 );
        CPPUNIT_ASSERT( handleA  != handleC  );
        CPPUNIT_ASSERT( handleB  == handleB2 );
        CPPUNIT_ASSERT( handleB  != handleC  );
        CPPUNIT_ASSERT( handleB2 != handleC  );

        CPPUNIT_ASSERT( InternedSymbol_Compare( handleA,  handleA  ) == 0 );
        CPPUNIT_ASSERT( InternedSymbol_Compare( handleB,  handleB  ) == 0 );
        CPPUNIT_ASSERT( InternedSymbol_Compare( handleB2, handleB2 ) == 0 );
        CPPUNIT_ASSERT( InternedSymbol_Compare( handleC,  handleC  ) == 0 );

        CPPUNIT_ASSERT( InternedSymbol_Compare( handleA,  handleB  ) < 0 );
        CPPUNIT_ASSERT( InternedSymbol_Compare( handleA,  handleB2 ) < 0 );
        CPPUNIT_ASSERT( InternedSymbol_Compare( handleA,  handleC  ) < 0 );
        CPPUNIT_ASSERT( InternedSymbol_Compare( handleB,  handleC  ) < 0 );

        CPPUNIT_ASSERT( InternedSymbol_Compare( handleB,  handleA  ) > 0 );
        CPPUNIT_ASSERT( InternedSymbol_Compare( handleB2, handleA  ) > 0 );
        CPPUNIT_ASSERT( InternedSymbol_Compare( handleC,  handleA  ) > 0 );
        CPPUNIT_ASSERT( InternedSymbol_Compare( handleC,  handleB  ) > 0 );

        InternedSymbol_ReleaseHandle( handleA  );
        InternedSymbol_ReleaseHandle( handleB  );
        InternedSymbol_ReleaseHandle( handleB2 );
        InternedSymbol_ReleaseHandle( handleC  );
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
                                                      InternHandle_t const handle,
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

        InternedSymbol_VisitHandleW( handle, &StrSetter::Callback, &setter );
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
        enum { NUM_ROUND = 1000 };
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

    void testStringEncoding( )
    {
        static char    const mbliteral[] =  "FooBar";
        static wchar_t const wcliteral[] = L"FooBar";

        char    mbbuf[100];
        wchar_t wcbuf[100];

        InternHandle_t handle1 = InternedSymbol_AcquireHandleA( mbliteral, 6 );
        InternHandle_t handle2 = InternedSymbol_AcquireHandleW( wcliteral, 6 );
        CPPUNIT_ASSERT( 0 != handle1 );
        CPPUNIT_ASSERT( 0 != handle2 );
        CPPUNIT_ASSERT( handle1 == handle2 );
        uint32_t len;

        InternedSymbol_CopyToA( handle1, mbbuf, &(len = 100) );
        CPPUNIT_ASSERT( 0 == strncmp( mbliteral, mbbuf, 100 ) );
        InternedSymbol_CopyToW( handle1, wcbuf, &(len = 100) );
        CPPUNIT_ASSERT( 0 == wcsncmp( wcliteral, wcbuf, 100 ) );

        InternedSymbol_CopyToA( handle2, mbbuf, &(len = 100) );
        CPPUNIT_ASSERT( 0 == strncmp( mbliteral, mbbuf, 100 ) );
        InternedSymbol_CopyToW( handle2, wcbuf, &(len = 100) );
        CPPUNIT_ASSERT( 0 == wcsncmp( wcliteral, wcbuf, 100 ) );


        wchar_t wcstr[] = L"\x03A6\x039A\x03A3";
        size_t const ret = wcstombs( mbbuf, wcstr, 100 );
        if( size_t(-1) != ret )
        {   // If we can't convert this string into MBCS, then there's no point
            // in doing this test.
            InternHandle_t escape1 = InternedSymbol_AcquireHandleA( mbbuf, 3 );
            InternHandle_t escape2 = InternedSymbol_AcquireHandleW( wcstr, 3 );
            CPPUNIT_ASSERT( 0 != escape1 );
            CPPUNIT_ASSERT( 0 != escape2 );
            CPPUNIT_ASSERT( escape1 == escape2 );
        }
    }
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CPPUNIT_TEST_NAME );

