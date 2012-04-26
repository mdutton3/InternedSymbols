InternedSymbols
===============
This is a C shared library for [interning immutable strings](http://en.wikipedia.org/wiki/String_interning).

Strings passed to the library are assigned a unique handle, such that equal
strings have equal handles. This allows strings to be passed without copying
and compared quickly. Interned string handles are reference counted and should
be released when no longer needed.

The interned string pool is accessed through the following exported methods:
```c
// An opaque handle to an interned string
typedef <opaque-ptr-type> InternHandle_t;

// Intern the given string and acquire its handle
InternHandle_t InternedSymbol_AcquireHandleA(
    char const * const str,
    uint32_t const len );
InternHandle_t InternedSymbol_AcquireHandleW(
    wchar_t const * const str,
    uint32_t const len );

// Duplicate an existing valid handle
void InternedSymbol_DuplicateHandle(
    InternHandle_t const handle );

// Release an acquired or duplicated handle
void InternedSymbol_ReleaseHandle(
    InternHandle_t const handle );

// Copy the string referenced by the given handle
void InternedSymbol_CopyToA(
    InternHandle_t const handle,
    char * const buf,
    uint32_t * len );
void InternedSymbol_CopyToW(
    InternHandle_t const handle,
    wchar_t * const buf,
    uint32_t * len );

// The callback signature used by InternedSymbol_VisitHandleW
typedef void (*InternedSymbol_CallbackFnW)(
    void * pUserData,
    wchar_t const * const str,
    uint32_t const len );

// Provide the underlying string buffer to a callback for processing
void InternedSymbol_VisitHandleW(
    InternHandle_t const handle,
    InternedSymbol_CallbackFnW const pCallback,
    void * const pUserData );
```

These methods accept either standard ASCII c-strings (multibyte chars) or wide
character strings (`wchar_t`) and following the WIN32 API naming convention for
these strings types. Methods accepting c-strings are suffixed with '`A`' and
methods accepting wide strings are suffixed with '`W`'. Under the covers, all
strings are converted to wide character strings for comparison.

The basic flow is that the application will acquire a handle for new strings
and use this handle for string comparison. When a handle is passed to another
module or unit, the new module or unit will duplicate the handle. When any
unit is done with the handle, it should release the handle (s.t. each
AcquireHandle or DuplicateHandle is paired with a ReleaseHandle).

```c
// String comparison is case-sensitive but insensitive to encoding
InternHandle_t handle1 = InternedSymbol_AcquireHandleA(  "Hello", 5 );
InternHandle_t handle2 = InternedSymbol_AcquireHandleW( L"Hello", 5 );
InternHandle_t handle3 = InternedSymbol_DuplicateHandle( handle1 );

// Just a pointer comparison!
assert( handle1 == handle2 );
assert( handle1 == handle3 );

// Copy the underlying string
wchar_t buffer[100];
uint32_t len = 100;
InternedSymbol_CopyToW( handle1, buffer, &len );

// Or have the underlying string passed to a function for processing
// This is useful when direct access to the underlying string buffer
// could help. It's done like this to make the lifetime of the
// underlying string buffer explicit (i.e. for the life of the
// callback invocation)
InternedSymbol_VisitHandleW( handle1, &foo, &foo_ctx );

// Must be released as many times as it is Acquired/Duplicated
// Since all the handles in this example are equal, we can call
// release on the same handle.
InternedSymbol_ReleaseHandle( handle1 );
InternedSymbol_ReleaseHandle( handle1 );
InternedSymbol_ReleaseHandle( handle1 );
```
