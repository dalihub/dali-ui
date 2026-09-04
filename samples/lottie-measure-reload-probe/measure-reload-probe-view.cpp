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

#include "measure-reload-probe-view.h"
#include "measure-reload-probe-view-impl.h"

using namespace Dali;
using namespace Dali::Ui;

namespace LottieProbeSample
{

namespace
{
// Define conversion methods
inline MeasureReloadProbeViewImpl& GetImpl(MeasureReloadProbeView& probeView)
{
  DALI_ASSERT_ALWAYS(probeView);
  Dali::RefObject& handle = probeView.GetImplementation();
  return static_cast<MeasureReloadProbeViewImpl&>(handle);
}

inline const MeasureReloadProbeViewImpl& GetImpl(const MeasureReloadProbeView& probeView)
{
  DALI_ASSERT_ALWAYS(probeView);
  const Dali::RefObject& handle = probeView.GetImplementation();
  return static_cast<const MeasureReloadProbeViewImpl&>(handle);
}
} // namespace

MeasureReloadProbeView MeasureReloadProbeView::New()
{
  IntrusivePtr<MeasureReloadProbeViewImpl> impl = MeasureReloadProbeViewImpl::New();
  MeasureReloadProbeView                   handle(*impl); // The handle takes ownership of the impl
  impl->Initialize();
  return handle;
}

MeasureReloadProbeView MeasureReloadProbeView::DownCast(BaseHandle handle)
{
  return View::DownCast<MeasureReloadProbeView, MeasureReloadProbeViewImpl>(handle);
}

MeasureReloadProbeView::MeasureReloadProbeView()
{
}

MeasureReloadProbeView::MeasureReloadProbeView(const MeasureReloadProbeView& probeView)
: View(probeView)
{
}

MeasureReloadProbeView::MeasureReloadProbeView(MeasureReloadProbeView&& rhs) noexcept
: View(std::move(rhs))
{
}

MeasureReloadProbeView::MeasureReloadProbeView(MeasureReloadProbeViewImpl& impl) // Needed by New()
: View(impl)
{
}

MeasureReloadProbeView::MeasureReloadProbeView(Dali::Internal::CustomActor* customActor) // Needed by DownCast()
: View(customActor)
{
  VerifyCustomActorPointer<MeasureReloadProbeViewImpl>(customActor);
}

MeasureReloadProbeView::~MeasureReloadProbeView()
{
}

void MeasureReloadProbeView::SetReloadInMeasure(bool enabled)
{
  GetImpl(*this).SetReloadInMeasure(enabled);
}

bool MeasureReloadProbeView::IsReloadInMeasure() const
{
  return GetImpl(*this).IsReloadInMeasure();
}

void MeasureReloadProbeView::SetExplicitReload(bool enabled)
{
  GetImpl(*this).SetExplicitReload(enabled);
}

bool MeasureReloadProbeView::IsExplicitReload() const
{
  return GetImpl(*this).IsExplicitReload();
}

void MeasureReloadProbeView::SetPerPassLogging(bool enabled)
{
  GetImpl(*this).SetPerPassLogging(enabled);
}

bool MeasureReloadProbeView::IsPerPassLogging() const
{
  return GetImpl(*this).IsPerPassLogging();
}

void MeasureReloadProbeView::SetPlaying(bool playing)
{
  GetImpl(*this).SetPlaying(playing);
}

bool MeasureReloadProbeView::IsPlaying() const
{
  return GetImpl(*this).IsPlaying();
}

uint32_t MeasureReloadProbeView::GetMeasureCount() const
{
  return GetImpl(*this).GetMeasureCount();
}

uint32_t MeasureReloadProbeView::GetReloadCount() const
{
  return GetImpl(*this).GetReloadCount();
}

} // namespace LottieProbeSample
