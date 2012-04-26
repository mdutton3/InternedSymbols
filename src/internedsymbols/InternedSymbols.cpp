
//! @file
//! @brief Implements methods exported from the Interned Symbols

// Disable warnings about xxx_s replacements, because we have to compile for gcc also
#define _CRT_SECURE_NO_WARNINGS

// We're only using locks from Boost.Thread and don't need auto-tss cleanup
#define BOOST_THREAD_NO_LIB

// I don't even know why Boost is trying to include the Date_Time library
#define BOOST_DATE_TIME_NO_LIB


#include "internedsymbols/InternedSymbols_API.h"
#include "atomic.hpp"

#include <cstring>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>


typedef std::size_t HashVal;

#pragma pack(push, 2)

//---------------------------------------------------------------------------------------
struct InternedSymbol
{
    mutable long m_refCount;  //!< Reference count for acquire/release
    HashVal      m_hash;      //!< A pre-computed hash of the symbol name    
    unsigned int m_length;    //!< Name length in characters, excluding NULL terminator    
    wchar_t      m_name[1];   //!< Start of name array, NULL terminated

    static HashVal hash( wchar_t const * const str, unsigned int const len )
    {
        return boost::hash_range( str, str + len );
    }

    //! @brief Create a symbol instance with a name suffix
    //! @param str The name of the symbol
    //! @param len The length in characters of the symbol name, excluding NULL terminator
    //! @param hash The hash of the symbol name
    static InternedSymbol * create( wchar_t const * const str,
                                    unsigned int len,
                                    HashVal const hash )
    {
        // m_name already has one character, which accounts for the NULL termiator
        unsigned int const size_bytes = sizeof( InternedSymbol ) + len * sizeof(wchar_t);

        // Round the size to a multiple of wchar_t
        unsigned int const num_char = (size_bytes + sizeof(wchar_t) - 1) / sizeof(wchar_t);

        wchar_t * const pRaw = new wchar_t[ num_char ];

        InternedSymbol * const pSymbol = new(pRaw) InternedSymbol( str, len, hash );
        return pSymbol;
    }

    //! @brief Increment the reference count
    void acquire( ) const
    {
        AtomicIncrement( &m_refCount );
    }

    //! @brief Decrement the reference count
    //! @return True if that was the last reference.
    bool release( ) const
    {
        if( m_refCount == 0 )
            throw new std::exception("Released an interned symbol too many times!");

        return (AtomicDecrement( &m_refCount ) == 0);
    }

private:
    //! @brief Initialize a symbol instance
    //! @details This constructor assumes that m_name is long enough to hold
    //!     the symbol name from the str buffer. This is ensured through the
    //!     static create(...) method.
    //! @param str The name of the symbol
    //! @param len The length in characters of the symbol name, excluding NULL terminator
    //! @param hash The hash of the symbol name
    InternedSymbol( wchar_t const * const str,
                    unsigned int len,
                    HashVal const hash )
        : m_hash( hash )
        , m_refCount( 1 )
        , m_length( len )        
    {
        wmemcpy( m_name, str, len );
        m_name[len] = 0;
    }
};

//! m_refCount must be 32-bit aligned. Malloc/new ensures that the structure will be,
//! so as long as the offset is a multiple of 4, we're good.
typedef char s__static_assert__m_refCount_offset4[ ((offsetof(InternedSymbol,m_refCount) & 0x3) == 0) ? 1 : -1 ];

//---------------------------------------------------------------------------------------
class SymbolStore
{
private:
    typedef std::vector<InternedSymbol const *> SymbolBucket;
    typedef boost::unordered_map< HashVal, SymbolBucket > HashMap;
    
    typedef boost::mutex Lock;
    Lock m_symbolLock;
    HashMap m_hashMap;

    SymbolStore() { }
    
public:
    ~SymbolStore( )
    {
        Lock::scoped_lock scopedLock( m_symbolLock );
        BOOST_FOREACH( HashMap::value_type const & hash_pair, m_hashMap )
            BOOST_FOREACH( InternedSymbol const * pSymbol, hash_pair.second )
                delete pSymbol;
    }

    static SymbolStore & Instance()
    {
        static SymbolStore s_instance;
        return s_instance;
    }

    InternedSymbol const * acquireHandle( wchar_t const * const str,
                                    uint32_t const len )
    {
        HashVal const nameHash = InternedSymbol::hash( str, len );

        Lock::scoped_lock scopedLock( m_symbolLock );

        SymbolBucket & bucket = m_hashMap[ nameHash ];

        BOOST_FOREACH( InternedSymbol const * pSymbol, bucket )
        {
            if( wcscmp( pSymbol->m_name, str ) == 0 )
            {   // Matched symbol name
                pSymbol->acquire( );
                return pSymbol;
            }
        }

        // No matches, create one
        InternedSymbol * const pSymbol = InternedSymbol::create( str, len, nameHash );
        bucket.push_back( pSymbol );

        return pSymbol;
    }

    void releaseHandle( InternedSymbol const * const pSymbol )
    {
        Lock::scoped_lock scopedLock( m_symbolLock );

        if( pSymbol->m_refCount > 0 )
            return; // Looks like someone has already reacquired this symbol

        SymbolBucket & bucket = m_hashMap[ pSymbol->m_hash ];
        SymbolBucket::iterator it = std::find( bucket.begin(), bucket.end(), pSymbol );
        if( it != bucket.end () )
        {
            bucket.erase( it );
            delete pSymbol;            
        }
    }
};


//---------------------------------------------------------------------------------------
InternHandle_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_AcquireHandleW(
    wchar_t const * const str,
    uint32_t const len )
{
    return SymbolStore::Instance( ).acquireHandle( str, len );
}

//---------------------------------------------------------------------------------------
void INTERNEDSYMBOLS_DLLAPI InternedSymbol_ReacquireHandle(
    InternHandle_t const handle )
{
    InternedSymbol const * const pSymbol = handle;
    if( pSymbol )
    {
        pSymbol->acquire( );
    }
}

//---------------------------------------------------------------------------------------
void INTERNEDSYMBOLS_DLLAPI InternedSymbol_ReleaseHandle(
    InternHandle_t const handle )
{
    InternedSymbol const * const pSymbol = handle;
    if( pSymbol )
    {
        if( pSymbol->release( ) )
        {
            SymbolStore::Instance().releaseHandle( pSymbol );
        }
    }
}

//---------------------------------------------------------------------------------------
void INTERNEDSYMBOLS_DLLAPI InternedSymbol_ResolveHandleW(
    InternHandle_t const handle,
    wchar_t * const buf,
    uint32_t * len )
{
    using namespace std;
    InternedSymbol const * const pSymbol = handle;
    if( pSymbol )
    {
        uint32_t const maxLen = min(*len,pSymbol->m_length+1);
        wcsncpy( buf, pSymbol->m_name, maxLen );
        buf[*len-1] = 0;
        *len = pSymbol->m_length;
    }
}

//---------------------------------------------------------------------------------------
void INTERNEDSYMBOLS_DLLAPI InternedSymbol_ResolveHandleCallbackW(
    InternHandle_t const handle,
    InternedSymbol_StrSetterW const pCallback,
    void * const pUserData )
{
    InternedSymbol const * const pSymbol = handle;
    if( pSymbol && pCallback )
    {
        pCallback( pUserData, pSymbol->m_name, pSymbol->m_length );
    }  
}

