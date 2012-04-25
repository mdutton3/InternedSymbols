
#include <string>

#include "internedsymbols/InternedSymbols_API.h"

struct StaticInit
{
    template<int N>
    StaticInit( wchar_t const (&str)[N] )
    {
        InternedSymbol_GetHandleW( str, N - 1 );
    }
};

StaticInit helper1( L"helper1" );
StaticInit helper2( L"helper2" );
StaticInit helper3( L"helper3" );

InternHandle_t DLLSYMBOL_EXPORT Helper_GetSymbol( std::wstring const & name )
{
    return InternedSymbol_GetHandleW( name.c_str(), name.length() );
}

