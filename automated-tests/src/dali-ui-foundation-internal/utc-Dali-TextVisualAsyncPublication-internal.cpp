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

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/text-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>

using namespace Dali;

namespace
{
constexpr float VISUAL_WIDTH             = 220.0f;
constexpr float VISUAL_HEIGHT            = 64.0f;
constexpr int   ASYNC_TEXT_THREAD_TIMEOUT = 5;

namespace UiInternal = Dali::Ui::Internal;
namespace UiIntegrationText = Dali::Ui::Integration::Text;
namespace UiText     = Dali::Ui::Text;

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
                          Actor                                actor,
                          CompletionAction                     action = CompletionAction::REQUEST_NEXT)
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
        mNextRequestAccepted = UiInternal::TextVisual::UpdateAsyncRenderer(mVisual, mNextParameters);
        mResultValidAfterAction = HasValidTexture(mActor);
        break;
      }
      case CompletionAction::CLEAR_TEXT:
      {
        mNextParameters.text.clear();
        mNextRequestAccepted = UiInternal::TextVisual::UpdateAsyncRenderer(mVisual, mNextParameters);
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
  }

  Dali::Ui::Integration::Visual::Base mVisual;
  Actor                                mActor;
  UiText::AsyncTextParameters          mNextParameters;
  uint32_t                             mCompletionCount{0u};
  uint32_t                             mMarqueeInitializationCount{0u};
  bool                                 mFirstResultValid{false};
  bool                                 mNextRequestAccepted{false};
  bool                                 mResultValidAfterAction{false};
  CompletionAction                     mAction{CompletionAction::REQUEST_NEXT};
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

  UiText::AsyncTextRenderInfo failedInfo;
  Ui::TextLoadObserver::TextInformation failed(failedInfo, parameters);
  Ui::TextLoadObserver& loadObserver = static_cast<UiInternal::TextVisual&>(
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

  UiText::AsyncTextParameters parameters = MakeParameters("initial published result");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  DALI_TEST_EQUALS(observer.mCompletionCount, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

  parameters = MakeParameters("cancelled request B remains invisible");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(HasValidTexture(rendered.view));
  parameters = MakeParameters("cancelled request C remains invisible");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(HasValidTexture(rendered.view));
  parameters = MakeParameters("current request D is the only result that may replace the published texture");
  DALI_TEST_CHECK(UiInternal::TextVisual::UpdateAsyncRenderer(rendered.visual, parameters));
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

  for(uint32_t trigger = 0u; trigger < 3u && observer.mCompletionCount < 2u; ++trigger)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
  }
  DALI_TEST_EQUALS(observer.mCompletionCount, 2u, TEST_LOCATION);
  DALI_TEST_CHECK(HasValidTexture(rendered.view));

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
