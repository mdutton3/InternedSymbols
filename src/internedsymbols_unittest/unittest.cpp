// Copyright 2012

#include <cppunit/extensions/HelperMacros.h>

#define CPPUNIT_TEST_NAME   InternedSymbol_UnitTest

class CPPUNIT_TEST_NAME : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CPPUNIT_TEST_NAME );
        CPPUNIT_TEST( test1 );
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() { }
    void tearDown( ) { }

    void test1()
    {

    }
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CPPUNIT_TEST_NAME );

