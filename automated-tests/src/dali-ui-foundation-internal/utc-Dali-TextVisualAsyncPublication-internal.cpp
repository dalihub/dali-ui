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
#include <cstring>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/text-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-data.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/text/styled-text/gradient-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-types.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
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
  rendered.view.SetProperty(Actor::Property::COLOR_ALPHA, 0.5f);
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
    DALI_TEST_EQUALS(rendered.view.GetCurrentProperty<Vector4>(Actor::Property::COLOR).a, 0.5f, TEST_LOCATION);
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
        DALI_TEST_CHECK(line.foreground && line.metadata);
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
  for(int invalidKind = 0; invalidKind < 4; ++invalidKind)
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
      partial->lines.front().foreground.Reset();
    }
    if(invalidKind == 2)
    {
      partial->options.radius = 48u;
    }
    if(invalidKind == 3)
    {
      partial->lines.front().metadata = CreatePixelData(1u, 1u, Pixel::RGBA8888);
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
