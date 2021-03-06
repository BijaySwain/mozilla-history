/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef nscore_h___
#define nscore_h___

/**
 * Make sure that we have the proper platform specific
 * c++ definitions needed by nscore.h
 */
#ifndef _XPCOM_CONFIG_H_
#include "xpcom-config.h"
#endif

/* Definitions of functions and operators that allocate memory. */
#if !defined(XPCOM_GLUE) && !defined(NS_NO_XPCOM) && !defined(MOZ_NO_MOZALLOC)
#  include "mozilla/mozalloc.h"
#  include "mozilla/mozalloc_macro_wrappers.h"
#endif

/**
 * Incorporate the core NSPR data types which XPCOM uses.
 */
#include "prtypes.h"

/* Core XPCOM declarations. */

/**
 * Macros defining the target platform...
 */
#ifdef _WIN32
#define NS_WIN32 1

#elif defined(__unix)
#define NS_UNIX 1

#elif defined(XP_OS2)
#define NS_OS2 1
#endif
/*----------------------------------------------------------------------*/
/* Import/export defines */

/**
 * Using the visibility("hidden") attribute allows the compiler to use
 * PC-relative addressing to call this function.  If a function does not
 * access any global data, and does not call any methods which are not either
 * file-local or hidden, then on ELF systems we avoid loading the address of
 * the PLT into a register at the start of the function, which reduces code
 * size and frees up a register for general use.
 *
 * As a general rule, this should be used for any non-exported symbol
 * (including virtual method implementations).  NS_IMETHOD uses this by
 * default; if you need to have your NS_IMETHOD functions exported, you can
 * wrap your class as follows:
 *
 * #undef  IMETHOD_VISIBILITY
 * #define IMETHOD_VISIBILITY NS_VISIBILITY_DEFAULT
 *
 * class Foo {
 * ...
 * };
 *
 * #undef  IMETHOD_VISIBILITY
 * #define IMETHOD_VISIBILITY NS_VISIBILITY_HIDDEN
 *
 * Don't forget to change the visibility back to hidden before the end
 * of a header!
 *
 * Other examples:
 *
 * NS_HIDDEN_(int) someMethod();
 * SomeCtor() NS_HIDDEN;
 */

#ifdef HAVE_VISIBILITY_HIDDEN_ATTRIBUTE
#define NS_VISIBILITY_HIDDEN   __attribute__ ((visibility ("hidden")))
#else
#define NS_VISIBILITY_HIDDEN
#endif

#if defined(HAVE_VISIBILITY_ATTRIBUTE)
#define NS_VISIBILITY_DEFAULT __attribute__ ((visibility ("default")))
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#define NS_VISIBILITY_DEFAULT __global
#else
#define NS_VISIBILITY_DEFAULT
#endif

#define NS_HIDDEN_(type)   NS_VISIBILITY_HIDDEN type
#define NS_EXTERNAL_VIS_(type) NS_VISIBILITY_DEFAULT type

#define NS_HIDDEN           NS_VISIBILITY_HIDDEN
#define NS_EXTERNAL_VIS     NS_VISIBILITY_DEFAULT

#undef  IMETHOD_VISIBILITY
#define IMETHOD_VISIBILITY  NS_VISIBILITY_HIDDEN

/**
 * Mark a function as using a potentially non-standard function calling
 * convention.  This can be used on functions that are called very
 * frequently, to reduce the overhead of the function call.  It is still worth
 * using the macro for C++ functions which take no parameters since it allows
 * passing |this| in a register.
 *
 *  - Do not use this on any scriptable interface method since xptcall won't be
 *    aware of the different calling convention.
 *  - This must appear on the declaration, not the definition.
 *  - Adding this to a public function _will_ break binary compatibility.
 *  - This may be used on virtual functions but you must ensure it is applied
 *    to all implementations - the compiler will _not_ warn but it will crash.
 *  - This has no effect for inline functions or functions which take a
 *    variable number of arguments.
 *  - __fastcall on windows should not be applied to class
 *    constructors/destructors - use the NS_CONSTRUCTOR_FASTCALL macro for
 *    constructors/destructors.
 *
 * Examples: int NS_FASTCALL func1(char *foo);
 *           NS_HIDDEN_(int) NS_FASTCALL func2(char *foo);
 */

#if defined(__i386__) && defined(__GNUC__) && \
    (__GNUC__ >= 3) && !defined(XP_OS2)
#define NS_FASTCALL __attribute__ ((regparm (3), stdcall))
#define NS_CONSTRUCTOR_FASTCALL __attribute__ ((regparm (3), stdcall))
#elif defined(XP_WIN) && !defined(_WIN64)
#define NS_FASTCALL __fastcall
#define NS_CONSTRUCTOR_FASTCALL
#else
#define NS_FASTCALL
#define NS_CONSTRUCTOR_FASTCALL
#endif

#ifdef NS_WIN32

#define NS_IMPORT __declspec(dllimport)
#define NS_IMPORT_(type) __declspec(dllimport) type __stdcall
#define NS_EXPORT __declspec(dllexport)
#define NS_EXPORT_(type) __declspec(dllexport) type __stdcall
#define NS_IMETHOD_(type) virtual type __stdcall
#define NS_IMETHODIMP_(type) type __stdcall
#define NS_METHOD_(type) type __stdcall
#define NS_CALLBACK_(_type, _name) _type (__stdcall * _name)
#define NS_STDCALL __stdcall
#define NS_FROZENCALL __cdecl

/*
  These are needed to mark static members in exported classes, due to
  gcc bug XXX insert bug# here.
 */

#define NS_EXPORT_STATIC_MEMBER_(type) type
#define NS_IMPORT_STATIC_MEMBER_(type) type

#elif defined(XP_OS2)

#define NS_IMPORT __declspec(dllimport)
#define NS_IMPORT_(type) type __declspec(dllimport)
#define NS_EXPORT __declspec(dllexport)
#define NS_EXPORT_(type) type __declspec(dllexport)
#define NS_IMETHOD_(type) virtual type
#define NS_IMETHODIMP_(type) type
#define NS_METHOD_(type) type
#define NS_CALLBACK_(_type, _name) _type (* _name)
#define NS_STDCALL
#define NS_FROZENCALL
#define NS_EXPORT_STATIC_MEMBER_(type) NS_EXTERNAL_VIS_(type)
#define NS_IMPORT_STATIC_MEMBER_(type) NS_EXTERNAL_VIS_(type)

#else

#define NS_IMPORT NS_EXTERNAL_VIS
#define NS_IMPORT_(type) NS_EXTERNAL_VIS_(type)
#define NS_EXPORT NS_EXTERNAL_VIS
#define NS_EXPORT_(type) NS_EXTERNAL_VIS_(type)
#define NS_IMETHOD_(type) virtual IMETHOD_VISIBILITY type
#define NS_IMETHODIMP_(type) type
#define NS_METHOD_(type) type
#define NS_CALLBACK_(_type, _name) _type (* _name)
#define NS_STDCALL
#define NS_FROZENCALL
#define NS_EXPORT_STATIC_MEMBER_(type) NS_EXTERNAL_VIS_(type)
#define NS_IMPORT_STATIC_MEMBER_(type) NS_EXTERNAL_VIS_(type)

#endif

/**
 * Macro for creating typedefs for pointer-to-member types which are
 * declared with stdcall.  It is important to use this for any type which is
 * declared as stdcall (i.e. NS_IMETHOD).  For example, instead of writing:
 *
 *  typedef nsresult (nsIFoo::*someType)(nsISupports* arg);
 *
 *  you should write:
 *
 *  typedef
 *  NS_STDCALL_FUNCPROTO(nsresult, someType, nsIFoo, typeFunc, (nsISupports*));
 *
 *  where nsIFoo::typeFunc is any method declared as
 *  NS_IMETHOD typeFunc(nsISupports*);
 *
 *  XXX this can be simplified to always use the non-typeof implementation
 *  when http://gcc.gnu.org/bugzilla/show_bug.cgi?id=11893 is fixed.
 */

#ifdef __GNUC__
#define NS_STDCALL_FUNCPROTO(ret, name, class, func, args) \
  typeof(&class::func) name
#else
#define NS_STDCALL_FUNCPROTO(ret, name, class, func, args) \
  ret (NS_STDCALL class::*name) args
#endif

/**
 * Deprecated declarations.
 */
#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
# define MOZ_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
# define MOZ_DEPRECATED __declspec(deprecated)
#else
# define MOZ_DEPRECATED
#endif

/**
 * Generic API modifiers which return the standard XPCOM nsresult type
 */
#define NS_IMETHOD          NS_IMETHOD_(nsresult)
#define NS_IMETHODIMP       NS_IMETHODIMP_(nsresult)
#define NS_METHOD           NS_METHOD_(nsresult)
#define NS_CALLBACK(_name)  NS_CALLBACK_(nsresult, _name)

/**
 * Import/Export macros for XPCOM APIs
 */

#ifdef __cplusplus
#define NS_EXTERN_C extern "C"
#else
#define NS_EXTERN_C
#endif

#define EXPORT_XPCOM_API(type) NS_EXTERN_C NS_EXPORT type NS_FROZENCALL
#define IMPORT_XPCOM_API(type) NS_EXTERN_C NS_IMPORT type NS_FROZENCALL
#define GLUE_XPCOM_API(type) NS_EXTERN_C NS_HIDDEN_(type) NS_FROZENCALL

#ifdef _IMPL_NS_COM
#define XPCOM_API(type) EXPORT_XPCOM_API(type)
#elif defined(XPCOM_GLUE)
#define XPCOM_API(type) GLUE_XPCOM_API(type)
#else
#define XPCOM_API(type) IMPORT_XPCOM_API(type)
#endif

#ifdef MOZILLA_INTERNAL_API
#  define NS_COM_GLUE
   /*
     The frozen string API has different definitions of nsAC?String
     classes than the internal API. On systems that explicitly declare
     dllexport symbols this is not a problem, but on ELF systems
     internal symbols can accidentally "shine through"; we rename the
     internal classes to avoid symbol conflicts.
   */
#  define nsAString nsAString_internal
#  define nsACString nsACString_internal
#else
#  ifdef HAVE_VISIBILITY_ATTRIBUTE
#    define NS_COM_GLUE NS_VISIBILITY_HIDDEN
#  else
#    define NS_COM_GLUE
#  endif
#endif

#if (defined(DEBUG) || defined(FORCE_BUILD_REFCNT_LOGGING))
/* Make refcnt logging part of the build. This doesn't mean that
 * actual logging will occur (that requires a separate enable; see
 * nsTraceRefcnt.h for more information).  */
#define NS_BUILD_REFCNT_LOGGING
#endif

/* If NO_BUILD_REFCNT_LOGGING is defined then disable refcnt logging
 * in the build. This overrides FORCE_BUILD_REFCNT_LOGGING. */
#if defined(NO_BUILD_REFCNT_LOGGING)
#undef NS_BUILD_REFCNT_LOGGING
#endif

/* If a program allocates memory for the lifetime of the app, it doesn't make
 * sense to touch memory pages and free that memory at shutdown,
 * unless we are running leak stats.
 */
#if defined(NS_TRACE_MALLOC) || defined(NS_BUILD_REFCNT_LOGGING) || defined(MOZ_VALGRIND)
#define NS_FREE_PERMANENT_DATA
#endif

/**
 * NS_NO_VTABLE is emitted by xpidl in interface declarations whenever
 * xpidl can determine that the interface can't contain a constructor.
 * This results in some space savings and possible runtime savings -
 * see bug 49416.  We undefine it first, as xpidl-generated headers
 * define it for IDL uses that don't include this file.
 */
#ifdef NS_NO_VTABLE
#undef NS_NO_VTABLE
#endif
#if defined(_MSC_VER) && _MSC_VER >= 1100
#define NS_NO_VTABLE __declspec(novtable)
#else
#define NS_NO_VTABLE
#endif


/**
 * Generic XPCOM result data type
 */
typedef PRUint32 nsresult;

/**
 * Reference count values
 *
 * This is the return type for AddRef() and Release() in nsISupports.
 * IUnknown of COM returns an unsigned long from equivalent functions.
 * The following ifdef exists to maintain binary compatibility with
 * IUnknown.
 */
#if defined(XP_WIN) && PR_BYTES_PER_LONG == 4
typedef unsigned long nsrefcnt;
#else
typedef PRUint32 nsrefcnt;
#endif

/**
 * The preferred symbol for null.  Make sure this is the same size as
 * void* on the target.  See bug 547964.
 */
#if defined(_WIN64)
# define nsnull 0LL
#else
# define nsnull 0L
#endif


#include "nsError.h"

/* ------------------------------------------------------------------------ */
/* Casting macros for hiding C++ features from older compilers */

  /*
    All our compiler support template specialization, but not all support the
    |template <>| notation.  The compiler that don't understand this notation
    just omit it for specialization.

    Need to add an autoconf test for this.
  */

  /* under VC++ (Windows), we don't have autoconf yet */
#if defined(_MSC_VER) && (_MSC_VER>=1100)
  #define HAVE_CPP_MODERN_SPECIALIZE_TEMPLATE_SYNTAX
  #define HAVE_CPP_2BYTE_WCHAR_T
#endif

#ifndef __PRUNICHAR__
#define __PRUNICHAR__
  /* For now, don't use wchar_t on Unix because it breaks the Netscape
   * commercial build.  When this is fixed there will be no need for the
   * |reinterpret_cast| in nsLiteralString.h either.
   */
  #if defined(HAVE_CPP_2BYTE_WCHAR_T) && defined(NS_WIN32)
    typedef wchar_t PRUnichar;
  #else
    typedef PRUint16 PRUnichar;
  #endif
#endif

#ifdef HAVE_CPP_MODERN_SPECIALIZE_TEMPLATE_SYNTAX
  #define NS_SPECIALIZE_TEMPLATE  template <>
#else
  #define NS_SPECIALIZE_TEMPLATE
#endif

/*
 * Use these macros to do 64bit safe pointer conversions.
 */

#define NS_PTR_TO_INT32(x)  ((PRInt32)  (PRWord) (x))
#define NS_PTR_TO_UINT32(x) ((PRUint32) (PRWord) (x))
#define NS_INT32_TO_PTR(x)  ((void *)   (PRWord) (x))

/*
 * Use NS_STRINGIFY to form a string literal from the value of a macro.
 */
#define NS_STRINGIFY_HELPER(x_) #x_
#define NS_STRINGIFY(x_) NS_STRINGIFY_HELPER(x_)

/*
 * Use NS_CLAMP to force a value (such as a preference) into a range.
 */
#define NS_CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

/*
 * These macros allow you to give a hint to the compiler about branch
 * probability so that it can better optimize.  Use them like this:
 *
 *  if (NS_LIKELY(v == 1)) {
 *    ... expected code path ...
 *  }
 *
 *  if (NS_UNLIKELY(v == 0)) {
 *    ... non-expected code path ...
 *  }
 *
 * These macros are guaranteed to always return 0 or 1.
 * The NS_FAILED/NS_SUCCEEDED macros depends on this.
 * @return 0 or 1
 */

#if defined(__GNUC__) && (__GNUC__ > 2)
#define NS_LIKELY(x)    (__builtin_expect(!!(x), 1))
#define NS_UNLIKELY(x)  (__builtin_expect(!!(x), 0))
#else
#define NS_LIKELY(x)    (!!(x))
#define NS_UNLIKELY(x)  (!!(x))
#endif

 /*
  * If we're being linked as standalone glue, we don't want a dynamic
  * dependency on NSPR libs, so we skip the debug thread-safety
  * checks, and we cannot use the THREADSAFE_ISUPPORTS macros.
  */
#if defined(XPCOM_GLUE) && !defined(XPCOM_GLUE_USE_NSPR)
#define XPCOM_GLUE_AVOID_NSPR
#endif

#if defined(HAVE_THREAD_TLS_KEYWORD)
#define NS_TLS __thread
#endif

/**
 * Static type annotations, enforced when static-checking is enabled:
 *
 * NS_STACK_CLASS: a class which must only be instantiated on the stack
 * NS_FINAL_CLASS: a class which may not be subclassed
 *
 * NS_MUST_OVERRIDE:
 *   a method which every immediate subclass of this class must
 *   override.  A subclass override can itself be NS_MUST_OVERRIDE, in
 *   which case its own subclasses must override the method as well.
 *
 *   This is similar to, but not the same as, marking a method pure
 *   virtual.  It has no effect on the class in which the annotation
 *   appears, you can still provide a definition for the method, and
 *   it objects to the mere existence of a subclass that doesn't
 *   override the method.  See examples in analysis/must-override.js.
 */
#ifdef NS_STATIC_CHECKING
#define NS_STACK_CLASS __attribute__((user("NS_stack")))
#define NS_OKONHEAP    __attribute__((user("NS_okonheap")))
#define NS_SUPPRESS_STACK_CHECK __attribute__((user("NS_suppress_stackcheck")))
#define NS_FINAL_CLASS __attribute__((user("NS_final")))
#define NS_MUST_OVERRIDE __attribute__((user("NS_must_override")))
#else
#define NS_STACK_CLASS
#define NS_OKONHEAP
#define NS_SUPPRESS_STACK_CHECK
#define NS_FINAL_CLASS
#define NS_MUST_OVERRIDE
#endif

/**
 * Attributes defined to help Dehydra GCC analysis.
 */
#ifdef NS_STATIC_CHECKING
# define NS_SCRIPTABLE __attribute__((user("NS_script")))
# define NS_INPARAM __attribute__((user("NS_inparam")))
# define NS_OUTPARAM  __attribute__((user("NS_outparam")))
# define NS_INOUTPARAM __attribute__((user("NS_inoutparam")))
# define NS_OVERRIDE __attribute__((user("NS_override")))
#else
# define NS_SCRIPTABLE
# define NS_INPARAM
# define NS_OUTPARAM
# define NS_INOUTPARAM
# define NS_OVERRIDE
#endif

/*
 * SEH exception macros.
 */
#ifdef HAVE_SEH_EXCEPTIONS
#define MOZ_SEH_TRY           __try
#define MOZ_SEH_EXCEPT(expr)  __except(expr)
#else
#define MOZ_SEH_TRY           if(PR_TRUE)
#define MOZ_SEH_EXCEPT(expr)  else
#endif

#endif /* nscore_h___ */
