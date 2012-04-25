#pragma once
#ifndef HEADER_GUARD__INTERNEDSYMBOLS_API_INTERNEDSYMBOLS_H_34980708974
#define HEADER_GUARD__INTERNEDSYMBOLS_API_INTERNEDSYMBOLS_H_34980708974
//! @file
//! @brief Defines the C-style interface for the Interned Symbols DLL


#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define INTERNEDSYMBOLS_API __attribute((__stdcall__))
    #ifdef INTERNEDSYMBOLS_EXPORT
      #define INTERNEDSYMBOLS_PUBLIC __attribute__ ((dllexport))
    #else
      #define INTERNEDSYMBOLS_PUBLIC __attribute__ ((dllimport))
    #endif
  #else
    #define INTERNEDSYMBOLS_API __stdcall
    #ifdef INTERNEDSYMBOLS_EXPORT
      #define INTERNEDSYMBOLS_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #else
      #define INTERNEDSYMBOLS_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define INTERNEDSYMBOLS_LOCAL
#else
  #if __GNUC__ >= 4
    #define INTERNEDSYMBOLS_API /*nothing*/
    #define INTERNEDSYMBOLS_PUBLIC __attribute__ ((visibility ("default")))
    #define INTERNEDSYMBOLS_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #error Unknown platform/compiler combo
  #endif
#endif

#define INTERNEDSYMBOLS_DLLAPI INTERNEDSYMBOLS_PUBLIC INTERNEDSYMBOLS_API

/*
#ifdef _MSC_VER
    #define INTERNEDSYMBOLS_API         __stdcall
    #ifdef INTERNEDSYMBOLS_EXPORT
    #   define INTERNEDSYMBOLS_DLLAPI   __declspec(dllexport) __stdcall
    #else
    #   define INTERNEDSYMBOLS_DLLAPI   __declspec(dllimport) __stdcall
    #endif
#else
    #define INTERNEDSYMBOLS_API         __stdcall
    #ifdef INTERNEDSYMBOLS_EXPORT
    #   define INTERNEDSYMBOLS_DLLAPI   __declspec(dllexport) __stdcall
    #else
    #   define INTERNEDSYMBOLS_DLLAPI   __declspec(dllimport) __stdcall
    #endif
#endif
*/

// C-style API
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1600)
    typedef __int32 int32_t;
    typedef unsigned __int32 uint32_t;
#else
    #include <stdint.h>
#endif


    //! @brief Opaque handle to an interned symbol
    typedef struct
    {
        uint32_t value;  //!< Private, do not use
    } InternHandle_t;

    //! @brief Get a new or existing handle to the given symbol name
    //! @param[in] str The ASCII symbol name, case sensitive
    //! @param[in] len The length of the name, in characters
    //! @return A handle to the interned symbol
    //InternHandle_t INTERNEDSYMBOLS_DLLAPI GetSymbolHandleA( char const * const str,
    //                                                        uint32_t const len );

    //! @brief Get a new or existing handle to the given symbol name
    //! @param[in] str The ASCII symbol name, case sensitive
    //! @param[in] len The length of the name, in characters
    //! @return A handle to the interned symbol
    InternHandle_t INTERNEDSYMBOLS_DLLAPI GetSymbolHandleW( wchar_t const * const str,
                                                            uint32_t const len );


    //! @brief Copy the ASCII name of the interned symbol to the buffer
    //! @details This method essentially calls wcstombs underneath after finding the symbol.
    //! @param[in] handle A valid handle to the symbol
    //! @param[out] buf An output buffer to hold the copied name
    //! @param[in,out] len (in) The maximum buffer length, in characters.
    //! @param[in,out] len (out) The actual length of the name, may be larger than the original buffer length
    //void INTERNEDSYMBOLS_DLLAPI ResolveSymbolNameA( InternHandle_t const handle,
    //                                                char * const buf,
    //                                                uint32_t * len );


    //! @brief Copy the UTF-16 name of the interned symbol to the buffer
    //! @details This method essentially calls wcscpy underneath after finding the symbol.
    //! @param[in] handle A valid handle to the symbol
    //! @param[out] buf An output buffer to hold the copied name
    //! @param[in,out] len (in) The maximum buffer length, in characters.
    //! @param[in,out] len (out) The actual length of the name, may be larger than the original buffer length
    void INTERNEDSYMBOLS_DLLAPI ResolveSymbolNameW( InternHandle_t const handle,
                                                    wchar_t * const buf,
                                                    uint32_t * len );


    //! @brief Type of callback function for string setter callback
    //! @details Users can implement callback methods that receive an instance of the
    //!          symbol name directly. This allows the symbol name to be copied in a
    //!          user-specific manner.
    //! @param[in] pUserData The user data supplied with the callback
    //! @param[in] str The NULL-terminated Unicode symbol name
    //! @param[in] len The length of the symbol name, in characters
    typedef void (INTERNEDSYMBOLS_API *InternedSymbol_StrSetterW)( void * pUserData,
                                               wchar_t const * const str,
                                               uint32_t const len );

    //! @brief Call pCallback with the name of the interned symbol
    //! @param[in] handle A valid handle to a symbol
    //! @param[in] pCallback The callback to call with the symbol name
    //! @param[in] pUserData Caller-specific data to be passed to pCallback
    void INTERNEDSYMBOLS_DLLAPI ResolveSymbolNameCallbackW( InternHandle_t const handle,
                                                            InternedSymbol_StrSetterW const pCallback,
                                                            void * const pUserData );

#ifdef __cplusplus
} // end extern "C"
#endif
#endif // HEADER_GUARD__INTERNEDSYMBOLS_API_INTERNEDSYMBOLS_H_34980708974
