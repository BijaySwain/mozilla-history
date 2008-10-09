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

#include "nsGeolocation.h"
#include "nsAutoPtr.h"
#include "nsCOMPtr.h"
#include "nsIDOMWindow.h"
#include "nsDOMClassInfo.h"
#include "nsComponentManagerUtils.h"
#include "nsServiceManagerUtils.h"
#include "nsContentUtils.h"
#include "nsIURI.h"
#include "nsIPermissionManager.h"
#include "nsIObserverService.h"
#include "nsIPrefService.h"
#include "nsIPrefBranch2.h"
#include "nsIProxyObjectManager.h"
#include "nsIJSContextStack.h"

#include <math.h>

#ifdef NS_MAEMO_LOCATION
#include "MaemoLocationProvider.h"
#endif

#include "nsIDOMDocument.h"
#include "nsIDocument.h"

////////////////////////////////////////////////////
// nsDOMGeoPositionError
////////////////////////////////////////////////////

class nsDOMGeoPositionError : public nsIDOMGeoPositionError
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMGEOPOSITIONERROR

  nsDOMGeoPositionError(PRInt16 aCode, const nsAString& aMessage);

private:
  ~nsDOMGeoPositionError();
  PRInt16 mCode;
  nsString mMessage;

};

NS_INTERFACE_MAP_BEGIN(nsDOMGeoPositionError)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIDOMGeoPositionError)
  NS_INTERFACE_MAP_ENTRY(nsIDOMGeoPositionError)
  NS_DOM_INTERFACE_MAP_ENTRY_CLASSINFO(GeoPositionError)
NS_INTERFACE_MAP_END

NS_IMPL_THREADSAFE_ADDREF(nsDOMGeoPositionError)
NS_IMPL_THREADSAFE_RELEASE(nsDOMGeoPositionError)

nsDOMGeoPositionError::nsDOMGeoPositionError(PRInt16 aCode, const nsAString& aMessage)
  : mCode(aCode), mMessage(aMessage)
{
}

nsDOMGeoPositionError::~nsDOMGeoPositionError(){}


NS_IMETHODIMP
nsDOMGeoPositionError::GetCode(PRInt16 *aCode)
{
  NS_ENSURE_ARG_POINTER(aCode);
  *aCode = mCode;
  return NS_OK;
}

NS_IMETHODIMP
nsDOMGeoPositionError::GetMessage(nsAString & aMessage)
{
  aMessage = mMessage;
  return NS_OK;
}

////////////////////////////////////////////////////
// nsGeolocationRequest
////////////////////////////////////////////////////

nsGeolocationRequest::nsGeolocationRequest(nsGeolocation* locator, nsIDOMGeoPositionCallback* callback, nsIDOMGeoPositionErrorCallback* errorCallback)
  : mAllowed(PR_FALSE), mCleared(PR_FALSE), mFuzzLocation(PR_FALSE), mCallback(callback), mErrorCallback(errorCallback), mLocator(locator)
{
}

nsGeolocationRequest::~nsGeolocationRequest()
{
}

NS_INTERFACE_MAP_BEGIN(nsGeolocationRequest)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIGeolocationRequest)
  NS_INTERFACE_MAP_ENTRY(nsIGeolocationRequest)
NS_INTERFACE_MAP_END

NS_IMPL_ADDREF(nsGeolocationRequest)
NS_IMPL_RELEASE(nsGeolocationRequest)

NS_IMETHODIMP
nsGeolocationRequest::GetRequestingURI(nsIURI * *aRequestingURI)
{
  NS_ENSURE_ARG_POINTER(aRequestingURI);
  *aRequestingURI = mLocator->GetURI();
  NS_IF_ADDREF(*aRequestingURI);
  return NS_OK;
}

NS_IMETHODIMP
nsGeolocationRequest::GetRequestingWindow(nsIDOMWindow * *aRequestingWindow)
{
  NS_ENSURE_ARG_POINTER(aRequestingWindow);
  *aRequestingWindow = mLocator->GetOwner();
  NS_IF_ADDREF(*aRequestingWindow);
  return NS_OK;
}

NS_IMETHODIMP
nsGeolocationRequest::Cancel()
{
  // remove ourselves from the locators callback lists.
  mLocator->RemoveRequest(this);
  return NS_OK;
}

NS_IMETHODIMP
nsGeolocationRequest::Allow()
{
  // Kick off the geo device, if it isn't already running
  nsRefPtr<nsGeolocationService> geoService = nsGeolocationService::GetInstance();
  nsresult rv = geoService->StartDevice();
  
  if (NS_FAILED(rv)) {

    if (!mErrorCallback)
      return NS_OK;  // If no one is listening for errors, fail silently.

    // TODO what are the real error values here!!
    nsRefPtr<nsDOMGeoPositionError> positionError = new nsDOMGeoPositionError(1, NS_LITERAL_STRING(""));

    nsCOMPtr<nsIDOMGeoPositionErrorCallback> callbackProxy;

    nsCOMPtr<nsIProxyObjectManager> proxyObjMgr = do_GetService("@mozilla.org/xpcomproxy;1");
    proxyObjMgr->GetProxyForObject(NS_PROXY_TO_MAIN_THREAD,
                                   NS_GET_IID(nsIDOMGeoPositionErrorCallback),
                                   mErrorCallback,
                                   NS_PROXY_ASYNC | NS_PROXY_ALWAYS,
                                   getter_AddRefs(callbackProxy));


    // Ensure that the proper context is on the stack (bug 452762)
    nsCOMPtr<nsIJSContextStack> stack(do_GetService("@mozilla.org/js/xpc/ContextStack;1"));
    if (!stack || NS_FAILED(stack->Push(nsnull)))
      return NS_OK; // silently fail

    callbackProxy->HandleEvent(positionError);

    // remove the stack
    JSContext* cx;
    stack->Pop(&cx);

    return rv;
  }

  mAllowed = PR_TRUE;
  return NS_OK;
}

NS_IMETHODIMP
nsGeolocationRequest::AllowButFuzz()
{
  mFuzzLocation = PR_TRUE;
  return Allow();
}

void
nsGeolocationRequest::MarkCleared()
{
  mCleared = PR_TRUE;
}

void
nsGeolocationRequest::SendLocation(nsIDOMGeoPosition* position)
{
  if (mCleared || !mAllowed)
    return;

  // Ensure that the proper context is on the stack (bug 452762)
  nsCOMPtr<nsIJSContextStack> stack(do_GetService("@mozilla.org/js/xpc/ContextStack;1"));
  if (!stack || NS_FAILED(stack->Push(nsnull)))
    return; // silently fail
  
  //TODO mFuzzLocation.  Needs to be defined what we do here.
  if (mFuzzLocation)
  {
    // need to make a copy because nsIDOMGeoPosition is
    // readonly, and we are not sure of its implementation.

    double lat, lon, alt, herror, verror, heading, velocity;
    DOMTimeStamp time;
    position->GetLatitude(&lat);
    position->GetLongitude(&lon);
    position->GetAltitude(&alt);
    position->GetAccuracy(&herror);
    position->GetAltitudeAccuracy(&verror);
    position->GetHeading(&heading);
    position->GetVelocity(&velocity);
    position->GetTimestamp(&time); 

    // Truncate ?
    // lat = floor(lat*10+.5)/10;
    // lon = floor(lon*10+.5)/10;
    // herror = 1600; /* about 1 mile */

    lat = 0;
    lon = 0;
    herror = 0;
    heading = 0; 
    velocity = 0;
    alt = 0;
    verror = 0;

    nsRefPtr<nsGeoPosition> somewhere = new nsGeoPosition(lat,
                                                          lon,
                                                          alt,
                                                          herror,
                                                          verror,
                                                          heading,
                                                          velocity,
                                                          time);
    mCallback->HandleEvent(somewhere);
  }
  else
  {
    mCallback->HandleEvent(position);
  }

  // remove the stack
  JSContext* cx;
  stack->Pop(&cx);
}

void
nsGeolocationRequest::Shutdown()
{
  mCleared = PR_TRUE;
  mCallback = nsnull;
  mErrorCallback = nsnull;
}

////////////////////////////////////////////////////
// nsGeoPosition
////////////////////////////////////////////////////
NS_INTERFACE_MAP_BEGIN(nsGeoPosition)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIDOMGeoPosition)
  NS_INTERFACE_MAP_ENTRY(nsIDOMGeoPosition)
  NS_DOM_INTERFACE_MAP_ENTRY_CLASSINFO(GeoPosition)
NS_INTERFACE_MAP_END

NS_IMPL_THREADSAFE_ADDREF(nsGeoPosition)
NS_IMPL_THREADSAFE_RELEASE(nsGeoPosition)

NS_IMETHODIMP
nsGeoPosition::GetLatitude(double *aLatitude)
{
  *aLatitude = mLat;
  return NS_OK;
}

NS_IMETHODIMP
nsGeoPosition::GetLongitude(double *aLongitude)
{
  *aLongitude = mLong;
  return NS_OK;
}

NS_IMETHODIMP
nsGeoPosition::GetAltitude(double *aAltitude)
{
  *aAltitude = mAlt;
  return NS_OK;
}

NS_IMETHODIMP
nsGeoPosition::GetAccuracy(double *aAccuracy)
{
  *aAccuracy = mHError;
  return NS_OK;
}

NS_IMETHODIMP
nsGeoPosition::GetAltitudeAccuracy(double *aAltitudeAccuracy)
{
  *aAltitudeAccuracy = mVError;
  return NS_OK;
}

NS_IMETHODIMP
nsGeoPosition::GetHeading(double *aHeading)
{
  *aHeading = mHeading;
  return NS_OK;
}

NS_IMETHODIMP
nsGeoPosition::GetVelocity(double *aVelocity)
{
  *aVelocity = mVelocity;
  return NS_OK;
}

NS_IMETHODIMP
nsGeoPosition::GetTimestamp(DOMTimeStamp* aTimestamp)
{
  *aTimestamp = mTimestamp;
  return NS_OK;
}

////////////////////////////////////////////////////
// nsGeolocationService
////////////////////////////////////////////////////
NS_INTERFACE_MAP_BEGIN(nsGeolocationService)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIGeolocationUpdate)
  NS_INTERFACE_MAP_ENTRY(nsIGeolocationUpdate)
  NS_INTERFACE_MAP_ENTRY(nsIGeolocationService)
  NS_INTERFACE_MAP_ENTRY(nsIObserver)
NS_INTERFACE_MAP_END

NS_IMPL_THREADSAFE_ADDREF(nsGeolocationService)
NS_IMPL_THREADSAFE_RELEASE(nsGeolocationService)

nsGeolocationService::nsGeolocationService()
{
  nsCOMPtr<nsIObserverService> obs = do_GetService("@mozilla.org/observer-service;1");
  if (obs) {
    obs->AddObserver(this, "quit-application", false);
  }

  mTimeout = nsContentUtils::GetIntPref("geo.timeout", 6000);
}

nsGeolocationService::~nsGeolocationService()
{
}

NS_IMETHODIMP
nsGeolocationService::Observe(nsISupports* aSubject, const char* aTopic,
                             const PRUnichar* aData)
{
  if (!strcmp("quit-application", aTopic))
  {
    nsCOMPtr<nsIObserverService> obs = do_GetService("@mozilla.org/observer-service;1");
    if (obs) {
      obs->RemoveObserver(this, "quit-application");
    }

    for (PRUint32 i = 0; i< mGeolocators.Length(); i++)
      mGeolocators[i]->Shutdown();

    StopDevice();

    // Remove our reference to any prompt that may have been set.
    mPrompt = nsnull;
    return NS_OK;
  }
  
  if (!strcmp("timer-callback", aTopic))
  {
    // decide if we can close down the service.
    for (PRUint32 i = 0; i< mGeolocators.Length(); i++)
      if (mGeolocators[i]->HasActiveCallbacks())
      {
        SetDisconnectTimer();
        return NS_OK;
      }
    
    // okay to close up.
    StopDevice();
    Update(nsnull);
    return NS_OK;
  }

  return NS_ERROR_FAILURE;
}

NS_IMETHODIMP
nsGeolocationService::GetPrompt(nsIGeolocationPrompt * *aPrompt)
{
  NS_ENSURE_ARG_POINTER(aPrompt);
  *aPrompt = mPrompt;
  NS_IF_ADDREF(*aPrompt);
  return NS_OK;
}

NS_IMETHODIMP
nsGeolocationService::SetPrompt(nsIGeolocationPrompt * aPrompt)
{
  mPrompt = aPrompt;
  return NS_OK;
}

NS_IMETHODIMP
nsGeolocationService::Update(nsIDOMGeoPosition *somewhere)
{
  for (PRUint32 i = 0; i< mGeolocators.Length(); i++)
    mGeolocators[i]->Update(somewhere);
  return NS_OK;
}

already_AddRefed<nsIDOMGeoPosition>
nsGeolocationService::GetLastKnownPosition()
{
  nsIDOMGeoPosition* p = nsnull;
  if (mProvider)
    mProvider->GetCurrentPosition(&p);

  return p;
}

PRBool
nsGeolocationService::IsDeviceReady()
{
  PRBool ready = PR_FALSE;
  if (mProvider)
    mProvider->IsReady(&ready);

  return ready;
}

nsresult
nsGeolocationService::StartDevice()
{
  if (!mProvider)
  {
    // Check to see if there is an override in place. if so, use it.
    mProvider = do_GetService(NS_GEOLOCATION_PROVIDER_CONTRACTID);

    // if NS_MAEMO_LOCATION, see if we should try the MAEMO location provider
#ifdef NS_MAEMO_LOCATION
    if (!mProvider)
    {
      // guess not, lets try a default one:  
      mProvider = new MaemoLocationProvider();
    }
#endif

    if (!mProvider)
      return NS_ERROR_NOT_AVAILABLE;
    
    // if we have one, start it up.
    nsresult rv = mProvider->Startup();
    if (NS_FAILED(rv)) 
      return NS_ERROR_NOT_AVAILABLE;
 
    // lets monitor it for any changes.
    mProvider->Watch(this);
    
    // we do not want to keep the geolocation devices online
    // indefinitely.  Close them down after a reasonable period of
    // inactivivity
    SetDisconnectTimer();
  }

  return NS_OK;
}

void
nsGeolocationService::SetDisconnectTimer()
{
  if (!mDisconnectTimer)
    mDisconnectTimer = do_CreateInstance("@mozilla.org/timer;1");
  else
    mDisconnectTimer->Cancel();

  mDisconnectTimer->Init(this,
                         mTimeout,
                         nsITimer::TYPE_ONE_SHOT);
}

void 
nsGeolocationService::StopDevice()
{
  if (mProvider) {
    mProvider->Shutdown();
    mProvider = nsnull;
  }

  if(mDisconnectTimer) {
    mDisconnectTimer->Cancel();
    mDisconnectTimer = nsnull;
  }
}

nsGeolocationService* nsGeolocationService::gService = nsnull;

nsGeolocationService*
nsGeolocationService::GetInstance()
{
  if (!nsGeolocationService::gService) {
    nsGeolocationService::gService = new nsGeolocationService();
  }
  return nsGeolocationService::gService;
}

nsGeolocationService*
nsGeolocationService::GetGeolocationService()
{
  nsGeolocationService* inst = nsGeolocationService::GetInstance();
  NS_ADDREF(inst);
  return inst;
}

void
nsGeolocationService::AddLocator(nsGeolocation* locator)
{
  mGeolocators.AppendElement(locator);
}

void
nsGeolocationService::RemoveLocator(nsGeolocation* locator)
{
  mGeolocators.RemoveElement(locator);
}

////////////////////////////////////////////////////
// nsGeolocation
////////////////////////////////////////////////////

NS_INTERFACE_MAP_BEGIN(nsGeolocation)
  NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIDOMGeoGeolocation)
  NS_INTERFACE_MAP_ENTRY(nsIDOMGeoGeolocation)
  NS_DOM_INTERFACE_MAP_ENTRY_CLASSINFO(GeoGeolocation)
NS_INTERFACE_MAP_END

NS_IMPL_ADDREF(nsGeolocation)
NS_IMPL_RELEASE(nsGeolocation)

nsGeolocation::nsGeolocation(nsIDOMWindow* contentDom) 
: mUpdateInProgress(PR_FALSE)
{
  // Remember the window
  nsCOMPtr<nsPIDOMWindow> window = do_QueryInterface(contentDom);
  if (window)
    mOwner = window->GetCurrentInnerWindow();

  // Grab the uri of the document
  nsCOMPtr<nsIDOMDocument> domdoc;
  contentDom->GetDocument(getter_AddRefs(domdoc));
  nsCOMPtr<nsIDocument> doc = do_QueryInterface(domdoc);
  if (doc)
    doc->NodePrincipal()->GetURI(getter_AddRefs(mURI));

  mService = nsGeolocationService::GetInstance();
  if (mService)
    mService->AddLocator(this);
}

nsGeolocation::~nsGeolocation()
{
}

void
nsGeolocation::Shutdown()
{
  // Shutdown and release all callbacks
  for (PRInt32 i = 0; i< mPendingCallbacks.Count(); i++)
    mPendingCallbacks[i]->Shutdown();
  mPendingCallbacks.Clear();

  for (PRInt32 i = 0; i< mWatchingCallbacks.Count(); i++)
    mWatchingCallbacks[i]->Shutdown();
  mWatchingCallbacks.Clear();

  if (mService)
    mService->RemoveLocator(this);

  mService = nsnull;
  mOwner = nsnull;
  mURI = nsnull;
}

PRBool
nsGeolocation::HasActiveCallbacks()
{
  return (PRBool) mWatchingCallbacks.Count();
}

void
nsGeolocation::RemoveRequest(nsGeolocationRequest* request)
{
  mPendingCallbacks.RemoveObject(request);

  // if it is in the mWatchingCallbacks, we can't do much
  // since we passed back the position in the array to who
  // ever called WatchPosition() and we do not want to mess
  // around with the ordering of the array.  Instead, just
  // mark the request as "cleared".

  request->MarkCleared();
}

void
nsGeolocation::Update(nsIDOMGeoPosition *somewhere)
{
  // This method calls out to objects which may spin and
  // event loop which may add new location objects into
  // mPendingCallbacks, and mWatchingCallbacks.  Since this
  // function can only be called on the primary thread, we
  // can lock this method with a member var.

  if (mUpdateInProgress)
    return;

  mUpdateInProgress = PR_TRUE;
  if (!OwnerStillExists())
  {
    Shutdown();
    return;
  }

  // notify anyone that has been waiting
  for (PRInt32 i = 0; i< mPendingCallbacks.Count(); i++)
    mPendingCallbacks[i]->SendLocation(somewhere);
  mPendingCallbacks.Clear();

  // notify everyone that is watching
  for (PRInt32 i = 0; i< mWatchingCallbacks.Count(); i++)
      mWatchingCallbacks[i]->SendLocation(somewhere);

  mUpdateInProgress = PR_FALSE;
}

NS_IMETHODIMP
nsGeolocation::GetLastPosition(nsIDOMGeoPosition * *aLastPosition)
{
  // we are advocating that this method be removed.
  NS_ENSURE_ARG_POINTER(aLastPosition);
  *aLastPosition = nsnull;
  return NS_OK;
}

NS_IMETHODIMP
nsGeolocation::GetCurrentPosition(nsIDOMGeoPositionCallback *callback,
                                  nsIDOMGeoPositionErrorCallback *errorCallback,
                                  nsIDOMGeoPositionOptions *options)
{
  nsIGeolocationPrompt* prompt = mService->GetPrompt();
  if (prompt == nsnull)
    return NS_ERROR_NOT_AVAILABLE;

  nsRefPtr<nsGeolocationRequest> request = new nsGeolocationRequest(this, callback, errorCallback);
  prompt->Prompt(request);

  // What if you have a location provider that only sends a location once, then stops.?  fix.
  mPendingCallbacks.AppendObject(request);
  return NS_OK;
}

NS_IMETHODIMP
nsGeolocation::WatchPosition(nsIDOMGeoPositionCallback *callback,
                             nsIDOMGeoPositionErrorCallback *errorCallback,
                             nsIDOMGeoPositionOptions *options, 
                             PRUint16 *_retval NS_OUTPARAM)
{
  nsIGeolocationPrompt* prompt = mService->GetPrompt();
  if (prompt == nsnull)
    return NS_ERROR_NOT_AVAILABLE;
    
  nsRefPtr<nsGeolocationRequest> request = new nsGeolocationRequest(this, callback, errorCallback);
  prompt->Prompt(request);

  // need to hand back an index/reference.
  mWatchingCallbacks.AppendObject(request);
  *_retval = mWatchingCallbacks.Count() - 1;
  return NS_OK;
}

NS_IMETHODIMP
nsGeolocation::ClearWatch(PRUint16 watchId)
{
  mWatchingCallbacks[watchId]->MarkCleared();
  return NS_OK;
}

PRBool
nsGeolocation::OwnerStillExists()
{
  if (!mOwner)
    return PR_FALSE;

  nsCOMPtr<nsIDOMWindowInternal> domWindow(mOwner);
  if (domWindow)
  {
    PRBool closed = PR_FALSE;
    domWindow->GetClosed(&closed);
    if (closed)
      return PR_FALSE;
  }

  nsPIDOMWindow* outer = mOwner->GetOuterWindow();
  if (!outer || outer->GetCurrentInnerWindow() != mOwner)
    return PR_FALSE;

  return PR_TRUE;
}