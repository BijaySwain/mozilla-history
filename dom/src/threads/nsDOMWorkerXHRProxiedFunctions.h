/* -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 40 -*- */
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
 * The Original Code is worker threads.
 *
 * The Initial Developer of the Original Code is
 *   Mozilla Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Ben Turner <bent.mozilla@gmail.com> (Original Author)
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

#ifndef __NSDOMWORKERXHRPROXIEDFUNCTIONS_H__
#define __NSDOMWORKERXHRPROXIEDFUNCTIONS_H__

#define MAKE_PROXIED_FUNCTION0(_name) \
  class _name : public nsRunnable \
  { \
  public: \
    _name (nsDOMWorkerXHRProxy* aXHR) \
    : mXHR(aXHR) \
    { \
      NS_ASSERTION(aXHR, "Null pointer!"); \
    } \
  \
    NS_IMETHOD Run() \
    { \
      nsCOMPtr<nsIXMLHttpRequest> xhr = mXHR->GetXMLHttpRequest(); \
      if (xhr) { \
        return xhr-> _name (); \
      } \
      return NS_OK; \
    } \
  private: \
    nsRefPtr<nsDOMWorkerXHRProxy> mXHR; \
  }

#define MAKE_PROXIED_FUNCTION1(_name, _arg1) \
  class _name : public nsRunnable \
  { \
  public: \
     _name (nsDOMWorkerXHRProxy* aXHR, _arg1 aArg1) \
    : mXHR(aXHR), mArg1(aArg1) \
    { \
      NS_ASSERTION(aXHR, "Null pointer!"); \
    } \
  \
    NS_IMETHOD Run() \
    { \
      nsCOMPtr<nsIXMLHttpRequest> xhr = mXHR->GetXMLHttpRequest(); \
      if (xhr) { \
        return xhr-> _name (mArg1); \
      } \
      return NS_OK; \
    } \
  private: \
    nsRefPtr<nsDOMWorkerXHRProxy> mXHR; \
     _arg1 mArg1; \
  }

#define MAKE_PROXIED_FUNCTION2(_name, _arg1, _arg2) \
  class _name : public nsRunnable \
  { \
  public: \
    _name (nsDOMWorkerXHRProxy* aXHR, _arg1 aArg1, _arg2 aArg2) \
    : mXHR(aXHR), mArg1(aArg1), mArg2(aArg2) \
    { \
      NS_ASSERTION(aXHR, "Null pointer!"); \
    } \
  \
    NS_IMETHOD Run() \
    { \
      nsCOMPtr<nsIXMLHttpRequest> xhr = mXHR->GetXMLHttpRequest(); \
      if (xhr) { \
        return xhr-> _name (mArg1, mArg2); \
      } \
      return NS_OK; \
    } \
  private: \
    nsRefPtr<nsDOMWorkerXHRProxy> mXHR; \
    _arg1 mArg1; \
    _arg2 mArg2; \
  }

#define MAKE_PROXIED_FUNCTION3(_name, _arg1, _arg2, _arg3) \
  class _name : public nsRunnable \
  { \
  public: \
    _name (nsDOMWorkerXHRProxy* aXHR, _arg1 aArg1, _arg2 aArg2, _arg3 aArg3) \
    : mXHR(aXHR), mArg1(aArg1), mArg2(aArg2), mArg3(aArg3) \
    { \
      NS_ASSERTION(aXHR, "Null pointer!"); \
    } \
  \
    NS_IMETHOD Run() \
    { \
      nsCOMPtr<nsIXMLHttpRequest> xhr = mXHR->GetXMLHttpRequest(); \
      if (xhr) { \
        return xhr-> _name (mArg1, mArg2, mArg3); \
      } \
      return NS_OK; \
    } \
  private: \
    nsRefPtr<nsDOMWorkerXHRProxy> mXHR; \
    _arg1 mArg1; \
    _arg2 mArg2; \
    _arg3 mArg3; \
  }

#define MAKE_PROXIED_FUNCTION4(_name, _arg1, _arg2, _arg3, _arg4) \
  class _name : public nsRunnable \
  { \
  public: \
    _name (nsDOMWorkerXHRProxy* aXHR, _arg1 aArg1, _arg2 aArg2, _arg3 aArg3, \
           _arg4 aArg4) \
    : mXHR(aXHR), mArg1(aArg1), mArg2(aArg2), mArg3(aArg3), mArg4(aArg4) \
    { \
      NS_ASSERTION(aXHR, "Null pointer!"); \
    } \
  \
    NS_IMETHOD Run() \
    { \
      nsCOMPtr<nsIXMLHttpRequest> xhr = mXHR->GetXMLHttpRequest(); \
      if (xhr) { \
        return xhr-> _name (mArg1, mArg2, mArg3, mArg4); \
      } \
      return NS_OK; \
    } \
  private: \
    nsRefPtr<nsDOMWorkerXHRProxy> mXHR; \
    _arg1 mArg1; \
    _arg2 mArg2; \
    _arg3 mArg3; \
    _arg4 mArg4; \
  }

#define MAKE_PROXIED_FUNCTION5(_name, _arg1, _arg2, _arg3, _arg4, _arg5) \
  class _name : public nsRunnable \
  { \
  public: \
    _name (nsDOMWorkerXHRProxy* aXHR, _arg1 aArg1, _arg2 aArg2, _arg3 aArg3, \
           _arg4 aArg4, _arg5 aArg5) \
    : mXHR(aXHR), mArg1(aArg1), mArg2(aArg2), mArg3(aArg3), mArg4(aArg4), \
      mArg5(aArg5) \
    { \
      NS_ASSERTION(aXHR, "Null pointer!"); \
    } \
  \
    NS_IMETHOD Run() \
    { \
      nsCOMPtr<nsIXMLHttpRequest> xhr = mXHR->GetXMLHttpRequest(); \
      if (xhr) { \
        return xhr-> _name (mArg1, mArg2, mArg3, mArg4, mArg5); \
      } \
      return NS_OK; \
    } \
  private: \
    nsRefPtr<nsDOMWorkerXHRProxy> mXHR; \
    _arg1 mArg1; \
    _arg2 mArg2; \
    _arg3 mArg3; \
    _arg4 mArg4; \
    _arg5 mArg5; \
  }

#define RUN_PROXIED_FUNCTION(_name, _args) \
  PR_BEGIN_MACRO \
    if (mCanceled) { \
      return NS_ERROR_ABORT; \
    } \
    \
    nsCOMPtr<nsIRunnable> method = new :: _name _args; \
    NS_ENSURE_TRUE(method, NS_ERROR_OUT_OF_MEMORY); \
    \
    nsRefPtr<nsResultReturningRunnable> runnable = \
      new nsResultReturningRunnable(mMainThread, method, mWorkerXHR->mWorker); \
    NS_ENSURE_TRUE(runnable, NS_ERROR_OUT_OF_MEMORY); \
    \
    nsresult _rv = runnable->Dispatch(); \
    if (NS_FAILED(_rv)) { \
      return _rv; \
    } \
  PR_END_MACRO

namespace nsDOMWorkerProxiedXHRFunctions
{
  class Abort : public nsRunnable
  {
  public:
    Abort (nsDOMWorkerXHRProxy* aXHR)
    : mXHR(aXHR)
    {
      NS_ASSERTION(aXHR, "Null pointer!");
    }

    NS_IMETHOD Run() {
      return mXHR->Abort();
    }
  private:
    nsRefPtr<nsDOMWorkerXHRProxy> mXHR;
  };

  MAKE_PROXIED_FUNCTION1(GetAllResponseHeaders, char**);

  MAKE_PROXIED_FUNCTION2(GetResponseHeader, const nsACString&, nsACString&);

  MAKE_PROXIED_FUNCTION5(OpenRequest, const nsACString&, const nsACString&,
                         PRBool, const nsAString&, const nsAString&);

  MAKE_PROXIED_FUNCTION1(Send, nsIVariant*);

  MAKE_PROXIED_FUNCTION1(SendAsBinary, const nsAString&);

  MAKE_PROXIED_FUNCTION2(SetRequestHeader, const nsACString&,
                         const nsACString&);

  MAKE_PROXIED_FUNCTION1(OverrideMimeType, const nsACString&);

  MAKE_PROXIED_FUNCTION1(SetMultipart, PRBool);
}

#endif /* __NSDOMWORKERXHRPROXIEDFUNCTIONS_H__ */