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
 * The Original Code is Geolocation.
 *
 * The Initial Developer of the Original Code is Mozilla Corporation
 * Portions created by the Initial Developer are Copyright (C) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Doug Turner <dougt@meer.net>  (Original Author)
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

#include "nsIGeolocationProvider.h"
#include "nsIDOMGeoPosition.h"
#include "nsCOMPtr.h"

#include <glib.h>
#include <errno.h>
#include <gpsbt.h>
#include <gpsmgr.h>

extern "C" {
  // need to extern these because of:
  // https://bugs.maemo.org/show_bug.cgi?id=3226
  #include <location/location-gps-device.h>
  #include <location/location-gpsd-control.h>
}

class MaemoLocationProvider : public nsIGeolocationProvider
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIGEOLOCATIONPROVIDER

  MaemoLocationProvider();

  void Update(nsIDOMGeoPosition* aPosition);

private:
  ~MaemoLocationProvider();

  nsCOMPtr<nsIDOMGeoPosition> mLastPosition;

  nsIGeolocationUpdate* mCallback; // weak reference by contract.


  gpsbt_t mGPSBT;
  LocationGPSDevice *mGPSDevice;
  gulong mLocationCallbackHandle;
  PRBool mHasSeenLocation;
};