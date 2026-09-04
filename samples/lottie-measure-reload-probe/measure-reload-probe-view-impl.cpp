/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "measure-reload-probe-view-impl.h"

#include <dali/devel-api/object/type-registry-helper.h>

#include <iostream>

using namespace Dali;
using namespace Dali::Ui;

namespace LottieProbeSample
{

namespace
{
// For type Registration
BaseHandle Create()
{
  return MeasureReloadProbeView::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN_FULL(MeasureReloadProbeView, MeasureReloadProbeViewImpl, Dali::Ui::View, Create)
DALI_TYPE_REGISTRATION_END()

/// The single Lottie resource this probe loads and then reloads with the SAME url.
/// RESOURCES_DIR is a target compile definition set by DALI_SET_SAMPLE_RESOURCES().
const char* const LOTTIE_URL = RESOURCES_DIR "jolly_walker.json";

/// Fixed child size. Deliberately not MATCH_PARENT: with a fixed size the default
/// arrange places the child straight from its measured size and never re-measures it
/// inside the arrange pass, so the console pass count counts measure passes only.
constexpr float LOTTIE_SIZE = 240.0f;

/// Passes between wall-clock rate reports.
constexpr uint32_t REPORT_INTERVAL = 25u;

} // namespace

IntrusivePtr<MeasureReloadProbeViewImpl> MeasureReloadProbeViewImpl::New()
{
  IntrusivePtr<MeasureReloadProbeViewImpl> impl(new MeasureReloadProbeViewImpl());

  // This class does not override OnArrange, so the default arrange producer and its
  // default ArrangePolicy::IF_CHANGED apply unchanged. Only measurement is customised.

  return impl;
}

MeasureReloadProbeViewImpl::MeasureReloadProbeViewImpl()
: ViewImpl(),
  mLottie(),
  mLastReportTime(Clock::now()),
  mMeasureCount(0u),
  mReloadCount(0u),
  mReloadInMeasure(true),
  mExplicitReload(false),
  mPerPassLogging(true),
  mPlaying(false)
{
  // [NOTE] The handle does not exist yet, so Self() is unavailable here. Everything that
  // needs it -- creating and adding the child, installing the measure callback -- happens
  // in OnInitialize().
}

void MeasureReloadProbeViewImpl::OnInitialize()
{
  ViewImpl::OnInitialize();

  MeasureReloadProbeView handle = MeasureReloadProbeView::DownCast(Self()); // Get handle

  // This hook is the DALi equivalent of "do it in the constructor": ViewImpl's own
  // constructor runs before the handle exists, so Self() -- and therefore Add() -- is
  // only reachable from here onwards. Initialize() calls this hook from
  // MeasureReloadProbeView::New(), so New() still returns a fully built view.
  mLottie = LottieAnimationView::New();
  mLottie.SetResourceUrl(LOTTIE_URL);
  mLottie.SetRequestedWidth(LOTTIE_SIZE);
  mLottie.SetRequestedHeight(LOTTIE_SIZE);
  handle.Add(mLottie);

  // WRAP_CONTENT is only a declaration here: the measure callback below replaces the
  // default measurement entirely, so this view's measured size is whatever the callback
  // returns.
  handle.SetRequestedWidth(WRAP_CONTENT);
  handle.SetRequestedHeight(WRAP_CONTENT);
  handle.SetBackgroundColor(UiColor(0xE8F0FEu));

  // The callback REPLACES the default measurement, so it has to measure the child itself.
  // Binding `this` is lifetime safe: the callback is stored on this very view, so it is
  // destroyed together with the impl that owns it.
  handle.SetMeasureCallback(MeasureCallback::New(this, &MeasureReloadProbeViewImpl::OnMeasurePass));
}

MeasuredSize MeasureReloadProbeViewImpl::OnMeasurePass(View /*self*/, float widthConstraint, float heightConstraint)
{
  ++mMeasureCount;

  if(mReloadInMeasure && mLottie)
  {
    // THE TWO IN-PASS CALLS THIS PROBE COMPARES.
    //
    // SetResourceUrl() with the url that is already set is a NO-OP: it leaves the child
    // clean, raises no InvalidateMeasure(), and the pass therefore converges. That is the
    // default mode, and it is what an application that re-applies its state every pass
    // now gets.
    //
    // Reload() is the explicit reload. It marks the child's visual dirty and calls
    // InvalidateMeasure() from inside a Measure pass, which is the contract violation
    // under test: the work is RETAINED and PARKED (no idle wake of its own), the visual
    // is rebuilt at the child's Measure() further down in this same callback, and the
    // re-dirtying happens while this view's own producer is still running, so this view
    // declines to publish its measure cache and this callback runs again next pass. The
    // child's public InvalidateMeasure() path logs that violation once per view.
    if(mExplicitReload)
    {
      mLottie.Reload();
    }
    else
    {
      mLottie.SetResourceUrl(LOTTIE_URL);
    }
    ++mReloadCount;
  }

  // Nothing else measures the child: a measure callback replaces the default
  // measurement. The default arrange then places the child from the size published here.
  MeasuredSize childSize;
  if(mLottie)
  {
    childSize = mLottie.Measure(widthConstraint, heightConstraint);
  }

  ReportPass();

  return childSize;
}

void MeasureReloadProbeViewImpl::ReportPass()
{
  if(mMeasureCount == 1u)
  {
    // Start the rate clock at the first pass, not at construction, so the first [rate]
    // line does not include application start-up and window creation.
    mLastReportTime = Clock::now();
  }

  // Console only, on purpose. Updating a Label from here would invalidate that Label's
  // measure from inside the pass and add a SECOND in-pass producer, and a Timer would
  // supply main loop wakes of its own -- either one would confound the single question
  // this sample exists to answer.
  if(mPerPassLogging)
  {
    std::cout << "measure #" << mMeasureCount
              << (mReloadInMeasure ? (mExplicitReload ? " (Reload issued)" : " (SetResourceUrl issued)")
                                   : " (reload disabled)")
              << std::endl;
  }

  if((mMeasureCount % REPORT_INTERVAL) == 0u)
  {
    const Clock::time_point now     = Clock::now();
    const double            elapsed = std::chrono::duration<double, std::milli>(now - mLastReportTime).count();

    std::cout << "  [rate] " << REPORT_INTERVAL << " measure passes in " << elapsed << " ms";
    if(elapsed > 0.0)
    {
      std::cout << " (" << (static_cast<double>(REPORT_INTERVAL) * 1000.0 / elapsed) << " passes/sec)";
    }
    std::cout << ", total reloads " << mReloadCount << std::endl;

    mLastReportTime = now;
  }
}

void MeasureReloadProbeViewImpl::SetReloadInMeasure(bool enabled)
{
  if(mReloadInMeasure == enabled)
  {
    return;
  }
  mReloadInMeasure = enabled;

  if(enabled)
  {
    // Event time is the only legal place for this call. It kicks a single pass so the
    // producer starts again after it was switched off and the layout settled.
    MeasureReloadProbeView::DownCast(Self()).InvalidateMeasure();
  }
}

bool MeasureReloadProbeViewImpl::IsReloadInMeasure() const
{
  return mReloadInMeasure;
}

void MeasureReloadProbeViewImpl::SetExplicitReload(bool enabled)
{
  if(mExplicitReload == enabled)
  {
    return;
  }
  mExplicitReload = enabled;

  // Event time is the only legal place for this call. Switching modes changes what the
  // next pass does, so kick one pass to make the new mode observable straight away.
  if(mReloadInMeasure)
  {
    MeasureReloadProbeView::DownCast(Self()).InvalidateMeasure();
  }
}

bool MeasureReloadProbeViewImpl::IsExplicitReload() const
{
  return mExplicitReload;
}

void MeasureReloadProbeViewImpl::SetPerPassLogging(bool enabled)
{
  mPerPassLogging = enabled;
}

bool MeasureReloadProbeViewImpl::IsPerPassLogging() const
{
  return mPerPassLogging;
}

void MeasureReloadProbeViewImpl::SetPlaying(bool playing)
{
  if(!mLottie || mPlaying == playing)
  {
    return;
  }
  mPlaying = playing;

  if(playing)
  {
    mLottie.SetLoopCount(-1);
    mLottie.Play();
  }
  else
  {
    mLottie.Pause();
  }
}

bool MeasureReloadProbeViewImpl::IsPlaying() const
{
  return mPlaying;
}

uint32_t MeasureReloadProbeViewImpl::GetMeasureCount() const
{
  return mMeasureCount;
}

uint32_t MeasureReloadProbeViewImpl::GetReloadCount() const
{
  return mReloadCount;
}

} // namespace LottieProbeSample
