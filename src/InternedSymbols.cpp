
//! @file
//! @brief Implements methods exported from the Interned Symbols

#include "InternedSymbols_API.h"

#include <cstring>
#include <vector>

#include <boost/functional/hash.hpp>

//---------------------------------------------------------------------------------------
struct Symbol
{
    InternHandle_t m_handle;    //!< The handle assigned to this symbol
    std::size_t    m_hash;      //!< A pre-computed hash of the symbol name
    std::wstring   m_name;      //!< The symbol name

    Symbol( InternHandle_t const & handle, std::size_t const hash, std::wstring const & name )
        : m_handle( handle )
        , m_hash( hash )
        , m_name( name )
    {   }
};

//---------------------------------------------------------------------------------------
class SymbolStore
{
private:
    std::vector<Symbol> m_symbols;

public:
    static SymbolStore & Instance()
    {
        static SymbolStore s_instance;
        return s_instance;
    }

    inline InternHandle_t getHandle( wchar_t const * const str,
                                     unsigned __int32 const len )
    {
        std::size_t const nameHash = boost::hash_range( str, str + len );

        for( std::vector<Symbol>::const_iterator it = m_symbols.begin();
             it != m_symbols.end(); ++it )
        {
            if( (nameHash == it->m_hash) &&
                (0 == it->m_name.compare(0, it->m_name.length(), str, len)) )
            {
                return it->m_handle;
            }                
        }

        // No matches, create one
        InternHandle_t const handle = { m_symbols.size() + 1 };
        m_symbols.push_back( Symbol( handle, nameHash, std::wstring(str,len) ) );

        return handle;
    }

    inline Symbol const * resolveHandle( InternHandle_t const & handle )
    {
        if( 0 == handle.value )
            return 0;
        else if( m_symbols.size() < handle.value )
            return 0;
        else
            return &m_symbols[ handle.value - 1 ];
    }
};


//---------------------------------------------------------------------------------------
InternHandle_t GetSymbolHandleW( wchar_t const * const str,
                                 unsigned __int32 const len )
{
    return SymbolStore::Instance( ).getHandle( str, len );
}


//---------------------------------------------------------------------------------------
void ResolveSymbolNameW( InternHandle_t const handle,
                         wchar_t * const buf,
                         unsigned __int32 * len )
{
    Symbol const * const pSymbol = SymbolStore::Instance().resolveHandle( handle );
    if( pSymbol )
    {
        wcsncpy_s( buf, *len, pSymbol->m_name.c_str(), _TRUNCATE );
        *len = pSymbol->m_name.length( );
    }
}

//---------------------------------------------------------------------------------------
void ResolveSymbolNameCallbackW( InternHandle_t const handle,
                                 InternedSymbol_StrSetterW const pCallback,
                                 void * const pUserData )
{
    Symbol const * const pSymbol = SymbolStore::Instance().resolveHandle( handle );
    if( pSymbol && pCallback )
    {
        pCallback( pUserData, pSymbol->m_name.c_str(), pSymbol->m_name.length() );
    }  
}

