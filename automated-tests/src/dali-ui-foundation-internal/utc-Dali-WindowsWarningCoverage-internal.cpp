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
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>

#define private public
#define protected public
#include <dali-ui-foundation/integration-api/utility/npatch-helper.h>
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/animated-image/animated-image-visual.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-data.h>
#include <dali-ui-foundation/internal/visuals/npatch/npatch-visual.h>
#include <dali-ui-foundation/internal/visuals/svg/svg-visual.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-base-data-impl.h>
#undef protected
#undef private

using namespace Dali;
using namespace Dali::Ui;

namespace
{
namespace UiInternal    = Dali::Ui::Internal;
namespace UiIntegration = Dali::Ui::Integration;
namespace UiText         = Dali::Ui::Text;

TextureSet CreateTextureSet(uint32_t width, uint32_t height)
{
  TextureSet textureSet = TextureSet::New();
  textureSet.SetTexture(0u, Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, width, height));
  return textureSet;
}

UiIntegration::Visual::Base CreateVisual(UiIntegration::InternalVisualType type, const char* url)
{
  Property::Map properties;
  properties.Add(VisualBasePropertyIndex::TYPE, type);
  properties.Add(ImageVisualPropertyIndex::URL, url);
  return UiIntegration::VisualFactory::Get().CreateVisual(properties);
}

UiInternal::Visual::Base& GetVisualObject(UiIntegration::Visual::Base visual)
{
  return Dali::Ui::GetImplementation(visual).GetVisualObject();
}

UiText::AsyncTextParameters CreateMarqueeParameters()
{
  UiText::AsyncTextParameters parameters;
  parameters.text               = "A long line of text used to exceed a deliberately small texture limit";
  parameters.fontSize           = 16.0f;
  parameters.textWidth          = 100.0f;
  parameters.textHeight         = 100.0f;
  parameters.originWidth        = 100.0f;
  parameters.originHeight       = 100.0f;
  parameters.maxTextureSize     = 16;
  parameters.requestType        = UiText::Async::RENDER_FIXED_SIZE;
  parameters.isMarqueeEnabled   = true;
  parameters.marqueeGap         = 4;
  parameters.marqueeOrientation = UiText::MarqueeOrientation::HORIZONTAL;
  return parameters;
}
} // unnamed namespace

int UtcDaliWindowsWarningCoverageNPatchP(void)
{
  UiTestApplication application;

  Geometry geometry = UiIntegration::NPatchHelper::CreateGridGeometry(Uint16Pair(2u, 2u));
  DALI_TEST_CHECK(geometry);

  Shader   shader   = Shader::New("void main(){}", "void main(){}");
  Renderer renderer = Renderer::New(geometry, shader);

  UiInternal::NPatchDataPtr data(new UiInternal::NPatchData());
  data->SetCroppedWidth(30u);
  data->SetCroppedHeight(40u);
  UiIntegration::NPatchUtility::StretchRanges stretchPixelsX;
  UiIntegration::NPatchUtility::StretchRanges stretchPixelsY;
  stretchPixelsX.PushBack(Uint16Pair(2u, 8u));
  stretchPixelsY.PushBack(Uint16Pair(3u, 10u));
  data->SetStretchPixelsX(stretchPixelsX);
  data->SetStretchPixelsY(stretchPixelsY);
  UiIntegration::NPatchHelper::ApplyTextureAndUniforms(renderer, data.Get());

  UiIntegration::Visual::Base visual = CreateVisual(UiIntegration::InternalVisualType::N_PATCH, "coverage.9.png");
  auto&                       npatch = static_cast<UiInternal::NPatchVisual&>(GetVisualObject(visual));
  data->SetId(12345);
  data->SetLoadingState(UiInternal::NPatchData::LoadingState::LOAD_COMPLETE);
  npatch.mLoader.mCache.emplace_back(data);
  npatch.mId = data->GetId();

  Vector2 naturalSize;
  visual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(30.0f, 40.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliWindowsWarningCoverageImageVisualsP(void)
{
  UiTestApplication application;

  Property::Map animatedProperties;
  animatedProperties.Add(VisualBasePropertyIndex::TYPE, UiIntegration::InternalVisualType::ANIMATED_IMAGE);
  animatedProperties.Add(ImageVisualPropertyIndex::URL, "coverage.gif");
  animatedProperties.Add(ImageVisualPropertyIndex::DESIRED_WIDTH, 20);
  animatedProperties.Add(ImageVisualPropertyIndex::DESIRED_HEIGHT, 30);
  animatedProperties.Add(ImageVisualPropertyIndex::WRAP_MODE_U, WrapMode::REPEAT);
  animatedProperties.Add(ImageVisualPropertyIndex::WRAP_MODE_V, WrapMode::MIRRORED_REPEAT);
  UiIntegration::Visual::Base animatedVisual = UiIntegration::VisualFactory::Get().CreateVisual(animatedProperties);
  auto&                       animatedImage = static_cast<UiInternal::AnimatedImageVisual&>(GetVisualObject(animatedVisual));
  TextureSet                  animatedTextures = CreateTextureSet(41u, 43u);
  animatedImage.mImpl->mRenderer.SetTextures(animatedTextures);

  Vector2 naturalSize;
  animatedVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(41.0f, 43.0f), TEST_LOCATION);

  UiIntegration::Visual::Base svgVisual = CreateVisual(UiIntegration::InternalVisualType::SVG, "coverage.svg");
  auto&                       svg       = static_cast<UiInternal::SvgVisual&>(GetVisualObject(svgVisual));
  svg.mDefaultWidth                     = 47u;
  svg.mDefaultHeight                    = 53u;
  svg.mDesiredSize                      = ImageDimensions();
  svg.mLoadFailed                       = false;
  svgVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(47.0f, 53.0f), TEST_LOCATION);

  svg.mLoadFailed = true;
  TextureSet svgTextures = CreateTextureSet(59u, 61u);
  svg.mImpl->mRenderer.SetTextures(svgTextures);
  svgVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(59.0f, 61.0f), TEST_LOCATION);

  UiIntegration::Visual::Base imageVisual = CreateVisual(UiIntegration::InternalVisualType::IMAGE, "/invalid/coverage.png");
  auto&                       image       = static_cast<UiInternal::ImageVisual&>(GetVisualObject(imageVisual));
  image.mFactoryCache.mBrokenImageInfoContainer.clear();
  image.mFactoryCache.mBrokenImageInfoContainer.emplace_back();
  auto& brokenImageInfo      = image.mFactoryCache.mBrokenImageInfoContainer.back();
  brokenImageInfo.visualType = UiInternal::VisualUrl::REGULAR_IMAGE;
  brokenImageInfo.texture    = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 67u, 71u);
  brokenImageInfo.width      = 67u;
  brokenImageInfo.height     = 71u;
  imageVisual.GetNaturalSize(naturalSize);
  DALI_TEST_EQUALS(naturalSize, Vector2(67.0f, 67.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliWindowsWarningCoverageAsyncTextP(void)
{
  UiTestApplication application;
  UiText::AsyncTextLoader loader = UiText::AsyncTextLoader::New();

  UiText::AsyncTextParameters horizontal = CreateMarqueeParameters();
  loader.RenderMarquee(horizontal, true, Size(100.0f, 16.0f));

  UiText::AsyncTextParameters vertical = CreateMarqueeParameters();
  vertical.textWidth                   = 20.0f;
  vertical.marqueeOrientation          = UiText::MarqueeOrientation::VERTICAL;
  loader.RenderMarquee(vertical, true, Size(20.0f, 100.0f));

  DALI_TEST_EQUALS(horizontal.textWidth, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(vertical.textHeight, 100.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliWindowsWarningCoverageTextVisualMaxTextureP(void)
{
  UiTestApplication application;
  View              view = View::New();

  Property::Map properties;
  properties.Add(VisualBasePropertyIndex::TYPE, UiIntegration::InternalVisualType::TEXT);
  properties.Add(TextVisualPropertyIndex::TEXT, "maximum texture size coverage");
  UiIntegration::Visual::Base visual = UiIntegration::VisualFactory::Get().CreateVisual(properties);
  UiInternal::ViewDataImpl::Get(Dali::Ui::GetImpl(view)).RegisterVisual(1, visual, UiIntegration::DepthIndex::CONTENT);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  UiText::AsyncTextParameters parameters = CreateMarqueeParameters();
  parameters.textWidth                   = 100000.0f;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(visual, parameters));
  DALI_TEST_CHECK(parameters.textWidth < 100000.0f);

  END_TEST;
}
