#pragma once
#ifndef HEADER_GUARD__INTERNEDSYMBOLS_SYMBOLREF_HPP_34980708974
#define HEADER_GUARD__INTERNEDSYMBOLS_SYMBOLREF_HPP_34980708974

//! @file
//! @brief Defines C++ utility classes for using the Interned Symbols module

#include "internedsymbols/CExports.h"

#include <cstring>

//! @brief Root namespace for all things from InternedSymbols library
namespace InternedSymbols
{
    namespace details
    {
        //template<typename T> struct base_traits;
        
        // For all things char_traits<T>
        template<typename T> struct base_traits
        {
            typedef T const * const param_type;
            typedef T char_type;
            typedef T const * str_type;
            typedef std::char_traits< char_type > char_traits;

            struct tag { };

            static str_type raw(param_type str ) { return str; }
            static size_t len( param_type str ) { return char_traits::length( raw(str) ); }
        };

        // For all things basic_string<...>
        template<class _Elem,
	             class _Traits,
	             class _Ax>
        struct base_traits< std::basic_string<_Elem,_Traits,_Ax> >
        {
            typedef std::basic_string<_Elem,_Traits,_Ax> const & param_type;
            typedef _Elem char_type;
            typedef _Elem const * str_type;
            typedef _Traits char_traits;

            struct tag { };

            static str_type raw( param_type str ) { return str.data(); }
            static size_t len( param_type str ) { return char_traits::length( raw(str) ); }
        };

        template<typename T> struct traits;
        template<> struct traits< char const * >    : public base_traits< char >    { };
        template<> struct traits< wchar_t const * > : public base_traits< wchar_t > { };
        template<> struct traits< std::string >     : public base_traits< std::string >  { };
        template<> struct traits< std::wstring >    : public base_traits< std::wstring > { };
    }

    struct Symbol
    {
    private:
        //-------------------------------------------------------------------------------
        // Members

        //! @brief The handle to the referenced symbol
        //! @note We must call release(...) when we're done with this!
        InternHandle_t m_handle;

        //-------------------------------------------------------------------------------
        // Internal utility methods
        template<typename T> struct type_tag : public details::traits<T>::tag { };

        template<typename T>
        static InternHandle_t acquire( T str, type_tag<T> const & _t = type_tag<T>() )
        {
            typedef details::traits<T> traits;
            return acquire( traits::raw(str), traits::len(str) );
        }

        template<typename T>
        static InternHandle_t acquire( T str,
                                       unsigned int const len,
                                       type_tag<T> const & _t = type_tag<T>() )
        {
            typedef typename details::traits<T> traits;
            return acquire( traits::raw(str), len );
        }

        static InternHandle_t acquire( char const * const str, unsigned int len )
        {
            return InternedSymbol_AcquireHandleA( str, len );
        }

        static InternHandle_t acquire( wchar_t const * const str, unsigned int len )
        {
            return InternedSymbol_AcquireHandleW( str, len );
        }

        static InternHandle_t clone( InternHandle_t const & handle )
        {
            InternedSymbol_DuplicateHandle( handle );
            return handle;
        }

        static void release( InternHandle_t const & handle )
        {
            if( handle != 0 )
                InternedSymbol_ReleaseHandle( handle );
        }

        void set( InternHandle_t const & handle )
        {
            release( m_handle );
            m_handle = handle;
        }

    public:
        //-------------------------------------------------------------------------------
        // Constructor/Destructor

        static Symbol const & Empty( )
        {
            static Symbol const s_empty( "", 0 );
            return s_empty;
        }


        Symbol( )
            : m_handle( clone( Empty().m_handle ) )
        {
        }

        Symbol( Symbol const & rhs )
            : m_handle( clone( rhs.m_handle ) )
        {   }

        template<typename T>
        Symbol( T str, type_tag<T> const & _t = type_tag<T>() )
            : m_handle( acquire( str ) )
        {   }

        template<typename T>
        Symbol( T str,
                unsigned int len,
                type_tag<T> const & _t = type_tag<T>() )
            : m_handle( acquire( str, len ) )
        {   }


        ~Symbol( )
        {
            release( m_handle );
        }


        //-------------------------------------------------------------------------------
        // Query

        bool valid( ) const { return (m_handle != 0); }

        bool empty( ) const { return (m_handle == Empty().m_handle); }

        size_t length( ) const { return InternedSymbol_GetLengthW( m_handle ); }

        std::string str( ) const
        {
            enum { BUF_LEN = 1024 };
            char staticBuffer[BUF_LEN];
            std::vector<char> dynamicBuffer;

            unsigned int const lenBytes = InternedSymbol_GetLengthA( m_handle );

            char * buffer = staticBuffer;
            if( lenBytes >= BUF_LEN )
            {
                dynamicBuffer.resize( lenBytes + 1 );
                buffer = &dynamicBuffer[0];
            }

            uint32_t const actualLen = InternedSymbol_CopyToA( m_handle, buffer, lenBytes );

            return std::string( buffer, buffer + actualLen );
        }

        std::wstring wstr( ) const
        {
            struct StrSetter
            {
                static void INTERNEDSYMBOLS_API Callback(
                    void * pUserData,
                    InternHandle_t const handle,
                    wchar_t const * const str,
                    uint32_t const len )
                {
                    std::wstring & ret = *reinterpret_cast<std::wstring*>( pUserData );
                    if( str && len )
                        ret.assign( str, len );
                    else
                        ret.clear( );
                }
            };

            std::wstring ret;
            InternedSymbol_VisitHandleW( m_handle, &StrSetter::Callback, &ret );
            return ret;
        }

        //-------------------------------------------------------------------------------
        // Compare
        int compare( Symbol const & rhs ) const
        {
            return InternedSymbol_Compare( m_handle, rhs.m_handle );
        }

        bool operator==( Symbol const & rhs ) const { return (m_handle == rhs.m_handle); }
        bool operator!=( Symbol const & rhs ) const { return (m_handle != rhs.m_handle); }
        bool operator< ( Symbol const & rhs ) const { return (compare( rhs ) <  0); }
        bool operator<=( Symbol const & rhs ) const { return (compare( rhs ) <= 0); }
        bool operator> ( Symbol const & rhs ) const { return (compare( rhs ) >  0); }
        bool operator>=( Symbol const & rhs ) const { return (compare( rhs ) >= 0); }

        //-------------------------------------------------------------------------------
        // Modification

        void clear( )
        {
            *this = Empty();
        }

        void nullify( )
        {
            set( 0 );
        }

        Symbol & operator=( Symbol const & rhs )
        {
            if( m_handle != rhs.m_handle )
                set( clone( rhs.m_handle ) );
            return *this;
        }

        template<typename T>
        Symbol & operator=( typename details::traits<T>::type const & str )
        {
            set( acquire( str ) );
            return *this;
        }
    };

    namespace { static Symbol const s_EMPTY_STRING( L"" ); }
}

#endif // HEADER_GUARD__INTERNEDSYMBOLS_SYMBOLREF_HPP_34980708974
