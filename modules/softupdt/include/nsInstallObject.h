/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
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

#ifndef nsInstallObject_h__
#define nsInstallObject_h__

#include "prtypes.h"

#include "nsSoftwareUpdate.h"

PR_BEGIN_EXTERN_C

struct nsInstallObject {

public:
  
  struct nsSoftwareUpdate* softUpdate;

  /* Public Methods */
  nsInstallObject(struct nsSoftwareUpdate* inSoftUpdate) {softUpdate = inSoftUpdate; }

  /* Override with your set-up action */
  virtual char* Prepare() = 0;

  /* Override with your Completion action */
  virtual char* Complete() = 0;

  /* Override with an explanatory string for the progress dialog */
  virtual char* toString() = 0;
	
  /* Override with your clean-up function */
  virtual void Abort() = 0;
  
  /* should these be protected? */
  virtual PRBool CanUninstall() = 0;
  virtual PRBool RegisterPackageNode() = 0;

private:

  /* Private Field Accessors */

  /* Private Methods */

};

PR_END_EXTERN_C

#endif /* nsInstallObject_h__ */
