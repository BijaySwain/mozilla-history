/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * ***** BEGIN LICENSE BLOCK *****
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
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
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
/*******************************************************************************
 * Source date: 9 Apr 1997 21:45:12 GMT
 * netscape/fonts/cdoer module C header file
 * Generated by jmc version 1.8 -- DO NOT EDIT
 ******************************************************************************/

#ifndef _Mcdoer_H_
#define _Mcdoer_H_

#include "jmc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * cdoer
 ******************************************************************************/

/* The type of the cdoer interface. */
struct cdoerInterface;

/* The public type of a cdoer instance. */
typedef struct cdoer {
	const struct cdoerInterface*	vtable;
} cdoer;

/* The inteface ID of the cdoer interface. */
#ifndef JMC_INIT_cdoer_ID
extern EXTERN_C_WITHOUT_EXTERN const JMCInterfaceID cdoer_ID;
#else
EXTERN_C const JMCInterfaceID cdoer_ID = { 0x31133d0d, 0x67392f05, 0x1367271c, 0x60794020 };
#endif /* JMC_INIT_cdoer_ID */
/*******************************************************************************
 * cdoer Operations
 ******************************************************************************/

#define cdoer_getInterface(self, a, exception)	\
	(((self)->vtable->getInterface)(self, cdoer_getInterface_op, a, exception))

#define cdoer_addRef(self, exception)	\
	(((self)->vtable->addRef)(self, cdoer_addRef_op, exception))

#define cdoer_release(self, exception)	\
	(((self)->vtable->release)(self, cdoer_release_op, exception))

#define cdoer_hashCode(self, exception)	\
	(((self)->vtable->hashCode)(self, cdoer_hashCode_op, exception))

#define cdoer_equals(self, a, exception)	\
	(((self)->vtable->equals)(self, cdoer_equals_op, a, exception))

#define cdoer_clone(self, exception)	\
	(((self)->vtable->clone)(self, cdoer_clone_op, exception))

#define cdoer_toString(self, exception)	\
	(((self)->vtable->toString)(self, cdoer_toString_op, exception))

#define cdoer_finalize(self, exception)	\
	(((self)->vtable->finalize)(self, cdoer_finalize_op, exception))

#define cdoer_Update(self, a, exception)	\
	(((self)->vtable->Update)(self, cdoer_Update_op, a, exception))

/*******************************************************************************
 * cdoer Interface
 ******************************************************************************/

struct netscape_jmc_JMCInterfaceID;
struct java_lang_Object;
struct java_lang_String;
struct netscape_fonts_nff;

struct cdoerInterface {
	void*	(*getInterface)(struct cdoer* self, jint op, const JMCInterfaceID* a, JMCException* *exception);
	void	(*addRef)(struct cdoer* self, jint op, JMCException* *exception);
	void	(*release)(struct cdoer* self, jint op, JMCException* *exception);
	jint	(*hashCode)(struct cdoer* self, jint op, JMCException* *exception);
	jbool	(*equals)(struct cdoer* self, jint op, void* a, JMCException* *exception);
	void*	(*clone)(struct cdoer* self, jint op, JMCException* *exception);
	const char*	(*toString)(struct cdoer* self, jint op, JMCException* *exception);
	void	(*finalize)(struct cdoer* self, jint op, JMCException* *exception);
	void	(*Update)(struct cdoer* self, jint op, struct nff* a, JMCException* *exception);
};

/*******************************************************************************
 * cdoer Operation IDs
 ******************************************************************************/

typedef enum cdoerOperations {
	cdoer_getInterface_op,
	cdoer_addRef_op,
	cdoer_release_op,
	cdoer_hashCode_op,
	cdoer_equals_op,
	cdoer_clone_op,
	cdoer_toString_op,
	cdoer_finalize_op,
	cdoer_Update_op
} cdoerOperations;

/*******************************************************************************
 * Writing your C implementation: "cdoer.h"
 * *****************************************************************************
 * You must create a header file named "cdoer.h" that implements
 * the struct cdoerImpl, including the struct cdoerImplHeader
 * as it's first field:
 * 
 * 		#include "Mcdoer.h" // generated header
 * 
 * 		struct cdoerImpl {
 * 			cdoerImplHeader	header;
 * 			<your instance data>
 * 		};
 * 
 * This header file will get included by the generated module implementation.
 ******************************************************************************/

/* Forward reference to the user-defined instance struct: */
typedef struct cdoerImpl	cdoerImpl;


/* This struct must be included as the first field of your instance struct: */
typedef struct cdoerImplHeader {
	const struct cdoerInterface*	vtablecdoer;
	jint		refcount;
} cdoerImplHeader;

/*******************************************************************************
 * Instance Casting Macros
 * These macros get your back to the top of your instance, cdoer,
 * given a pointer to one of its interfaces.
 ******************************************************************************/

#undef  cdoerImpl2nfdoer
#define cdoerImpl2nfdoer(cdoerImplPtr) \
	((nfdoer*)((char*)(cdoerImplPtr) + offsetof(cdoerImplHeader, vtablecdoer)))

#undef  nfdoer2cdoerImpl
#define nfdoer2cdoerImpl(nfdoerPtr) \
	((cdoerImpl*)((char*)(nfdoerPtr) - offsetof(cdoerImplHeader, vtablecdoer)))

#undef  cdoerImpl2cdoer
#define cdoerImpl2cdoer(cdoerImplPtr) \
	((cdoer*)((char*)(cdoerImplPtr) + offsetof(cdoerImplHeader, vtablecdoer)))

#undef  cdoer2cdoerImpl
#define cdoer2cdoerImpl(cdoerPtr) \
	((cdoerImpl*)((char*)(cdoerPtr) - offsetof(cdoerImplHeader, vtablecdoer)))

/*******************************************************************************
 * Operations you must implement
 ******************************************************************************/


extern JMC_PUBLIC_API(void*)
_cdoer_getBackwardCompatibleInterface(struct cdoer* self, const JMCInterfaceID* iid,
	JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cdoer_init(struct cdoer* self, JMCException* *exception, nfFontObserverCallback a, void* b);

extern JMC_PUBLIC_API(void*)
_cdoer_getInterface(struct cdoer* self, jint op, const JMCInterfaceID* a, JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cdoer_addRef(struct cdoer* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cdoer_release(struct cdoer* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(jint)
_cdoer_hashCode(struct cdoer* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(jbool)
_cdoer_equals(struct cdoer* self, jint op, void* a, JMCException* *exception);

extern JMC_PUBLIC_API(void*)
_cdoer_clone(struct cdoer* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(const char*)
_cdoer_toString(struct cdoer* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cdoer_finalize(struct cdoer* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cdoer_Update(struct cdoer* self, jint op, struct nff* a, JMCException* *exception);

/*******************************************************************************
 * Factory Operations
 ******************************************************************************/

JMC_PUBLIC_API(cdoer*)
cdoerFactory_Create(JMCException* *exception, nfFontObserverCallback a, void* b);

/******************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _Mcdoer_H_ */
