#pragma once
#ifndef HEADER_GUARD__INTERNEDSYMBOLS_API_INTERNEDSYMBOLS_H_34980708974
#define HEADER_GUARD__INTERNEDSYMBOLS_API_INTERNEDSYMBOLS_H_34980708974
//! @file
//! @brief Defines the C-style interface for the Interned Symbols DLL

#if !defined(DLLSYMBOL_EXPORT) && !defined(DLLSYMBOL_IMPORT) && !defined(DLLSYMBOL_CALL)
  #if __GNUC__ >= 4
    #define DLLSYMBOL_CALL   /*nothing*/
    #define DLLSYMBOL_EXPORT __attribute__ ((visibility ("default")))
    #define DLLSYMBOL_IMPORT __attribute__ ((visibility ("default")))
    #define DLLSYMBOL_HIDDEN __attribute__ ((visibility ("hidden")))
  #elif defined(__GNUC__)
    #define DLLSYMBOL_CALL   __attribute((__stdcall__))
    #define DLLSYMBOL_EXPORT __attribute__ ((dllexport))
    #define DLLSYMBOL_IMPORT __attribute__ ((dllimport))
    #define DLLSYMBOL_HIDDEN /*nothing*/
  #elif defined(_MSC_VER)
    #define DLLSYMBOL_CALL   __stdcall
    #define DLLSYMBOL_EXPORT __declspec(dllexport)
    #define DLLSYMBOL_IMPORT __declspec(dllimport)
    #define DLLSYMBOL_HIDDEN /*nothing*/
  #else
    #error Unknown compiler
  #endif
#endif


#define INTERNEDSYMBOLS_API DLLSYMBOL_CALL

#if defined(INTERNEDSYMBOLS_EXPORT)
    #define INTERNEDSYMBOLS_PUBLIC DLLSYMBOL_EXPORT
    #define INTERNEDSYMBOLS_DLLAPI DLLSYMBOL_EXPORT INTERNEDSYMBOLS_API
#else
    #define INTERNEDSYMBOLS_PUBLIC DLLSYMBOL_IMPORT
    #define INTERNEDSYMBOLS_DLLAPI DLLSYMBOL_IMPORT INTERNEDSYMBOLS_API
#endif


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
    struct InternedSymbol;
    typedef struct InternedSymbol const * InternHandle_t;

    //! @brief Get a new or existing handle to the given symbol name
    //! @param[in] str The ASCII symbol name, case sensitive
    //! @param[in] len The length of the name, in characters
    //! @return A handle to the interned symbol
    //InternHandle_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_AcquireHandleA(
    //    char const * const str,
    //    uint32_t const len );

    //! @brief Get a new or existing handle to the given symbol name
    //! @param[in] str The Unicode symbol name, case sensitive
    //! @param[in] len The length of the name, in characters
    //! @return A handle to the interned symbol
    InternHandle_t INTERNEDSYMBOLS_DLLAPI InternedSymbol_AcquireHandleW(
        wchar_t const * const str,
        uint32_t const len );

    //! @brief Reacquire an existing handle
    //! @details This increase the reference count to the handle and is more efficient
    //!     than calling InternedSymbol_AcquireHandleT again with the symbol name.
    //! @param[in] handle The valid handle to acquire
    void INTERNEDSYMBOLS_DLLAPI InternedSymbol_DuplicateHandle(
        InternHandle_t const handle );

    //! @brief Release an existing handle acquired with InternedSymbol_AcquireHandleT
    //! @param[in] handle The valid handle to release
    void INTERNEDSYMBOLS_DLLAPI InternedSymbol_ReleaseHandle(
        InternHandle_t const handle );

    //! @brief Copy the ASCII name of the interned symbol to the buffer
    //! @details This method essentially calls wcstombs underneath after finding the symbol.
    //! @param[in] handle A valid handle to the symbol
    //! @param[out] buf An output buffer to hold the copied name
    //! @param[in,out] len (in) The maximum buffer length, in characters.
    //! @param[in,out] len (out) The actual length of the name, may be larger than the original buffer length
    //void INTERNEDSYMBOLS_DLLAPI InternedSymbol_ResolveHandleA(
    //    InternHandle_t const handle,
    //    char * const buf,
    //    uint32_t * len );


    //! @brief Copy the Unicode name of the interned symbol to the buffer
    //! @details This method essentially calls wcscpy underneath after finding the symbol.
    //! @param[in] handle A valid handle to the symbol
    //! @param[out] buf An output buffer to hold the copied name
    //! @param[in,out] len (in) The maximum buffer length, in characters.
    //! @param[in,out] len (out) The actual length of the name, may be larger than the original buffer length
    void INTERNEDSYMBOLS_DLLAPI InternedSymbol_ResolveHandleW(
        InternHandle_t const handle,
        wchar_t * const buf,
        uint32_t * len );


    //! @brief Type of callback function for string setter callback
    //! @details Users can implement callback methods that receive an instance of the
    //!          symbol name directly. This allows the symbol name to be copied in a
    //!          user-specific manner.
    //! @param[in] pUserData The user data supplied with the callback
    //! @param[in] str The NULL-terminated Unicode symbol name
    //! @param[in] len The length of the symbol name, in characters
    typedef void (INTERNEDSYMBOLS_API *InternedSymbol_StrSetterW)(
        void * pUserData,
        wchar_t const * const str,
        uint32_t const len );

    //! @brief Call pCallback with the name of the interned symbol
    //! @param[in] handle A valid handle to a symbol
    //! @param[in] pCallback The callback to call with the symbol name
    //! @param[in] pUserData Caller-specific data to be passed to pCallback
    void INTERNEDSYMBOLS_DLLAPI InternedSymbol_ResolveHandleCallbackW(
        InternHandle_t const handle,
        InternedSymbol_StrSetterW const pCallback,
        void * const pUserData );

#ifdef __cplusplus
} // end extern "C"
#endif
#endif // HEADER_GUARD__INTERNEDSYMBOLS_API_INTERNEDSYMBOLS_H_34980708974
