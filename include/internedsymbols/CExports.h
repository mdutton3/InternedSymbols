#pragma once
#ifndef HEADER_GUARD__INTERNEDSYMBOLS_CEXPORTS_H_34980708974
#define HEADER_GUARD__INTERNEDSYMBOLS_CEXPORTS_H_34980708974

//! @file
//! @brief Defines the C-style interface for the Interned Symbols DLL

#include "internedsymbols/MethodSignatures.h"

// C-style API
#ifdef __cplusplus
extern "C" {
#endif

#define INTERNEDSYMBOLS_EXPORTFN(fn_name)  fn_name##_type INTERNEDSYMBOLS_PUBLIC fn_name;

        INTERNEDSYMBOLS_EXPORTFN( InternedSymbol_AcquireHandleA );
        INTERNEDSYMBOLS_EXPORTFN( InternedSymbol_AcquireHandleW );

        INTERNEDSYMBOLS_EXPORTFN( InternedSymbol_DuplicateHandle );

        INTERNEDSYMBOLS_EXPORTFN( InternedSymbol_ReleaseHandle );

        INTERNEDSYMBOLS_EXPORTFN( InternedSymbol_GetLength );

        INTERNEDSYMBOLS_EXPORTFN( InternedSymbol_Compare );

        INTERNEDSYMBOLS_EXPORTFN( InternedSymbol_CopyToA );
        INTERNEDSYMBOLS_EXPORTFN( InternedSymbol_CopyToW );

        INTERNEDSYMBOLS_EXPORTFN( InternedSymbol_VisitHandleW );

#undef  INTERNEDSYMBOLS_EXPORTFN

#ifdef __cplusplus
} // end extern "C"
#endif

#endif // HEADER_GUARD__INTERNEDSYMBOLS_CEXPORTS_H_34980708974
