/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */
#include <dali/public-api/adaptor-framework/native-image.h>
#include <test-trace-call-stack.h>
#include <test-native-image.h>
#include <unordered_map>

// The engine callback takes NativeImage, whereas TestNativeImage implements its
// base interface. As in the toolkit harness, interpose the platform wrapper for
// this test executable so frames exercise WidgetView without an X11/EGL backend.
namespace
{
std::unordered_map<const Dali::NativeImage*, Dali::TestNativeImagePointer> images;
}

Dali::TestNativeImagePointer GetWidgetTestNativeImage(const Dali::NativeImage& image)
{
  return images.at(&image);
}

namespace DALI_NAMESPACE
{
NativeImagePtr NativeImage::New(uint32_t width, uint32_t height, ColorDepth depth)
{
  return new NativeImage(width, height, depth, Any());
}

NativeImage::NativeImage(uint32_t width, uint32_t height, ColorDepth, Any)
: mImpl(nullptr)
{
  images.emplace(this, TestNativeImage::New(width, height));
}

NativeImage::~NativeImage()
{
  images.erase(this);
}

bool NativeImage::CreateResource()
{
  return images.at(this)->CreateResource();
}

void NativeImage::DestroyResource()
{
  images.at(this)->DestroyResource();
}

uint32_t NativeImage::TargetTexture()
{
  return images.at(this)->TargetTexture();
}

NativeImageInterface::PrepareTextureResult NativeImage::PrepareTexture()
{
  return images.at(this)->PrepareTexture();
}

uint32_t NativeImage::GetWidth() const
{
  return images.at(this)->GetWidth();
}

uint32_t NativeImage::GetHeight() const
{
  return images.at(this)->GetHeight();
}

bool NativeImage::RequiresBlending() const
{
  return images.at(this)->RequiresBlending();
}

int NativeImage::GetTextureTarget() const
{
  return images.at(this)->GetTextureTarget();
}

bool NativeImage::ApplyNativeFragmentShader(String& shader, int mask)
{
  return images.at(this)->ApplyNativeFragmentShader(shader, mask);
}

const char* NativeImage::GetCustomSamplerTypename() const
{
  return images.at(this)->GetCustomSamplerTypename();
}

Any NativeImage::GetNativeImageHandle() const
{
  return images.at(this)->GetNativeImageHandle();
}

bool NativeImage::SourceChanged() const
{
  return images.at(this)->SourceChanged();
}

Rect<uint32_t> NativeImage::GetUpdatedArea()
{
  return images.at(this)->GetUpdatedArea();
}

void NativeImage::PostRender()
{
  images.at(this)->PostRender();
}

NativeImageInterface::Extension* NativeImage::GetExtension()
{
  return images.at(this)->GetExtension();
}
} // namespace DALI_NAMESPACE
