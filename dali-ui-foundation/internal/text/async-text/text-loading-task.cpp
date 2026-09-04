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
#include <dali-ui-foundation/internal/text/async-text/text-loading-task.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/thread-settings.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>
#include <dali/public-api/signals/callback.h>

#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/async-text/async-text-manager-impl.h> ///< To call AsyncTextManager::ReleaseLoader
#include <dali-ui-foundation/internal/text/async-text/async-text-manager.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_TEXT_ASYNC, false);
} // namespace

TextLoadingTask::TextLoadingTask(const uint32_t id, Text::AsyncTextParameters&& parameters, CallbackBase* callback)
: AsyncTask(callback),
  mId(id),
  mParameters(std::move(parameters)),
  mRenderInfo(),
  mIsReady(false),
  mMutex()
{
}

TextLoadingTask::~TextLoadingTask()
{
  if(DALI_LIKELY(Dali::Adaptor::IsAvailable()))
  {
    // To avoid loader leaking. Never ever happend, but for safety.
    if(DALI_UNLIKELY(mLoader))
    {
      DALI_LOG_ERROR("Need to release loader!!");
      Text::Internal::AsyncTextManager::ReleaseLoaderToManager(nullptr, mLoader);
    }
  }
}

uint32_t TextLoadingTask::GetId()
{
  return mId;
}

void TextLoadingTask::SetLoader(Text::AsyncTextLoader& loader)
{
  mLoader = loader;

  if(DALI_LIKELY(!mIsReady && mLoader))
  {
    {
      Dali::Mutex::ScopedLock lock(mMutex);
      mIsReady = true;
    }
    NotifyToReady();
  }
}

void TextLoadingTask::Process()
{
  if(DALI_LIKELY(mId != 0u))
  {
    DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_LOADING_TASK_PROCESS");
    Load();
  }
  ReleaseLoader();
}

bool TextLoadingTask::IsReady()
{
  Dali::Mutex::ScopedLock lock(mMutex);
  return mIsReady;
}

void TextLoadingTask::Load()
{
  switch(mParameters.requestType)
  {
    case Ui::Integration::Text::Async::RENDER_FIXED_SIZE:
    case Ui::Integration::Text::Async::RENDER_FIXED_WIDTH:
    case Ui::Integration::Text::Async::RENDER_FIXED_HEIGHT:
    case Ui::Integration::Text::Async::RENDER_CONSTRAINT:
    {
      // To avoid duplicate calculation, we can skip Initialize and Update.
      Size naturalSize       = Size::ZERO;
      bool cachedNaturalSize = false;

      if(mParameters.renderScale > 1.0f)
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("SetupRenderScale : %f\n", mParameters.renderScale);
        }
#endif
        naturalSize = mLoader.SetupRenderScale(mParameters, cachedNaturalSize);
      }

      if(!mParameters.suppressAutoMarquee && mParameters.marqueeTriggerPolicy == Text::MarqueeTriggerPolicy::ON_OVERFLOW)
      {
        if(mParameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL)
        {
          if(mParameters.isMultiLine)
          {
            DALI_LOG_DEBUG_INFO("Marquee Horizontal: valid only for single-line text\n");
            mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
          }
          else
          {
            if(!cachedNaturalSize)
            {
              naturalSize       = mLoader.ComputeNaturalSize(mParameters);
              cachedNaturalSize = true;
            }
            if(mParameters.textWidth < naturalSize.width)
            {
#ifdef TRACE_ENABLED
              if(gTraceFilter && gTraceFilter->IsTraceEnabled())
              {
                DALI_LOG_RELEASE_INFO("RenderMarquee, MarqueeTriggerPolicy::ON_OVERFLOW\n");
              }
#endif
              mParameters.isMarqueeEnabled = true;
              mRenderInfo                  = mLoader.RenderMarquee(mParameters, cachedNaturalSize, naturalSize);
            }
            else
            {
              mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
            }
          }
        }
        else // MarqueeOrientation::VERTICAL
        {
          if(!mParameters.isMultiLine)
          {
            DALI_LOG_DEBUG_INFO("Marquee Vertical: valid only for multi-line text\n");
            mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
          }
          else
          {
            const float textHeight = mLoader.ComputeHeightForWidth(mParameters, mParameters.textWidth, cachedNaturalSize);
            if(mParameters.textHeight < textHeight)
            {
#ifdef TRACE_ENABLED
              if(gTraceFilter && gTraceFilter->IsTraceEnabled())
              {
                DALI_LOG_RELEASE_INFO("RenderMarquee, MarqueeTriggerPolicy::ON_OVERFLOW\n");
              }
#endif
              mParameters.isMarqueeEnabled = true;
              mRenderInfo                  = mLoader.RenderMarquee(mParameters, true, naturalSize);
            }
            else
            {
              mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
            }
          }
        }
      }
      else if(mParameters.isMarqueeEnabled &&
              ((!mParameters.isMultiLine && mParameters.marqueeOrientation == Text::MarqueeOrientation::HORIZONTAL) ||
               (mParameters.isMultiLine && mParameters.marqueeOrientation == Text::MarqueeOrientation::VERTICAL)))
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RenderMarquee\n");
        }
#endif
        mRenderInfo = mLoader.RenderMarquee(mParameters, cachedNaturalSize, naturalSize);
      }
      else if(mParameters.isTextFitEnabled || mParameters.isTextFitCandidatesEnabled)
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RenderTextFit\n");
        }
#endif
        mRenderInfo = mLoader.RenderTextFit(mParameters, cachedNaturalSize, naturalSize);
      }
      else
      {
#ifdef TRACE_ENABLED
        if(gTraceFilter && gTraceFilter->IsTraceEnabled())
        {
          DALI_LOG_RELEASE_INFO("RenderText\n");
        }
#endif
        mRenderInfo = mLoader.RenderText(mParameters, cachedNaturalSize, naturalSize);
      }
      break;
    }
    case Ui::Integration::Text::Async::COMPUTE_NATURAL_SIZE:
    {
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        DALI_LOG_RELEASE_INFO("GetNaturalSize\n");
      }
#endif
      mRenderInfo = mLoader.GetNaturalSize(mParameters);
      break;
    }
    case Ui::Integration::Text::Async::COMPUTE_HEIGHT_FOR_WIDTH:
    {
#ifdef TRACE_ENABLED
      if(gTraceFilter && gTraceFilter->IsTraceEnabled())
      {
        DALI_LOG_RELEASE_INFO("GetHeightForWidth\n");
      }
#endif
      mRenderInfo = mLoader.GetHeightForWidth(mParameters);
      break;
    }
    default:
    {
      DALI_LOG_ERROR("Unexpected request type recieved : %d\n", mParameters.requestType);
      break;
    }
  }
}

void TextLoadingTask::ReleaseLoader()
{
  // Release all local varaibles before execute callback.
  if(DALI_LIKELY(mLoader))
  {
    DALI_TRACE_SCOPE(gTraceFilter, "DALI_TEXT_ASYNC_LOADING_TASK_RELEASE");
    auto loader = std::move(mLoader);
    Text::Internal::AsyncTextManager::ReleaseLoaderToManager(this, loader);
  }
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
