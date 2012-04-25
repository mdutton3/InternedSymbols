#pragma once
#ifndef HEADER_GUARD__INTERNEDSYMBOLS_API_INTERNEDSYMBOLS_H_34980708974
#define HEADER_GUARD__INTERNEDSYMBOLS_API_INTERNEDSYMBOLS_H_34980708974
//! @file
//! @brief Defines the C-style interface for the Interned Symbols DLL

#include <windows.h>

#ifndef _MSC_VER
#error This library was only written/tested on Windows with Visual Studio. It needs review before using with something else.
#endif

#define INTERNEDSYMBOLS_API         __stdcall
#ifdef INTERNEDSYMBOLS_EXPORT
#   define INTERNEDSYMBOLS_DLLAPI   /*__declspec(dllexport) __stdcall*/
#else
#   define INTERNEDSYMBOLS_DLLAPI   __declspec(dllimport) __stdcall
#endif

// C-style API
#ifdef __cplusplus
extern "C" {
#endif

    //! @brief Opaque handle to an interned symbol
    typedef struct
    {
        unsigned __int32 value;  //!< Private, do not use
    } InternHandle_t;

    //! @brief Get a new or existing handle to the given symbol name
    //! @param[in] str The ASCII symbol name, case sensitive
    //! @param[in] len The length of the name, in characters
    //! @return A handle to the interned symbol
    //INTERNEDSYMBOLS_DLLAPI InternHandle_t GetSymbolHandleA( char const * const str,
    //                                                        unsigned __int32 const len );

    //! @brief Get a new or existing handle to the given symbol name
    //! @param[in] str The ASCII symbol name, case sensitive
    //! @param[in] len The length of the name, in characters
    //! @return A handle to the interned symbol
    INTERNEDSYMBOLS_DLLAPI InternHandle_t GetSymbolHandleW( wchar_t const * const str,
                                                            unsigned __int32 const len );


    //! @brief Copy the ASCII name of the interned symbol to the buffer
    //! @details This method essentially calls wcstombs underneath after finding the symbol.
    //! @param[in] handle A valid handle to the symbol
    //! @param[out] buf An output buffer to hold the copied name
    //! @param[in,out] len (in) The maximum buffer length, in characters.
    //! @param[in,out] len (out) The actual length of the name, may be larger than the original buffer length
    //INTERNEDSYMBOLS_DLLAPI void ResolveSymbolNameA( InternHandle_t const handle,
    //                                                char * const buf,
    //                                                unsigned __int32 * len );


    //! @brief Copy the UTF-16 name of the interned symbol to the buffer
    //! @details This method essentially calls wcscpy underneath after finding the symbol.
    //! @param[in] handle A valid handle to the symbol
    //! @param[out] buf An output buffer to hold the copied name
    //! @param[in,out] len (in) The maximum buffer length, in characters.
    //! @param[in,out] len (out) The actual length of the name, may be larger than the original buffer length
    INTERNEDSYMBOLS_DLLAPI void ResolveSymbolNameW( InternHandle_t const handle,
                                                    wchar_t * const buf,
                                                    unsigned __int32 * len );


    //! @brief Type of callback function for string setter callback
    //! @details Users can implement callback methods that receive an instance of the
    //!          symbol name directly. This allows the symbol name to be copied in a
    //!          user-specific manner.
    //! @param[in] pUserData The user data supplied with the callback
    //! @param[in] str The NULL-terminated Unicode symbol name
    //! @param[in] len The length of the symbol name, in characters
    typedef void (*InternedSymbol_StrSetterW)( void * pUserData,
                                               wchar_t const * const str,
                                               unsigned __int32 const len );

    //! @brief Call pCallback with the name of the interned symbol
    //! @param[in] handle A valid handle to a symbol
    //! @param[in] pCallback The callback to call with the symbol name
    //! @param[in] pUserData Caller-specific data to be passed to pCallback
    INTERNEDSYMBOLS_DLLAPI void ResolveSymbolNameCallbackW( InternHandle_t const handle,
                                                            InternedSymbol_StrSetterW const pCallback,
                                                            void * const pUserData );

#ifdef __cplusplus
} // end extern "C"
#endif
#endif // HEADER_GUARD__INTERNEDSYMBOLS_API_INTERNEDSYMBOLS_H_34980708974