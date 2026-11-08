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
#include <dali/public-api/adaptor-framework/native-image.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/any.h>

namespace DALI_NAMESPACE
{

NativeImagePtr NativeImage::New(unsigned int width, unsigned int height, ColorDepth depth)
{
  Any            empty;
  NativeImagePtr image = new NativeImage(width, height, depth, empty);
  return image;
}

NativeImagePtr NativeImage::New(Any nativeImage)
{
  NativeImagePtr image = new NativeImage(0, 0, COLOR_DEPTH_DEFAULT, nativeImage);
  return image;
}

Any NativeImage::GetNativeImage()
{
  Any source;
  return source;
}

bool NativeImage::GetPixels(Dali::Vector<uint8_t>& pixbuf, uint32_t& width, uint32_t& height, Pixel::Format& pixelFormat) const
{
  return false;
}

bool NativeImage::EncodeToFile(const Dali::String& filename) const
{
  return false;
}

void NativeImage::SetSource(Any source)
{
}

void NativeImage::SetAlphaPreMultiplied(bool preMultiplied)
{
}

bool NativeImage::IsColorDepthSupported(ColorDepth colorDepth)
{
  return false;
}

uint32_t NativeImage::GetWidth() const
{
  return 64;
}

uint32_t NativeImage::GetHeight() const
{
  return 32;
}

bool NativeImage::IsAlphaPreMultiplied() const
{
  return false;
}

bool NativeImage::ApplyNativeFragmentShader(Dali::String& shader, int mask)
{
  shader = String("#extension GL_OES_EGL_image_external:require\n") + shader;
  return true;
}

int NativeImage::GetTextureTarget() const
{
  return 0;
}

const char* NativeImage::GetCustomSamplerTypename() const
{
  return "";
}

bool NativeImage::CreateResource()
{
  return false;
}

void NativeImage::DestroyResource()
{
}

uint32_t NativeImage::TargetTexture()
{
  return 0;
}

Dali::NativeImageInterface::PrepareTextureResult NativeImage::PrepareTexture()
{
  return Dali::NativeImageInterface::PrepareTextureResult::NO_ERROR;
}

bool NativeImage::RequiresBlending() const
{
  return false;
}

Any NativeImage::GetNativeImageHandle() const
{
  return nullptr;
}

bool NativeImage::SourceChanged() const
{
  return false;
}

Rect<uint32_t> NativeImage::GetUpdatedArea()
{
  return Rect<uint32_t>(0, 0, 0, 0);
}

void NativeImage::PostRender()
{
}

NativeImageInterface::Extension* NativeImage::GetExtension()
{
  return nullptr;
}

NativeImage::NativeImage(unsigned int width, unsigned int height, ColorDepth depth, Any nativeImage)
{
}

NativeImage::~NativeImage()
{
}

} //namespace DALI_NAMESPACE
