/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 *
 */

// CLASS HEADER
#include <dali-ui-foundation/public-api/views/gl/gl-view-offscreen-config.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_OFFSCREEN_CONFIG(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from GlViewOffscreenConfig object")

namespace DALI_NAMESPACE
{
namespace Ui
{
struct GlViewOffscreenConfig::Impl
{
  ColorFormat        colorFormat{ColorFormat::RGBA8888};
  GraphicsApiVersion apiVersion{GraphicsApiVersion::GLES_VERSION_3_0};
  uint32_t           sampleCount{0u};
  bool               depthEnabled{false};
  bool               stencilEnabled{false};
};

GlViewOffscreenConfig::GlViewOffscreenConfig()
: mImpl(MakeUnique<Impl>())
{
}

GlViewOffscreenConfig::~GlViewOffscreenConfig() = default;

GlViewOffscreenConfig::GlViewOffscreenConfig(const GlViewOffscreenConfig& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(rhs.mImpl);
  mImpl = MakeUnique<Impl>(*rhs.mImpl);
}

GlViewOffscreenConfig::GlViewOffscreenConfig(GlViewOffscreenConfig&& rhs) noexcept = default;

GlViewOffscreenConfig& GlViewOffscreenConfig::operator=(const GlViewOffscreenConfig& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_OFFSCREEN_CONFIG(rhs.mImpl);
    mImpl = MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

GlViewOffscreenConfig& GlViewOffscreenConfig::operator=(GlViewOffscreenConfig&& rhs) noexcept = default;

void GlViewOffscreenConfig::SetColorFormat(ColorFormat format)
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  mImpl->colorFormat = format;
}

GlViewOffscreenConfig::ColorFormat GlViewOffscreenConfig::GetColorFormat() const
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  return mImpl->colorFormat;
}

void GlViewOffscreenConfig::SetDepthEnabled(bool enabled)
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  mImpl->depthEnabled = enabled;
}

bool GlViewOffscreenConfig::IsDepthEnabled() const
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  return mImpl->depthEnabled;
}

void GlViewOffscreenConfig::SetStencilEnabled(bool enabled)
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  mImpl->stencilEnabled = enabled;
}

bool GlViewOffscreenConfig::IsStencilEnabled() const
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  return mImpl->stencilEnabled;
}

void GlViewOffscreenConfig::SetSampleCount(uint32_t samples)
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  mImpl->sampleCount = samples;
}

uint32_t GlViewOffscreenConfig::GetSampleCount() const
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  return mImpl->sampleCount;
}

void GlViewOffscreenConfig::SetGraphicsApiVersion(GraphicsApiVersion version)
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  mImpl->apiVersion = version;
}

GlViewOffscreenConfig::GraphicsApiVersion GlViewOffscreenConfig::GetGraphicsApiVersion() const
{
  DALI_ASSERT_VALID_OFFSCREEN_CONFIG(mImpl);
  return mImpl->apiVersion;
}

} // namespace Ui
} //namespace DALI_NAMESPACE
