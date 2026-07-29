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

// Verifies the Windows backend NativeImage CPU-buffer support:
//   - DevelNativeImage::SetPixels() stores software-rendered pixels     (was a stub)
//   - EncodeToFile() reads them back via GetPixels()                    (was a stub)
//   - Texture::New(NativeImage) uploads them via TargetTexture() and    (glTexImage2D)
//     the four-quadrant image is shown on screen for visual confirmation.

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <dali-ui-foundation/integration-api/image-loader/texture-manager.h>
#include <dali/devel-api/adaptor-framework/native-image-devel.h>
#include <dali/public-api/adaptor-framework/native-image.h>
#include <dali/public-api/rendering/texture.h>

#include <cstdint>
#include <iostream>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

class NativeImageCpuController : public ConnectionTracker
{
public:
  explicit NativeImageCpuController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &NativeImageCpuController::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Build a 128x128 RGBA8888 four-quadrant test pattern (R / G / B / white).
    const uint32_t       width  = 128u;
    const uint32_t       height = 128u;
    std::vector<uint8_t> pixels(static_cast<size_t>(width) * height * 4u);
    for(uint32_t y = 0u; y < height; ++y)
    {
      for(uint32_t x = 0u; x < width; ++x)
      {
        const size_t i      = (static_cast<size_t>(y) * width + x) * 4u;
        const bool   right  = x >= width / 2u;
        const bool   bottom = y >= height / 2u;
        pixels[i + 0u]      = right ? 255u : 0u;             // R
        pixels[i + 1u]      = bottom ? 255u : 0u;            // G
        pixels[i + 2u]      = (right == bottom) ? 255u : 0u; // B
        pixels[i + 3u]      = 255u;                          // A
      }
    }

    NativeImagePtr nativeImage = NativeImage::New(width, height, NativeImage::COLOR_DEPTH_32);

    const bool setOk = DevelNativeImage::SetPixels(*nativeImage, pixels.data(), Pixel::RGBA8888);
    std::cout << "[native-image-cpu] SetPixels(RGBA8888) -> " << (setOk ? "true" : "false") << std::endl;

    // EncodeToFile() calls GetPixels() internally, so a valid PNG proves the CPU read-back path.
    const bool encodeOk = nativeImage->EncodeToFile("native-image-cpu-test.png");
    std::cout << "[native-image-cpu] EncodeToFile(native-image-cpu-test.png) -> " << (encodeOk ? "true" : "false") << std::endl;

    // Show it on screen: Texture::New(NativeImage) drives CreateResource()/TargetTexture().
    Texture      texture = Texture::New(*nativeImage);
    Dali::String url     = Integration::TextureManager::AddTexture(texture);

    ImageView imageView = ImageView::New(url);
    imageView.SetRequestedWidth(256_spx);
    imageView.SetRequestedHeight(256_spx);
    window.Add(imageView);

    std::cout << "[native-image-cpu] displaying the CPU-buffer native image (256x256)." << std::endl;
  }

private:
  Application& mApplication;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application              application = Application::New(&argc, &argv);
  UiConfig                 config      = UiConfig::New();
  config.Apply();
  NativeImageCpuController controller(application);
  application.MainLoop();
  return 0;
}
