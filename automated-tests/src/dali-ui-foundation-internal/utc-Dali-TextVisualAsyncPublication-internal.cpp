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
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/text/styled-text/gradient-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>

using namespace Dali;

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
  view.SetProperty(Actor::Property::SIZE, Vector3(VISUAL_WIDTH, VISUAL_HEIGHT, 0.0f));

  Property::Map properties;
  properties.Add(Dali::Ui::VisualBasePropertyIndex::TYPE,
                 Dali::Ui::Integration::InternalVisualType::TEXT);
  properties.Add(Dali::Ui::TextVisualPropertyIndex::TEXT, "initial");
  properties.Add(Dali::Ui::TextVisualPropertyIndex::FONT_SIZE, 16.0f);

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
