#pragma once
#ifndef HEADER_GUARD__INTERNEDSYMBOLS_METHODSIGNATURES_H_34980708974
#define HEADER_GUARD__INTERNEDSYMBOLS_METHODSIGNATURES_H_34980708974
//! @file
//! @brief Defines the types of the exported C-style interface methods 

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


#if defined(_MSC_VER) && (_MSC_VER < 1600)
    typedef __int32 int32_t;
    typedef unsigned __int32 uint32_t;
#else
    #include <stdint.h>
#endif


// C-style API
#ifdef __cplusplus
extern "C" {
#endif


    //! @brief Opaque handle to an interned symbol
    struct InternedSymbol;
    typedef struct InternedSymbol const * InternHandle_t;

    //! @brief Get a new or existing handle to the given symbol name
    //! @param[in] str The ASCII symbol name, case sensitive
    //! @param[in] len The length of the name, in characters
    //! @return A handle to the interned symbol
    typedef InternHandle_t (INTERNEDSYMBOLS_API InternedSymbol_AcquireHandleA_type) (
        char const * const str,
        uint32_t const len );

    //! @brief Get a new or existing handle to the given symbol name
    //! @param[in] str The Unicode symbol name, case sensitive
    //! @param[in] len The length of the name, in characters
    //! @return A handle to the interned symbol
    typedef InternHandle_t (INTERNEDSYMBOLS_API InternedSymbol_AcquireHandleW_type) (
        wchar_t const * const str,
        uint32_t const len );

    //! @brief Reacquire an existing handle
    //! @details This increase the reference count to the handle and is more efficient
    //!     than calling InternedSymbol_AcquireHandleT again with the symbol name.
    //! @param[in] handle The valid handle to acquire
    typedef void (INTERNEDSYMBOLS_API InternedSymbol_DuplicateHandle_type) (
        InternHandle_t const handle );

    //! @brief Release an existing handle acquired with InternedSymbol_AcquireHandleT
    //! @param[in] handle The valid handle to release
    typedef void (INTERNEDSYMBOLS_API InternedSymbol_ReleaseHandle_type) (
        InternHandle_t const handle );

    //! @brief Get the length, in bytes, of the symbol name
    //! @param[in] handle A valid handle to a symbol
    //! @return The length of the symbol name, in bytes, excluding the NULL terminator
    typedef uint32_t (INTERNEDSYMBOLS_API InternedSymbol_GetLengthA_type) (
        InternHandle_t const handle );

    //! @brief Get the length, in wide characters, of the symbol name
    //! @param[in] handle A valid handle to a symbol
    //! @return The length of the symbol name, in wide characters, excluding the NULL terminator
    typedef uint32_t (INTERNEDSYMBOLS_API InternedSymbol_GetLengthW_type) (
        InternHandle_t const handle );

    //! @brief Compare the names of the two symbols, lexographically
    //! @param[in] lhs Handle of the left-hand symbol for comparison
    //! @param[in] rhs Handle of the right-hand symbol for comparison
    //! @return <0 if (lhs <  rhs)
    //! @return  0 if (lhs == rhs)
    //! @return >0 if (lhs >  rhs)
    typedef int32_t (INTERNEDSYMBOLS_API InternedSymbol_Compare_type) (
        InternHandle_t const lhs, InternHandle_t const rhs );

    //! @brief Copy the ASCII name of the interned symbol to the buffer
    //! @details This method essentially calls wcstombs underneath after finding the symbol.
    //! @param[in] handle A valid handle to the symbol
    //! @param[out] buf An output buffer to hold the copied name
    //! @param[in,out] len (in) The maximum buffer length, in (multibyte) characters.
    //! @param[in,out] len (out) The actual length of the name copied, may be larger than the original buffer length
    typedef int32_t (INTERNEDSYMBOLS_API InternedSymbol_CopyToA_type) (
        InternHandle_t const handle,
        char * const buf,
        int32_t const len );


    //! @brief Copy the Unicode name of the interned symbol to the buffer
    //! @details This method essentially calls wcscpy underneath after finding the symbol.
    //! @param[in] handle A valid handle to the symbol
    //! @param[out] buf An output buffer to hold the copied name
    //! @param[in,out] len (in) The maximum buffer length, in characters.
    //! @param[in,out] len (out) The actual length of the name, in characters.
    typedef int32_t (INTERNEDSYMBOLS_API InternedSymbol_CopyToW_type) (
        InternHandle_t const handle,
        wchar_t * const buf,
        int32_t const len );


    //! @brief Type of callback function for string setter callback
    //! @note No method is exported with this signature. The importing module
    //!     should define a method with this signature if it is going to use
    //!     the InternedSymbol_VisitHandleW method.
    //! @details Users can implement callback methods that receive an instance of the
    //!          symbol name directly. This allows the symbol name to be copied in a
    //!          user-specific manner.
    //! @param[in] pUserData The user data supplied with the callback
    //! @param[in] str The NULL-terminated Unicode symbol name
    //! @param[in] len The length of the symbol name, in characters
    typedef void (INTERNEDSYMBOLS_API *InternedSymbol_CallbackFnW)(
        void * pUserData,
        InternHandle_t const handle,
        wchar_t const * const str,
        uint32_t const len );

    //! @brief Call pCallback with the name and handle of the interned symbol
    //! @param[in] handle A valid handle to a symbol
    //! @param[in] pCallback The callback to call with the symbol name
    //! @param[in] pUserData Caller-specific data to be passed to pCallback
    //! @see InternedSymbol_CallbackFnW
    typedef void (INTERNEDSYMBOLS_API InternedSymbol_VisitHandleW_type) (
        InternHandle_t const handle,
        InternedSymbol_CallbackFnW const pCallback,
        void * const pUserData );

#ifdef __cplusplus
} // end extern "C"
#endif
#endif // HEADER_GUARD__INTERNEDSYMBOLS_METHODSIGNATURES_H_34980708974
