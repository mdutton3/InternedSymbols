#pragma once
#ifndef HEADER_GUARD__INTERNEDSYMBOLS_ATOMIC_HPP_34980708974
#define HEADER_GUARD__INTERNEDSYMBOLS_ATOMIC_HPP_34980708974

//! @file
//! @brief Defines some portable wrappers around atomic increment/decrement functions

#if defined(__GNUC__)
    template<typename T>
    static inline T AtomicIncrement( T volatile * const pVal )
    {
        return __sync_add_and_fetch( pVal, 1 );
    }

    template<typename T>
    static inline T AtomicDecrement( T volatile * const pVal )
    {
        return __sync_sub_and_fetch( pVal, 1 );
    }
#elif defined(_MSC_VER)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    template<typename T>
    static inline T AtomicIncrement( T volatile * const pVal )
    {
        return InterlockedIncrement( pVal );
    }

    template<typename T>
    static inline T AtomicDecrement( T volatile * const pVal )
    {
        return InterlockedDecrement( pVal );
    }
#else
  #error Unsupported compiler.
#endif





#endif // HEADER_GUARD__INTERNEDSYMBOLS_ATOMIC_HPP_34980708974
