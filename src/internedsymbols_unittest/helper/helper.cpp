
#include <string>

#include "internedsymbols/CExports.h"


template<int N>
static InternHandle_t GetHandle( wchar_t const (&str)[N] )
{
    return InternedSymbol_AcquireHandleW( str, N - 1 );
}

struct StaticInit
{
    InternHandle_t handle;

    template<int N>
    StaticInit( wchar_t const (&str)[N] )
    {
        handle = GetHandle( str );
    }

    ~StaticInit( )
    {
        InternedSymbol_ReleaseHandle( handle );
    }
};

// Test that interning works during static initialization
StaticInit helper1( L"helper1" );
StaticInit helper2( L"helper2" );

InternHandle_t DLLSYMBOL_EXPORT Helper_GetHelper1( ) { return helper1.handle; }
InternHandle_t DLLSYMBOL_EXPORT Helper_GetHelper2( ) { return helper2.handle; }

InternHandle_t DLLSYMBOL_EXPORT Helper_GetSymbol( std::wstring const & name )
{
    return InternedSymbol_AcquireHandleW( name.c_str(), name.length() );
}



