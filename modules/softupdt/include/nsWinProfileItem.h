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

#ifndef nsWinProfileItem_h__
#define nsWinProfileItem_h__

#include "prtypes.h"
#include "nsSoftwareUpdate.h"
#include "nsInstallObject.h"
#include "nsWinProfile.h"


PR_BEGIN_EXTERN_C

class nsWinProfileItem : public nsInstallObject {

public:

  /* Public Fields */

  /* Public Methods */

  nsWinProfileItem(nsWinProfile* profileObj,
                   char* sectionName,
                   char* keyName,
                   char* val);

  /**
   * Completes the install:
   * - writes the data into the .INI file
   */
  char* Complete();
  
  float GetInstallOrder();
  
  char* toString();
  
  // no need for special clean-up
  void Abort();
  
  // no need for set-up
  char* Prepare();
  
  /* should these be protected? */
  PRBool CanUninstall();
  PRBool RegisterPackageNode();
  
private:
  
  /* Private Fields */
  nsWinProfile* profile; // initiating profile object
  char* section;     // Name of section
  char* key;         // Name of key
  char* value;       // data to write
  
  /* Private Methods */
  
};

PR_END_EXTERN_C

#endif /* nsWinProfileItem_h__ */
