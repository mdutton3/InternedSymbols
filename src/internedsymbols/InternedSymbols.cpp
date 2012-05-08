
//! @file
//! @brief Implements methods exported from the Interned Symbols

// Disable warnings about xxx_s replacements, because we have to compile for gcc also
#define _CRT_SECURE_NO_WARNINGS

// We're only using locks from Boost.Thread and don't need auto-tss cleanup
#define BOOST_THREAD_NO_LIB

// I don't even know why Boost is trying to include the Date_Time library
#define BOOST_DATE_TIME_NO_LIB


#include "internedsymbols/CExports.h"
#include "atomic.hpp"

#include <cstring>
#include <vector>
#include <stdexcept>
#include <memory>

#include <boost/foreach.hpp>
#include <boost/functional/hash.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>

// Windows headers were included somewhere along the way
// causing stupid namespace collisions
#ifdef min
#undef min
#undef max
#endif

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
    //! @param len The length in characters of the symbol name, excluding NULL terminator
    //! @param hash The hash of the symbol name
    static InternedSymbol * create( unsigned int len,
                                    HashVal const hash )
    {
        // m_name already has one character, which accounts for the NULL termiator
        unsigned int const size_bytes = sizeof( InternedSymbol ) + len * sizeof(wchar_t);

        // Round the size to a multiple of wchar_t
        unsigned int const num_char = (size_bytes + sizeof(wchar_t) - 1) / sizeof(wchar_t);

        wchar_t * const pRaw = new wchar_t[ num_char ];

        InternedSymbol * const pSymbol = new(pRaw) InternedSymbol( len, hash );
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
            throw new std::invalid_argument("Released an interned symbol too many times!");

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
    InternedSymbol( unsigned int len,
                    HashVal const hash )
        : m_hash( hash )
        , m_refCount( 1 )
        , m_length( len )
    {
        m_name[0] = 0;
    }
};

//! m_refCount must be 32-bit aligned. Malloc/new ensures that the structure will be,
//! so as long as the offset is a multiple of 4, we're good.
typedef char s__static_assert__m_refCount_offset4[ ((offsetof(InternedSymbol,m_refCount) & 0x3) == 0) ? 1 : -1 ];


//---------------------------------------------------------------------------------------
//! Call a functor for each wide-character converted from a multi-byte string
//! Excludes the terminating NULL character!
//! @param mbstr The multibyte string to iterator/convert
//! @param mblen The number of bytes in the multi-byte string
//! @param fn The functor to call with each converted wide-character
template<typename FUNC>
static inline void foreach_mbc( char const * mbstr, uint32_t mblen, FUNC & fn )
{
    mbstate_t state = { 0 };

    wchar_t wc = 0;

    while(mblen > 0)
    {
        int const ret = mbrtowc( &wc, mbstr, mblen, &state );
        if( ret < 0 )
            break;

        fn( wc );

        // mbrtowc returns 0 when *mbstr is the NULL terminator
        // but we still need to skip over the NULL terminator.
        if( ret != 0 )
        {
            mbstr += ret;
            mblen -= ret;
        }
        else
        {
            // As a MBCS could be using any encoding, it seems plausible
            // that the MBCS NULL terminator is larger than one byte
            // in some encodings.
            //
            // The fact that strlen et al. are compatible with MBCS
            // suggests that the NULL terminator is simply just 0x00.
            //
            // C99, Section 7.20.7.3 "The wctomb function" states:
            // "If wc is a null wide character, a null byte is stored,
            //  preceded by any shift sequence needed to restore the
            //  initial shift state"
            // This suggests that multiple MBCS bytes could be decoded
            // to produce the wide-char NULL but the final byte should
            // be 0x00.
            while( mblen > 0 && *mbstr != 0 ) // Skip shift sequence
            {
                ++mbstr;
                --mblen;
            }
            if( mblen > 0 && *mbstr == 0 ) // Skip NULL terminator
            {
                ++mbstr;
                --mblen;

                // The documentation says that the new state is the
                // "initial conversion state" but its ambiguous (to me)
                // whether that means zero-initialized or the state
                // before the call. We reset explicitly here to be sure.
                mbstate_t newState = {0};
                state = newState;
            }
        }
    } // while(mblen > 0)
}

// Functor for accumulating a wide string and its hash code
template<int LEN>
struct WSBufHashAccum
{
    enum { MAX_LEN = LEN };
    wchar_t buffer[ MAX_LEN ];
    unsigned int wlen;
    HashVal seed;

    WSBufHashAccum( ) : wlen(0), seed() { }

    inline void operator()( wchar_t const wc )
    {
        if( wlen < MAX_LEN )
            buffer[wlen] = wc;
        ++wlen;
        boost::hash_combine(seed, wc);
    }
};

// Functor for writing wide characters to a wide string buffer
struct WSBufAppender
{
    wchar_t * wstr;
    inline void operator()( wchar_t const wc )
    {
        *wstr = wc;
        ++wstr;
    }
};

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

    static InternedSymbol const * SearchBucket( SymbolBucket const & bucket,
                                                wchar_t const * const str,
                                                unsigned int const len )
    {
        BOOST_FOREACH( InternedSymbol const * pSymbol, bucket )
        {
            if( (pSymbol->m_length == len) &&
                (wmemcmp( pSymbol->m_name, str, len ) == 0) )
            {   // Matched symbol name
                return pSymbol;
            }
        }

        return 0;
    }

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

    InternedSymbol const * acquireHandle( char const * const str,
                                          uint32_t const len )
    {
        // Compute the wide-char length and hash
        // We have to compute the length manually ahead of time
        // because str may not be NULL terminated and all the ANSI
        // string conversion methods expect it to be.
        // We cache the string as we convert to prevent re-converting later, if it fits.
        typedef WSBufHashAccum<512> BufferAccum;
        BufferAccum mbstr_accum;
        foreach_mbc( str, len, mbstr_accum );

        // Convienent aliases
        HashVal const nameHash = mbstr_accum.seed;
        unsigned int const wcLen = mbstr_accum.wlen;

        // We converted the string as we computed the hash and it fit, just use it
        if( wcLen <= BufferAccum::MAX_LEN )
        {
            return acquireHandle( mbstr_accum.buffer, wcLen, nameHash );
        }

        // We create this early here because we'll use it as a buffer
        std::auto_ptr<InternedSymbol> pTemp( InternedSymbol::create( wcLen, nameHash ) );
        wchar_t const * pNameBuf = pTemp->m_name;

        // Transform to wide string directly into symbol name buffer
        WSBufAppender mbstr_xform = { pTemp->m_name };
        foreach_mbc( str, len, mbstr_xform );
        *(mbstr_xform.wstr) = 0; // Ensure NULL termination

        // Look for an existing entry that matches
        Lock::scoped_lock scopedLock( m_symbolLock );
        SymbolBucket & bucket = m_hashMap[ nameHash ];
        if( InternedSymbol const * pSymbol = SearchBucket( bucket, pNameBuf, len ) )
        {
            pSymbol->acquire( );
            return pSymbol;
        }

        // No matches, use the temp
        bucket.push_back( pTemp.get() );
        return pTemp.release();
    }

    InternedSymbol const * acquireHandle( wchar_t const * const str,
                                          uint32_t const len )
    {
        HashVal const nameHash = InternedSymbol::hash( str, len );
        return acquireHandle( str, len, nameHash );
    }

    InternedSymbol const * acquireHandle( wchar_t const * const str,
                                          uint32_t const len,
                                          HashVal const nameHash )
    {
        Lock::scoped_lock scopedLock( m_symbolLock );
        SymbolBucket & bucket = m_hashMap[ nameHash ];

        if( InternedSymbol const * pSymbol = SearchBucket( bucket, str, len ) )
        {
            pSymbol->acquire( );
            return pSymbol;
        }

        // No matches, create one
        InternedSymbol * const pSymbol = InternedSymbol::create( len, nameHash );
        wmemcpy( pSymbol->m_name, str, len );
        pSymbol->m_name[len] = 0; // Ensure NULL termination
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
InternHandle_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_AcquireHandleA(
    char const * const str,
    uint32_t const len )
{
    return SymbolStore::Instance( ).acquireHandle( str, len );
}

//---------------------------------------------------------------------------------------
InternHandle_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_AcquireHandleW(
    wchar_t const * const str,
    uint32_t const len )
{
    return SymbolStore::Instance( ).acquireHandle( str, len );
}

//---------------------------------------------------------------------------------------
void INTERNEDSYMBOLS_DLLAPI InternedSymbol_DuplicateHandle(
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
uint32_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_GetLengthA(
    InternHandle_t const handle )
{
    uint32_t ret = 0;

    InternedSymbol const * const pSymbol = handle;
    if( pSymbol )
    {
        size_t const len = wcstombs( NULL, pSymbol->m_name, 0 );
        if (len >= 0)
            ret = uint32_t(len);
    }

    return ret;
}

//---------------------------------------------------------------------------------------
uint32_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_GetLengthW(
    InternHandle_t const handle )
{
    InternedSymbol const * const pSymbol = handle;
    if( pSymbol )
        return pSymbol->m_length;
    else
        return 0;
}

//---------------------------------------------------------------------------------------
int32_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_Compare( InternHandle_t const lhs,
                                                       InternHandle_t const rhs )
{
    using std::min;

    if( lhs == rhs )
        return 0; // Equal handle <==> Equal strings
    else
    {
        InternedSymbol const * const pLeft = lhs;
        InternedSymbol const * const pRight = rhs;

        // String may be different lengths
        // We can only compare the longest prefix of the two strings
        int const minLen = min( pLeft->m_length, pRight->m_length );
        int const result = wmemcmp( pLeft->m_name, pRight->m_name, minLen );

        // Since we might be comparing prefixes, instead of full strings,
        // the meaning of wmemcmp(...)==0 has changed.
        // We follow the convention of the documented by GNU that shorter
        // strings precede longer strings, with equal prefixes.
        int const lenDiff = pLeft->m_length - pRight->m_length; // (x<0) if left is shorter than right, etc.
        return ( result == 0 ) ? lenDiff : result;
    }
}

//---------------------------------------------------------------------------------------
int32_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_CopyToA(
    InternHandle_t const handle,
    char * const buf,
    int32_t const len )
{
    using namespace std;
    InternedSymbol const * const pSymbol = handle;
    if( pSymbol == 0 )
        return 0;

    size_t const req_len = wcstombs( NULL, pSymbol->m_name, 0 );
    if( len > 0 )
    {
        size_t const min_len = min( size_t(len), req_len );
        size_t const ret = wcstombs( buf, pSymbol->m_name, min_len );
        // Ensure NULL termination
        if( size_t(-1) == ret )
            buf[0] = 0;
        else
            buf[ min( ret, size_t(len - 1) ) ] = 0;
    }
    return req_len;
}

//---------------------------------------------------------------------------------------
int32_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_CopyToW(
    InternHandle_t const handle,
    wchar_t * const buf,
    int32_t const len )
{
    using namespace std;
    InternedSymbol const * const pSymbol = handle;
    if( pSymbol == 0 )
        return 0;

    if( len > 0 )
    {
        uint32_t const maxLen = min( uint32_t(len), pSymbol->m_length+1 );
        wmemcpy( buf, pSymbol->m_name, maxLen );
        buf[len-1] = 0;
    }

    return pSymbol->m_length;
}

//---------------------------------------------------------------------------------------
void INTERNEDSYMBOLS_DLLAPI InternedSymbol_VisitHandleW(
    InternHandle_t const handle,
    InternedSymbol_CallbackFnW const pCallback,
    void * const pUserData )
{
    InternedSymbol const * const pSymbol = handle;
    if( pSymbol )
    {
        pCallback( pUserData, handle, pSymbol->m_name, pSymbol->m_length );
    }
}

