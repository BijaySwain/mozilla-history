/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set sw=2 ts=2 et tw=80 : */
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
 * The Original Code is Mozilla Audio IPC
 *
 * The Initial Developer of the Original Code is
 *   The Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Doug Turner <dougt@mozilla.com>
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

#include "mozilla/dom/AudioChild.h"

namespace mozilla {
namespace dom {
NS_IMPL_THREADSAFE_ADDREF(AudioChild);
NS_IMPL_THREADSAFE_RELEASE(AudioChild);

AudioChild::AudioChild()
  : mLastPosition(-1),
    mLastPositionTimestamp(0),
    mMinWriteSize(-2),// Initial value, -2, error on -1
    mAudioReentrantMonitor("AudioChild.mReentrantMonitor"),
    mIPCOpen(PR_TRUE),
    mDrained(PR_FALSE)
{
  MOZ_COUNT_CTOR(AudioChild);
}

AudioChild::~AudioChild()
{
  MOZ_COUNT_DTOR(AudioChild);
}

void
AudioChild::ActorDestroy(ActorDestroyReason aWhy)
{
  mIPCOpen = PR_FALSE;
}

bool
AudioChild::RecvPositionInFramesUpdate(const PRInt64& position,
                                       const PRInt64& time)
{
  mLastPosition = position;
  mLastPositionTimestamp = time;
  return true;
}

bool
AudioChild::RecvDrainDone()
{
  ReentrantMonitorAutoEnter mon(mAudioReentrantMonitor);
  mDrained = PR_TRUE;
  mAudioReentrantMonitor.NotifyAll();
  return true;
}

PRInt32
AudioChild::WaitForMinWriteSize()
{
  ReentrantMonitorAutoEnter mon(mAudioReentrantMonitor);
  // -2 : initial value
  while (mMinWriteSize == -2 && mIPCOpen) {
    mAudioReentrantMonitor.Wait();
  }
  return mMinWriteSize;
}

bool
AudioChild::RecvMinWriteSizeDone(const PRInt32& minFrames)
{
  ReentrantMonitorAutoEnter mon(mAudioReentrantMonitor);
  mMinWriteSize = minFrames;
  mAudioReentrantMonitor.NotifyAll();
  return true;
}

void
AudioChild::WaitForDrain()
{
  ReentrantMonitorAutoEnter mon(mAudioReentrantMonitor);
  while (!mDrained && mIPCOpen) {
    mAudioReentrantMonitor.Wait();
  }
}

PRInt64
AudioChild::GetLastKnownPosition()
{
  return mLastPosition;
}

PRInt64
AudioChild::GetLastKnownPositionTimestamp()
{
  return mLastPositionTimestamp;
}

} // namespace dom
} // namespace mozilla
