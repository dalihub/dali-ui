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

// EXTERNAL INCLUDES
#include <dali.h>
#include <dali/devel-api/adaptor-framework/image-loading-devel.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/public-api/object/weak-handle.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <ctime>
#include <functional>
#include <utility>

// Install the callback mock before production headers.
#include <dali-ui/ui-event-thread-callback.h>

// Load task types before other headers introduce Internal::Text.
#include <dali-ui-foundation/internal/text/async-text/async-text-manager-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/integration-api/text/reveal-integ.h>
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/text-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-data.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/text/styled-text/gradient-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-async-task-manager.h>
#include "inline-replacement-manager-test-accessor.h"

using namespace Dali;

namespace Dali::Ui::Internal
{
struct TextVisualTestAccessor
{
  static bool PublishBlur(Ui::Integration::Visual::Base visual, Actor owner, const PreparedRevealBlur& prepared, uint64_t revision,
                          const Vector<Ui::Text::ReplacementRevealTiming>& ordinaryTimings)
  {
    return static_cast<TextVisual&>(Ui::GetImplementation(visual).GetVisualObject()).PublishPreparedRevealBlur(owner, prepared, revision, ordinaryTimings);
  }
};
} // namespace Dali::Ui::Internal

namespace
{
constexpr float VISUAL_WIDTH              = 220.0f;
constexpr float VISUAL_HEIGHT             = 64.0f;
constexpr int   ASYNC_TEXT_THREAD_TIMEOUT = 5;

namespace UiInternal        = Dali::Ui::Internal;
namespace UiIntegrationText = Dali::Ui::Integration::Text;
namespace UiText            = Dali::Ui::Text;

struct RenderedTextVisual
{
  Dali::Ui::View                      view;
  Dali::Ui::Integration::Visual::Base visual;
};

bool HasValidTexture(Actor actor)
{
  for(uint32_t rendererIndex = 0u; rendererIndex < actor.GetRendererCount(); ++rendererIndex)
  {
    TextureSet textures = actor.GetRendererAt(rendererIndex).GetTextures();
    if(!textures || textures.GetTextureCount() == 0u)
    {
      continue;
    }

    Texture texture = textures.GetTexture(0u);
    if(texture && texture.GetWidth() > 0u && texture.GetHeight() > 0u)
    {
      return true;
    }
  }
  return false;
}

bool HasMultiColorTextRenderer(Actor actor)
{
  for(uint32_t rendererIndex = 0u; rendererIndex < actor.GetRendererCount(); ++rendererIndex)
  {
    Renderer              renderer = actor.GetRendererAt(rendererIndex);
    const Property::Index index    = renderer.GetPropertyIndex("uHasMultipleTextColors");
    if(index != Property::INVALID_INDEX && renderer.GetProperty<float>(index) > 0.5f)
    {
      return true;
    }
  }
  return false;
}

bool HasTextRevealRenderer(Actor actor)
{
  for(uint32_t rendererIndex = 0u; rendererIndex < actor.GetRendererCount(); ++rendererIndex)
  {
    Renderer renderer = actor.GetRendererAt(rendererIndex);
    if(renderer.GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX &&
       renderer.GetTextures().GetTextureCount() >= 2u)
    {
      return true;
    }
  }
  return false;
}

PixelData CreatePixelData(uint32_t width, uint32_t height, Pixel::Format pixelFormat)
{
  const uint32_t bufferSize = width * height * Pixel::GetBytesPerPixel(pixelFormat);
  return PixelData::New(new uint8_t[bufferSize](), bufferSize, width, height, pixelFormat, PixelData::DELETE_ARRAY);
}

RenderedTextVisual CreateTextVisual(UiTestApplication& application)
{
  Dali::Ui::View view = Dali::Ui::View::New();
  // Keep a real owner extent after layout, as production Label does. Setting
  // Actor::SIZE alone is overwritten by View layout before direct publication.
  view.SetLayoutMode(Dali::Ui::LayoutMode::STANDALONE);
  view.SetRequestedWidth(VISUAL_WIDTH);
  view.SetRequestedHeight(VISUAL_HEIGHT);
  view.SetProperty(Actor::Property::SIZE, Vector3(VISUAL_WIDTH, VISUAL_HEIGHT, 0.0f));

  Property::Map properties;
  properties.Add(Dali::Ui::Integration::Visual::Property::TYPE,
                 Dali::Ui::Integration::InternalVisualType::TEXT);
  properties.Add(Dali::Ui::Integration::TextVisual::Property::TEXT, "initial");
  properties.Add(Dali::Ui::Integration::TextVisual::Property::FONT_SIZE, 16.0f);

  Dali::Ui::Integration::Visual::Base visual =
    Dali::Ui::Integration::VisualFactory::Get().CreateVisual(properties);
  Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(view))
    .RegisterVisual(Dali::Ui::Text::LabelPropertyIndex::TEXT,
                    visual,
                    Dali::Ui::Integration::DepthIndex::CONTENT);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  // Production Label enables this controller state before submitting async
  // render work. TextVisual intentionally rejects render publication without
  // it, so the shared direct-visual fixture must model that prerequisite.
  Dali::Ui::Internal::TextVisual::GetController(visual)->SetAsyncRendering(true);
  return {view, visual};
}

UiText::AsyncTextParameters MakeParameters(const std::string& text)
{
  UiText::AsyncTextParameters parameters;
  parameters.text               = text;
  parameters.fontSize           = 16.0f;
  parameters.textWidth          = VISUAL_WIDTH;
  parameters.textHeight         = VISUAL_HEIGHT;
  parameters.originWidth        = VISUAL_WIDTH;
  parameters.originHeight       = VISUAL_HEIGHT;
  parameters.maxTextureSize     = 4096;
  parameters.requestType        = UiIntegrationText::Async::RENDER_FIXED_SIZE;
  parameters.isMarqueeEnabled   = true;
  parameters.marqueeLoopCount   = 0;
  parameters.marqueeOrientation = UiText::MarqueeOrientation::HORIZONTAL;
  return parameters;
}

UiText::AsyncTextParameters MakeGradientParameters(const std::string& text)
{
  Dali::Ui::Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, Dali::Ui::UiColor(Color::RED)),
                         Dali::Ui::Gradient::StopNode(0.5f, Dali::Ui::UiColor(Color::GREEN)),
                         Dali::Ui::Gradient::StopNode(1.0f, Dali::Ui::UiColor(Color::BLUE))});

  UiText::StyledTextBuilder builder = UiText::StyledTextBuilder::New(text.c_str());
  DALI_TEST_CHECK(builder.SetSpan(UiText::GradientSpan::New(gradient),
                                  0u,
                                  static_cast<uint32_t>(text.size())));

  UiText::AsyncTextParameters parameters = MakeParameters(text);
  parameters.isMarqueeEnabled            = false;
  parameters.hasStyledTextStyleSnapshot  = true;
  parameters.styledTextStyleSnapshot =
    UiInternal::Text::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(), 96.0f);
  return parameters;
}

UiText::AsyncTextParameters MakeRevealParameters(
  const std::string&                 text,
  uint64_t                           revision,
  UiText::Internal::Reveal::Unit     unit,
  float                              fadeDurationRatio,
  UiText::Internal::Reveal::Sequence sequence             = UiText::Internal::Reveal::Sequence::WHOLE_TEXT,
  float                              sequenceStaggerRatio = 0.0f)
{
  UiText::AsyncTextParameters parameters    = MakeParameters(text);
  parameters.isMarqueeEnabled               = false;
  parameters.isTextRevealEnabled            = true;
  parameters.textRevealUnit                 = unit;
  parameters.textRevealFadeDurationRatio    = fadeDurationRatio;
  parameters.textRevealSequence             = sequence;
  parameters.textRevealSequenceStaggerRatio = sequenceStaggerRatio;
  parameters.textRevealRevision             = revision;
  return parameters;
}

UiText::AsyncTextRenderInfo MakeRevealRenderInfo(uint32_t width, uint32_t height)
{
  UiText::AsyncTextRenderInfo renderInfo;
  renderInfo.size                   = Size(static_cast<float>(width), static_cast<float>(height));
  renderInfo.renderedSize           = renderInfo.size;
  renderInfo.textPixelData          = CreatePixelData(width, height, Pixel::L8);
  renderInfo.isTextRevealEnabled    = true;
  renderInfo.textRevealFadeDuration = 0.2f;
  renderInfo.revealMetadataTiles.push_back(CreatePixelData(width, height, Pixel::RGBA8888));
  return renderInfo;
}

void ConfigureReveal(
  RenderedTextVisual&                rendered,
  UiText::Internal::Reveal::Unit     unit,
  float                              fadeDurationRatio,
  uint64_t                           revision,
  UiText::Internal::Reveal::Sequence sequence             = UiText::Internal::Reveal::Sequence::WHOLE_TEXT,
  float                              sequenceStaggerRatio = 0.0f)
{
  Property::Index progress = rendered.view.GetPropertyIndex("testRevealProgress");
  if(progress == Property::INVALID_INDEX)
  {
    progress = rendered.view.RegisterProperty("testRevealProgress", 0.5f);
  }
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual,
                                              unit,
                                              fadeDurationRatio,
                                              progress,
                                              revision,
                                              sequence,
                                              sequenceStaggerRatio);
}

void PublishDirect(RenderedTextVisual&                rendered,
                   const UiText::AsyncTextParameters& parameters,
                   const UiText::AsyncTextRenderInfo& renderInfo)
{
  Ui::TextLoadObserver::TextInformation completion(renderInfo, parameters);
  Ui::TextLoadObserver&                 loadObserver = static_cast<UiInternal::TextVisual&>(
    Dali::Ui::GetImplementation(rendered.visual).GetVisualObject());
  loadObserver.LoadComplete(true, completion);
}

enum class CompletionAction
{
  NONE,
  REQUEST_NEXT,
  CLEAR_TEXT,
  DISABLE_ASYNC,
  DISCONNECT_SCENE
};

class ReentrantAsyncInterface : public UiIntegrationText::AsyncTextInterface
{
public:
  ReentrantAsyncInterface(Dali::Ui::Integration::Visual::Base visual,
                          Actor                               actor,
                          CompletionAction                    action = CompletionAction::REQUEST_NEXT)
  : mVisual(visual),
    mActor(actor),
    mNextParameters(MakeParameters(
      "second asynchronous marquee request deliberately contains enough text to remain pending")),
    mAction(action)
  {
  }

  void AsyncInitializeMarquee(const UiText::AsyncTextRenderInfo&) override
  {
    ++mMarqueeInitializationCount;
  }

  void AsyncTextFitChanged(float) override
  {
  }

  void AsyncRenderFinished(UiText::AsyncTextRenderInfo&&) override
  {
    ++mCompletionCount;
    if(mCompletionCount != 1u)
    {
      return;
    }

    mFirstResultValid = HasValidTexture(mActor);
    switch(mAction)
    {
      case CompletionAction::REQUEST_NEXT:
      {
        mNextRequestAccepted    = UiInternal::TextVisual::UpdateAsyncRenderer(mVisual, mNextParameters);
        mResultValidAfterAction = HasValidTexture(mActor);
        break;
      }
      case CompletionAction::CLEAR_TEXT:
      {
        mNextParameters.text.clear();
        mNextRequestAccepted    = UiInternal::TextVisual::UpdateAsyncRenderer(mVisual, mNextParameters);
        mResultValidAfterAction = HasValidTexture(mActor);
        break;
      }
      case CompletionAction::DISABLE_ASYNC:
      {
        UiInternal::TextVisual::GetController(mVisual)->SetAsyncRendering(false);
        mResultValidAfterAction = HasValidTexture(mActor);
        break;
      }
      case CompletionAction::DISCONNECT_SCENE:
      {
        mActor.Unparent();
        mResultValidAfterAction = HasValidTexture(mActor);
        break;
      }
      case CompletionAction::NONE:
      {
        mResultValidAfterAction = HasValidTexture(mActor);
        break;
      }
    }
  }

  void AsyncSizeComputed(const UiText::AsyncTextRenderInfo&) override
  {
    ++mSizeCompletionCount;
  }

  Dali::Ui::Integration::Visual::Base mVisual;
  Actor                               mActor;
  UiText::AsyncTextParameters         mNextParameters;
  uint32_t                            mCompletionCount{0u};
  uint32_t                            mSizeCompletionCount{0u};
  uint32_t                            mMarqueeInitializationCount{0u};
  bool                                mFirstResultValid{false};
  bool                                mNextRequestAccepted{false};
  bool                                mResultValidAfterAction{false};
  CompletionAction                    mAction{CompletionAction::REQUEST_NEXT};
};
} // unnamed namespace

void utc_dali_text_visual_async_publication_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_visual_async_publication_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextVisualInvalidAsyncSizeRequestDoesNotSubmitP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  (void)fontClient;

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  UiText::AsyncTextParameters invalid = MakeParameters("render request is invalid for size computation");
  UiInternal::TextVisual::RequestAsyncSizeComputation(rendered.visual, invalid);
  DALI_TEST_CHECK(!Test::WaitForEventThreadTrigger(1, 0));
  DALI_TEST_EQUALS(observer.mSizeCompletionCount, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(invalid.requestType, UiIntegrationText::Async::RENDER_FIXED_SIZE, TEST_LOCATION);

  UiText::AsyncTextParameters valid = MakeParameters("valid natural size after rejected request");
  valid.requestType                 = UiIntegrationText::Async::COMPUTE_NATURAL_SIZE;
  UiInternal::TextVisual::RequestAsyncSizeComputationOwned(rendered.visual, std::move(valid));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mSizeCompletionCount, 1u, TEST_LOCATION);

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualReentrantAsyncPublicationKeepsTextureP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
  (void)fontClient;

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  UiText::AsyncTextParameters first =
    MakeParameters("first asynchronous marquee result remains published while its callback requests the second");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, first));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));

  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.mFirstResultValid);
  DALI_TEST_CHECK(observer.mNextRequestAccepted);
  DALI_TEST_CHECK(observer.mResultValidAfterAction);
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mCompletionCount, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.mMarqueeInitializationCount >= 2u);
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualFailurePreservesPublishedTextureP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  UiText::AsyncTextParameters parameters = MakeParameters("valid asynchronous text publication");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

  UiText::AsyncTextRenderInfo           failedInfo;
  Ui::TextLoadObserver::TextInformation failed(failedInfo, parameters);
  Ui::TextLoadObserver&                 loadObserver = static_cast<UiInternal::TextVisual&>(
    Dali::Ui::GetImplementation(rendered.visual).GetVisualObject());
  loadObserver.LoadComplete(false, failed);
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualExplicitClearRemovesPublishedTextureP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  UiText::AsyncTextParameters parameters = MakeParameters("valid asynchronous text before explicit clear");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

  parameters.text.clear();
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(!HasValidTexture(rendered.view));

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualReentrantExplicitClearRemovesTextureP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::CLEAR_TEXT);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  UiText::AsyncTextParameters parameters = MakeParameters("publication cleared explicitly from its completion callback");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));

  DALI_TEST_CHECK(observer.mFirstResultValid);
  DALI_TEST_CHECK(observer.mNextRequestAccepted);
  DALI_TEST_EQUALS(observer.mCompletionCount, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(!observer.mResultValidAfterAction);
  DALI_TEST_CHECK(!HasValidTexture(rendered.view));

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualReentrantAsyncOffKeepsTextureP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::DISABLE_ASYNC);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  UiInternal::TextVisual::GetController(rendered.visual)->SetAsyncRendering(true);

  UiText::AsyncTextParameters parameters = MakeParameters("publication remains valid when async mode is disabled in the callback");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));

  DALI_TEST_CHECK(observer.mFirstResultValid);
  DALI_TEST_CHECK(observer.mResultValidAfterAction);
  DALI_TEST_CHECK(!UiInternal::TextVisual::GetController(rendered.visual)->IsAsyncRendering());
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualReentrantSceneDisconnectClearsTextureP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::DISCONNECT_SCENE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  UiText::AsyncTextParameters parameters = MakeParameters("scene disconnect is an explicit renderer clear boundary");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));

  DALI_TEST_CHECK(observer.mFirstResultValid);
  DALI_TEST_CHECK(!observer.mResultValidAfterAction);
  DALI_TEST_CHECK(!HasValidTexture(rendered.view));

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualRapidRequestsDiscardStaleResultsP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  UiText::AsyncTextParameters parameters = MakeGradientParameters("styled gradient request A");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(HasValidTexture(rendered.view));
  DALI_TEST_CHECK(HasMultiColorTextRenderer(rendered.view));

  parameters                  = MakeParameters(std::string(30000u, 'B'));
  parameters.isMarqueeEnabled = false;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(HasValidTexture(rendered.view));
  parameters = MakeGradientParameters("styled gradient request C");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(HasValidTexture(rendered.view));
  parameters                  = MakeParameters("plain request D is the only result that may replace request A");
  parameters.isMarqueeEnabled = false;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

  for(uint32_t trigger = 0u; trigger < 3u && observer.mCompletionCount < 2u; ++trigger)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  }
  DALI_TEST_EQUALS(observer.mCompletionCount, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(HasValidTexture(rendered.view));
  DALI_TEST_CHECK(!HasMultiColorTextRenderer(rendered.view));

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualPendingRequestCancelledOnDestructionP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual rendered = CreateTextVisual(application);
  {
    ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
    UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

    UiText::AsyncTextParameters parameters = MakeParameters("pending request is cancelled when its visual is destroyed");
    DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));

    rendered.view.Unparent();
    Dali::Ui::Internal::ViewDataImpl::Get(Dali::Ui::GetImpl(rendered.view))
      .UnregisterVisual(Dali::Ui::Text::LabelPropertyIndex::TEXT);
    rendered.visual.Reset();
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(observer.mCompletionCount, 0u, TEST_LOCATION);
  }

  rendered.view.Reset();
  application.SendNotification();
  application.Render();
  END_TEST;
}

int UtcDaliTextVisualAsyncOffOnDiscardsOlderRequestsP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  auto Verify = [&](bool enableReveal)
  {
    RenderedTextVisual      rendered = CreateTextVisual(application);
    ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
    UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

    constexpr uint64_t revision = 1u;
    if(enableReveal)
    {
      ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::CHARACTER,
                      UiText::Reveal::AUTO_FADE_DURATION_RATIO, revision,
                      UiText::Internal::Reveal::Sequence::PER_LINE, 0.5f);
    }

    UiText::AsyncTextParameters oldParameters =
      enableReveal ? MakeRevealParameters(std::string(30000u, 'A'), revision,
                                          UiText::Internal::Reveal::Unit::CHARACTER,
                                          UiText::Reveal::AUTO_FADE_DURATION_RATIO,
                                          UiText::Internal::Reveal::Sequence::PER_LINE, 0.5f)
                   : MakeParameters(std::string(30000u, 'A'));
    oldParameters.isMarqueeEnabled = false;
    DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, oldParameters));

    auto controller = UiInternal::TextVisual::GetController(rendered.visual);
    controller->SetAsyncRendering(false);
    controller->SetAsyncRendering(true);

    UiText::AsyncTextParameters currentParameters =
      enableReveal ? MakeRevealParameters("current reveal request\nwith two visual lines", revision,
                                          UiText::Internal::Reveal::Unit::CHARACTER,
                                          UiText::Reveal::AUTO_FADE_DURATION_RATIO,
                                          UiText::Internal::Reveal::Sequence::PER_LINE, 0.5f)
                   : MakeParameters("current ordinary request");
    currentParameters.isMarqueeEnabled = false;
    currentParameters.isMultiLine      = enableReveal;
    DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, currentParameters));

    for(uint32_t trigger = 0u; trigger < 3u && observer.mCompletionCount == 0u; ++trigger)
    {
      DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
    }
    DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
    DALI_TEST_CHECK(HasValidTexture(rendered.view));
    if(enableReveal)
    {
      DALI_TEST_CHECK(HasTextRevealRenderer(rendered.view));
    }

    UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  };

  Verify(false); // Reveal attachment has never been created.
  Verify(true);
  END_TEST;
}

int UtcDaliTextVisualRevealNoneRejectsOlderCompletionP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::CHARACTER,
                  UiText::Reveal::AUTO_FADE_DURATION_RATIO, 1u);
  const auto oldParameters = MakeRevealParameters("stale enabled Reveal", 1u,
                                                  UiText::Internal::Reveal::Unit::CHARACTER,
                                                  UiText::Reveal::AUTO_FADE_DURATION_RATIO);
  const auto oldRenderInfo = MakeRevealRenderInfo(32u, 16u);

  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::DISABLED,
                                              UiText::Reveal::AUTO_FADE_DURATION_RATIO,
                                              Property::INVALID_INDEX, 2u);
  PublishDirect(rendered, oldParameters, oldRenderInfo);
  DALI_TEST_EQUALS(observer.mCompletionCount, 0u, TEST_LOCATION);

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualRevealRatioRejectsOlderCompletionP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::CHARACTER,
                  UiText::Reveal::AUTO_FADE_DURATION_RATIO, 1u);
  PublishDirect(rendered,
                MakeRevealParameters("stale automatic ratio", 1u,
                                     UiText::Internal::Reveal::Unit::CHARACTER,
                                     UiText::Reveal::AUTO_FADE_DURATION_RATIO),
                MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);

  ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::CHARACTER, 0.2f, 2u);
  PublishDirect(rendered,
                MakeRevealParameters("stale automatic ratio", 1u,
                                     UiText::Internal::Reveal::Unit::CHARACTER,
                                     UiText::Reveal::AUTO_FADE_DURATION_RATIO),
                MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);

  PublishDirect(rendered,
                MakeRevealParameters("current explicit ratio", 2u,
                                     UiText::Internal::Reveal::Unit::CHARACTER, 0.2f),
                MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(HasTextRevealRenderer(rendered.view));

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualRevealUnitRejectsOlderCompletionP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::CHARACTER, 0.2f, 1u);
  const auto oldParameters = MakeRevealParameters("stale character unit", 1u,
                                                  UiText::Internal::Reveal::Unit::CHARACTER, 0.2f);
  const auto oldRenderInfo = MakeRevealRenderInfo(32u, 16u);

  ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::WORD, 0.2f, 2u);
  PublishDirect(rendered, oldParameters, oldRenderInfo);
  DALI_TEST_EQUALS(observer.mCompletionCount, 0u, TEST_LOCATION);

  PublishDirect(rendered,
                MakeRevealParameters("current word unit", 2u, UiText::Internal::Reveal::Unit::WORD, 0.2f),
                MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualRevealSequenceRejectsOlderCompletionP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  ConfigureReveal(rendered,
                  UiText::Internal::Reveal::Unit::CHARACTER,
                  0.25f,
                  1u,
                  UiText::Internal::Reveal::Sequence::WHOLE_TEXT);
  const auto staleWholeText = MakeRevealParameters("stale WHOLE_TEXT sequence",
                                                   1u,
                                                   UiText::Internal::Reveal::Unit::CHARACTER,
                                                   0.25f);

  ConfigureReveal(rendered,
                  UiText::Internal::Reveal::Unit::CHARACTER,
                  0.25f,
                  2u,
                  UiText::Internal::Reveal::Sequence::PER_LINE,
                  0.25f);
  PublishDirect(rendered, staleWholeText, MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 0u, TEST_LOCATION);

  const auto stalePerLine = MakeRevealParameters("stale PER_LINE sequence",
                                                 2u,
                                                 UiText::Internal::Reveal::Unit::CHARACTER,
                                                 0.25f,
                                                 UiText::Internal::Reveal::Sequence::PER_LINE,
                                                 0.25f);
  ConfigureReveal(rendered,
                  UiText::Internal::Reveal::Unit::CHARACTER,
                  0.25f,
                  3u,
                  UiText::Internal::Reveal::Sequence::WHOLE_TEXT);
  PublishDirect(rendered, stalePerLine, MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 0u, TEST_LOCATION);

  ConfigureReveal(rendered,
                  UiText::Internal::Reveal::Unit::CHARACTER,
                  0.25f,
                  4u,
                  UiText::Internal::Reveal::Sequence::PER_LINE,
                  0.25f);
  const auto staleStagger = MakeRevealParameters("stale PER_LINE stagger",
                                                 4u,
                                                 UiText::Internal::Reveal::Unit::CHARACTER,
                                                 0.25f,
                                                 UiText::Internal::Reveal::Sequence::PER_LINE,
                                                 0.25f);
  ConfigureReveal(rendered,
                  UiText::Internal::Reveal::Unit::CHARACTER,
                  0.25f,
                  5u,
                  UiText::Internal::Reveal::Sequence::PER_LINE,
                  0.5f);
  PublishDirect(rendered, staleStagger, MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 0u, TEST_LOCATION);

  PublishDirect(rendered,
                MakeRevealParameters("current PER_LINE stagger",
                                     5u,
                                     UiText::Internal::Reveal::Unit::CHARACTER,
                                     0.25f,
                                     UiText::Internal::Reveal::Sequence::PER_LINE,
                                     0.5f),
                MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualMarqueeRejectsOlderRevealCompletionP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::CHARACTER, 0.2f, 1u);

  UiInternal::TextVisual::GetController(rendered.visual)->SetMarqueeEnabled(true, false, UiText::MarqueeOrientation::HORIZONTAL);
  PublishDirect(rendered,
                MakeRevealParameters("Reveal requested before marquee", 1u,
                                     UiText::Internal::Reveal::Unit::CHARACTER, 0.2f),
                MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 0u, TEST_LOCATION);

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualCutoutRejectsOlderRevealCompletionP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::CHARACTER, 0.2f, 1u);

  UiInternal::TextVisual::GetController(rendered.visual)->SetTextCutout(true);
  PublishDirect(rendered,
                MakeRevealParameters("Reveal requested before cutout", 1u,
                                     UiText::Internal::Reveal::Unit::CHARACTER, 0.2f),
                MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 0u, TEST_LOCATION);

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualUnsupportedOldRequestCannotReplaceNewerRequestP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);

  auto controller = UiInternal::TextVisual::GetController(rendered.visual);
  controller->SetMarqueeEnabled(true, false, UiText::MarqueeOrientation::HORIZONTAL);
  UiText::AsyncTextParameters oldParameters = MakeParameters(std::string(30000u, 'M'));
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, oldParameters));

  controller->SetMarqueeEnabled(false, false, UiText::MarqueeOrientation::HORIZONTAL);
  UiText::AsyncTextParameters currentParameters = MakeParameters("current non-marquee request");
  currentParameters.isMarqueeEnabled            = false;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, currentParameters));

  for(uint32_t trigger = 0u; trigger < 3u && observer.mCompletionCount == 0u; ++trigger)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  }
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(observer.mMarqueeInitializationCount, 0u, TEST_LOCATION);

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualIncompleteRevealMetadataFallsBackAtomicallyP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  auto VerifyFallback = [&](UiText::AsyncTextRenderInfo renderInfo)
  {
    RenderedTextVisual      rendered = CreateTextVisual(application);
    ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
    UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
    ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::CHARACTER, 0.2f, 1u);

    PublishDirect(rendered,
                  MakeRevealParameters("metadata validation fallback", 1u,
                                       UiText::Internal::Reveal::Unit::CHARACTER, 0.2f),
                  renderInfo);
    DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
    DALI_TEST_CHECK(HasValidTexture(rendered.view));
    DALI_TEST_CHECK(!HasTextRevealRenderer(rendered.view));
    UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  };

  UiText::AsyncTextRenderInfo absent = MakeRevealRenderInfo(32u, 16u);
  absent.revealMetadataTiles.clear();
  VerifyFallback(absent);

  const uint32_t              maxTextureSize = static_cast<uint32_t>(Dali::GetMaxTextureSize());
  UiText::AsyncTextRenderInfo incomplete     = MakeRevealRenderInfo(2u, maxTextureSize + 8u);
  incomplete.revealMetadataTiles.clear();
  incomplete.revealMetadataTiles.push_back(CreatePixelData(2u, maxTextureSize, Pixel::RGBA8888));
  VerifyFallback(incomplete);

  UiText::AsyncTextRenderInfo inconsistent = MakeRevealRenderInfo(32u, 16u);
  inconsistent.revealMetadataTiles.clear();
  inconsistent.revealMetadataTiles.push_back(CreatePixelData(31u, 16u, Pixel::RGBA8888));
  VerifyFallback(inconsistent);

  UiText::AsyncTextRenderInfo invalid = MakeRevealRenderInfo(32u, 16u);
  invalid.revealMetadataTiles.front().Reset();
  VerifyFallback(invalid);

  UiText::AsyncTextRenderInfo mismatchedTile = MakeRevealRenderInfo(2u, maxTextureSize + 8u);
  mismatchedTile.revealMetadataTiles.clear();
  mismatchedTile.revealMetadataTiles.push_back(CreatePixelData(2u, maxTextureSize, Pixel::RGBA8888));
  mismatchedTile.revealMetadataTiles.push_back(CreatePixelData(2u, 7u, Pixel::RGBA8888));
  VerifyFallback(mismatchedTile);

  UiText::AsyncTextRenderInfo mismatchedRendererTile = MakeRevealRenderInfo(2u, maxTextureSize + 8u);
  mismatchedRendererTile.revealMetadataTiles.clear();
  mismatchedRendererTile.revealMetadataTiles.push_back(CreatePixelData(2u, maxTextureSize, Pixel::RGBA8888));
  mismatchedRendererTile.revealMetadataTiles.push_back(CreatePixelData(2u, 8u, Pixel::RGBA8888));
  mismatchedRendererTile.size.height = static_cast<float>(maxTextureSize + 4u);
  VerifyFallback(mismatchedRendererTile);
  END_TEST;
}

int UtcDaliTextVisualRevealMetadataFallbackRecoversP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);

  RenderedTextVisual      rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  ConfigureReveal(rendered, UiText::Internal::Reveal::Unit::CHARACTER, 0.2f, 1u);
  const UiText::AsyncTextParameters parameters =
    MakeRevealParameters("valid invalid valid metadata publication", 1u,
                         UiText::Internal::Reveal::Unit::CHARACTER, 0.2f);

  PublishDirect(rendered, parameters, MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(HasTextRevealRenderer(rendered.view));
  DALI_TEST_EQUALS(rendered.view.GetRendererCount(), 1u, TEST_LOCATION);
  Renderer revealRenderer = rendered.view.GetRendererAt(0u);
  DALI_TEST_CHECK(revealRenderer.GetTextures().GetTextureCount() >= 2u);
  const Shader revealShader = revealRenderer.GetShader();

  UiText::AsyncTextRenderInfo incomplete = MakeRevealRenderInfo(32u, 16u);
  incomplete.revealMetadataTiles.clear();
  PublishDirect(rendered, parameters, incomplete);
  DALI_TEST_EQUALS(observer.mCompletionCount, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(HasValidTexture(rendered.view));
  DALI_TEST_CHECK(!HasTextRevealRenderer(rendered.view));
  const Shader ordinaryShader = rendered.view.GetRendererAt(0u).GetShader();
  DALI_TEST_CHECK(ordinaryShader != revealShader);
  for(uint32_t rendererIndex = 0u; rendererIndex < rendered.view.GetRendererCount(); ++rendererIndex)
  {
    Renderer renderer = rendered.view.GetRendererAt(rendererIndex);
    DALI_TEST_EQUALS(renderer.GetTextures().GetTextureCount(), 1u, TEST_LOCATION);
  }

  // The default renderer retains its registered custom property, but the
  // Reveal constraint must be gone while the ordinary shader is active.
  const Property::Index sourceProgress = rendered.view.GetPropertyIndex("testRevealProgress");
  const Property::Index rendererProgress =
    rendered.view.GetRendererAt(0u).GetPropertyIndex("uTextRevealProgress");
  DALI_TEST_CHECK(sourceProgress != Property::INVALID_INDEX && rendererProgress != Property::INVALID_INDEX);
  const float detachedProgress = rendered.view.GetRendererAt(0u).GetCurrentProperty<float>(rendererProgress);
  rendered.view.SetProperty(sourceProgress, 0.9f);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(rendered.view.GetRendererAt(0u).GetCurrentProperty<float>(rendererProgress),
                   detachedProgress, 0.0001f, TEST_LOCATION);

  PublishDirect(rendered, parameters, MakeRevealRenderInfo(32u, 16u));
  DALI_TEST_EQUALS(observer.mCompletionCount, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(HasTextRevealRenderer(rendered.view));
  DALI_TEST_EQUALS(rendered.view.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(rendered.view.GetRendererAt(0u).GetTextures().GetTextureCount() >= 2u);
  DALI_TEST_CHECK(rendered.view.GetRendererAt(0u).GetShader() != ordinaryShader);

  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

namespace
{
int CheckAsyncRevealPendingOrdinary(UiTestApplication& application, bool blur, bool hidden)
{
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto       rendered = CreateTextVisual(application);
  // Complete an ordinary async publication explicitly; this fixture is a View
  // with a TextVisual, not a Label that submits its controller layout itself.
  auto ordinaryParameters = MakeParameters("initial ordinary publication");
  ordinaryParameters.isMarqueeEnabled = false;
  auto initialLoader = UiText::AsyncTextLoader::New();
  PublishDirect(rendered, ordinaryParameters, initialLoader.RenderText(ordinaryParameters, false, Size::ZERO));
  application.SendNotification();
  application.Render();
  const auto original = rendered.view.GetRendererAt(0u);
  DALI_TEST_CHECK(HasValidTexture(rendered.view));
  const auto progress = rendered.view.RegisterProperty("testRevealProgress", 0.0f);

  // A completed None result can remain on a hidden Label between presets.
  // Show it and start Reveal, but deliberately withhold the worker completion.
  rendered.view.SetProperty(Actor::Property::VISIBLE, !hidden);
  application.SendNotification();
  application.Render();
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::PIXEL,
                                              0.0f, progress, 1u, UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f, blur ? 24.0f : 0.0f, 0.5f);
  rendered.view.SetProperty(Actor::Property::VISIBLE, true);
  for(int frame = 0; frame < 4; ++frame)
  {
    application.SendNotification();
    application.Render();
    for(uint32_t index = 0; index < rendered.view.GetRendererCount(); ++index)
    {
      const auto renderer = rendered.view.GetRendererAt(index);
      // The old ordinary renderer cannot represent progress zero without
      // metadata. It must not remain drawable while the request is pending.
      DALI_TEST_CHECK(renderer != original || !renderer.GetTextures() ||
                      renderer.GetTextures().GetTextureCount() == 0u ||
                      renderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY) == 0.0f);
    }
  }
  auto parameters                        = MakeRevealParameters("First line\nAnother line", 1u, UiText::Internal::Reveal::Unit::PIXEL,
                                                                0.0f, UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f);
  parameters.isMultiLine                 = true;
  parameters.textRevealBlurRadius        = blur ? 24.0f : 0.0f;
  parameters.textRevealBlurDurationRatio = 0.5f;
  parameters.isTextRevealBlurRequested   = blur;
  auto       loader                      = UiText::AsyncTextLoader::New();
  const auto result                      = loader.RenderText(parameters, false, Size::ZERO);
  PublishDirect(rendered, parameters, result);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(blur ? static_cast<bool>(rendered.view.FindChildByName("RevealGaussianForeground"))
                       : HasTextRevealRenderer(rendered.view));
  DALI_TEST_EQUALS(rendered.view.GetCurrentProperty<float>(progress), 0.0f, TEST_LOCATION);
  rendered.view.Unparent();
  application.SendNotification();
  application.Render();
  END_TEST;
}
} // unnamed namespace

int UtcDaliTextVisualAsyncRevealPendingOrdinaryP(void)
{
  UiTestApplication application;
  const int visible = CheckAsyncRevealPendingOrdinary(application, false, false);
  return visible == 0 ? CheckAsyncRevealPendingOrdinary(application, false, true) : visible;
}

int UtcDaliTextVisualAsyncBlurPendingOrdinaryP(void)
{
  UiTestApplication application;
  const int visible = CheckAsyncRevealPendingOrdinary(application, true, false);
  return visible == 0 ? CheckAsyncRevealPendingOrdinary(application, true, true) : visible;
}

int UtcDaliTextVisualAsyncRevealRetainsCompatiblePendingP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  for(const bool blur : {false, true})
  {
    auto       rendered  = CreateTextVisual(application);
    const auto tasks     = application.GetScene().GetRenderTaskList();
    const auto baseline  = tasks.GetTaskCount();
    const auto progress  = rendered.view.RegisterProperty("testRevealProgress", 1.0f);
    auto       configure = [&](uint64_t revision, UiText::Internal::Reveal::Unit unit)
    {
      UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, unit, 0.0f, progress, revision,
                                                  UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f, blur ? 24.0f : 0.0f, 0.5f);
    };
    configure(1u, UiText::Internal::Reveal::Unit::PIXEL);
    auto parameters                        = MakeRevealParameters("First line\nSecond line", 1u, UiText::Internal::Reveal::Unit::PIXEL,
                                                                  0.0f, UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f);
    parameters.isMultiLine                 = true;
    parameters.isTextRevealBlurRequested   = blur;
    parameters.textRevealBlurRadius        = blur ? 24.0f : 0.0f;
    parameters.textRevealBlurDurationRatio = 0.5f;
    auto       loader                      = UiText::AsyncTextLoader::New();
    const auto result                      = loader.RenderText(parameters, false, Size::ZERO);
    PublishDirect(rendered, parameters, result);
    application.SendNotification();
    application.Render();
    Actor foreground = blur ? rendered.view.FindChildByName("RevealGaussianForeground") : rendered.view;
    DALI_TEST_CHECK(foreground && foreground.GetRendererCount() > 0u);
    const auto original    = foreground.GetRendererAt(0u);
    const auto texture     = original.GetTextures().GetTexture(0u);
    const auto activeTasks = tasks.GetTaskCount();
    configure(2u, UiText::Internal::Reveal::Unit::CHARACTER);
    rendered.view.SetProperty(progress, 0.0f);
    PublishDirect(rendered, parameters, result); // stale completion must not replace the retained result
    for(int frame = 0; frame < 4; ++frame)
    {
      application.SendNotification();
      application.Render();
      DALI_TEST_EQUALS(tasks.GetTaskCount(), activeTasks, TEST_LOCATION);
      DALI_TEST_CHECK(!blur || rendered.view.FindChildByName("RevealGaussianForeground") == foreground);
      DALI_TEST_CHECK(foreground.GetRendererCount() > 0u && foreground.GetRendererAt(0u) == original);
      DALI_TEST_CHECK(original.GetTextures().GetTexture(0u) == texture);
      DALI_TEST_EQUALS(original.GetCurrentProperty<float>(original.GetPropertyIndex("uTextRevealProgress")),
                       0.0f, TEST_LOCATION);
    }

    // None removes the progress binding immediately. Re-enable before any
    // disable result arrives: old metadata alone is no longer a usable result.
    configure(3u, UiText::Internal::Reveal::Unit::DISABLED);
    configure(4u, UiText::Internal::Reveal::Unit::PIXEL);
    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(!HasValidTexture(rendered.view));
    DALI_TEST_CHECK(!rendered.view.FindChildByName("RevealGaussianForeground"));
    DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
    parameters.textRevealRevision = 4u;
    PublishDirect(rendered, parameters, result);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(tasks.GetTaskCount(), activeTasks, TEST_LOCATION);
    DALI_TEST_CHECK(blur ? static_cast<bool>(rendered.view.FindChildByName("RevealGaussianForeground"))
                         : HasTextRevealRenderer(rendered.view));
    rendered.view.Unparent();
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextVisualAsyncRevealPendingFullFadeP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  for(const bool blur : {false, true})
  {
    auto rendered = CreateTextVisual(application);
    auto ordinary = MakeParameters("A completed ordinary publication");
    ordinary.isMarqueeEnabled = false;
    auto loader = UiText::AsyncTextLoader::New();
    PublishDirect(rendered, ordinary, loader.RenderText(ordinary, false, Size::ZERO));
    application.SendNotification();
    application.Render();
    const auto original = rendered.view.GetRendererAt(0u);
    const auto textures = original.GetTextures();
    const auto progress = rendered.view.RegisterProperty("testRevealProgress", 1.0f);
    auto configure = [&](uint64_t revision, UiText::Internal::Reveal::Unit unit)
    {
      UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, unit, 1.0f, progress, revision,
                                                  UiText::Internal::Reveal::Sequence::WHOLE_TEXT, 0.0f, blur ? 24.0f : 0.0f, 1.0f);
    };
    configure(1u, UiText::Internal::Reveal::Unit::PIXEL);
    // No worker completion or animation is needed to expose the old exit gap.
    // Full fade has identical opacity for all units and can be evaluated on
    // the previous ordinary foreground without waiting for glyph metadata.
    for(const float value : {1.0f, 0.0f, 0.5f, 1.0f})
    {
      rendered.view.SetProperty(progress, value);
      for(int frame = 0; frame < 3; ++frame)
      {
        application.SendNotification();
        application.Render();
        DALI_TEST_CHECK(rendered.view.GetRendererAt(0u) == original);
        DALI_TEST_CHECK(original.GetTextures() == textures);
        DALI_TEST_EQUALS(original.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY),
                         value, TEST_LOCATION);
      }
    }
    // Cancelling at zero must discard the temporary opacity guard, including
    // when the same renderer survives until the ordinary result arrives.
    rendered.view.SetProperty(progress, 0.0f);
    application.SendNotification();
    application.Render();
    configure(2u, UiText::Internal::Reveal::Unit::DISABLED);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(original.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), 1.0f, TEST_LOCATION);

    configure(3u, UiText::Internal::Reveal::Unit::PIXEL);
    rendered.view.SetProperty(progress, 1.0f);
    application.SendNotification();
    application.Render();
    Animation animation = Animation::New(0.25f);
    animation.AnimateTo(Property(rendered.view, progress), 0.0f, AlphaFunction::LINEAR);
    animation.Play();
    for(int frame = 0; frame < 20; ++frame)
    {
      application.SendNotification();
      application.Render(16);
      DALI_TEST_CHECK(rendered.view.GetRendererAt(0u) == original);
      DALI_TEST_CHECK(original.GetTextures() == textures);
      DALI_TEST_EQUALS(original.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY),
                       rendered.view.GetCurrentProperty<float>(progress), 0.0001f, TEST_LOCATION);
    }
    DALI_TEST_EQUALS(rendered.view.GetCurrentProperty<float>(progress), 0.0f, TEST_LOCATION);
    auto parameters = MakeRevealParameters("A completed ordinary publication", 3u, UiText::Internal::Reveal::Unit::PIXEL, 1.0f);
    parameters.textRevealBlurRadius = blur ? 24.0f : 0.0f;
    parameters.textRevealBlurDurationRatio = 1.0f;
    parameters.isTextRevealBlurRequested = blur;
    PublishDirect(rendered, parameters, loader.RenderText(parameters, false, Size::ZERO));
    application.SendNotification();
    application.Render();
    Actor foreground = blur ? rendered.view.FindChildByName("RevealGaussianForeground") : rendered.view;
    DALI_TEST_CHECK(foreground && foreground.GetRendererCount() > 0u);
    auto foregroundRenderer = foreground.GetRendererAt(0u);
    DALI_TEST_EQUALS(foregroundRenderer.GetCurrentProperty<float>(foregroundRenderer.GetPropertyIndex("uTextRevealProgress")),
                     0.0f, TEST_LOCATION);
    rendered.view.SetProperty(progress, 0.5f);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    // A valid publication no longer has the pending ordinary opacity binding.
    DALI_TEST_EQUALS(foreground.GetRendererAt(0u).GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), 1.0f, TEST_LOCATION);
    rendered.view.Unparent();
    application.SendNotification();
    application.Render();
  }
  END_TEST;
}

int UtcDaliTextVisualAsyncRevealPendingNoneFullFadeP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  for(bool blur : {false, true})
  {
    auto rendered = CreateTextVisual(application);
    const auto progress = rendered.view.RegisterProperty("testRevealProgress", 1.0f);
    UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::CHARACTER,
                                                0.25f, progress, 1u, UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f, blur ? 24.0f : 0.0f, 0.5f);
    auto parameters = MakeRevealParameters("First line\nSecond line", 1u, UiText::Internal::Reveal::Unit::CHARACTER,
                                           0.25f, UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f);
    parameters.isMultiLine = true;
    parameters.isTextRevealBlurRequested = blur;
    parameters.textRevealBlurRadius = blur ? 24.0f : 0.0f;
    parameters.textRevealBlurDurationRatio = 0.5f;
    auto loader = UiText::AsyncTextLoader::New();
    const auto result = loader.RenderText(parameters, false, Size::ZERO);
    PublishDirect(rendered, parameters, result);
    application.SendNotification();
    application.Render();
    UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::DISABLED,
                                                1.0f, progress, 2u);
    // The application can start an exit before the preceding None request
    // publishes. Neither the old enabled result nor None may win this race.
    UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::PIXEL,
                                                1.0f, progress, 3u, UiText::Internal::Reveal::Sequence::WHOLE_TEXT, 0.0f, blur ? 48.0f : 0.0f, 1.0f);
    DALI_TEST_EQUALS(rendered.view.GetRendererCount(), 1u, TEST_LOCATION);
    const auto fallback = rendered.view.GetRendererAt(0u);
    const auto texture = fallback.GetTextures().GetTexture(0u);
    PublishDirect(rendered, parameters, result); // stale enabled completion
    auto none = MakeParameters("First line\nSecond line");
    none.isMarqueeEnabled = false;
    none.textRevealRevision = 2u;
    PublishDirect(rendered, none, loader.RenderText(none, false, Size::ZERO));
    for(float value : {1.0f, 0.75f, 0.25f, 0.0f})
    {
      rendered.view.SetProperty(progress, value);
      application.SendNotification();
      application.Render();
      application.SendNotification();
      application.Render();
      DALI_TEST_CHECK(rendered.view.GetRendererAt(0u) == fallback);
      DALI_TEST_CHECK(fallback.GetTextures().GetTexture(0u) == texture);
      DALI_TEST_EQUALS(fallback.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), value, TEST_LOCATION);
      DALI_TEST_CHECK(!rendered.view.FindChildByName("TextRevealRuntimeGaussian"));
      DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
    }
    rendered.view.Unparent();
    application.SendNotification();
    application.Render();
  }
  END_TEST;
}

int UtcDaliTextVisualAsyncRevealPendingOpacityCompositionP(void)
{
  UiTestApplication application;
  auto              rendered   = CreateTextVisual(application);
  auto              parameters = MakeParameters("Pending opacity composition");
  parameters.isMarqueeEnabled  = false;
  auto loader                  = UiText::AsyncTextLoader::New();
  PublishDirect(rendered, parameters, loader.RenderText(parameters, false, Size::ZERO));
  auto renderer = rendered.view.GetRendererAt(0u);
  renderer.SetProperty(DevelRenderer::Property::OPACITY, 0.5f);
  rendered.view.SetProperty(Actor::Property::COLOR_MULTIPLIER_ALPHA, 0.5f);
  const auto progress  = rendered.view.RegisterProperty("testRevealProgress", 0.5f);
  const auto reference = rendered.view.RegisterProperty("referenceOpacity", 0.5f);
  Animation  animation = Animation::New(1.0f);
  animation.AnimateTo(Property(renderer, DevelRenderer::Property::OPACITY), 0.9f);
  animation.AnimateTo(Property(rendered.view, reference), 0.9f);
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::PIXEL,
                                              1.0f, progress, 1u, UiText::Internal::Reveal::Sequence::WHOLE_TEXT, 0.0f, 24.0f, 1.0f);
  for(int frame = 0; frame < 3; ++frame)
  {
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), 0.25f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(rendered.view.GetCurrentProperty<Vector4>(Actor::Property::COLOR_MULTIPLIER).a, 0.5f, TEST_LOCATION);
  }
  animation.Play();
  for(int frame = 0; frame < 6; ++frame)
  {
    application.SendNotification();
    application.Render(16);
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY),
                     rendered.view.GetCurrentProperty<float>(reference) * 0.5f, 0.0001f, TEST_LOCATION);
  }
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::DISABLED,
                                              1.0f, progress, 2u);
  for(int frame = 0; frame < 6; ++frame)
  {
    application.SendNotification();
    application.Render(16);
    // Removing the temporary constraint restores the existing animation's
    // authority; it must not bake the half-opacity result into that animation.
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY),
                     rendered.view.GetCurrentProperty<float>(reference), 0.0001f, TEST_LOCATION);
  }
  animation.Stop();
  rendered.view.Unparent();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextVisualAsyncShutdownCompletionP(void)
{
  UiTestApplication application;
  auto rendered = CreateTextVisual(application);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  const auto progress = rendered.view.RegisterProperty("testRevealProgress", 1.0f);
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::PIXEL,
                                              1.0f, progress, 1u, UiText::Internal::Reveal::Sequence::WHOLE_TEXT, 0.0f, 24.0f, 1.0f);
  auto parameters = MakeRevealParameters("Completion after shutdown", 1u, UiText::Internal::Reveal::Unit::PIXEL, 1.0f);
  parameters.isTextRevealBlurRequested = true;
  parameters.textRevealBlurRadius = 24.0f;
  auto loader = UiText::AsyncTextLoader::New();
  const auto result = loader.RenderText(parameters, false, Size::ZERO);
  const auto tasks = application.GetScene().GetRenderTaskList();
  application.GetAdaptor().Stop();
  PublishDirect(rendered, parameters, result);
  DALI_TEST_EQUALS(observer.mCompletionCount, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!rendered.view.FindChildByName("TextRevealRuntimeGaussian"));
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  parameters.requestType = UiIntegrationText::Async::COMPUTE_NATURAL_SIZE;
  PublishDirect(rendered, parameters, result);
  DALI_TEST_EQUALS(observer.mSizeCompletionCount, 0u, TEST_LOCATION);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  rendered.view.Unparent();
  END_TEST;
}

int UtcDaliTextVisualAsyncBlurConstructionReentryP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  for(uint32_t mutation = 0u; mutation < 6u; ++mutation)
  {
    auto rendered = CreateTextVisual(application);
    const auto progress = rendered.view.RegisterProperty("testRevealProgress", 0.5f);
    auto configure = [&](uint64_t revision, bool enabled, float radius)
    {
      UiInternal::TextVisual::ConfigureTextReveal(rendered.visual,
        enabled ? UiText::Internal::Reveal::Unit::PIXEL : UiText::Internal::Reveal::Unit::DISABLED,
        1.0f, progress, revision, UiText::Internal::Reveal::Sequence::WHOLE_TEXT, 0.0f, radius, 1.0f);
    };
    configure(1u, true, 24.0f);
    auto parameters = MakeRevealParameters("Old source", 1u, UiText::Internal::Reveal::Unit::PIXEL, 1.0f);
    parameters.isTextRevealBlurRequested = true;
    parameters.textRevealBlurRadius = 24.0f;
    auto loader = UiText::AsyncTextLoader::New();
    auto oldResult = loader.RenderText(parameters, false, Size::ZERO);
    auto next = parameters;
    next.textRevealRevision = 2u;
    next.textRevealBlurRadius = 48.0f;
    if(mutation == 2u)
    {
      next.text = "Latest replacement source";
    }
    auto nextResult = loader.RenderText(next, false, Size::ZERO);
    Renderer latestForeground;
    bool fired = false;
    bool propertyCancelled = false;
    bool inMutation = false;
    std::vector<WeakHandleBase> candidateActors;
    ConnectionTracker tracker;
    application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
    {
      if(!inMutation && Actor::DownCast(object))
      {
        candidateActors.emplace_back(object);
      }
      if(!fired && Actor::DownCast(object))
      {
        fired = true;
        inMutation = true;
        if(mutation == 0u)
        {
          configure(2u, false, 0.0f);
        }
        else if(mutation < 3u)
        {
          configure(2u, true, 48.0f);
          PublishDirect(rendered, next, nextResult);
          latestForeground = rendered.view.FindChildByName("RevealGaussianForeground").GetRendererAt(0u);
        }
        else if(mutation == 3u)
        {
          UiInternal::TextVisual::GetController(rendered.visual)->SetAsyncRendering(false);
        }
        else if(mutation == 4u)
        {
          rendered.view.Unparent();
        }
        else
        {
          auto foreground = rendered.view.GetRendererAt(0u);
          const auto fade = foreground.GetPropertyIndex("uTextRevealFadeDuration");
          foreground.PropertySetSignal().Connect(&tracker, [&, fade](Handle, Property::Index index, const Property::Value&)
          {
            if(!propertyCancelled && index == fade)
            {
              propertyCancelled = true;
              configure(2u, false, 0.0f);
            }
          });
        }
        inMutation = false;
      }
    });
    PublishDirect(rendered, parameters, oldResult);
    tracker.DisconnectAll();
    DALI_TEST_CHECK(fired);
    DALI_TEST_CHECK(mutation != 5u || propertyCancelled);
    for(const auto& object : candidateActors)
    {
      DALI_TEST_CHECK(!object.GetBaseHandle());
    }
    application.SendNotification();
    application.Render();
    auto runtime = rendered.view.FindChildByName("TextRevealRuntimeGaussian");
    if(mutation == 1u || mutation == 2u)
    {
      DALI_TEST_CHECK(runtime);
      DALI_TEST_CHECK(runtime.FindChildByName("RevealGaussianForeground").GetRendererAt(0u) == latestForeground);
      DALI_TEST_EQUALS(rendered.view.GetRendererCount(), 0u, TEST_LOCATION);
      DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 4u, TEST_LOCATION);
    }
    else
    {
      DALI_TEST_CHECK(!runtime);
      DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
    }
    rendered.view.Unparent();
    DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextVisualAsyncBlurWorkerPayloadP(void)
{
  UiTestApplication application;
  TextAbstraction::FontClient::Get();
  const auto samePixels = [](PixelData first, PixelData second)
  {
    if(!first || !second || first.GetWidth() != second.GetWidth() ||
       first.GetHeight() != second.GetHeight() || first.GetPixelFormat() != second.GetPixelFormat())
    {
      return false;
    }
    const auto a = Integration::GetPixelDataBuffer(first);
    const auto b = Integration::GetPixelDataBuffer(second);
    return a.bufferSize == b.bufferSize && std::memcmp(a.buffer, b.buffer, a.bufferSize) == 0;
  };
  for(const auto unit : {UiText::Internal::Reveal::Unit::CHARACTER, UiText::Internal::Reveal::Unit::WORD,
                         UiText::Internal::Reveal::Unit::LINE, UiText::Internal::Reveal::Unit::PIXEL})
  {
    for(const auto sequence : {UiText::Internal::Reveal::Sequence::WHOLE_TEXT, UiText::Internal::Reveal::Sequence::PER_LINE})
    {
      auto parameters                        = MakeRevealParameters("First visual line\nShort end", 1u, unit, 0.0f, sequence, 0.25f);
      parameters.isMultiLine                 = true;
      parameters.ellipsis                    = false;
      parameters.textRevealBlurRadius        = 24.0f;
      parameters.textRevealBlurDurationRatio = 0.5f;
      auto loader                            = UiText::AsyncTextLoader::New();
      auto ordinary                          = loader.RenderText(parameters, false, Size::ZERO);
      DALI_TEST_CHECK(!ordinary.revealBlur);
      parameters.isTextRevealBlurRequested = true;
      auto blurred                         = loader.RenderText(parameters, false, Size::ZERO);
      DALI_TEST_CHECK(blurred.revealBlur);
      const auto& prepared = *blurred.revealBlur;
      DALI_TEST_EQUALS(prepared.options.radius, 24u, TEST_LOCATION);
      DALI_TEST_EQUALS(prepared.options.perLine, sequence == UiText::Internal::Reveal::Sequence::PER_LINE, TEST_LOCATION);
      DALI_TEST_CHECK(prepared.metadata && prepared.blurDuration > 0.0f);
      DALI_TEST_EQUALS(!prepared.lines.empty(), prepared.options.perLine, TEST_LOCATION);
      for(const auto& line : prepared.lines)
      {
        if(line.atlasIndex == UiInternal::RevealBlurLineRaster::NO_ATLAS)
        {
          DALI_TEST_CHECK(line.foreground && line.metadata);
        }
        else
        {
          DALI_TEST_CHECK(line.atlasIndex < prepared.sourceAtlases.size());
          DALI_TEST_CHECK(!line.foreground && !line.metadata && !line.mask);
          const auto& atlas = prepared.sourceAtlases[line.atlasIndex];
          DALI_TEST_CHECK(atlas.foreground && atlas.metadata);
        }
        DALI_TEST_CHECK(line.sequence.start >= 0.0f && line.sequence.start < 1.0f);
      }
      // Preparing blur must not mutate the independent ordinary fallback.
      DALI_TEST_CHECK(samePixels(ordinary.textPixelData, blurred.textPixelData));
      DALI_TEST_EQUALS(ordinary.revealMetadataTiles.size(), blurred.revealMetadataTiles.size(), TEST_LOCATION);
      DALI_TEST_CHECK(samePixels(ordinary.revealMetadataTiles.front(), blurred.revealMetadataTiles.front()));
      DALI_TEST_EQUALS(ordinary.textRevealFadeDuration, blurred.textRevealFadeDuration, TEST_LOCATION);
      const auto copied = blurred;
      DALI_TEST_CHECK(copied.revealBlur.get() == blurred.revealBlur.get());
    }
  }
  // Supersampled input keeps display-sized blur targets and an independent
  // ordinary Reveal fallback. Supersampling must not multiply the FBO size.
  for(const float scale : {1.25f, 1.5f})
  {
    auto parameters                      = MakeRevealParameters("Scaled ordinary Reveal", 1u, UiText::Internal::Reveal::Unit::CHARACTER, 0.2f);
    parameters.isTextRevealBlurRequested = true;
    parameters.textRevealBlurRadius      = 24.0f;
    parameters.renderScale               = scale;
    auto       loader                    = UiText::AsyncTextLoader::New();
    bool cachedNaturalSize = false;
    const auto naturalSize = loader.SetupRenderScale(parameters, cachedNaturalSize);
    const auto result = loader.RenderText(parameters, cachedNaturalSize, naturalSize);
    DALI_TEST_CHECK(result.revealBlur);
    DALI_TEST_EQUALS(result.revealBlur->options.controlSize, result.renderedSize, TEST_LOCATION);
    DALI_TEST_CHECK(result.textPixelData && !result.revealMetadataTiles.empty());
  }
  END_TEST;
}

int UtcDaliTextVisualAsyncBlurValidInvalidValidP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TextAbstraction::FontClient::Get();
  auto       rendered = CreateTextVisual(application);
  const auto tasks    = application.GetScene().GetRenderTaskList();
  const auto baseline = tasks.GetTaskCount();
  const auto progress = rendered.view.RegisterProperty("testRevealProgress", 0.4f);
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::CHARACTER,
                                              0.0f, progress, 1u, UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f, 24.0f, 0.5f);
  auto parameters                        = MakeRevealParameters("First line\nSecond line", 1u, UiText::Internal::Reveal::Unit::CHARACTER,
                                                                0.0f, UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f);
  parameters.isMultiLine                 = true;
  parameters.isTextRevealBlurRequested   = true;
  parameters.textRevealBlurRadius        = 24.0f;
  parameters.textRevealBlurDurationRatio = 0.5f;
  auto       loader                      = UiText::AsyncTextLoader::New();
  const auto valid                       = loader.RenderText(parameters, false, Size::ZERO);
  DALI_TEST_CHECK(valid.revealBlur && !valid.revealBlur->lines.empty());
  DALI_TEST_CHECK(!valid.revealBlur->sourceAtlases.empty());
  for(int invalidKind = 0; invalidKind < 7; ++invalidKind)
  {
    PublishDirect(rendered, parameters, valid);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
    const auto foreground = rendered.view.FindChildByName("RevealGaussianForeground");
    DALI_TEST_CHECK(foreground && HasTextRevealRenderer(foreground));

    auto invalid = valid;
    auto partial = std::make_shared<UiInternal::PreparedRevealBlur>(*valid.revealBlur);
    if(invalidKind == 0)
    {
      partial->metadata.Reset();
    }
    if(invalidKind == 1)
    {
      const auto atlasIndex = partial->lines.front().atlasIndex;
      if(atlasIndex == UiInternal::RevealBlurLineRaster::NO_ATLAS)
      {
        partial->lines.front().foreground.Reset();
      }
      else
      {
        partial->sourceAtlases[atlasIndex].foreground.Reset();
      }
    }
    if(invalidKind == 2)
    {
      partial->options.radius = 48u;
    }
    if(invalidKind == 3)
    {
      partial->lines.front().metadata = CreatePixelData(1u, 1u, Pixel::RGBA8888);
    }
    if(invalidKind == 4)
    {
      partial->lines.front().atlasIndex = static_cast<uint32_t>(partial->sourceAtlases.size());
    }
    if(invalidKind == 5)
    {
      partial->lines.front().atlasRectangle.x = 0u; // Missing replicated edge.
    }
    if(invalidKind == 6)
    {
      partial->sourceAtlases.front().metadata = CreatePixelData(1u, 1u, Pixel::RGBA8888);
    }
    invalid.revealBlur = partial;
    PublishDirect(rendered, parameters, invalid);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
    DALI_TEST_CHECK(HasTextRevealRenderer(rendered.view));
    DALI_TEST_EQUALS(rendered.view.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(rendered.view.GetRendererAt(0u).GetProperty<float>("uTextRevealFadeDuration"),
                     valid.textRevealFadeDuration, 0.0001f, TEST_LOCATION);
  }
  PublishDirect(rendered, parameters, valid);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(rendered.view.GetProperty<float>(progress), 0.4f, TEST_LOCATION);
  rendered.view.Unparent();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  END_TEST;
}

namespace
{
int CheckBlurUploadFailureRestoresImages(bool direct)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TextAbstraction::FontClient::Get();
  auto        rendered   = CreateTextVisual(application);
  auto        controller = UiInternal::TextVisual::GetController(rendered.visual);
  Texture     image      = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  auto        url        = Ui::ImageUrl::New(image, true);
  const char* text       = "First long line\n\xef\xbf\xbc\nLast";
  auto        builder    = UiText::StyledTextBuilder::New(text);
  DALI_TEST_CHECK(builder.SetSpan(UiText::ImageSpan::New(UiText::ImageAttributes(url.GetUrl(), Vector2(24.0f, 18.0f))), 16u, 17u));
  controller->SetStyledText(builder.Build());
  const auto progress = rendered.view.RegisterProperty("testRevealProgress", 0.4f);
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::CHARACTER,
                                              0.25f, progress, 1u, UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f, 24.0f, 1.0f);
  auto parameters                        = MakeRevealParameters(text, 1u, UiText::Internal::Reveal::Unit::CHARACTER,
                                                                0.25f, UiText::Internal::Reveal::Sequence::PER_LINE, 0.25f);
  parameters.isMultiLine                 = true;
  parameters.isTextRevealBlurRequested   = true;
  parameters.textRevealBlurRadius        = 24.0f;
  parameters.textRevealBlurDurationRatio = 1.0f;
  parameters.replacementSourceSnapshot   = controller->GetReplacementSourceSnapshot();
  parameters.replacementLayoutGeneration = 1u;
  auto       loader                      = UiText::AsyncTextLoader::New();
  const auto result                      = loader.RenderText(parameters, false, Size::ZERO);
  DALI_TEST_CHECK(result.revealBlur && !result.revealBlur->images.empty());
  DALI_TEST_CHECK(!result.replacementRevealTimings.Empty() && !result.revealBlur->timings.Empty());
  DALI_TEST_CHECK(std::abs(result.revealBlur->timings[0u].fadeDuration - result.replacementRevealTimings[0u].fadeDuration) > 0.0001f ||
                  std::abs(result.revealBlur->timings[0u].start - result.replacementRevealTimings[0u].start) > 0.0001f);
  auto& data                = UiInternal::Text::GetOrCreateInlineReplacementData(rendered.view);
  data.lastRenderGeneration = 1u;
  DALI_TEST_CHECK(data.manager.Update(data.host, parameters.replacementSourceSnapshot, result.replacementPlacements,
                                      Vector2::ZERO, Vector2::ZERO, Vector2(VISUAL_WIDTH, VISUAL_HEIGHT),
                                      Vector2(VISUAL_WIDTH, VISUAL_HEIGHT), 1.0f, result.replacementSourceRevision, false));
  DALI_TEST_CHECK(data.manager.ApplyRevealTimings(result.replacementRevealTimings, result.replacementSourceRevision, progress));
  application.SendNotification();
  application.Render();
  data.manager.Refresh();
  using Accessor = UiInternal::Text::InlineReplacementManagerTestAccessor;
  DALI_TEST_EQUALS(Accessor::GetEntryCount(data.manager), 1u, TEST_LOCATION);
  const auto imageRenderer = Accessor::GetEntryVisual(data.manager, 1u).GetRenderer();
  DALI_TEST_CHECK(imageRenderer);
  UiText::ReplacementRevealTiming ordinary;
  DALI_TEST_CHECK(Accessor::GetRevealTiming(data.manager, 1u, ordinary));
  if(direct)
  {
    auto ordinaryResult = result;
    ordinaryResult.revealBlur.reset();
    PublishDirect(rendered, parameters, ordinaryResult);
    controller->SetAsyncRendering(false);
  }
  Renderer          ordinaryForeground;
  TextureSet        ordinaryTextures;
  Shader            ordinaryShader;
  float             ordinaryFade = 0.0f;
  bool              failed       = false;
  ConnectionTracker tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(!failed && Actor::DownCast(object))
    {
      failed = true;
      for(uint32_t index = 0u; index < rendered.view.GetRendererCount(); ++index)
      {
        auto       renderer = rendered.view.GetRendererAt(index);
        const auto fade     = renderer.GetPropertyIndex("uTextRevealFadeDuration");
        if(fade != Property::INVALID_INDEX)
        {
          ordinaryForeground = renderer;
          ordinaryTextures   = renderer.GetTextures();
          ordinaryShader     = renderer.GetShader();
          ordinaryFade       = renderer.GetProperty<float>(fade);
        }
      }
      // Invalidate only the optional payload after validation. Core Upload
      // returns false for an empty buffer without enqueuing a texture update.
      Dali::Integration::ReleasePixelDataBuffer(result.revealBlur->metadata);
    }
  });
  if(direct)
  {
    DALI_TEST_CHECK(!UiInternal::TextVisualTestAccessor::PublishBlur(rendered.visual, rendered.view, *result.revealBlur,
                                                                     result.replacementSourceRevision, result.replacementRevealTimings));
  }
  else
  {
    PublishDirect(rendered, parameters, result);
  }
  tracker.DisconnectAll();
  DALI_TEST_CHECK(failed);
  DALI_TEST_CHECK(ordinaryForeground);
  DALI_TEST_CHECK(ordinaryForeground.GetTextures() == ordinaryTextures);
  DALI_TEST_CHECK(ordinaryForeground.GetShader() == ordinaryShader);
  DALI_TEST_EQUALS(ordinaryForeground.GetProperty<float>(ordinaryForeground.GetPropertyIndex("uTextRevealFadeDuration")),
                   ordinaryFade, 0.0001f, TEST_LOCATION);
  DALI_TEST_CHECK(!rendered.view.FindChildByName("RevealGaussianForeground"));
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  UiText::ReplacementRevealTiming restored;
  DALI_TEST_CHECK(Accessor::GetRevealTiming(data.manager, 1u, restored));
  DALI_TEST_EQUALS(restored.start, ordinary.start, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(restored.fadeDuration, ordinary.fadeDuration, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(restored.progressionSpan, ordinary.progressionSpan, 0.0001f, TEST_LOCATION);
  DALI_TEST_CHECK(Accessor::GetRevealConstraint(data.manager, 1u).GetTargetObject() == imageRenderer);
  bool restoredRenderer = false;
  for(uint32_t index = 0u; index < rendered.view.GetRendererCount(); ++index)
  {
    restoredRenderer |= rendered.view.GetRendererAt(index) == imageRenderer;
  }
  DALI_TEST_CHECK(restoredRenderer);
  rendered.view.Unparent();
  END_TEST;
}
} // unnamed namespace

int UtcDaliTextVisualAsyncBlurUploadFailureRestoresImagesP(void)
{
  return CheckBlurUploadFailureRestoresImages(false);
}

int UtcDaliTextVisualBlurUploadFailureRestoresImagesP(void)
{
  // Exercise the shared publication transaction without the async completion's
  // later ordinary timing delivery masking an incomplete rollback.
  return CheckBlurUploadFailureRestoresImages(true);
}

int UtcDaliTextVisualAsyncBlurStaleRevisionP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TextAbstraction::FontClient::Get();
  auto       rendered                  = CreateTextVisual(application);
  const auto tasks                     = application.GetScene().GetRenderTaskList();
  const auto baseline                  = tasks.GetTaskCount();
  const auto progress                  = rendered.view.RegisterProperty("testRevealProgress", 0.4f);
  auto       parameters                = MakeRevealParameters("Blur revision", 1u, UiText::Internal::Reveal::Unit::CHARACTER, 0.0f);
  parameters.isTextRevealBlurRequested = true;
  parameters.textRevealBlurRadius      = 24.0f;
  auto       loader                    = UiText::AsyncTextLoader::New();
  const auto result                    = loader.RenderText(parameters, false, Size::ZERO);
  DALI_TEST_CHECK(result.revealBlur);
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, parameters.textRevealUnit, 0.0f,
                                              progress, 1u, parameters.textRevealSequence, 0.0f, 24.0f, 1.0f);
  PublishDirect(rendered, parameters, result);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, UiText::Internal::Reveal::Unit::DISABLED,
                                              0.0f, progress, 2u);
  // Keep the displayed publication until the current disable request arrives.
  // A stale completion must neither replace it nor resurrect it afterwards.
  PublishDirect(rendered, parameters, result);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  auto disabledParameters                      = parameters;
  disabledParameters.textRevealRevision        = 2u;
  disabledParameters.isTextRevealEnabled       = false;
  disabledParameters.isTextRevealBlurRequested = false;
  auto disabledResult                          = loader.RenderText(disabledParameters, false, Size::ZERO);
  PublishDirect(rendered, disabledParameters, disabledResult);
  PublishDirect(rendered, parameters, result);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  UiInternal::TextVisual::ConfigureTextReveal(rendered.visual, parameters.textRevealUnit, 0.0f,
                                              progress, 3u, parameters.textRevealSequence, 0.0f, 48.0f, 1.0f);
  PublishDirect(rendered, parameters, result);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  parameters.textRevealRevision   = 3u;
  parameters.textRevealBlurRadius = 48.0f;
  const auto current              = loader.RenderText(parameters, false, Size::ZERO);
  PublishDirect(rendered, parameters, current);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextDecorationScaleAsyncSupersedeP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  RenderedTextVisual rendered = CreateTextVisual(application);
  struct DecorationObserver : ReentrantAsyncInterface
  {
    DecorationObserver(RenderedTextVisual& rendered)
    : ReentrantAsyncInterface(rendered.visual, rendered.view, CompletionAction::NONE)
    {
    }
    void AsyncRenderFinished(UiText::AsyncTextRenderInfo&& info) override
    {
      overlay = info.overlayStylePixelData;
      ReentrantAsyncInterface::AsyncRenderFinished(std::move(info));
    }
    PixelData overlay;
  } observer(rendered);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  auto parameters               = MakeParameters("Decoration scale");
  parameters.isMarqueeEnabled   = false;
  parameters.isUnderlineEnabled = true;
  parameters.underlineHeight    = 2.f;
  parameters.underlineColor     = Color::RED;
  const auto original           = parameters;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  PixelData baseline = observer.overlay;
  DALI_TEST_CHECK(baseline);
  parameters.decorationUiScale  = 2.f;
  parameters.effectiveTextScale = 2.f;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mCompletionCount, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.overlay);
  for(float scale : {1.25f, 1.5f})
  {
    parameters.text               = std::string(30000u, 'B');
    parameters.decorationUiScale  = scale;
    parameters.effectiveTextScale = scale;
    DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  }
  parameters = original;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  for(uint32_t trigger = 0u; trigger < 3u && observer.mCompletionCount < 3u; ++trigger)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  }
  DALI_TEST_EQUALS(observer.mCompletionCount, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(observer.overlay);
  DALI_TEST_EQUALS(observer.overlay.GetWidth(), baseline.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(observer.overlay.GetHeight(), baseline.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(observer.overlay.GetPixelFormat(), baseline.GetPixelFormat(), TEST_LOCATION);
  DALI_TEST_EQUALS(observer.overlay.GetStrideBytes(), baseline.GetStrideBytes(), TEST_LOCATION);
  const auto expected = Dali::Integration::GetPixelDataBuffer(baseline);
  const auto actual   = Dali::Integration::GetPixelDataBuffer(observer.overlay);
  DALI_TEST_EQUALS(actual.bufferSize, expected.bufferSize, TEST_LOCATION);
  DALI_TEST_CHECK(actual.bufferSize == expected.bufferSize &&
                  std::memcmp(actual.buffer, expected.buffer, actual.bufferSize) == 0);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

int UtcDaliTextVisualUserSpaceScaleAllocationReentryP(void)
{
  UiTestApplication application;
  const auto originalScale = Ui::UiScaleManager::Get().GetScale();
  const auto originalScalable = Ui::UiScaleManager::Get().IsScalable();
  Ui::UiScaleManager::Get().SetScalable(true);
  Ui::UiScaleManager::Get().SetScale(1.0f);
  auto rendered = CreateTextVisual(application);
  rendered.view.SetUiScalePolicy(Ui::UiScalePolicy::ENABLED);
  Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2(100.0f, 0.0f));
  gradient.SetUnits(Ui::Gradient::Units::USER_SPACE);
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  UiInternal::TextVisual::SetTextGradientStyle(rendered.visual, UiText::Internal::Gradient::CreateStyle(gradient));
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  auto parameters = MakeParameters("Before allocation reentry");
  parameters.isMarqueeEnabled = false;
  parameters.isTextGradientRequested = true;
  parameters.hasUserSpaceGradient = true;
  auto loader = UiText::AsyncTextLoader::New();
  PublishDirect(rendered, parameters, loader.RenderText(parameters, false, Size::ZERO));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  auto oldRenderer = rendered.view.GetRendererAt(0u);
  auto oldTexture = oldRenderer.GetTextures().GetTexture(0u);
  parameters.text = "New text must not replace the previous texture during allocation reentry";
  // Force a new shader variant: texture/constraint allocation does not emit
  // ObjectCreated, whereas shader allocation is an actual application callback.
  UiInternal::TextVisual::SetTextGradientOverlayStyle(rendered.visual, UiText::Internal::Gradient::CreateStyle(gradient));
  parameters.isTextGradientOverlayRequested = true;
  const auto candidate = loader.RenderText(parameters, false, Size::ZERO);
  bool fired = false;
  ConnectionTracker tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(!fired && Shader::DownCast(object))
    {
      fired = true;
      Ui::UiScaleManager::Get().SetScale(2.0f);
    }
  });
  PublishDirect(rendered, parameters, candidate);
  DALI_TEST_CHECK(fired);
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(rendered.view.GetRendererAt(0u) == oldRenderer);
  DALI_TEST_CHECK(oldRenderer.GetTextures().GetTexture(0u) == oldTexture);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  rendered.view.Unparent();
  Ui::UiScaleManager::Get().SetScale(originalScale);
  Ui::UiScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

int UtcDaliTextVisualGradientCompletedSnapshotP(void)
{
  UiTestApplication    application;
  auto                 rendered = CreateTextVisual(application);
  Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2(100.0f, 0.0f));
  gradient.SetUnits(Ui::Gradient::Units::USER_SPACE);
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  UiInternal::TextVisual::SetTextGradientStyle(rendered.visual, UiText::Internal::Gradient::CreateStyle(gradient));
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  auto parameters                    = MakeParameters("Completed user space snapshot");
  parameters.isMarqueeEnabled        = false;
  parameters.isTextGradientRequested = true;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  UiInternal::TextVisual::SetTextGradientStyle(rendered.visual, {});
  DALI_TEST_CHECK(!UiInternal::TextVisual::UsesUserSpaceGradient(rendered.visual));
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

namespace
{
// source: Global / Overlay / Span. Style: None / BBOX / USER_SPACE.
int CheckGradientStyleAllocationReentry(int source, int from, int to)
{
  UiTestApplication application;
  const auto        originalScale    = Ui::UiScaleManager::Get().GetScale();
  const auto        originalScalable = Ui::UiScaleManager::Get().IsScalable();
  Ui::UiScaleManager::Get().SetScalable(true);
  Ui::UiScaleManager::Get().SetScale(1.0f);
  auto rendered = CreateTextVisual(application);
  rendered.view.SetUiScalePolicy(Ui::UiScalePolicy::ENABLED);
  auto controller             = UiInternal::TextVisual::GetController(rendered.visual);
  auto parameters             = MakeParameters("Gradient callback test");
  parameters.isMarqueeEnabled = false;
  auto setStyle               = [&](int mode, bool snapshot)
  {
    Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2(100.0f, 0.0f));
    gradient.SetUnits(mode == 2 ? Ui::Gradient::Units::USER_SPACE : Ui::Gradient::Units::OBJECT_BOUNDING_BOX);
    gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
    const auto style = mode ? UiText::Internal::Gradient::CreateStyle(gradient) : UiText::Internal::Gradient::Style{};
    if(source == 0)
    {
      UiInternal::TextVisual::SetTextGradientStyle(rendered.visual, style);
    }
    else if(source == 1)
    {
      UiInternal::TextVisual::SetTextGradientOverlayStyle(rendered.visual, style);
    }
    else
    {
      auto builder = UiText::StyledTextBuilder::New(parameters.text.c_str());
      if(mode)
      {
        builder.SetSpan(UiText::GradientSpan::New(gradient), 0u, static_cast<uint32_t>(parameters.text.size()));
      }
      auto styled = builder.Build();
      controller->SetStyledText(styled);
      if(snapshot)
      {
        parameters.hasStyledTextStyleSnapshot = true;
        parameters.styledTextStyleSnapshot    = UiInternal::Text::StyledTextApplier::BuildTextStyleRunSnapshot(styled, 96.0f);
      }
    }
    if(snapshot)
    {
      parameters.hasUserSpaceGradient           = mode == 2;
      parameters.isTextGradientRequested        = source == 0 && mode != 0;
      parameters.isTextGradientOverlayRequested = source == 1 && mode != 0;
    }
  };
  setStyle(from, true);
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  auto loader = UiText::AsyncTextLoader::New();
  PublishDirect(rendered, parameters, loader.RenderText(parameters, false, Size::ZERO));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  auto oldRenderer              = rendered.view.GetRendererAt(0u);
  auto oldTexture               = oldRenderer.GetTextures().GetTexture(0u);
  auto oldShader                = oldRenderer.GetShader();
  parameters.isUnderlineEnabled = true;
  parameters.underlineHeight    = 2.0f;
  parameters.underlineColor     = Color::WHITE;
  const auto        candidate   = loader.RenderText(parameters, false, Size::ZERO);
  bool              fired       = false;
  ConnectionTracker tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(!fired && Shader::DownCast(object))
    {
      fired = true;
      if(from != to)
      {
        setStyle(to, false);
      }
      Ui::UiScaleManager::Get().SetScale(2.0f);
    }
  });
  PublishDirect(rendered, parameters, candidate);
  DALI_TEST_CHECK(fired);
  const bool reject = from == 2 || to == 2;
  DALI_TEST_EQUALS(observer.mCompletionCount, reject ? 1u : 2u, TEST_LOCATION);
  if(reject)
  {
    DALI_TEST_CHECK(rendered.view.GetRendererCount() == 1u);
    DALI_TEST_CHECK(rendered.view.GetRendererAt(0u) == oldRenderer);
    DALI_TEST_CHECK(oldRenderer.GetTextures().GetTexture(0u) == oldTexture);
    DALI_TEST_CHECK(oldRenderer.GetShader() == oldShader);
  }
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  rendered.view.Unparent();
  Ui::UiScaleManager::Get().SetScale(originalScale);
  Ui::UiScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

} // unnamed namespace

int UtcDaliTextVisualGradientGlobalUserToUserShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(0, 2, 2);
}

int UtcDaliTextVisualGradientGlobalUserToBboxShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(0, 2, 1);
}

int UtcDaliTextVisualGradientGlobalUserToNoneShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(0, 2, 0);
}

int UtcDaliTextVisualGradientGlobalBboxToUserShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(0, 1, 2);
}

int UtcDaliTextVisualGradientGlobalNoneToUserShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(0, 0, 2);
}

int UtcDaliTextVisualGradientGlobalBboxToBboxShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(0, 1, 1);
}

int UtcDaliTextVisualGradientGlobalNoneToNoneShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(0, 0, 0);
}

int UtcDaliTextVisualGradientOverlayUserToUserShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(1, 2, 2);
}

int UtcDaliTextVisualGradientOverlayUserToBboxShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(1, 2, 1);
}

int UtcDaliTextVisualGradientOverlayUserToNoneShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(1, 2, 0);
}

int UtcDaliTextVisualGradientOverlayBboxToUserShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(1, 1, 2);
}

int UtcDaliTextVisualGradientOverlayNoneToUserShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(1, 0, 2);
}

int UtcDaliTextVisualGradientOverlayBboxToBboxShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(1, 1, 1);
}

int UtcDaliTextVisualGradientOverlayNoneToNoneShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(1, 0, 0);
}

int UtcDaliTextVisualGradientSpanUserToUserShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(2, 2, 2);
}

int UtcDaliTextVisualGradientSpanUserToBboxShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(2, 2, 1);
}

int UtcDaliTextVisualGradientSpanUserToNoneShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(2, 2, 0);
}

int UtcDaliTextVisualGradientSpanBboxToUserShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(2, 1, 2);
}

int UtcDaliTextVisualGradientSpanNoneToUserShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(2, 0, 2);
}

int UtcDaliTextVisualGradientSpanBboxToBboxShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(2, 1, 1);
}

int UtcDaliTextVisualGradientSpanNoneToNoneShaderReentryP(void)
{
  return CheckGradientStyleAllocationReentry(2, 0, 0);
}

int UtcDaliTextVisualGradientRequestShaderReentryP(void)
{
  UiTestApplication    application;
  auto                 rendered = CreateTextVisual(application);
  Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2(100.0f, 0.0f));
  gradient.SetUnits(Ui::Gradient::Units::USER_SPACE);
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  UiInternal::TextVisual::SetTextGradientStyle(rendered.visual, UiText::Internal::Gradient::CreateStyle(gradient));
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  auto parameters                    = MakeParameters("Initial");
  parameters.isMarqueeEnabled        = false;
  parameters.isTextGradientRequested = true;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  auto oldRenderer              = rendered.view.GetRendererAt(0u);
  auto oldTexture               = oldRenderer.GetTextures().GetTexture(0u);
  parameters.text               = "Old result A";
  parameters.isUnderlineEnabled = true;
  parameters.underlineHeight    = 2.0f;
  bool              fired = false, accepted = false;
  ConnectionTracker tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(!fired && Shader::DownCast(object))
    {
      fired     = true;
      auto next = parameters;
      next.text = "New request B owns completion";
      accepted  = UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, next);
    }
  });
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_CHECK(fired && accepted);
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(oldRenderer.GetTextures().GetTexture(0u) == oldTexture);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mCompletionCount, 2u, TEST_LOCATION);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}

namespace
{
int CheckGradientRequestCancellationReentry(bool emptyRequest)
{
  UiTestApplication    application;
  auto                 rendered = CreateTextVisual(application);
  Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2(100.0f, 0.0f));
  gradient.SetUnits(Ui::Gradient::Units::USER_SPACE);
  gradient.SetStopNodes({{0.0f, Ui::UiColor(Color::RED)}, {1.0f, Ui::UiColor(Color::BLUE)}});
  UiInternal::TextVisual::SetTextGradientStyle(rendered.visual, UiText::Internal::Gradient::CreateStyle(gradient));
  ReentrantAsyncInterface observer(rendered.visual, rendered.view, CompletionAction::NONE);
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, &observer);
  auto parameters                    = MakeParameters("Initial");
  parameters.isMarqueeEnabled        = false;
  parameters.isTextGradientRequested = true;
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  auto oldRenderer              = rendered.view.GetRendererAt(0u);
  auto oldTexture               = oldRenderer.GetTextures().GetTexture(0u);
  parameters.text               = "Old result A";
  parameters.isUnderlineEnabled = true;
  parameters.underlineHeight    = 2.0f;
  bool              fired = false, accepted = false;
  ConnectionTracker tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(!fired && Shader::DownCast(object))
    {
      fired = true;
      if(emptyRequest)
      {
        auto next = parameters;
        next.text.clear();
        accepted = UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, next);
      }
      else
      {
        UiInternal::TextVisual::CancelAsyncRender(rendered.visual);
        accepted = true;
      }
    }
  });
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_CHECK(fired && accepted);
  DALI_TEST_EQUALS(observer.mCompletionCount, emptyRequest ? 2u : 1u, TEST_LOCATION);
  if(emptyRequest)
  {
    DALI_TEST_CHECK(!oldRenderer.GetTextures());
  }
  else
  {
    DALI_TEST_CHECK(oldRenderer.GetTextures().GetTexture(0u) == oldTexture);
  }
  UiInternal::TextVisual::SetAsyncTextInterface(rendered.visual, nullptr);
  END_TEST;
}
} // unnamed namespace

int UtcDaliTextVisualGradientCancelShaderReentryP(void)
{
  return CheckGradientRequestCancellationReentry(false);
}

int UtcDaliTextVisualGradientEmptyShaderReentryP(void)
{
  return CheckGradientRequestCancellationReentry(true);
}

namespace
{
bool WaitForLabelPublication(UiTestApplication& application, const unsigned& completed, unsigned expected)
{
  for(unsigned attempt = 0u; attempt < 8u && completed < expected; ++attempt)
  {
    if(!Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT))
    {
      return false;
    }
    application.SendNotification();
    application.Render();
  }
  return completed == expected;
}

void CheckAutomaticPublication(bool blur, float renderScale = 1.0f, bool tiling = false)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto label = Ui::Label::New("Automatic asynchronous text publication across changing layout constraints");
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetRenderScale(renderScale);
  if(tiling)
  {
    std::string text;
    for(unsigned line = 0u; line < 400u; ++line)
    {
      text += "Text tile\n";
    }
    label.SetText(text.c_str());
    label.SetMultiLine(true);
    label.SetRequestedHeight(static_cast<float>(Dali::GetMaxTextureSize()) + 1000.0f);
  }
  label.SetAsyncRendering(true);
  if(blur)
  {
    Ui::Text::Reveal reveal;
    reveal.SetUnit(Ui::Text::Reveal::Unit::WORD);
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 4.0f);
    Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
    label.SetTextReveal(reveal);
    label.SetTextRevealProgress(0.4f);
  }
  unsigned          completed      = 0u;
  float             completedWidth = 0.0f;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float width, float)
  {
    ++completed;
    completedWidth = width;
  });
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 1u));
  const auto foreground = [&]() -> Actor
  {
    auto capture = label.FindChildByName("RevealGaussianForeground");
    return capture ? capture : label;
  };
  DALI_TEST_CHECK(HasValidTexture(foreground()));
  const auto rendererCount = label.GetRendererCount();
  DALI_TEST_CHECK(!tiling || rendererCount > 1u);
  const auto oldRenderer = foreground().GetRendererAt(0u);
  const auto oldTexture  = oldRenderer.GetTextures().GetTexture(0u);
  const auto oldShader   = oldRenderer.GetShader();
  const auto oldSize     = oldRenderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  for(float width : {180.0f, 160.0f, 140.0f})
  {
    label.SetRequestedWidth(width);
    application.SendNotification();
    application.Render(); // Workers run; their event-thread completions are held.
    DALI_TEST_EQUALS(completed, 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetRendererCount(), rendererCount, TEST_LOCATION);
    DALI_TEST_CHECK(foreground().GetRendererAt(0u) == oldRenderer);
    DALI_TEST_CHECK(oldRenderer.GetTextures().GetTexture(0u) == oldTexture);
    DALI_TEST_CHECK(oldRenderer.GetShader() == oldShader);
    DALI_TEST_EQUALS(oldRenderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), oldSize, TEST_LOCATION);
  }
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 2u));
  DALI_TEST_EQUALS(completedWidth, 140.0f, TEST_LOCATION);
  DALI_TEST_CHECK(foreground().GetRendererAt(0u) == oldRenderer);
  DALI_TEST_CHECK(oldRenderer.GetTextures().GetTexture(0u) != oldTexture);
  for(int frame = 0; frame < 3; ++frame)
  {
    application.SendNotification();
    application.Render();
    Test::WaitForEventThreadTrigger(1, 0);
  }
  DALI_TEST_EQUALS(completed, 2u, TEST_LOCATION);
}
} // namespace

int UtcDaliLabelAutomaticAsyncPublicationResizeP(void)
{
  CheckAutomaticPublication(false);
  END_TEST;
}

int UtcDaliLabelAutomaticAsyncPublicationBlurResizeP(void)
{
  CheckAutomaticPublication(true);
  END_TEST;
}

int UtcDaliLabelAutomaticAsyncPublicationRenderScaleResizeP(void)
{
  CheckAutomaticPublication(false, 2.0f);
  END_TEST;
}

int UtcDaliLabelAutomaticAsyncPublicationTilingResizeP(void)
{
  CheckAutomaticPublication(false, 1.0f, true);
  END_TEST;
}

int UtcDaliLabelAutomaticAsyncPublicationReentrantResizeP(void)
{
  UiTestApplication application;
  auto              label = Ui::Label::New("The callback requests the next automatic render");
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  unsigned          completed      = 0u;
  float             completedWidth = 0.0f;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float width, float)
  {
    ++completed;
    completedWidth = width;
    if(completed == 1u)
    {
      label.SetText("New text requested from completion");
      label.SetRequestedWidth(140.0f);
    }
  });
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 2u));
  DALI_TEST_EQUALS(completedWidth, 140.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelAutomaticAsyncPublicationShaderReentryP(void)
{
  UiTestApplication application;
  auto              label = Ui::Label::New("Previously published text");
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  unsigned          completed = 0u;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
  { ++completed; });
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 1u));
  const auto renderer = label.GetRendererAt(0u);
  const auto texture  = renderer.GetTextures().GetTexture(0u);
  const auto shader   = renderer.GetShader();
  const auto size     = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  bool       fired    = false;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(!fired && Shader::DownCast(object))
    {
      fired = true;
      label.SetText("Latest text requested during shader creation");
    }
  });
  label.SetTextUnderline(Ui::Text::Underline());
  label.SetRequestedWidth(140.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_CHECK(fired);
  DALI_TEST_EQUALS(completed, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == texture);
  DALI_TEST_CHECK(renderer.GetShader() == shader);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), size, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 2u));
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) != texture);
  END_TEST;
}

int UtcDaliLabelEmptyManualAsyncPublicationReentrantP(void)
{
  UiTestApplication application;
  auto              label = Ui::Label::New("Initial text");
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  unsigned          completed   = 0u;
  bool              requestNext = false;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
  {
    ++completed;
    if(requestNext)
    {
      requestNext = false;
      label.SetText("The final automatic request must complete");
      label.SetRequestedWidth(140.0f);
    }
  });
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 1u));
  label.SetText("");
  requestNext = true;
  auto& impl  = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
  impl.RequestAsyncRenderWithFixedSize(220.0f, 80.0f);
  DALI_TEST_EQUALS(completed, 2u, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 3u));
  DALI_TEST_CHECK(HasValidTexture(label));
  END_TEST;
}

int UtcDaliLabelAutomaticAsyncPublicationIntermediateBeforeRelayoutP(void)
{
  UiTestApplication application;
  auto              label = Ui::Label::New("Previously published text");
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  unsigned          completed = 0u;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
  { ++completed; });
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 1u));
  const auto renderer = label.GetRendererAt(0u);
  const auto texture  = renderer.GetTextures().GetTexture(0u);
  label.SetText("This worker result will become stale before the next relayout");
  application.SendNotification();
  application.Render();
  label.SetText("Latest text has been requested but not yet laid out");
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(completed, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) != texture);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 3u));
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) != texture);
  END_TEST;
}

int UtcDaliLabelAutomaticAsyncPublicationSceneReconnectP(void)
{
  UiTestApplication application;
  auto              label = Ui::Label::New("Previously published text");
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  unsigned          completed = 0u;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
  { ++completed; });
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 1u));
  label.SetText("Pending text at scene disconnection");
  application.SendNotification();
  application.Render();
  label.Unparent();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(completed, 1u, TEST_LOCATION);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(WaitForLabelPublication(application, completed, 2u));
  DALI_TEST_CHECK(HasValidTexture(label));
  END_TEST;
}

namespace
{
// Only the mock scheduler is intercepted. These are the actual tasks submitted
// by Label::OnRelayout, using the production loader and manager cancellation map.
struct CapturedPublication
{
  UiTestApplication application;
  Ui::Label         label = Ui::Label::New("The complete initial publication stays visible while new work is pending");
  ConnectionTracker tracker;
  unsigned          completed = 0u;
  float             width     = 0.0f;

  CapturedPublication()
  {
    Test::AsyncTaskManager::SetTaskCaptureEnabled(true);
    label.SetRequestedWidth(220.0f);
    label.SetRequestedHeight(80.0f);
    label.SetAsyncRendering(true);
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float w, float)
    {
      ++completed;
      width = w;
    });
    application.GetScene().Add(label);
    auto task = Dispatch();
    DALI_TEST_CHECK(task);
    Finish(task);
    DALI_TEST_EQUALS(completed, 1u, TEST_LOCATION);
  }

  ~CapturedPublication()
  {
    Test::AsyncTaskManager::SetTaskCaptureEnabled(false);
  }

  UiInternal::TextLoadingTaskPtr Dispatch()
  {
    application.SendNotification();
    application.Render();
    auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
    if(tasks.size() != 1u)
    {
      tet_printf("Expected one automatic text task, got %zu", tasks.size());
      tet_result(TET_FAIL);
      return {};
    }
    return UiInternal::TextLoadingTaskPtr(static_cast<UiInternal::TextLoadingTask*>(tasks.front().Get()));
  }

  void Deliver(UiInternal::TextLoadingTaskPtr task)
  {
    auto manager = UiText::AsyncTextManager::Get();
    UiText::GetImplementation(manager).LoadComplete(task);
  }

  void Finish(UiInternal::TextLoadingTaskPtr task)
  {
    DALI_TEST_CHECK(task && task->IsReady());
    if(task && task->IsReady())
    {
      task->Process();
      Deliver(task);
      application.SendNotification();
      application.Render();
    }
  }
};

struct PublishedVisual
{
  Renderer             renderer;
  Shader               shader;
  Geometry             geometry;
  TextureSet           textures;
  std::vector<Texture> slots;
  Vector2              size, offset;
  unsigned             rendererCount;

  explicit PublishedVisual(Ui::Label label)
  : renderer(label.GetRendererAt(0u)),
    shader(renderer.GetShader()),
    geometry(renderer.GetGeometry()),
    textures(renderer.GetTextures()),
    size(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE)),
    offset(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET)),
    rendererCount(label.GetRendererCount())
  {
    for(unsigned i = 0u; i < textures.GetTextureCount(); ++i)
    {
      slots.push_back(textures.GetTexture(i));
    }
  }

  void Check(Ui::Label label) const
  {
    DALI_TEST_EQUALS(label.GetRendererCount(), rendererCount, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
    DALI_TEST_CHECK(renderer.GetShader() == shader);
    DALI_TEST_CHECK(renderer.GetGeometry() == geometry);
    DALI_TEST_CHECK(renderer.GetTextures() == textures);
    DALI_TEST_EQUALS(textures.GetTextureCount(), slots.size(), TEST_LOCATION);
    for(unsigned i = 0u; i < slots.size(); ++i)
    {
      DALI_TEST_CHECK(textures.GetTexture(i) == slots[i]);
    }
    DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), size, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET), offset, TEST_LOCATION);
  }
};
} // namespace

int UtcDaliLabelAsyncPublicationAllCompletionOrdersP(void)
{
  CapturedPublication     test;
  std::array<unsigned, 3> workerOrder{{0, 1, 2}};
  unsigned                cases = 0u;
  do
  {
    std::array<unsigned, 3> deliveryOrder{{0, 1, 2}};
    do
    {
      const unsigned                                before = test.completed;
      PublishedVisual                               previous(test.label);
      std::array<UiInternal::TextLoadingTaskPtr, 3> tasks;
      for(unsigned i = 0u; i < 3u; ++i)
      {
        test.label.SetText(i == 0u ? "A must be discarded" : i == 1u ? "B must be discarded"
                                                                     : "C is the latest complete text");
        test.label.SetRequestedWidth(180.0f - 20.0f * i);
        tasks[i] = test.Dispatch();
        DALI_TEST_CHECK(tasks[i]);
        previous.Check(test.label);
      }
      for(auto i : workerOrder)
      {
        DALI_TEST_CHECK(tasks[i]->IsReady());
        tasks[i]->Process();
        previous.Check(test.label);
        DALI_TEST_EQUALS(test.completed, before, TEST_LOCATION);
      }
      bool    latestDelivered = false;
      Texture latestTexture;
      for(auto i : deliveryOrder)
      {
        test.Deliver(tasks[i]);
        test.application.SendNotification();
        test.application.Render();
        latestDelivered |= i == 2u;
        DALI_TEST_EQUALS(test.completed, before + (latestDelivered ? 1u : 0u), TEST_LOCATION);
        if(latestDelivered)
        {
          const auto texture = test.label.GetRendererAt(0u).GetTextures().GetTexture(0u);
          if(latestTexture)
          {
            DALI_TEST_CHECK(texture == latestTexture);
          }
          latestTexture = texture;
          DALI_TEST_EQUALS(test.width, 140.0f, TEST_LOCATION);
        }
        else
          previous.Check(test.label);
      }
      // A duplicate delivery must not notify or alter the publication either.
      test.Deliver(tasks[2]);
      DALI_TEST_EQUALS(test.completed, before + 1u, TEST_LOCATION);
      ++cases;
    } while(std::next_permutation(deliveryOrder.begin(), deliveryOrder.end()));
  } while(std::next_permutation(workerOrder.begin(), workerOrder.end()));
  DALI_TEST_EQUALS(cases, 36u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelAsyncPublicationPropertyMatrixP(void)
{
  CapturedPublication                                              test;
  const std::vector<std::pair<const char*, std::function<void()>>> changes = {
    {"text", [&]
  { test.label.SetText("Final changed text"); }},
    {"font-family", [&]
  { test.label.SetFontFamily("DejaVu Sans"); }},
    {"font-size", [&]
  { test.label.SetFontSize(26.0f); }},
    {"horizontal-alignment", [&]
  { test.label.SetHorizontalTextAlignment(UiText::Alignment::END); }},
    {"vertical-alignment", [&]
  { test.label.SetVerticalTextAlignment(UiText::Alignment::END); }},
    {"multiline", [&]
  { test.label.SetMultiLine(true); }},
    {"clip", [&]
  { test.label.SetTextOverflowMode(UiText::OverflowMode::CLIP); }},
    {"ellipsis", [&]
  { test.label.SetTextOverflowMode(UiText::OverflowMode::ELLIPSIS); }},
    {"text-fit", [&]
  { test.label.SetTextFit(UiText::Fit::Range(10.0f, 18.0f, 2.0f)); }},
    {"render-scale", [&]
  { test.label.SetRenderScale(1.5f); }},
    {"combined", [&]
  {test.label.SetFontSize(28.0f); test.label.SetRenderScale(2.0f); test.label.SetMultiLine(false); test.label.SetText("Final combined properties"); }}};
  for(const auto& change : changes)
  {
    tet_printf("Property invalidation: %s", change.first);
    test.label.SetText("The older worker must not publish after a property invalidates it");
    auto stale = test.Dispatch();
    DALI_TEST_CHECK(stale);
    PublishedVisual previous(test.label);
    const auto      before = test.completed;
    change.second();
    stale->Process();
    test.Deliver(stale); // deliberately before the next relayout
    // The old snapshot may publish before the replacement is dispatched.
    DALI_TEST_EQUALS(test.completed, before + 1u, TEST_LOCATION);
    auto current = test.Dispatch();
    DALI_TEST_CHECK(current);
    test.Finish(current);
    DALI_TEST_EQUALS(test.completed, before + 2u, TEST_LOCATION);
    const auto latest = test.label.GetRendererAt(0u).GetTextures();
    test.Deliver(stale);
    DALI_TEST_EQUALS(test.completed, before + 2u, TEST_LOCATION);
    DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == latest);
    DALI_TEST_CHECK(HasValidTexture(test.label));
  }
  END_TEST;
}

int UtcDaliLabelAsyncPublicationManualTransitionsP(void)
{
  CapturedPublication test;
  auto&               impl = static_cast<Ui::Integration::LabelImpl&>(test.label.GetImplementation());
  using Request            = void (Ui::Integration::LabelImpl::*)(float, float);
  const Request requests[] = {&Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedSize,
                              &Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedWidth,
                              &Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedHeight,
                              &Ui::Integration::LabelImpl::RequestAsyncRenderWithConstraints};
  for(auto request : requests)
  {
    for(bool manualNext : {false, true})
    {
      test.label.SetText("");
      const auto        before    = test.completed;
      bool              reentered = false;
      ConnectionTracker callback;
      test.label.AsyncRenderFinishedSignal().Connect(&callback, [&](Ui::View, float, float)
      {
        if(!reentered)
        {
          reentered = true;
          test.label.SetText("The request created inside the empty completion must survive");
          test.label.SetRequestedWidth(140.0f);
          if(manualNext) (impl.*request)(170.0f, 60.0f);
        }
      });
      (impl.*request)(220.0f, 80.0f);
      DALI_TEST_EQUALS(test.completed, before + 1u, TEST_LOCATION);
      auto next = test.Dispatch();
      DALI_TEST_CHECK(next);
      test.Finish(next);
      DALI_TEST_EQUALS(test.completed, before + 2u, TEST_LOCATION);
      DALI_TEST_CHECK(HasValidTexture(test.label));
      DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
    }
  }
  END_TEST;
}

int UtcDaliLabelAsyncPublicationAutomaticToManualP(void)
{
  CapturedPublication test;
  auto&               impl      = static_cast<Ui::Integration::LabelImpl&>(test.label.GetImplementation());
  bool                reentered = false;
  ConnectionTracker   callback;
  test.label.AsyncRenderFinishedSignal().Connect(&callback, [&](Ui::View, float, float)
  {
    if(!reentered)
    {
      reentered = true;
      test.label.SetText("Manual render requested by automatic completion");
      impl.RequestAsyncRenderWithFixedSize(170.0f, 60.0f);
    }
  });
  test.label.SetText("Automatic completion");
  auto first = test.Dispatch();
  DALI_TEST_CHECK(first);
  test.Finish(first);
  auto next = test.Dispatch();
  DALI_TEST_CHECK(next);
  test.Finish(next);
  DALI_TEST_EQUALS(test.completed, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(test.width, 170.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelAsyncPublicationModeReentryP(void)
{
  CapturedPublication test;
  auto&               impl      = static_cast<Ui::Integration::LabelImpl&>(test.label.GetImplementation());
  bool                reentered = false;
  ConnectionTracker   callback;
  test.label.AsyncRenderFinishedSignal().Connect(&callback, [&](Ui::View, float, float)
  {
    if(!reentered)
    {
      reentered = true;
      test.label.SetAsyncRendering(false);
      test.label.SetText("Synchronous text from completion");
    }
  });
  test.label.SetText("");
  impl.RequestAsyncRenderWithFixedSize(220.0f, 80.0f);
  test.application.SendNotification();
  test.application.Render();
  DALI_TEST_CHECK(HasValidTexture(test.label));
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  DALI_TEST_EQUALS(test.completed, 2u, TEST_LOCATION);
  test.label.SetAsyncRendering(true);
  auto task = test.Dispatch();
  DALI_TEST_CHECK(task);
  test.Finish(task);
  DALI_TEST_EQUALS(test.completed, 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelAsyncPublicationNoChangeManualP(void)
{
  CapturedPublication test;
  auto&               impl = static_cast<Ui::Integration::LabelImpl&>(test.label.GetImplementation());
  impl.RequestAsyncRenderWithFixedSize(170.0f, 60.0f);
  auto task = test.Dispatch();
  DALI_TEST_CHECK(task);
  for(unsigned repeat = 0u; repeat < 10u; ++repeat)
  {
    test.label.SetFontFamily(test.label.GetFontFamily());
    test.label.SetFontSize(test.label.GetFontSize());
    test.label.SetMultiLine(test.label.IsMultiLine());
    test.label.SetHorizontalTextAlignment(test.label.GetHorizontalTextAlignment());
    test.label.SetVerticalTextAlignment(test.label.GetVerticalTextAlignment());
    test.label.SetRenderScale(test.label.GetRenderScale());
    test.label.SetRequestedWidth(220.0f);
    test.application.SendNotification();
    test.application.Render();
  }
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  test.Finish(task);
  DALI_TEST_EQUALS(test.completed, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(test.width, 170.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelAsyncPublicationColorAndSizeP(void)
{
  CapturedPublication test;
  const auto          initialTexture = test.label.GetRendererAt(0u).GetTextures().GetTexture(0u);
  test.label.SetTextColor(Ui::UiColor(Color::RED));
  test.application.SendNotification();
  test.application.Render();
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures().GetTexture(0u) == initialTexture);
  for(bool height : {false, true})
  {
    PublishedVisual previous(test.label);
    for(unsigned i = 0u; i < 3u; ++i)
    {
      if(height)
        test.label.SetRequestedHeight(70.0f - 10.0f * i);
      else
        test.label.SetRequestedWidth(190.0f - 10.0f * i);
      auto task = test.Dispatch();
      DALI_TEST_CHECK(task);
      previous.Check(test.label);
      if(i == 2u)
        test.Finish(task);
      else
      {
        // Make the next request before delivering the old completion.
        task->Process();
        if(height)
          test.label.SetRequestedHeight(65.0f - 10.0f * i);
        else
          test.label.SetRequestedWidth(185.0f - 10.0f * i);
        auto next = test.Dispatch();
        DALI_TEST_CHECK(next);
        test.Deliver(task);
        previous.Check(test.label);
        next->Process(); // Let the next loop supersede this request too.
      }
    }
  }
  DALI_TEST_EQUALS(test.completed, 3u, TEST_LOCATION);
  DALI_TEST_CHECK(HasValidTexture(test.label));
  END_TEST;
}

namespace
{
Ui::Gradient::Linear PublicationGradient(bool userSpace = false)
{
  Ui::Gradient::Linear gradient(Vector2::ZERO, userSpace ? Vector2(140.0f, 0.0f) : Vector2::XAXIS);
  gradient.SetUnits(userSpace ? Ui::Gradient::Units::USER_SPACE : Ui::Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetStopNodes({Ui::Gradient::StopNode(0.0f, Ui::UiColor(Color::RED)),
                         Ui::Gradient::StopNode(1.0f, Ui::UiColor(Color::BLUE))});
  return gradient;
}

void CheckMarqueePublication(UiText::MarqueeOrientation orientation, const char* content, bool gradient, float scale)
{
  CapturedPublication test;
  test.label.SetText(content);
  test.label.SetMarqueeTriggerPolicy(UiText::MarqueeTriggerPolicy::MANUAL);
  test.label.SetMarqueeOrientation(orientation);
  test.label.SetMultiLine(orientation == UiText::MarqueeOrientation::VERTICAL);
  test.label.SetMarqueeLoopCount(0);
  test.label.SetRenderScale(scale);
  if(gradient) test.label.SetTextGradient(PublicationGradient());
  auto setup = test.Dispatch();
  DALI_TEST_CHECK(setup);
  test.Finish(setup);
  auto            before = test.completed;
  PublishedVisual previous(test.label);
  test.label.StartMarquee();
  auto start = test.Dispatch();
  DALI_TEST_CHECK(start);
  start->Process();
  test.label.StopMarquee();
  test.Deliver(start);
  DALI_TEST_EQUALS(test.completed, before, TEST_LOCATION);
  previous.Check(test.label);
  auto stop = test.Dispatch();
  DALI_TEST_CHECK(stop);
  test.Finish(stop);
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());

  test.label.StartMarquee();
  auto moving = test.Dispatch();
  DALI_TEST_CHECK(moving);
  test.Finish(moving);
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  before = test.completed;
  PublishedVisual movingVisual(test.label);
  test.label.SetHorizontalTextAlignment(UiText::Alignment::END);
  test.label.SetRequestedWidth(140.0f);
  test.label.SetRequestedHeight(40.0f);
  test.label.SetTextFit(UiText::Fit::Range(12.0f, 20.0f, 2.0f));
  test.label.SetTextOverflowMode(UiText::OverflowMode::CLIP);
  auto update = test.Dispatch();
  DALI_TEST_CHECK(update);
  movingVisual.Check(test.label);
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  test.Finish(update);
  DALI_TEST_EQUALS(test.completed, before + 1u, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());

  test.label.StopMarquee();
  auto stopping = test.Dispatch();
  DALI_TEST_CHECK(stopping);
  stopping->Process();
  test.label.StartMarquee();
  before = test.completed;
  test.Deliver(stopping);
  DALI_TEST_EQUALS(test.completed, before + 1u, TEST_LOCATION);
  auto restart = test.Dispatch();
  DALI_TEST_CHECK(restart);
  test.Finish(restart);
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  const auto restarted = test.label.GetRendererAt(0u).GetTextures();
  test.Deliver(stopping);
  DALI_TEST_EQUALS(test.completed, before + 2u, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == restarted);
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());

  test.label.StopMarquee();
  test.label.SetText("Fit");
  test.label.SetMarqueeTriggerPolicy(UiText::MarqueeTriggerPolicy::ON_OVERFLOW);
  test.label.SetRequestedWidth(320.0f);
  test.label.SetRequestedHeight(100.0f);
  auto fitting = test.Dispatch();
  DALI_TEST_CHECK(fitting);
  test.Finish(fitting);
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  DALI_TEST_CHECK(HasValidTexture(test.label));
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
}
} // namespace

int UtcDaliLabelAsyncPublicationMarqueeHorizontalLtrP(void)
{
  CheckMarqueePublication(UiText::MarqueeOrientation::HORIZONTAL, "Long scrolling text with several words beyond the viewport", false, 1.0f);
  END_TEST;
}
int UtcDaliLabelAsyncPublicationMarqueeHorizontalRtlP(void)
{
  CheckMarqueePublication(UiText::MarqueeOrientation::HORIZONTAL, "אבג דהו זחט יכל מנס עפר צקת אבג דהו זחט יכל", true, 1.5f);
  END_TEST;
}
int UtcDaliLabelAsyncPublicationMarqueeHorizontalBidiP(void)
{
  CheckMarqueePublication(UiText::MarqueeOrientation::HORIZONTAL, "English אבג العربية several words beyond the viewport", true, 2.0f);
  END_TEST;
}
int UtcDaliLabelAsyncPublicationMarqueeVerticalLtrP(void)
{
  CheckMarqueePublication(UiText::MarqueeOrientation::VERTICAL, "First line\nSecond line\nThird line\nFourth line", false, 1.0f);
  END_TEST;
}
int UtcDaliLabelAsyncPublicationMarqueeVerticalRtlP(void)
{
  CheckMarqueePublication(UiText::MarqueeOrientation::VERTICAL, "אבג דהו\nזחט יכל\nמנס עפר\nצקת אבג", true, 1.5f);
  END_TEST;
}
int UtcDaliLabelAsyncPublicationMarqueeVerticalBidiP(void)
{
  CheckMarqueePublication(UiText::MarqueeOrientation::VERTICAL, "English אבג\nالعربية mixed\nAnother line\nFinal line", true, 2.0f);
  END_TEST;
}

int UtcDaliLabelAsyncPublicationGradientMatrixP(void)
{
  CapturedPublication test;
  auto                visual = UiInternal::ViewDataImpl::Get(Ui::GetImpl(test.label)).GetVisual(UiText::LabelPropertyIndex::TEXT);
  for(unsigned mode = 0u; mode < 6u; ++mode)
  {
    tet_printf("Gradient publication mode %u", mode);
    auto gradient = PublicationGradient(mode == 5u);
    test.label.SetTextGradient(gradient);
    if(mode == 1u || mode == 2u)
    {
      auto builder = UiText::StyledTextBuilder::New("LOCAL normal LOCAL normal");
      DALI_TEST_CHECK(builder.SetSpan(UiText::GradientSpan::New(gradient), 0u, 5u));
      if(mode == 2u)
      {
        DALI_TEST_CHECK(builder.SetSpan(UiText::GradientSpan::New(gradient), 13u, 18u));
      }
      test.label.SetStyledText(builder.Build());
    }
    else if(mode == 3u)
      test.label.SetStyledText(UiText::StyledText::FromMarkup("Default <color value='red'>Red</color> Default"));
    else
      test.label.SetText("Control gradient with changing layout and stops");
    if(mode == 4u) test.label.SetTextGradientOverlay(PublicationGradient());
    test.label.SetRenderScale(mode % 2u ? 1.5f : 1.0f);
    auto initial = test.Dispatch();
    DALI_TEST_CHECK(initial);
    test.Finish(initial);
    const auto                                    before = test.completed;
    PublishedVisual                               previous(test.label);
    std::array<UiInternal::TextLoadingTaskPtr, 3> tasks;
    for(unsigned i = 0u; i < 3u; ++i)
    {
      test.label.SetRequestedWidth(180.0f - i * 10.0f);
      test.label.SetFontSize(18.0f + i * 2.0f);
      gradient.SetSpreadMethod(i == 0u ? Ui::Gradient::SpreadMethod::REPEAT : Ui::Gradient::SpreadMethod::REFLECT);
      gradient.SetStopNodes({Ui::Gradient::StopNode(0.0f, Ui::UiColor(i == 2u ? Color::GREEN : Color::RED)),
                             Ui::Gradient::StopNode(1.0f, Ui::UiColor(Color::BLUE))});
      test.label.SetTextGradient(gradient);
      tasks[i] = test.Dispatch();
      DALI_TEST_CHECK(tasks[i]);
      previous.Check(test.label);
    }
    tasks[2]->Process();
    test.Deliver(tasks[2]);
    const auto latest = test.label.GetRendererAt(0u).GetTextures();
    for(unsigned i : {1u, 0u})
    {
      tasks[i]->Process();
      test.Deliver(tasks[i]);
      DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == latest);
    }
    test.application.SendNotification();
    test.application.Render();
    DALI_TEST_EQUALS(test.completed, before + 1u, TEST_LOCATION);
    DALI_TEST_CHECK(HasValidTexture(test.label));
    DALI_TEST_CHECK(latest.GetTextureCount() >= 2u);
    if(mode == 3u)
    {
      DALI_TEST_CHECK(UiInternal::TextVisual::GetTextGradientMaskPixelData(visual));
    }
  }
  END_TEST;
}

namespace
{
void CheckOwnerlessManualDispatch(unsigned requestType, bool reenter)
{
  UiTestApplication application;
  auto              label = Ui::Label::New("A manual request submitted after the label joins the scene");
  ConnectionTracker tracker;
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  auto& impl    = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
  auto  request = [&]()
  {
    switch(requestType)
    {
      case 0u:
        impl.RequestAsyncRenderWithFixedSize(137.0f, 40.0f);
        break;
      case 1u:
        impl.RequestAsyncRenderWithFixedWidth(137.0f, 80.0f);
        break;
      case 2u:
        impl.RequestAsyncRenderWithFixedHeight(220.0f, 40.0f);
        break;
      case 3u:
        impl.RequestAsyncRenderWithConstraints(137.0f, 80.0f);
        break;
    }
  };
  unsigned ready     = 0u;
  unsigned completed = 0u;
  label.ResourceReadySignal().Connect(&tracker, [&](Ui::View)
  {
    ++ready;
    if(reenter && ready == 1u)
    {
      application.GetScene().Add(label);
      request();
    }
  });
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
  { ++completed; });
  Test::AsyncTaskManager::SetTaskCaptureEnabled(true);
  request();
  DALI_TEST_EQUALS(ready, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(completed, 0u, TEST_LOCATION);
  auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
  if(reenter)
  {
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    // The rejected outer request must not let relayout cancel the accepted one.
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  }
  else
  {
    DALI_TEST_CHECK(tasks.empty());
    application.GetScene().Add(label);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    tasks = Test::AsyncTaskManager::TakeCapturedTasks();
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
  }
  auto task = UiInternal::TextLoadingTaskPtr(static_cast<UiInternal::TextLoadingTask*>(tasks.front().Get()));
  DALI_TEST_CHECK(task->IsReady());
  task->Process();
  auto manager = UiText::AsyncTextManager::Get();
  UiText::GetImplementation(manager).LoadComplete(task);
  DALI_TEST_EQUALS(completed, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(HasValidTexture(label));
  application.SendNotification();
  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  Test::AsyncTaskManager::SetTaskCaptureEnabled(false);
}
} // namespace

int UtcDaliLabelOwnerlessManualFixedSizeReentryP(void)
{
  CheckOwnerlessManualDispatch(0u, true);
  END_TEST;
}
int UtcDaliLabelOwnerlessManualFixedWidthReentryP(void)
{
  CheckOwnerlessManualDispatch(1u, true);
  END_TEST;
}
int UtcDaliLabelOwnerlessManualFixedHeightReentryP(void)
{
  CheckOwnerlessManualDispatch(2u, true);
  END_TEST;
}
int UtcDaliLabelOwnerlessManualConstraintsReentryP(void)
{
  CheckOwnerlessManualDispatch(3u, true);
  END_TEST;
}
int UtcDaliLabelOwnerlessManualFixedSizeRejectedP(void)
{
  CheckOwnerlessManualDispatch(0u, false);
  END_TEST;
}
int UtcDaliLabelOwnerlessManualFixedWidthRejectedP(void)
{
  CheckOwnerlessManualDispatch(1u, false);
  END_TEST;
}
int UtcDaliLabelOwnerlessManualFixedHeightRejectedP(void)
{
  CheckOwnerlessManualDispatch(2u, false);
  END_TEST;
}
int UtcDaliLabelOwnerlessManualConstraintsRejectedP(void)
{
  CheckOwnerlessManualDispatch(3u, false);
  END_TEST;
}

// Measure captured processing/publication separately without timing pass thresholds.
int UtcDaliLabelAsyncPublicationMetricsP(void)
{
  CapturedPublication test;
  auto&               gl = test.application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);
  for(unsigned mode = 0u; mode < 3u; ++mode)
  {
    if(mode == 1u) test.label.SetTextGradient(PublicationGradient());
    if(mode == 2u)
    {
      test.label.SetMarqueeTriggerPolicy(UiText::MarqueeTriggerPolicy::MANUAL);
      test.label.SetMarqueeLoopCount(0);
      test.label.StartMarquee();
    }
    test.label.SetText("A fixed warm-up workload for measuring publication cost");
    auto warmup = test.Dispatch();
    DALI_TEST_CHECK(warmup);
    test.Finish(warmup);
    auto original         = test.label.GetRendererAt(0u);
    auto originalGeometry = original.GetGeometry();
    gl.ResetTextureCallStack();
    double            relayout = 0.0, publication = 0.0, raster = 0.0;
    unsigned          renderersCreated = 0u, shadersCreated = 0u, animationsCreated = 0u;
    ConnectionTracker allocations;
    test.application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&allocations, [&](BaseHandle handle)
    {
      if(Renderer::DownCast(handle)) ++renderersCreated;
      if(Shader::DownCast(handle)) ++shadersCreated;
      if(Animation::DownCast(handle)) ++animationsCreated;
    });
    const unsigned before = test.completed;
    for(unsigned i = 0u; i < 64u; ++i)
    {
      auto start = std::clock();
      test.label.SetRequestedWidth(i % 2u ? 220.0f : 180.0f);
      test.label.SetText(i % 2u ? "A fixed even text workload measuring publication" : "A fixed odd text workload measuring publication");
      auto task = test.Dispatch();
      relayout += static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC;
      DALI_TEST_CHECK(task);
      start = std::clock();
      task->Process();
      raster += static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC;
      start = std::clock();
      test.Deliver(task);
      publication += static_cast<double>(std::clock() - start) / CLOCKS_PER_SEC;
      test.application.SendNotification();
      test.application.Render();
      DALI_TEST_EQUALS(test.label.GetRendererCount(), 1u, TEST_LOCATION);
      DALI_TEST_CHECK(test.label.GetRendererAt(0u) == original);
      DALI_TEST_CHECK(original.GetGeometry() == originalGeometry);
    }
    DALI_TEST_EQUALS(test.completed, before + 64u, TEST_LOCATION);
    tet_printf("METRICS mode=%u updates=64 relayout_us=%.3f publication_us=%.3f raster_us=%.3f renderer_objects=%u shader_objects=%u animation_objects=%u geometry_reused=1 texture_uploads=%d texture_generations=%d\n",
               mode, relayout * 1e6 / 64.0, publication * 1e6 / 64.0, raster * 1e6 / 64.0,
               renderersCreated, shadersCreated, animationsCreated, gl.GetTextureTrace().CountMethod("TexImage2D"), gl.GetTextureTrace().CountMethod("GenTextures"));
  }
  END_TEST;
}

namespace
{
void CheckOnSceneEmptyResourceReadyManual(bool reenter)
{
  UiTestApplication application;
  auto              label = Ui::Label::New(reenter ? "" : "The latest manual request must survive the empty completion");
  ConnectionTracker tracker;
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  application.GetScene().Add(label);
  auto&                impl  = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
  unsigned             ready = 0u;
  std::vector<Vector2> completions;
  label.ResourceReadySignal().Connect(&tracker, [&](Ui::View)
  {
    if(++ready == 1u && reenter)
    {
      label.SetText("The latest manual request must survive the empty completion");
      impl.RequestAsyncRenderWithFixedSize(137.0f, 40.0f);
    }
  });
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float width, float height)
  {
    completions.emplace_back(width, height);
    tet_printf("empty ready reentry completion %u: %.1f x %.1f", static_cast<unsigned>(completions.size()), width, height);
  });
  Test::AsyncTaskManager::SetTaskCaptureEnabled(true);
  impl.RequestAsyncRenderWithFixedSize(reenter ? 220.0f : 137.0f, reenter ? 80.0f : 40.0f);
  auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
  DALI_TEST_EQUALS(ready, reenter ? 1u : 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  label.SetRequestedWidth(240.0f);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  auto replacements = Test::AsyncTaskManager::TakeCapturedTasks();
  tet_printf("empty ready reentry accepted=%u replacements=%u", static_cast<unsigned>(tasks.size()), static_cast<unsigned>(replacements.size()));
  DALI_TEST_CHECK(replacements.empty());
  auto task = UiInternal::TextLoadingTaskPtr(static_cast<UiInternal::TextLoadingTask*>(tasks.front().Get()));
  task->Process();
  auto manager = UiText::AsyncTextManager::Get();
  UiText::GetImplementation(manager).LoadComplete(task);
  DALI_TEST_EQUALS(completions.back(), Vector2(137.0f, 40.0f), TEST_LOCATION);
  DALI_TEST_CHECK(HasValidTexture(label));
  Test::AsyncTaskManager::SetTaskCaptureEnabled(false);
  DALI_TEST_EQUALS(completions.size(), reenter ? 2u : 1u, TEST_LOCATION);
}
} // namespace

int UtcDaliLabelOnSceneEmptyResourceReadyManualP(void)
{
  CheckOnSceneEmptyResourceReadyManual(true);
  END_TEST;
}

int UtcDaliLabelManualPendingRepeatedLayoutControlP(void)
{
  CheckOnSceneEmptyResourceReadyManual(false);
  END_TEST;
}

namespace
{
void RequestGateManual(Ui::Label label, unsigned api, float width, float height)
{
  auto& impl = static_cast<Ui::Integration::LabelImpl&>(label.GetImplementation());
  switch(api)
  {
    case 0u:
      impl.RequestAsyncRenderWithFixedSize(width, height);
      break;
    case 1u:
      impl.RequestAsyncRenderWithFixedWidth(width, height);
      break;
    case 2u:
      impl.RequestAsyncRenderWithFixedHeight(width, height);
      break;
    case 3u:
      impl.RequestAsyncRenderWithConstraints(width, height);
      break;
  }
}

// content: empty, zero width, zero height, or a normal worker request.
// Both callbacks use the public View signals; only worker delivery is controlled.
void CheckManualCompletionReentry(unsigned api, unsigned content, bool resourceReady,
                                  bool automatic = false, bool resize = false, bool attach = false)
{
  UiTestApplication application;
  auto              label = Ui::Label::New(content == 0u ? "" : "The original manual request");
  ConnectionTracker tracker;
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  if(!attach) application.GetScene().Add(label);

  bool                 submitted = false;
  unsigned             ready     = 0u;
  std::vector<Vector2> completions;
  auto                 pump = [&]()
  {
    application.SendNotification();
    application.Render();
    application.SendNotification();
  };
  auto submit = [&]()
  {
    if(submitted) return;
    submitted = true;
    if(attach) application.GetScene().Add(label);
    label.SetText("The latest manual request must survive the earlier completion");
    if(resize) label.SetRequestedWidth(230.0f);
    if(!automatic) RequestGateManual(label, api, 137.0f, 40.0f);
  };
  label.ResourceReadySignal().Connect(&tracker, [&](Ui::View)
  {
    ++ready;
    if(resourceReady) submit();
  });
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float width, float height)
  {
    completions.emplace_back(width, height);
    if(!resourceReady) submit();
  });
  auto manager = UiText::AsyncTextManager::Get();
  auto deliver = [&](UiInternal::TextLoadingTaskPtr task)
  {
    DALI_TEST_CHECK(task->IsReady());
    task->Process();
    const Vector2 size = task->mRenderInfo.renderedSize;
    UiText::GetImplementation(manager).LoadComplete(task);
    return size;
  };
  Test::AsyncTaskManager::SetTaskCaptureEnabled(true);
  RequestGateManual(label, api, content == 1u ? 0.0f : 220.0f, content == 2u ? 0.0f : 80.0f);
  auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
  if(content == 3u && !attach)
  {
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    auto first = UiInternal::TextLoadingTaskPtr(static_cast<UiInternal::TextLoadingTask*>(tasks.front().Get()));
    deliver(first);
    tasks = Test::AsyncTaskManager::TakeCapturedTasks();
  }
  DALI_TEST_CHECK(submitted);
  DALI_TEST_CHECK(ready >= 1u);
  const unsigned firstCompletions = attach ? 0u : 1u;
  DALI_TEST_EQUALS(completions.size(), firstCompletions, TEST_LOCATION);
  if(automatic)
  {
    DALI_TEST_CHECK(tasks.empty());
    label.SetRequestedWidth(240.0f);
    pump();
    tasks = Test::AsyncTaskManager::TakeCapturedTasks();
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
  }
  else
  {
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    pump();
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
    // The first relayout can consume the previous completion's one-shot size
    // exemption. A second layout must still preserve the pending manual task.
    label.SetRequestedWidth(240.0f);
    pump();
    DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  }
  auto latest = UiInternal::TextLoadingTaskPtr(static_cast<UiInternal::TextLoadingTask*>(tasks.front().Get()));
  DALI_TEST_EQUALS(latest->mParameters.textWidth, automatic ? 240.0f : 137.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(latest->mParameters.textHeight, automatic ? 80.0f : 40.0f, TEST_LOCATION);
  const auto expected = deliver(latest);
  DALI_TEST_EQUALS(completions.size(), firstCompletions + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(completions.back(), expected, TEST_LOCATION);
  if(automatic || api == 0u || api == 1u)
  {
    DALI_TEST_EQUALS(expected.x, automatic ? 240.0f : 137.0f, TEST_LOCATION);
  }
  if(automatic || api == 0u || api == 2u)
  {
    DALI_TEST_EQUALS(expected.y, automatic ? 80.0f : 40.0f, TEST_LOCATION);
  }
  DALI_TEST_CHECK(HasValidTexture(label));
  const auto texture = label.GetRendererAt(0u).GetTextures().GetTexture(0u);
  pump();
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  DALI_TEST_EQUALS(completions.size(), firstCompletions + 1u, TEST_LOCATION);

  // The accepted manual completion must also release automatic rendering.
  label.SetText("An automatic request after the manual completion");
  label.SetRequestedWidth(260.0f);
  pump();
  tasks = Test::AsyncTaskManager::TakeCapturedTasks();
  DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
  deliver(UiInternal::TextLoadingTaskPtr(static_cast<UiInternal::TextLoadingTask*>(tasks.front().Get())));
  DALI_TEST_EQUALS(completions.size(), firstCompletions + 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(completions.back(), Vector2(260.0f, 80.0f), TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTexture(0u) != texture);
  pump();
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  Test::AsyncTaskManager::SetTaskCaptureEnabled(false);
}
} // namespace

int UtcDaliLabelGateFixedSizeEmptyReadyManualP(void)
{
  CheckManualCompletionReentry(0u, 0u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeZeroWidthReadyManualP(void)
{
  CheckManualCompletionReentry(0u, 1u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeZeroHeightReadyManualP(void)
{
  CheckManualCompletionReentry(0u, 2u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeReadySizeManualP(void)
{
  CheckManualCompletionReentry(0u, 1u, true, false, true);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeEmptyFinishedManualP(void)
{
  CheckManualCompletionReentry(0u, 0u, false);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeEmptyReadyAutomaticP(void)
{
  CheckManualCompletionReentry(0u, 0u, true, true);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeEmptyFinishedAutomaticP(void)
{
  CheckManualCompletionReentry(0u, 0u, false, true);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeWorkerReadyManualP(void)
{
  CheckManualCompletionReentry(0u, 3u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeOwnerlessReadyManualP(void)
{
  CheckManualCompletionReentry(0u, 0u, true, false, false, true);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthEmptyReadyManualP(void)
{
  CheckManualCompletionReentry(1u, 0u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthZeroWidthReadyManualP(void)
{
  CheckManualCompletionReentry(1u, 1u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthZeroHeightReadyManualP(void)
{
  CheckManualCompletionReentry(1u, 2u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthReadySizeManualP(void)
{
  CheckManualCompletionReentry(1u, 1u, true, false, true);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthEmptyFinishedManualP(void)
{
  CheckManualCompletionReentry(1u, 0u, false);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthEmptyReadyAutomaticP(void)
{
  CheckManualCompletionReentry(1u, 0u, true, true);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthEmptyFinishedAutomaticP(void)
{
  CheckManualCompletionReentry(1u, 0u, false, true);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthWorkerReadyManualP(void)
{
  CheckManualCompletionReentry(1u, 3u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthOwnerlessReadyManualP(void)
{
  CheckManualCompletionReentry(1u, 0u, true, false, false, true);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightEmptyReadyManualP(void)
{
  CheckManualCompletionReentry(2u, 0u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightZeroWidthReadyManualP(void)
{
  CheckManualCompletionReentry(2u, 1u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightZeroHeightReadyManualP(void)
{
  CheckManualCompletionReentry(2u, 2u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightReadySizeManualP(void)
{
  CheckManualCompletionReentry(2u, 1u, true, false, true);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightEmptyFinishedManualP(void)
{
  CheckManualCompletionReentry(2u, 0u, false);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightEmptyReadyAutomaticP(void)
{
  CheckManualCompletionReentry(2u, 0u, true, true);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightEmptyFinishedAutomaticP(void)
{
  CheckManualCompletionReentry(2u, 0u, false, true);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightWorkerReadyManualP(void)
{
  CheckManualCompletionReentry(2u, 3u, true);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightOwnerlessReadyManualP(void)
{
  CheckManualCompletionReentry(2u, 0u, true, false, false, true);
  END_TEST;
}

int UtcDaliLabelGateConstraintsEmptyReadyManualP(void)
{
  CheckManualCompletionReentry(3u, 0u, true);
  END_TEST;
}

int UtcDaliLabelGateConstraintsZeroWidthReadyManualP(void)
{
  CheckManualCompletionReentry(3u, 1u, true);
  END_TEST;
}

int UtcDaliLabelGateConstraintsZeroHeightReadyManualP(void)
{
  CheckManualCompletionReentry(3u, 2u, true);
  END_TEST;
}

int UtcDaliLabelGateConstraintsReadySizeManualP(void)
{
  CheckManualCompletionReentry(3u, 1u, true, false, true);
  END_TEST;
}

int UtcDaliLabelGateConstraintsEmptyFinishedManualP(void)
{
  CheckManualCompletionReentry(3u, 0u, false);
  END_TEST;
}

int UtcDaliLabelGateConstraintsEmptyReadyAutomaticP(void)
{
  CheckManualCompletionReentry(3u, 0u, true, true);
  END_TEST;
}

int UtcDaliLabelGateConstraintsEmptyFinishedAutomaticP(void)
{
  CheckManualCompletionReentry(3u, 0u, false, true);
  END_TEST;
}

int UtcDaliLabelGateConstraintsWorkerReadyManualP(void)
{
  CheckManualCompletionReentry(3u, 3u, true);
  END_TEST;
}

int UtcDaliLabelGateConstraintsOwnerlessReadyManualP(void)
{
  CheckManualCompletionReentry(3u, 0u, true, false, false, true);
  END_TEST;
}

namespace
{
// action: detach and reattach, release the last app handle, switch to sync,
// or switch off/on and submit a new manual request.
void CheckManualReadyLifetime(unsigned api, unsigned action, bool resourceReady = true)
{
  UiTestApplication     application;
  auto                  label = Ui::Label::New("");
  WeakHandle<Ui::Label> weak(label);
  ConnectionTracker     tracker;
  label.SetRequestedWidth(220.0f);
  label.SetRequestedHeight(80.0f);
  label.SetAsyncRendering(true);
  application.GetScene().Add(label);
  bool                 changed = false;
  std::vector<Vector2> completions;
  auto                 change = [&]()
  {
    if(changed) return;
    changed = true;
    if(action == 0u || action == 1u)
    {
      label.Unparent();
      if(action == 1u)
        label.Reset();
      else
        RequestGateManual(label, api, 137.0f, 40.0f);
    }
    else
    {
      label.SetText("The latest text after changing async mode");
      label.SetAsyncRendering(false);
      if(action == 3u)
      {
        label.SetAsyncRendering(true);
        RequestGateManual(label, api, 137.0f, 40.0f);
      }
    }
  };
  label.ResourceReadySignal().Connect(&tracker, [&](Ui::View)
  {
    if(resourceReady) change();
  });
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float width, float height)
  {
    completions.emplace_back(width, height);
    if(!resourceReady) change();
  });
  Test::AsyncTaskManager::SetTaskCaptureEnabled(true);
  RequestGateManual(label, api, 220.0f, 80.0f);
  DALI_TEST_CHECK(changed);
  DALI_TEST_EQUALS(completions.size(), 1u, TEST_LOCATION);
  auto tasks = Test::AsyncTaskManager::TakeCapturedTasks();
  if(action == 0u)
  {
    DALI_TEST_CHECK(tasks.empty());
    label.SetText("A manual request after scene reattachment");
    application.GetScene().Add(label);
    RequestGateManual(label, api, 137.0f, 40.0f);
    tasks = Test::AsyncTaskManager::TakeCapturedTasks();
  }
  if(action == 0u || action == 3u)
  {
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    for(float width : {230.0f, 240.0f})
    {
      label.SetRequestedWidth(width);
      application.SendNotification();
      application.Render();
      application.SendNotification();
      DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
    }
    auto task = UiInternal::TextLoadingTaskPtr(static_cast<UiInternal::TextLoadingTask*>(tasks.front().Get()));
    DALI_TEST_CHECK(task->IsReady());
    task->Process();
    const Vector2 expected = task->mRenderInfo.renderedSize;
    auto          manager  = UiText::AsyncTextManager::Get();
    UiText::GetImplementation(manager).LoadComplete(task);
    DALI_TEST_EQUALS(completions.size(), 2u, TEST_LOCATION);
    DALI_TEST_EQUALS(completions.back(), expected, TEST_LOCATION);
    DALI_TEST_CHECK(HasValidTexture(label));
  }
  else
  {
    DALI_TEST_CHECK(tasks.empty());
  }
  application.SendNotification();
  application.Render();
  application.SendNotification();
  DALI_TEST_CHECK(Test::AsyncTaskManager::TakeCapturedTasks().empty());
  if(action == 1u)
  {
    DALI_TEST_CHECK(!weak.GetHandle());
  }
  if(action == 2u)
  {
    DALI_TEST_CHECK(!label.IsAsyncRendering());
    DALI_TEST_CHECK(HasValidTexture(label));
    DALI_TEST_EQUALS(completions.size(), 1u, TEST_LOCATION);
  }
  Test::AsyncTaskManager::SetTaskCaptureEnabled(false);
}
} // namespace

int UtcDaliLabelGateFixedSizeReadyDetachP(void)
{
  CheckManualReadyLifetime(0u, 0u);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeReadyDestroyP(void)
{
  CheckManualReadyLifetime(0u, 1u);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeReadyDisableAsyncP(void)
{
  CheckManualReadyLifetime(0u, 2u);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeReadyToggleAsyncManualP(void)
{
  CheckManualReadyLifetime(0u, 3u);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthReadyDetachP(void)
{
  CheckManualReadyLifetime(1u, 0u);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthReadyDestroyP(void)
{
  CheckManualReadyLifetime(1u, 1u);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthReadyDisableAsyncP(void)
{
  CheckManualReadyLifetime(1u, 2u);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthReadyToggleAsyncManualP(void)
{
  CheckManualReadyLifetime(1u, 3u);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightReadyDetachP(void)
{
  CheckManualReadyLifetime(2u, 0u);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightReadyDestroyP(void)
{
  CheckManualReadyLifetime(2u, 1u);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightReadyDisableAsyncP(void)
{
  CheckManualReadyLifetime(2u, 2u);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightReadyToggleAsyncManualP(void)
{
  CheckManualReadyLifetime(2u, 3u);
  END_TEST;
}

int UtcDaliLabelGateConstraintsReadyDetachP(void)
{
  CheckManualReadyLifetime(3u, 0u);
  END_TEST;
}

int UtcDaliLabelGateConstraintsReadyDestroyP(void)
{
  CheckManualReadyLifetime(3u, 1u);
  END_TEST;
}

int UtcDaliLabelGateConstraintsReadyDisableAsyncP(void)
{
  CheckManualReadyLifetime(3u, 2u);
  END_TEST;
}

int UtcDaliLabelGateConstraintsReadyToggleAsyncManualP(void)
{
  CheckManualReadyLifetime(3u, 3u);
  END_TEST;
}

int UtcDaliLabelGateFixedSizeFinishedDestroyP(void)
{
  CheckManualReadyLifetime(0u, 1u, false);
  END_TEST;
}

int UtcDaliLabelGateFixedWidthFinishedDestroyP(void)
{
  CheckManualReadyLifetime(1u, 1u, false);
  END_TEST;
}

int UtcDaliLabelGateFixedHeightFinishedDestroyP(void)
{
  CheckManualReadyLifetime(2u, 1u, false);
  END_TEST;
}

int UtcDaliLabelGateConstraintsFinishedDestroyP(void)
{
  CheckManualReadyLifetime(3u, 1u, false);
  END_TEST;
}
