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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/text/reveal-integ.h>
#include <dali-ui-foundation/internal/render-effects/gaussian-blur-algorithm.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-data.h>
#include <dali-ui-foundation/internal/visuals/text/text-reveal-blur-renderer.h>
#include <dali-ui-foundation/internal/visuals/text/text-reveal-runtime-blur.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <dali/devel-api/adaptor-framework/image-loading-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/devel-api/text-abstraction/segmentation.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/rendering/visual-renderer.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/object/weak-handle.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

#include "inline-replacement-manager-test-accessor.h"
#include "test-graphics-buffer.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
void Settle(UiTestApplication& application)
{
  for(int i = 0; i < 4; ++i)
  {
    application.SendNotification();
    application.Render(16);
  }
}

Label MakeLabel(UiTestApplication& application)
{
  Label label = Label::New("A bright foreground — AVATAR ffi office");
  label.SetLayoutMode(LayoutMode::STANDALONE);
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(72.0f);
  Text::Reveal reveal;
  reveal.SetFadeDurationRatio(0.25f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.0f);
  application.GetScene().Add(label);
  Settle(application);
  return label;
}

Actor Find(Actor parent, const char* name)
{
  for(uint32_t i = 0u; i < parent.GetChildCount(); ++i)
  {
    Actor child = parent.GetChildAt(i);
    if(child.GetProperty<Dali::String>(Actor::Property::NAME) == name)
    {
      return child;
    }
    Actor found = Find(child, name);
    if(found)
    {
      return found;
    }
  }
  return {};
}

std::vector<Renderer> Passes(Actor parent, const char* name)
{
  std::vector<Renderer> result;
  if(parent.GetProperty<Dali::String>(Actor::Property::NAME) == name)
  {
    for(uint32_t index = 0u; index < parent.GetRendererCount(); ++index)
    {
      result.push_back(parent.GetRendererAt(index));
    }
  }
  for(uint32_t i = 0u; i < parent.GetChildCount(); ++i)
  {
    Actor child  = parent.GetChildAt(i);
    auto  nested = Passes(child, name);
    result.insert(result.end(), nested.begin(), nested.end());
  }
  return result;
}

// A logical line binding is not a draw. PER_LINE blur packs several
// bindings into one renderer; WHOLE_TEXT uses a scalar strength property.
struct BlurLineBinding
{
  Renderer        renderer;
  Property::Index state{Property::INVALID_INDEX};

  float Start() const
  {
    return state == Property::INVALID_INDEX ? renderer.GetProperty<float>(renderer.GetPropertyIndex("uRevealSequenceStart"))
                                            : renderer.GetCurrentProperty<Vector2>(state).y;
  }
  float Strength() const
  {
    return state == Property::INVALID_INDEX ? renderer.GetCurrentProperty<float>(renderer.GetPropertyIndex("uAnimationRatio"))
                                            : renderer.GetCurrentProperty<Vector2>(state).x;
  }
  float Progress() const
  {
    return renderer.GetCurrentProperty<float>(renderer.GetPropertyIndex("uTextRevealBlurProgress"));
  }
};

std::vector<BlurLineBinding> BlurLines(Actor parent, bool horizontal = true)
{
  std::vector<BlurLineBinding> result;
  for(auto renderer : Passes(parent, horizontal ? "RevealGaussianH" : "RevealGaussianV"))
  {
    result.push_back({renderer});
  }
  for(auto renderer : Passes(parent, horizontal ? "RevealGaussianBatchH" : "RevealGaussianBatchV"))
  {
    for(uint32_t line = 0u; line < Ui::Internal::TextRevealBlurRenderer::MAX_LINES_PER_DRAW; ++line)
    {
      const std::string name  = "uRevealBlurState[" + std::to_string(line) + "]";
      const auto        index = renderer.GetPropertyIndex(name.c_str());
      if(index == Property::INVALID_INDEX)
      {
        break;
      }
      result.push_back({renderer, index});
    }
  }
  return result;
}

// Output batches preserve the H/V draw's indices even when image format
// ordering splits one draw into several non-contiguous presentation ranges.
std::vector<float> OutputStrengths(Actor parent)
{
  std::vector<float> result;
  for(auto renderer : Passes(parent, "RevealGaussianOutput"))
  {
    const auto scalar = renderer.GetPropertyIndex("uAnimationRatio");
    if(scalar != Property::INVALID_INDEX)
    {
      result.push_back(renderer.GetCurrentProperty<float>(scalar));
      continue;
    }
    for(uint32_t line = 0u; line < Ui::Internal::TextRevealBlurRenderer::MAX_LINES_PER_DRAW; ++line)
    {
      const std::string name  = "uRevealBlurStrength[" + std::to_string(line) + "]";
      const auto        index = renderer.GetPropertyIndex(name.c_str());
      if(index != Property::INVALID_INDEX)
      {
        result.push_back(renderer.GetCurrentProperty<float>(index));
      }
    }
  }
  return result;
}

// Inspect uploaded quad positions; actor positions alone do not describe
// line placement within a batched output.
bool HasUploadedBatchVertices(UiTestApplication& application, const std::vector<float>& expected)
{
  for(const auto* buffer : application.GetGraphicsController().mAllocatedBuffers)
  {
    if(!(buffer->mUsage & static_cast<uint32_t>(Graphics::BufferUsage::VERTEX_BUFFER)) ||
       buffer->memory.size() != expected.size() * sizeof(float))
    {
      continue;
    }
    bool equal = true;
    for(size_t index = 0u; index < expected.size(); ++index)
    {
      float actual;
      std::memcpy(&actual, buffer->memory.data() + index * sizeof(float), sizeof(float));
      if(!std::isfinite(actual) || std::abs(actual - expected[index]) > 0.0001f)
      {
        equal = false;
        break;
      }
    }
    if(equal)
    {
      return true;
    }
  }
  return false;
}

// Capture vertices carry inverse tile sizes and line indices. QUALITY output
// omits both; PERFORMANCE output keeps only the line index for its handoff.
// Read the latest matching draw to inspect placement without requiring actors
// or additional production properties for individual lines.
std::vector<float> UploadedCaptureVertices(UiTestApplication& application, uint32_t lines, bool horizontal = false)
{
  const size_t count   = static_cast<size_t>(lines) * 4u * 11u;
  const auto&  buffers = application.GetGraphicsController().mAllocatedBuffers;
  for(auto iter = buffers.rbegin(); iter != buffers.rend(); ++iter)
  {
    const auto* buffer = *iter;
    if(!(buffer->mUsage & static_cast<uint32_t>(Graphics::BufferUsage::VERTEX_BUFFER)) ||
       buffer->memory.size() != count * sizeof(float))
    {
      continue;
    }
    std::vector<float> values(count);
    std::memcpy(values.data(), buffer->memory.data(), buffer->memory.size());
    if(count != 0u && values[8u] > 0.0f && values[9u] > 0.0f)
    {
      const bool cropped = values[3u] > 0.0f || values[25u] < 1.0f;
      if(cropped == horizontal)
      {
        return values;
      }
    }
  }
  return {};
}

std::vector<float> UploadedSourceVertices(UiTestApplication& application, uint32_t lines)
{
  const size_t count   = static_cast<size_t>(lines) * 4u * 12u;
  const auto&  buffers = application.GetGraphicsController().mAllocatedBuffers;
  for(auto iter = buffers.rbegin(); iter != buffers.rend(); ++iter)
  {
    const auto* buffer = *iter;
    if((buffer->mUsage & static_cast<uint32_t>(Graphics::BufferUsage::VERTEX_BUFFER)) &&
       buffer->memory.size() == count * sizeof(float))
    {
      std::vector<float> values(count);
      std::memcpy(values.data(), buffer->memory.data(), buffer->memory.size());
      if(count && values[0u] == -0.5f && values[1u] == -0.5f && values[10u] > 0.0f && values[11u] > 0.0f)
      {
        return values;
      }
    }
  }
  return {};
}

uint64_t BatchStoragePixels(const std::vector<Ui::Internal::RuntimeRevealBlurBatch>& batches)
{
  uint64_t pixels = 0u;
  for(size_t index = 0u; index < batches.size(); ++index)
  {
    const auto& size   = batches[index].size;
    bool        shared = false;
    for(size_t previous = 0u; previous < index; ++previous)
    {
      shared |= batches[previous].size == size;
    }
    pixels += (shared ? 1u : 3u) * static_cast<uint64_t>(size.x) * static_cast<uint64_t>(size.y);
  }
  return pixels;
}

// Independent reference for the previous fallback's allocation ceiling.
std::vector<Ui::Internal::RuntimeRevealBlurBatch> FourLineBatches(const std::vector<Vector2>& sizes, uint32_t maximum)
{
  std::vector<Ui::Internal::RuntimeRevealBlurBatch> result;
  for(size_t first = 0u; first < sizes.size();)
  {
    Ui::Internal::RuntimeRevealBlurBatch batch{first, 1u, sizes[first]};
    uint64_t                             occupied = static_cast<uint64_t>(sizes[first].x) * static_cast<uint64_t>(sizes[first].y);
    for(size_t next = first + 1u; next < sizes.size() && next < first + 4u; ++next)
    {
      const Vector2 combined(std::max(batch.size.x, sizes[next].x), batch.size.y + sizes[next].y);
      occupied += static_cast<uint64_t>(sizes[next].x) * static_cast<uint64_t>(sizes[next].y);
      const uint64_t area = static_cast<uint64_t>(combined.x) * static_cast<uint64_t>(combined.y);
      if(combined.x > static_cast<float>(maximum) || combined.y > static_cast<float>(maximum) ||
         area > 1024u * 1024u || area * 4u > occupied * 5u)
      {
        break;
      }
      batch.size = combined;
      ++batch.count;
    }
    first += batch.count;
    result.push_back(batch);
  }
  return result;
}
} // unnamed namespace

int UtcDaliTextRevealRuntimeGaussianSamplingPathsP(void)
{
  using Ui::Internal::RuntimeRevealBlurPath;
  UiTestApplication application;
  const auto tasks = application.GetScene().GetRenderTaskList();
  for(bool perLine : {false, true})
  {
    for(bool alphaOnly : {true, false, true})
    {
      Label label = MakeLabel(application);
      const auto foreground = label.GetRendererAt(0u);
      for(auto path : {RuntimeRevealBlurPath::FULL_RESOLUTION, RuntimeRevealBlurPath::AXIS_AWARE_QUARTER})
      {
        std::vector<Ui::Internal::RuntimeRevealBlurSequence> sequences;
        if(perLine)
        {
          sequences.resize(2u);
          for(uint32_t index = 0u; index < 2u; ++index)
          {
            sequences[index].textures = foreground.GetTextures();
            sequences[index].textureRect = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
            sequences[index].lineIndex = index;
            sequences[index].start = static_cast<float>(index) * 0.25f;
          }
        }
        const bool reduced = path == RuntimeRevealBlurPath::AXIS_AWARE_QUARTER;
        auto companion = Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f),
                                                               label.GetPropertyIndex("uTextRevealProgress"), 16u, 0.5f,
                                                               std::move(sequences), alphaOnly, {}, {}, {path, 15.5f});
        DALI_TEST_CHECK(companion);
        Settle(application);
        DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
        Texture source, horizontal, vertical;
        for(uint32_t index = 1u; index < tasks.GetTaskCount(); ++index)
        {
          const auto task = tasks.GetTask(index);
          const auto name = task.GetSourceActor().GetProperty<Dali::String>(Actor::Property::NAME);
          const auto texture = task.GetFrameBuffer().GetColorTexture();
          DALI_TEST_EQUALS(texture.GetPixelFormat(), alphaOnly ? Pixel::A8 : Pixel::RGBA8888, TEST_LOCATION);
          DALI_TEST_EQUALS(task.GetRefreshRate(), static_cast<uint32_t>(RenderTask::REFRESH_ALWAYS), TEST_LOCATION);
          if(name == (perLine ? "RevealGaussianBatchSource" : "RevealGaussianSource"))
          {
            source = texture;
          }
          if(name == (perLine ? "RevealGaussianBatchH" : "RevealGaussianH"))
          {
            horizontal = texture;
          }
          if(name == (perLine ? "RevealGaussianBatchV" : "RevealGaussianV"))
          {
            vertical = texture;
          }
        }
        DALI_TEST_CHECK(source && horizontal && vertical);
        DALI_TEST_EQUALS(horizontal.GetWidth(), reduced ? (source.GetWidth() + 3u) / 4u : source.GetWidth(), TEST_LOCATION);
        DALI_TEST_EQUALS(horizontal.GetHeight(), source.GetHeight(), TEST_LOCATION);
        DALI_TEST_EQUALS(vertical.GetWidth(), horizontal.GetWidth(), TEST_LOCATION);
        DALI_TEST_EQUALS(vertical.GetHeight(), reduced ? (source.GetHeight() + 3u) / 4u : source.GetHeight(), TEST_LOCATION);
        const auto outputs = Passes(companion, "RevealGaussianOutput");
        DALI_TEST_EQUALS(outputs.size(), static_cast<size_t>(1u), TEST_LOCATION);
        for(auto output : outputs)
        {
          DALI_TEST_EQUALS(output.GetTextures().GetTexture(0u), vertical, TEST_LOCATION);
          DALI_TEST_EQUALS(output.GetTextures().GetTextureCount(), reduced ? 2u : 1u, TEST_LOCATION);
          if(reduced)
          {
            DALI_TEST_EQUALS(output.GetTextures().GetTexture(1u), source, TEST_LOCATION);
            DALI_TEST_EQUALS(output.GetProperty<float>(output.GetPropertyIndex("uQuarterAuthoredRadius")), 15.5f, TEST_LOCATION);
            const auto program = output.GetShader().GetProperty(Shader::Property::PROGRAM);
            Dali::String fragment;
            DALI_TEST_CHECK(program.GetMap()->Find("fragment")->Get(fragment));
            const std::string code(fragment.CStr());
            DALI_TEST_CHECK((code.find("#define ALPHA_ONLY\n") != std::string::npos) == alphaOnly);
            DALI_TEST_CHECK(code.find("mix(sharpColor, color, blurMix)") != std::string::npos);
          }
        }
        for(float progress : {0.0f, 0.2f, 0.4f, 0.6f, 1.0f, 0.4f, 0.0f})
        {
          label.SetTextRevealProgress(progress);
          Settle(application);
          if(reduced)
          {
            const auto lines = BlurLines(companion);
            const auto strengths = OutputStrengths(companion);
            DALI_TEST_EQUALS(lines.size(), strengths.size(), TEST_LOCATION);
            for(size_t index = 0u; index < strengths.size(); ++index)
            {
              DALI_TEST_EQUALS(strengths[index], lines[index].Strength(), 0.0001f, TEST_LOCATION);
            }
          }
        }
        // The output binds the existing Source, not a copied sharp texture.
        // Remove returns the original foreground regardless of the chosen path.
        Ui::Internal::RemoveRuntimeRevealBlur(companion, label);
        Settle(application);
        DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
        DALI_TEST_EQUALS(label.GetRendererAt(0u), foreground, TEST_LOCATION);
      }
      label.Unparent();
      Settle(application);
    }
  }
  END_TEST;
}

int UtcDaliTextRevealBlurMemoryAdmissionP(void)
{
  // A normal paragraph remains admitted under the 256 MiB publication ceiling.
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TextAbstraction::FontClient::Get();
  const auto  tasks = application.GetScene().GetRenderTaskList();
  std::string text("A normal paragraph with room for blur.");
  for(uint32_t line = 1u; line < 15u; ++line)
  {
    text += "\nA normal paragraph with room for blur.";
  }
  for(bool async : {false, true})
  {
    for(auto mode : {Ui::Integration::Text::Reveal::BlurMode::QUALITY, Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE})
    {
      Label label = Label::New(text.c_str());
      label.SetLayoutMode(LayoutMode::STANDALONE);
      label.SetRequestedWidth(1500.0f);
      label.SetRequestedHeight(850.0f);
      label.SetFontSize(32.0f);
      label.SetMultiLine(true);
      label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
      label.SetLineHeight(50.0f);
      label.SetAsyncRendering(async);
      Text::Reveal reveal;
      reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
      reveal.SetSequenceStaggerRatio(0.25f);
      Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
      Ui::Integration::Text::Reveal::SetBlurMode(reveal, mode);
      label.SetTextReveal(reveal);
      label.SetTextRevealProgress(0.4f);
      uint32_t          completed = 0u;
      ConnectionTracker tracker;
      label.AsyncRenderFinishedSignal().Connect(&tracker, [&](View, float, float)
      {
        ++completed;
      });
      application.GetScene().Add(label);
      Settle(application);
      for(uint32_t attempt = 0u; async && completed == 0u && attempt < 12u; ++attempt)
      {
        DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
        Settle(application);
      }
      DALI_TEST_CHECK(!async || completed > 0u);
      DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
      const auto lines = BlurLines(label);
      DALI_TEST_CHECK(lines.size() >= 10u);
      DALI_TEST_CHECK(tasks.GetTaskCount() > 1u);
      label.Unparent();
      label.Reset();
      Settle(application);
      DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliTextRevealBlurMemoryRejectionP(void)
{
  UiTestApplication application;
  auto              controller = Text::Controller::New();
  controller->SetMultiLineEnabled(true);
  controller->SetDefaultFontSize(10.0f, Text::Controller::PIXEL_SIZE);
  std::string text("Text");
  for(uint32_t line = 1u; line < 50u; ++line)
  {
    text += "\nText";
  }
  controller->SetText(text);
  const Vector2 size(128.0f, 1200.0f);
  controller->Relayout(size);
  auto typesetter = Text::Typesetter::New(controller->GetRenderTextModel());
  DALI_TEST_CHECK(typesetter->Render(size, Text::Direction::LEFT_TO_RIGHT, Text::Typesetter::RENDER_NO_STYLES, false, Pixel::L8));
  auto source = Text::Internal::Reveal::BuildCharacterPlan(*controller->GetRenderTextModel(), 0.25f);
  auto plan   = typesetter->CreateFinalRevealPlan(source, Text::Internal::Reveal::Unit::CHARACTER,
                                                  Text::Internal::Reveal::Sequence::PER_LINE, 0.25f);
  DALI_TEST_CHECK(Ui::Internal::BuildRevealBlurSequences(*typesetter, plan).size() >= 40u);
  Ui::Internal::RevealBlurPreparationOptions options;
  options.rasterSize     = Vector2(2048.0f, 2048.0f);
  options.controlSize    = options.rasterSize;
  options.radius         = 64u;
  options.maxTextureSize = 4096u;
  options.perLine        = true;
  // Dimensions fit; many halo-expanded bands still exceed the memory ceiling.
  DALI_TEST_CHECK(!Ui::Internal::PrepareRevealBlur(*typesetter, plan, {}, options));
  options.rasterSize = options.controlSize = Vector2(5120.0f, 5120.0f);
  options.maxTextureSize                   = 16384u;
  for(bool perLine : {false, true})
  {
    options.perLine = perLine;
    DALI_TEST_CHECK(!Ui::Internal::PrepareRevealBlur(*typesetter, plan, {}, options));
  }
  const float nan        = std::numeric_limits<float>::quiet_NaN();
  const float infinity   = std::numeric_limits<float>::infinity();
  options.maxTextureSize = std::numeric_limits<uint32_t>::max();
  for(const auto invalidSize : {Vector2(1.0e30f, 1.0e30f), Vector2(infinity, 64.0f),
                                Vector2(64.0f, nan), Vector2(-1.0f, 64.0f)})
  {
    options.rasterSize = options.controlSize = invalidSize;
    DALI_TEST_CHECK(!Ui::Internal::PrepareRevealBlur(*typesetter, plan, {}, options));
  }
  options.rasterSize = options.controlSize = size;
  options.radius                           = std::numeric_limits<uint32_t>::max();
  DALI_TEST_CHECK(!Ui::Internal::PrepareRevealBlur(*typesetter, plan, {}, options));
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianBatchLimitsP(void)
{
  using Ui::Internal::BuildRuntimeRevealBlurBatches;
  DALI_TEST_CHECK(BuildRuntimeRevealBlurBatches({}, 4096u).empty());
  const auto equal = BuildRuntimeRevealBlurBatches(std::vector<Vector2>(12u, Vector2(200.0f, 60.0f)), 4096u);
  DALI_TEST_EQUALS(equal.size(), static_cast<size_t>(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(equal.front().first, static_cast<size_t>(0u), TEST_LOCATION);
  DALI_TEST_EQUALS(equal.front().count, static_cast<size_t>(12u), TEST_LOCATION);
  DALI_TEST_EQUALS(equal.front().size, Vector2(200.0f, 720.0f), TEST_LOCATION);
  const auto tail = BuildRuntimeRevealBlurBatches(std::vector<Vector2>(5u, Vector2(200.0f, 60.0f)), 4096u);
  DALI_TEST_EQUALS(tail.size(), static_cast<size_t>(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(tail.front().count, static_cast<size_t>(5u), TEST_LOCATION);
  // The complete page fits even though its first two lines exceed 25% waste.
  const auto prefix = BuildRuntimeRevealBlurBatches({Vector2(80.0f, 100.0f), Vector2(800.0f, 100.0f),
                                                     Vector2(800.0f, 100.0f), Vector2(800.0f, 100.0f), Vector2(800.0f, 100.0f)},
                                                    4096u);
  DALI_TEST_EQUALS(prefix.size(), static_cast<size_t>(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(prefix.front().count, static_cast<size_t>(5u), TEST_LOCATION);
  const auto dimension = BuildRuntimeRevealBlurBatches(std::vector<Vector2>(4u, Vector2(100.0f, 80.0f)), 256u);
  DALI_TEST_EQUALS(dimension.size(), static_cast<size_t>(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(dimension.front().count, static_cast<size_t>(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(dimension.back().count, static_cast<size_t>(2u), TEST_LOCATION);
  const auto budget = BuildRuntimeRevealBlurBatches(std::vector<Vector2>(2u, Vector2(1024.0f, 600.0f)), 4096u);
  DALI_TEST_EQUALS(budget.size(), static_cast<size_t>(2u), TEST_LOCATION);
  // Above the single-page budget, retain equal-sized fallback pages so source
  // and H scratch remain shareable rather than growing an unequal last page.
  const auto pages = BuildRuntimeRevealBlurBatches(std::vector<Vector2>(12u, Vector2(1024.0f, 100.0f)), 4096u);
  DALI_TEST_EQUALS(pages.size(), static_cast<size_t>(3u), TEST_LOCATION);
  for(size_t index = 0u; index < pages.size(); ++index)
  {
    DALI_TEST_EQUALS(pages[index].first, index * 4u, TEST_LOCATION);
    DALI_TEST_EQUALS(pages[index].count, static_cast<size_t>(4u), TEST_LOCATION);
    DALI_TEST_EQUALS(pages[index].size, Vector2(1024.0f, 400.0f), TEST_LOCATION);
  }
  const auto waste = BuildRuntimeRevealBlurBatches({Vector2(800.0f, 100.0f), Vector2(80.0f, 100.0f)}, 4096u);
  DALI_TEST_EQUALS(waste.size(), static_cast<size_t>(2u), TEST_LOCATION);
  // A single large line is not rescaled or clipped to satisfy the page budget.
  const auto large = BuildRuntimeRevealBlurBatches({Vector2(2048.0f, 1024.0f)}, 4096u);
  DALI_TEST_EQUALS(large.front().size, Vector2(2048.0f, 1024.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianBalancedBatchesP(void)
{
  using Ui::Internal::BuildRuntimeRevealBlurBatches;
  const std::vector<Vector2> sizes(18u, Vector2(1024.0f, 100.0f));
  const auto                 batches = BuildRuntimeRevealBlurBatches(sizes, 4096u);
  DALI_TEST_EQUALS(batches.size(), static_cast<size_t>(3u), TEST_LOCATION);
  DALI_TEST_EQUALS(BatchStoragePixels(batches), BatchStoragePixels(FourLineBatches(sizes, 4096u)), TEST_LOCATION);
  for(size_t page = 0u; page < batches.size(); ++page)
  {
    DALI_TEST_EQUALS(batches[page].first, page * 6u, TEST_LOCATION);
    DALI_TEST_EQUALS(batches[page].count, static_cast<size_t>(6u), TEST_LOCATION);
    DALI_TEST_EQUALS(batches[page].size, Vector2(1024.0f, 600.0f), TEST_LOCATION);
  }
  // The old unequal tail needs another pair of scratch targets. Equal pages
  // can reduce storage even when they do not reduce the number of tasks.
  for(size_t count : {9u, 10u})
  {
    const std::vector<Vector2> tail(count, Vector2(1024.0f, 120.0f));
    const auto                 balanced = BuildRuntimeRevealBlurBatches(tail, 4096u);
    DALI_TEST_EQUALS(balanced.size(), count == 9u ? static_cast<size_t>(3u) : static_cast<size_t>(2u), TEST_LOCATION);
    DALI_TEST_CHECK(BatchStoragePixels(balanced) < BatchStoragePixels(FourLineBatches(tail, 4096u)));
  }
  // Equal area alone does not permit source/H sharing: dimensions must match.
  DALI_TEST_EQUALS(BatchStoragePixels({{0u, 1u, Vector2(200.0f, 100.0f)}, {1u, 1u, Vector2(100.0f, 200.0f)}}),
                   static_cast<uint64_t>(120000u), TEST_LOCATION);

  // Deterministic varied line bounds exercise sparse prefixes/tails, unequal
  // heights, texture limits and large single targets without font dependencies.
  for(uint32_t maximum : {256u, 1024u, 4096u})
  {
    for(size_t count = 2u; count <= 96u; ++count)
    {
      for(uint32_t pattern = 0u; pattern < 5u; ++pattern)
      {
        std::vector<Vector2> input;
        Vector2              whole    = Vector2::ZERO;
        uint64_t             occupied = 0u;
        for(size_t line = 0u; line < count; ++line)
        {
          const float width  = static_cast<float>(std::min(maximum, pattern == 0u ? 1024u : pattern == 1u ? (line % 2u ? 800u : 80u)
                                                                                          : pattern == 2u ? (line == 0u ? 80u : 800u)
                                                                                          : pattern == 3u ? (line % 7u == 0u ? 2048u : 512u)
                                                                                                          : 400u));
          const float height = static_cast<float>(std::min(maximum, pattern == 3u ? 1024u : pattern == 4u ? 50u + static_cast<uint32_t>(line % 3u) * 30u
                                                                                                          : 100u));
          input.emplace_back(width, height);
          whole.x = std::max(whole.x, width);
          whole.y += height;
          occupied += static_cast<uint64_t>(width) * static_cast<uint64_t>(height);
        }
        const auto     actual = BuildRuntimeRevealBlurBatches(input, maximum);
        const auto     repeat = BuildRuntimeRevealBlurBatches(input, maximum);
        const uint64_t area   = static_cast<uint64_t>(whole.x) * static_cast<uint64_t>(whole.y);
        if(whole.y <= static_cast<float>(maximum) && area <= 1024u * 1024u && area * 4u <= occupied * 5u)
        {
          DALI_TEST_EQUALS(actual.size(), static_cast<size_t>(1u), TEST_LOCATION);
        }
        else
        {
          const auto reference = FourLineBatches(input, maximum);
          DALI_TEST_CHECK(actual.size() <= reference.size());
          DALI_TEST_CHECK(BatchStoragePixels(actual) <= BatchStoragePixels(reference));
        }
        DALI_TEST_EQUALS(actual.size(), repeat.size(), TEST_LOCATION);
        size_t first = 0u;
        for(size_t page = 0u; page < actual.size(); ++page)
        {
          const auto& batch = actual[page];
          DALI_TEST_EQUALS(batch.first, first, TEST_LOCATION);
          DALI_TEST_CHECK(batch.count > 0u && batch.count <= input.size() - first);
          DALI_TEST_EQUALS(batch.count, repeat[page].count, TEST_LOCATION);
          DALI_TEST_EQUALS(batch.size, repeat[page].size, TEST_LOCATION);
          Vector2  bounds = Vector2::ZERO;
          uint64_t used   = 0u;
          for(size_t line = first; line < first + batch.count; ++line)
          {
            bounds.x = std::max(bounds.x, input[line].x);
            bounds.y += input[line].y;
            used += static_cast<uint64_t>(input[line].x) * static_cast<uint64_t>(input[line].y);
          }
          DALI_TEST_CHECK(batch.size.x >= bounds.x && batch.size.y >= bounds.y);
          if(batch.count > 1u || batch.size != bounds)
          {
            const uint64_t pixels = static_cast<uint64_t>(batch.size.x) * static_cast<uint64_t>(batch.size.y);
            DALI_TEST_CHECK(batch.size.x <= static_cast<float>(maximum) && batch.size.y <= static_cast<float>(maximum));
            DALI_TEST_CHECK(pixels <= 1024u * 1024u && pixels * 4u <= used * 5u);
          }
          first += batch.count;
        }
        DALI_TEST_EQUALS(first, input.size(), TEST_LOCATION);
      }
    }
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianNormalizedPagesP(void)
{
  using Ui::Internal::BuildRuntimeRevealBlurBatches;
  // Two equal allocated extents share source/H even though the last page has
  // one fewer line. The extra empty row costs less than a separate scratch pair.
  const auto normalized = BuildRuntimeRevealBlurBatches(std::vector<Vector2>(11u, Vector2(1024.0f, 100.0f)), 4096u);
  DALI_TEST_EQUALS(normalized.size(), static_cast<size_t>(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(normalized[0u].count, static_cast<size_t>(6u), TEST_LOCATION);
  DALI_TEST_EQUALS(normalized[1u].first, static_cast<size_t>(6u), TEST_LOCATION);
  DALI_TEST_EQUALS(normalized[1u].count, static_cast<size_t>(5u), TEST_LOCATION);
  for(const auto& page : normalized)
  {
    DALI_TEST_EQUALS(page.size, Vector2(1024.0f, 600.0f), TEST_LOCATION);
  }
  DALI_TEST_EQUALS(BatchStoragePixels(normalized), static_cast<uint64_t>(2457600u), TEST_LOCATION);
  DALI_TEST_CHECK(BatchStoragePixels(normalized) < static_cast<uint64_t>(2560000u));

  // Scratch sharing also permits width padding; sampled line bounds remain
  // unchanged. Reject that padding when it would exceed the 25% waste limit.
  const auto widths = BuildRuntimeRevealBlurBatches({Vector2(1024.0f, 600.0f), Vector2(900.0f, 600.0f)}, 4096u);
  DALI_TEST_EQUALS(widths.size(), static_cast<size_t>(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(widths[0u].size, Vector2(1024.0f, 600.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(widths[1u].size, widths[0u].size, TEST_LOCATION);
  const auto sparse = BuildRuntimeRevealBlurBatches({Vector2(1024.0f, 600.0f), Vector2(800.0f, 600.0f)}, 4096u);
  DALI_TEST_EQUALS(sparse[1u].size, Vector2(800.0f, 600.0f), TEST_LOCATION);

  // A small last page cannot be padded to the tallest page unconditionally.
  const auto tail = BuildRuntimeRevealBlurBatches(std::vector<Vector2>(7u, Vector2(1024.0f, 160.0f)), 4096u);
  DALI_TEST_EQUALS(tail.size(), static_cast<size_t>(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(tail[0u].size, Vector2(1024.0f, 640.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(tail[1u].size, Vector2(1024.0f, 480.0f), TEST_LOCATION);
  // Padding must not grow an already oversized single target.
  const auto large = BuildRuntimeRevealBlurBatches({Vector2(2048.0f, 1024.0f), Vector2(1800.0f, 1024.0f)}, 4096u);
  DALI_TEST_EQUALS(large[1u].size, Vector2(1800.0f, 1024.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianBatchFormatLifecycleP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetRequestedWidth(600.0f);
  label.SetRequestedHeight(440.0f);
  label.SetFontSize(18.0f);
  label.SetMultiLine(true);
  label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
  label.SetLineHeight(30.0f);
  const char* corpus =
    "A complete line\nA complete line\nA complete line\nA complete line\n"
    "A complete line\nA complete line\nA complete line\nA complete line\n"
    "A complete line\nA complete line\nA complete line\nA complete line";
  label.SetText(corpus);
  Text::Reveal reveal;
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.4f);
  const auto progressIndex = label.GetPropertyIndex("uTextRevealProgress");
  const auto tasks         = application.GetScene().GetRenderTaskList();
  Settle(application);
  DALI_TEST_EQUALS(BlurLines(label).size(), static_cast<size_t>(12u), TEST_LOCATION);
  DALI_TEST_EQUALS(Passes(label, "RevealGaussianBatchH").size(), static_cast<size_t>(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(Passes(label, "RevealGaussianBatchV").size(), static_cast<size_t>(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(Passes(label, "RevealGaussianOutput").size(), static_cast<size_t>(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
  auto checkFormat = [&](Pixel::Format format)
  {
    DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
    for(uint32_t index = 1u; index < tasks.GetTaskCount(); ++index)
    {
      const auto texture = tasks.GetTask(index).GetFrameBuffer().GetColorTexture();
      DALI_TEST_EQUALS(static_cast<int>(texture.GetPixelFormat()), static_cast<int>(format), TEST_LOCATION);
    }
    DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
  };
  checkFormat(Pixel::A8);

  // Each output follows the original renderer's current premultiplied color,
  // including a completely transparent endpoint. This is not an event snapshot.
  const auto original = Find(label, "RevealGaussianForeground").GetRendererAt(0u);
  for(Vector4 color : {Vector4(0.2f, 0.7f, 0.9f, 1.0f), Vector4(0.9f, 0.1f, 0.3f, 0.02f), Vector4::ZERO})
  {
    Animation animation = Animation::New(0.1f);
    animation.AnimateTo(Property(label, Label::Property::TEXT_COLOR), color);
    animation.Play();
    for(int frame = 0; frame < 9; ++frame)
    {
      application.SendNotification();
      application.Render(16);
      const auto expected = original.GetCurrentProperty<Vector4>(original.GetPropertyIndex("uTextColorAnimatable"));
      for(auto output : Passes(label, "RevealGaussianOutput"))
      {
        DALI_TEST_EQUALS(output.GetCurrentProperty<Vector4>(output.GetPropertyIndex("uTextColorAnimatable")), expected, 0.0001f, TEST_LOCATION);
      }
    }
    animation.Stop();
  }
  label.SetTextColor(UiColor(Color::WHITE));
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
  for(int iteration = 0; iteration < 2; ++iteration)
  {
    label.SetTextGradient(gradient);
    Settle(application);
    checkFormat(Pixel::RGBA8888);
    label.SetTextGradient(Gradient::Base::None());
    Settle(application);
    checkFormat(Pixel::A8);
    label.SetTextGradientOverlay(gradient);
    Settle(application);
    checkFormat(Pixel::RGBA8888);
    label.SetTextGradientOverlay(Gradient::Base::None());
    label.SetStyledText(Text::StyledText::FromMarkup("Plain <color value='red'>colored</color> text\nAnother line"));
    Settle(application);
    checkFormat(Pixel::RGBA8888);
    label.SetText(corpus);
    Settle(application);
    checkFormat(Pixel::A8);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
    application.GetScene().Remove(label);
    Settle(application);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
    application.GetScene().Add(label);
    Settle(application);
    checkFormat(Pixel::A8);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
  }
  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianWholeAlphaLifecycleP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  const auto        tasks = application.GetScene().GetRenderTaskList();
  Text::Reveal      reveal;
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  const Renderer original = label.GetRendererAt(0u);
  const Shader   ordinary = original.GetShader();
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.4f);
  Settle(application);
  DALI_TEST_EQUALS(Find(label, "RevealGaussianForeground").GetRendererAt(0u), original, TEST_LOCATION);
  DALI_TEST_CHECK(original.GetShader() != ordinary);
  const auto alphaShader = original.GetShader();
  auto       checkFormat = [&](Pixel::Format format)
  {
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
    for(uint32_t index = 1u; index < tasks.GetTaskCount(); ++index)
    {
      DALI_TEST_EQUALS(static_cast<int>(tasks.GetTask(index).GetFrameBuffer().GetColorTexture().GetPixelFormat()),
                       static_cast<int>(format), TEST_LOCATION);
    }
  };
  checkFormat(Pixel::A8);
  for(Vector4 color : {Vector4(0.2f, 0.7f, 0.9f, 1.0f), Vector4(0.9f, 0.1f, 0.3f, 0.02f), Vector4::ZERO})
  {
    Animation animation = Animation::New(0.1f);
    animation.AnimateTo(Property(label, Label::Property::TEXT_COLOR), color);
    animation.Play();
    for(int frame = 0; frame < 9; ++frame)
    {
      application.SendNotification();
      application.Render(16);
      const auto output = Find(label, "RevealGaussianOutput").GetRendererAt(0u);
      DALI_TEST_EQUALS(output.GetCurrentProperty<Vector4>(output.GetPropertyIndex("uTextColorAnimatable")),
                       original.GetCurrentProperty<Vector4>(original.GetPropertyIndex("uTextColorAnimatable")), 0.0001f, TEST_LOCATION);
    }
    animation.Stop();
  }
  // Even an externally retained renderer must regain its original shader.
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 0.0f);
  label.SetTextReveal(reveal);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetShader(), ordinary, TEST_LOCATION);
  label.SetTextColor(UiColor(Color::WHITE));
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  label.SetTextReveal(reveal);
  Settle(application);
  checkFormat(Pixel::A8);
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
  label.SetTextGradient(gradient);
  Settle(application);
  checkFormat(Pixel::RGBA8888);
  DALI_TEST_CHECK(Find(label, "RevealGaussianForeground").GetRendererAt(0u).GetShader() != alphaShader);
  label.SetTextGradient(Gradient::Base::None());
  Settle(application);
  checkFormat(Pixel::A8);
  const Renderer retained = Find(label, "RevealGaussianForeground").GetRendererAt(0u);
  application.GetScene().Remove(label);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(retained.GetShader() != alphaShader);
  application.GetScene().Add(label);
  Settle(application);
  checkFormat(Pixel::A8);
  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianWholeAlphaShaderOwnershipP(void)
{
  UiTestApplication application;
  Label             label     = MakeLabel(application);
  Renderer          original  = label.GetRendererAt(0u);
  const auto        ordinary  = original.GetShader();
  Actor             companion = Ui::Internal::CreateRuntimeRevealBlur(label, original, Vector2(360.0f, 72.0f),
                                                                      label.GetPropertyIndex("uTextRevealProgress"), 16u, 1.0f,
                                                                      {}, true);
  DALI_TEST_CHECK(original.GetShader() != ordinary);
  // Match async publication ordering: install the next shader before releasing
  // the previous companion. Its saved shader must not overwrite this result.
  const auto  program = ordinary.GetProperty(Shader::Property::PROGRAM);
  const auto* map     = program.GetMap();
  DALI_TEST_CHECK(map && map->Find("vertex") && map->Find("fragment"));
  Dali::String vertex;
  Dali::String fragment;
  DALI_TEST_CHECK(map->Find("vertex")->Get(vertex));
  DALI_TEST_CHECK(map->Find("fragment")->Get(fragment));
  Shader published = Shader::New(StringView(vertex), StringView(fragment));
  DALI_TEST_CHECK(published != ordinary);
  original.SetShader(published);
  Ui::Internal::RemoveRuntimeRevealBlur(companion, label);
  DALI_TEST_CHECK(!companion);
  DALI_TEST_EQUALS(original.GetShader(), published, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererAt(0u), original, TEST_LOCATION);
  Settle(application);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianForegroundPropertiesP(void)
{
  UiTestApplication application;
  Label             label    = MakeLabel(application);
  Renderer          original = label.GetRendererAt(0u);
  const auto        scalar   = original.RegisterProperty("cloneFloat", 0.0f);
  const auto        vector2  = original.RegisterProperty("cloneVector2", Vector2(1.0f, 2.0f));
  const auto        vector3  = original.RegisterProperty("cloneVector3", Vector3(1.0f, 2.0f, 3.0f));
  const auto        vector4  = original.RegisterProperty("cloneVector4", Vector4(1.0f, 2.0f, 3.0f, 4.0f));
  const auto        event    = original.RegisterProperty("cloneEvent", 0, Property::READ_WRITE);
  original.RegisterProperty("cloneReadOnly", 7, Property::READ_ONLY);
  uint32_t          created = 0u;
  ConnectionTracker tracker;
  auto              registry = application.GetCore().GetObjectRegistry();
  registry.ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    auto target = VisualRenderer::DownCast(object);
    if(!target)
    {
      return;
    }
    ++created;
    // A callback can add source properties and shift target custom indices.
    target.RegisterProperty("clonePrefix", static_cast<int>(created));
    original.SetProperty(scalar, static_cast<float>(created));
    if(created == 2u)
    {
      original.RegisterProperty("cloneLate", 0.5f);
    }
    target.PropertySetSignal().Connect(&tracker, [&, generation = created](Handle, Property::Index index, const Property::Value&)
    {
      if(index == Renderer::Property::BLEND_MODE)
      {
        // Event-only values can change type without changing property count.
        original.SetProperty(event, generation == 1u ? Property::Value(true) : Property::Value(static_cast<int>(generation)));
      }
    });
  });
  std::vector<Ui::Internal::RuntimeRevealBlurSequence> sequences(3u);
  for(uint32_t line = 0u; line < sequences.size(); ++line)
  {
    sequences[line].textures    = original.GetTextures();
    sequences[line].textureRect = Vector4(0.0f, static_cast<float>(line) / 3.0f, 1.0f, 1.0f / 3.0f);
    sequences[line].start       = static_cast<float>(line) * 0.2f;
  }
  Actor companion = Ui::Internal::CreateRuntimeRevealBlur(label, original, Vector2(360.0f, 72.0f),
                                                          label.GetPropertyIndex("uTextRevealProgress"), 16u, 0.5f,
                                                          sequences);
  tracker.DisconnectAll();
  const auto clones = Passes(companion, "RevealGaussianLineForeground");
  DALI_TEST_EQUALS(created, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(clones.size(), static_cast<size_t>(3u), TEST_LOCATION);
  for(size_t line = 0u; line < clones.size(); ++line)
  {
    auto clone = clones[line];
    DALI_TEST_EQUALS(clone.GetProperty<float>(clone.GetPropertyIndex("cloneFloat")), static_cast<float>(line + 1u), TEST_LOCATION);
    DALI_TEST_EQUALS(clone.GetProperty<int>(clone.GetPropertyIndex("clonePrefix")), static_cast<int>(line + 1u), TEST_LOCATION);
    DALI_TEST_EQUALS(clone.GetPropertyIndex("cloneReadOnly"), Property::INVALID_INDEX, TEST_LOCATION);
    DALI_TEST_EQUALS(clone.GetPropertyIndex("cloneLate") != Property::INVALID_INDEX, line > 0u, TEST_LOCATION);
    const auto copied = clone.GetProperty(clone.GetPropertyIndex("cloneEvent"));
    if(line == 0u)
    {
      DALI_TEST_EQUALS(copied.Get<bool>(), true, TEST_LOCATION);
    }
    else
    {
      DALI_TEST_EQUALS(copied.Get<int>(), static_cast<int>(line + 1u), TEST_LOCATION);
    }
  }
  Settle(application);
  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(original, scalar), 9.0f);
  animation.AnimateTo(Property(original, vector2), Vector2(4.0f, 5.0f));
  animation.AnimateTo(Property(original, vector3), Vector3(4.0f, 5.0f, 6.0f));
  animation.AnimateTo(Property(original, vector4), Vector4(4.0f, 5.0f, 6.0f, 7.0f));
  animation.Play();
  for(float progress : {0.2f, 0.8f, 0.4f})
  {
    animation.SetCurrentProgress(progress);
    Settle(application);
    for(auto clone : clones)
    {
      DALI_TEST_EQUALS(clone.GetCurrentProperty<float>(clone.GetPropertyIndex("cloneFloat")), original.GetCurrentProperty<float>(scalar), TEST_LOCATION);
      DALI_TEST_EQUALS(clone.GetCurrentProperty<Vector2>(clone.GetPropertyIndex("cloneVector2")), original.GetCurrentProperty<Vector2>(vector2), TEST_LOCATION);
      DALI_TEST_EQUALS(clone.GetCurrentProperty<Vector3>(clone.GetPropertyIndex("cloneVector3")), original.GetCurrentProperty<Vector3>(vector3), TEST_LOCATION);
      DALI_TEST_EQUALS(clone.GetCurrentProperty<Vector4>(clone.GetPropertyIndex("cloneVector4")), original.GetCurrentProperty<Vector4>(vector4), TEST_LOCATION);
    }
  }
  animation.Stop();
  Ui::Internal::RemoveRuntimeRevealBlur(companion, label);
  application.GetScene().Remove(label);
  Settle(application);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianSharedForegroundP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetRequestedWidth(600.0f);
  label.SetRequestedHeight(540.0f);
  label.SetFontSize(18.0f);
  label.SetMultiLine(true);
  label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
  label.SetLineHeight(30.0f);
  Text::Reveal reveal;
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
  const auto tasks = application.GetScene().GetRenderTaskList();
  for(uint32_t count : {6u, 12u})
  {
    std::string text("A complete line");
    for(uint32_t line = 1u; line < count; ++line)
    {
      text += "\nA complete line";
    }
    label.SetText(text.c_str());
    for(bool useGradient : {false, true})
    {
      label.SetTextGradient(useGradient ? gradient : Gradient::Base::None());
      for(float stagger : {0.0f, 0.25f})
      {
        reveal.SetSequenceStaggerRatio(stagger);
        label.SetTextReveal(reveal);
        Settle(application);
        const auto host = Find(label, "TextRevealRuntimeGaussian");
        DALI_TEST_CHECK(host);
        DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
        auto foreground = Find(host, "RevealGaussianLineForeground");
        DALI_TEST_CHECK(foreground);
        DALI_TEST_EQUALS(foreground.GetRendererCount(), 1u, TEST_LOCATION);
        DALI_TEST_EQUALS(foreground.GetParent(), Find(host, "RevealGaussianBatchSource"), TEST_LOCATION);
        DALI_TEST_EQUALS(Passes(host, "RevealGaussianBatchH").size(), static_cast<size_t>(1u), TEST_LOCATION);
        DALI_TEST_EQUALS(Passes(host, "RevealGaussianBatchV").size(), static_cast<size_t>(1u), TEST_LOCATION);
        DALI_TEST_CHECK(!Find(host, "RevealGaussianSource"));
        uint32_t actors = 0u, cameras = 0u, renderers = 0u;
        auto     visit = [&](auto&& self, Actor actor) -> void
        {
          ++actors;
          cameras += CameraActor::DownCast(actor) ? 1u : 0u;
          renderers += actor.GetRendererCount();
          DALI_TEST_CHECK(!View::DownCast(actor));
          for(uint32_t child = 0u; child < actor.GetChildCount(); ++child)
          {
            self(self, actor.GetChildAt(child));
          }
        };
        visit(visit, host);
        // Both qualities batch output without adding one actor per line.
        DALI_TEST_EQUALS(actors - cameras, 7u, TEST_LOCATION);
        DALI_TEST_EQUALS(cameras, 3u, TEST_LOCATION);
        DALI_TEST_EQUALS(renderers, 5u, TEST_LOCATION);
        DALI_TEST_EQUALS(BlurLines(host).size(), static_cast<size_t>(count), TEST_LOCATION);
        application.GetScene().Remove(label);
        Settle(application);
        DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
        application.GetScene().Add(label);
        Settle(application);
        DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
        DALI_TEST_EQUALS(Passes(label, "RevealGaussianLineForeground").size(), static_cast<size_t>(1u), TEST_LOCATION);
        label.SetTextReveal(Text::Reveal::None());
        Settle(application);
        DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
        DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
      }
    }
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianDrawBatchBoundaryP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetRequestedWidth(64.0f);
  label.SetRequestedHeight(900.0f);
  label.SetFontSize(8.0f);
  label.SetMultiLine(true);
  label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
  label.SetLineHeight(12.0f);
  const uint32_t limit = Ui::Internal::TextRevealBlurRenderer::MAX_LINES_PER_DRAW;
  std::string    text("A");
  for(uint32_t line = 0u; line < limit; ++line)
  {
    text += "\nA";
  }
  label.SetText(text.c_str());
  Text::Reveal reveal;
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 2.0f);
  label.SetTextReveal(reveal);
  Settle(application);
  const auto tasks = application.GetScene().GetRenderTaskList();
  // Crossing the uniform limit adds a draw, not another task or framebuffer.
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
  const auto horizontal = Passes(label, "RevealGaussianBatchH");
  const auto vertical   = Passes(label, "RevealGaussianBatchV");
  DALI_TEST_EQUALS(horizontal.size(), static_cast<size_t>(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(vertical.size(), horizontal.size(), TEST_LOCATION);
  DALI_TEST_EQUALS(Passes(label, "RevealGaussianLineForeground").size(), static_cast<size_t>(2u), TEST_LOCATION);
  DALI_TEST_EQUALS(Passes(label, "RevealGaussianOutput").size(), horizontal.size(), TEST_LOCATION);
  DALI_TEST_CHECK(horizontal[0].GetGeometry() != vertical[0].GetGeometry());
  DALI_TEST_CHECK(horizontal[1].GetGeometry() != vertical[1].GetGeometry());
  const auto lines         = BlurLines(label);
  const auto verticalLines = BlurLines(label, false);
  DALI_TEST_EQUALS(lines.size(), static_cast<size_t>(limit + 1u), TEST_LOCATION);
  DALI_TEST_EQUALS(verticalLines.size(), lines.size(), TEST_LOCATION);
  const float firstTailStart = lines[limit].Start();
  DALI_TEST_CHECK(firstTailStart > lines[limit - 1u].Start());
  for(float progress : {0.0f, firstTailStart - 0.001f, firstTailStart + 0.001f, 1.0f, 0.0f})
  {
    label.SetTextRevealProgress(progress);
    Settle(application);
    const auto outputStrengths = OutputStrengths(label);
    DALI_TEST_EQUALS(outputStrengths.size(), lines.size(), TEST_LOCATION);
    for(size_t line = 0u; line < lines.size(); ++line)
    {
      DALI_TEST_EQUALS(lines[line].Progress(), progress, 0.0001f, TEST_LOCATION);
      DALI_TEST_EQUALS(lines[line].Strength(), verticalLines[line].Strength(), 0.0001f, TEST_LOCATION);
      DALI_TEST_EQUALS(lines[line].Strength(), outputStrengths[line], 0.0001f, TEST_LOCATION);
      if(progress == 0.0f || progress == 1.0f)
      {
        DALI_TEST_EQUALS(lines[line].Strength(), 1.0f - progress, 0.0001f, TEST_LOCATION);
      }
    }
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
  }
  END_TEST;
}

namespace
{
PixelData SourceAtlasTestPlane(uint32_t width, uint32_t height, Pixel::Format format, uint32_t seed)
{
  const uint32_t bytes  = Pixel::GetBytesPerPixel(format);
  const uint32_t stride = (width + 3u) * bytes;
  auto*          buffer = new uint8_t[static_cast<size_t>(stride) * height];
  for(uint32_t y = 0u; y < height; ++y)
  {
    for(uint32_t x = 0u; x < stride; ++x)
    {
      buffer[static_cast<size_t>(y) * stride + x] = static_cast<uint8_t>((seed + y * 13u + x) % 256u);
    }
  }
  return PixelData::New(buffer, stride * height, width, height, stride, format, PixelData::DELETE_ARRAY);
}
} //namespace

int UtcDaliTextRevealSourceAtlasPackingP(void)
{
  UiTestApplication application;
  for(auto format : {Pixel::L8, Pixel::RGBA8888})
  {
    for(bool masked : {false, true})
    {
      Ui::Internal::PreparedRevealBlur prepared;
      prepared.options.perLine        = true;
      prepared.options.maxTextureSize = 16u;
      for(uint32_t i = 0u; i < 6u; ++i)
      {
        Ui::Internal::RevealBlurLineRaster line;
        line.sequence.lineIndex = i;
        line.foreground         = SourceAtlasTestPlane(5u + i % 3u, 4u, format, i * 19u);
        line.metadata           = SourceAtlasTestPlane(line.foreground.GetWidth(), 4u, Pixel::RGBA8888, i * 23u);
        if(masked)
        {
          line.mask = SourceAtlasTestPlane(line.foreground.GetWidth(), 4u, Pixel::L8, i * 29u);
        }
        prepared.lines.push_back(std::move(line));
      }
      const auto originals = prepared.lines;
      DALI_TEST_CHECK(Ui::Internal::PrepareRevealBlurSourceAtlases(prepared));
      // Two guarded entries fit per chunk. Atlas boundaries do not reorder lines.
      DALI_TEST_EQUALS(prepared.sourceAtlases.size(), static_cast<size_t>(3u), TEST_LOCATION);
      for(uint32_t i = 0u; i < prepared.lines.size(); ++i)
      {
        const auto& line = prepared.lines[i];
        DALI_TEST_EQUALS(line.sequence.lineIndex, i, TEST_LOCATION);
        DALI_TEST_EQUALS(line.atlasIndex, i / 2u, TEST_LOCATION);
        DALI_TEST_CHECK(!line.foreground && !line.metadata && !line.mask);
        const auto& atlas = prepared.sourceAtlases[line.atlasIndex];
        DALI_TEST_CHECK(atlas.foreground.GetWidth() <= 16u && atlas.foreground.GetHeight() <= 16u);
        for(uint32_t plane = 0u; plane < (masked ? 3u : 2u); ++plane)
        {
          const auto input  = plane == 0u ? originals[i].foreground : (plane == 1u ? originals[i].metadata : originals[i].mask);
          const auto output = plane == 0u ? atlas.foreground : (plane == 1u ? atlas.metadata : atlas.mask);
          const auto source = Dali::Integration::GetPixelDataBuffer(input);
          const auto target = Dali::Integration::GetPixelDataBuffer(output);
          const auto bytes  = Pixel::GetBytesPerPixel(input.GetPixelFormat());
          DALI_TEST_EQUALS(output.GetWidth(), atlas.foreground.GetWidth(), TEST_LOCATION);
          DALI_TEST_EQUALS(output.GetHeight(), atlas.foreground.GetHeight(), TEST_LOCATION);
          for(uint32_t y = 0u; y < input.GetHeight() + 2u; ++y)
          {
            for(uint32_t x = 0u; x < input.GetWidth() + 2u; ++x)
            {
              const auto  sx = std::clamp(x, 1u, input.GetWidth()) - 1u;
              const auto  sy = std::clamp(y, 1u, input.GetHeight()) - 1u;
              const auto* a  = source.buffer + static_cast<size_t>(sy) * input.GetStrideBytes() + sx * bytes;
              const auto* b  = target.buffer + static_cast<size_t>(line.atlasRectangle.y + y - 1u) * output.GetStrideBytes() + (line.atlasRectangle.x + x - 1u) * bytes;
              // Includes padded input stride and all four replicated corners.
              DALI_TEST_CHECK(std::memcmp(a, b, bytes) == 0);
            }
          }
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextRevealSourceAtlasGroupingP(void)
{
  UiTestApplication                application;
  Ui::Internal::PreparedRevealBlur prepared;
  prepared.options.perLine        = true;
  prepared.options.maxTextureSize = 64u;
  for(uint32_t i = 0u; i < 9u; ++i)
  {
    Ui::Internal::RevealBlurLineRaster line;
    line.sequence.lineIndex         = i;
    line.sequence.hasTextForeground = i != 4u;
    if(line.sequence.hasTextForeground)
    {
      const auto format = i < 2u ? Pixel::L8 : Pixel::RGBA8888;
      line.foreground   = SourceAtlasTestPlane(i == 7u ? 64u : 5u, 4u, format, i);
      line.metadata     = SourceAtlasTestPlane(line.foreground.GetWidth(), 4u, Pixel::RGBA8888, i);
      if(i == 5u || i == 6u)
      {
        line.mask = SourceAtlasTestPlane(5u, 4u, Pixel::L8, i);
      }
    }
    prepared.lines.push_back(std::move(line));
  }
  DALI_TEST_CHECK(Ui::Internal::PrepareRevealBlurSourceAtlases(prepared));
  DALI_TEST_EQUALS(prepared.sourceAtlases.size(), static_cast<size_t>(3u), TEST_LOCATION);
  for(uint32_t i : {0u, 1u, 2u, 3u, 5u, 6u})
  {
    DALI_TEST_CHECK(prepared.lines[i].atlasIndex != Ui::Internal::RevealBlurLineRaster::NO_ATLAS);
    DALI_TEST_CHECK(!prepared.lines[i].foreground);
  }
  // Image-only, no room for the guard, and a singleton keep their old path.
  for(uint32_t i : {4u, 7u, 8u})
  {
    DALI_TEST_EQUALS(prepared.lines[i].atlasIndex, Ui::Internal::RevealBlurLineRaster::NO_ATLAS, TEST_LOCATION);
  }
  DALI_TEST_CHECK(!prepared.lines[4u].foreground);
  DALI_TEST_CHECK(prepared.lines[7u].foreground && prepared.lines[8u].foreground);
  DALI_TEST_CHECK(prepared.sourceAtlases[0u].foreground.GetPixelFormat() == Pixel::L8);
  DALI_TEST_CHECK(prepared.sourceAtlases[1u].foreground.GetPixelFormat() == Pixel::RGBA8888);
  DALI_TEST_CHECK(!prepared.sourceAtlases[1u].mask && prepared.sourceAtlases[2u].mask);
  Ui::Internal::PreparedRevealBlur invalid;
  invalid.options = prepared.options;
  invalid.lines.resize(2u);
  for(auto& line : invalid.lines)
  {
    line.foreground = SourceAtlasTestPlane(5u, 4u, Pixel::L8, 0u);
    line.metadata   = SourceAtlasTestPlane(4u, 4u, Pixel::RGBA8888, 0u);
  }
  DALI_TEST_CHECK(!Ui::Internal::PrepareRevealBlurSourceAtlases(invalid));
  END_TEST;
}

int UtcDaliTextRevealSourceAtlasLifetimeP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetMultiLine(true);
  label.SetRequestedHeight(350.0f);
  label.SetFontSize(24.0f);
  label.SetText("One ordinary line\nA second line\nAnd a final line");
  const auto tasks = application.GetScene().GetRenderTaskList();
  for(auto mode : {Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE, Ui::Integration::Text::Reveal::BlurMode::QUALITY})
  {
    Text::Reveal reveal;
    reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
    Ui::Integration::Text::Reveal::SetBlurMode(reveal, mode);
    label.SetTextReveal(reveal);
    Settle(application);
    auto source = Passes(label, "RevealGaussianLineForeground");
    DALI_TEST_EQUALS(source.size(), static_cast<size_t>(1u), TEST_LOCATION);
    const auto program = source[0u].GetShader().GetProperty(Shader::Property::PROGRAM);
    DALI_TEST_CHECK(program.GetMap() && program.GetMap()->Find("hints"));
    DALI_TEST_EQUALS(program.GetMap()->Find("hints")->Get<Dali::String>(), Dali::String("MODIFIES_GEOMETRY"), TEST_LOCATION);
    std::vector<WeakHandleBase> retired;
    retired.emplace_back(source[0u]);
    auto geometry = source[0u].GetGeometry();
    retired.emplace_back(geometry);
    geometry.Reset();
    auto textures = source[0u].GetTextures();
    for(uint32_t slot = 0u; slot < textures.GetTextureCount(); ++slot)
    {
      auto texture = textures.GetTexture(slot);
      retired.emplace_back(texture);
    }
    textures.Reset();
    source.clear();
    label.SetTextReveal(Text::Reveal::None());
    Settle(application);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
    for(auto& weak : retired)
    {
      DALI_TEST_CHECK(!weak.GetBaseHandle());
    }
  }
  END_TEST;
}

int UtcDaliTextRevealSourceAtlasImageBoundaryP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  Texture    image   = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  ImageUrl   url     = ImageUrl::New(image, true);
  auto       builder = Text::StyledTextBuilder::New("First\nMiddle ");
  const auto start   = builder.GetUtf32Length();
  builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
  DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(url.GetUrl(), Vector2(24.0f, 24.0f))), start, start + 1u));
  builder.AppendText("\nLast");
  Label label = Label::New();
  label.SetLayoutMode(LayoutMode::STANDALONE);
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(240.0f);
  label.SetFontSize(16.0f);
  label.SetMultiLine(true);
  label.SetStyledText(builder.Build());
  Text::Reveal reveal;
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
  label.SetTextReveal(reveal);
  application.GetScene().Add(label);
  Settle(application);
  const auto            sources = Passes(label, "RevealGaussianLineForeground");
  std::vector<Renderer> textSources;
  uint32_t              imageDraws = 0u;
  for(auto renderer : sources)
  {
    DALI_TEST_CHECK(renderer.GetTextures().GetTextureCount() <= 8u);
    if(renderer.GetTextures().GetTexture(0u) == image)
    {
      ++imageDraws;
    }
    else
    {
      textSources.push_back(renderer);
    }
  }
  DALI_TEST_EQUALS(imageDraws, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(textSources.size(), static_cast<size_t>(3u), TEST_LOCATION);
  // First and Last share prepared storage, but must not be merged across the
  // middle image-bearing line merely because their A8 page is shared.
  DALI_TEST_CHECK(textSources[0u].GetTextures() == textSources[1u].GetTextures());
  DALI_TEST_CHECK(textSources[0u].GetGeometry() != textSources[1u].GetGeometry());
  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianAlphaBackendFallbackP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().mShaderLanguageVersion = 100u;
  Label label                                           = MakeLabel(application);
  label.SetMultiLine(true);
  label.SetRequestedHeight(180.0f);
  label.SetText("One complete line\nOne complete line");
  Text::Reveal reveal;
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  for(auto sequence : {Text::Reveal::Sequence::WHOLE_TEXT, Text::Reveal::Sequence::PER_LINE})
  {
    reveal.SetSequence(sequence);
    label.SetTextReveal(reveal);
    Settle(application);
    DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
    auto tasks = application.GetScene().GetRenderTaskList();
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
    for(uint32_t index = 1u; index < tasks.GetTaskCount(); ++index)
    {
      DALI_TEST_EQUALS(static_cast<int>(tasks.GetTask(index).GetFrameBuffer().GetColorTexture().GetPixelFormat()),
                       static_cast<int>(Pixel::RGBA8888), TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianResourceLifecycleP(void)
{
  UiTestApplication application;
  for(const Vector2 size : {Vector2(600.0f, 200.0f), Vector2(1920.0f, 1080.0f)})
  {
    for(int mode = 0; mode < 4; ++mode)
    {
      // Recreate each configuration to verify both allocation and teardown.
      for(int iteration = 0; iteration < 2; ++iteration)
      {
        Label label = Label::New("A bright morning in the city.\nTake a quiet walk by the river.\nFind a new cafe along the way.\nEnjoy the rest of your day.");
        label.SetLayoutMode(LayoutMode::STANDALONE);
        label.SetRequestedWidth(size.x);
        label.SetRequestedHeight(size.y);
        label.SetFontSize(24.0f);
        label.SetMultiLine(true);
        if(mode != 0)
        {
          Text::Reveal reveal;
          reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
          reveal.SetSequenceStaggerRatio(0.25f);
          Ui::Integration::Text::Reveal::SetBlurRadius(reveal, mode >= 2 ? 32.0f : 0.0f);
          Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, mode == 2 ? 0.0f : 0.5f);
          label.SetTextReveal(reveal);
          label.SetTextRevealProgress(0.4f);
        }
        application.GetScene().Add(label);
        Settle(application);
        const auto tasks = application.GetScene().GetRenderTaskList();
        if(mode < 3)
        {
          DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
          DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
        }
        else
        {
          DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
        }
        application.GetScene().Remove(label);
        label.Reset();
        Settle(application);
        DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
      }
    }
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianStoppedAdaptorP(void)
{
  UiTestApplication application;
  Label             label      = MakeLabel(application);
  Renderer          foreground = label.GetRendererAt(0u);
  const auto        textures   = foreground.GetTextures();
  const auto        shader     = foreground.GetShader();
  const auto        tasks      = application.GetScene().GetRenderTaskList();
  const auto        taskCount  = tasks.GetTaskCount();

  // A retained actor can still be connected after Adaptor::Stop. No new
  // offscreen resources may be published into that retired scene.
  application.GetAdaptor().Stop();
  DALI_TEST_CHECK(!Adaptor::IsAvailable());
  Actor companion = Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f),
                                                          label.GetPropertyIndex("uTextRevealProgress"), 16u, 0.5f);
  DALI_TEST_CHECK(!companion);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), taskCount, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u) == foreground);
  DALI_TEST_CHECK(foreground.GetTextures() == textures);
  DALI_TEST_CHECK(foreground.GetShader() == shader);
  label.Unparent();
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianActivationCancelledP(void)
{
  UiTestApplication application;
  Label label = MakeLabel(application);
  Renderer foreground = label.GetRendererAt(0u);
  const auto textures = foreground.GetTextures();
  const auto shader = foreground.GetShader();
  View owner = View::New();
  owner.AddRenderer(foreground);
  application.GetScene().Add(owner);
  const auto progress = owner.RegisterProperty("testProgress", 1.0f);
  ConnectionTracker tracker;
  bool disconnected = false;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(!disconnected && Actor::DownCast(object))
    {
      disconnected = true;
      owner.Unparent();
    }
  });
  Actor companion = Ui::Internal::CreateRuntimeRevealBlur(owner, foreground, Vector2(360.0f, 72.0f), progress, 16u, 0.5f);
  tracker.DisconnectAll();
  DALI_TEST_CHECK(disconnected && !companion);
  DALI_TEST_EQUALS(owner.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(owner.GetRendererAt(0u) == foreground);
  DALI_TEST_CHECK(foreground.GetTextures() == textures);
  DALI_TEST_CHECK(foreground.GetShader() == shader);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianActivationOnceP(void)
{
  UiTestApplication application;
  Label             label      = MakeLabel(application);
  Renderer          foreground = label.GetRendererAt(0u);
  const auto        shader     = foreground.GetShader();
  const auto        textures   = foreground.GetTextures();
  Actor             companion  = Ui::Internal::PrepareRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f),
                                                                        label.GetPropertyIndex("uTextRevealProgress"), 16u, 0.5f, {}, false, {}, {});
  DALI_TEST_CHECK(companion);
  DALI_TEST_CHECK(Ui::Internal::ActivateRuntimeRevealBlur(companion, label));
  DALI_TEST_CHECK(!Ui::Internal::ActivateRuntimeRevealBlur(companion, label));
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 4u, TEST_LOCATION);
  Actor retired = companion;
  Ui::Internal::RemoveRuntimeRevealBlur(companion, label);
  DALI_TEST_CHECK(!Ui::Internal::ActivateRuntimeRevealBlur(retired, label));
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u) == foreground);
  DALI_TEST_CHECK(foreground.GetShader() == shader && foreground.GetTextures() == textures);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianFactoryValidationP(void)
{
  UiTestApplication application;
  Label             label      = MakeLabel(application);
  auto              foreground = label.GetRendererAt(0u);
  const auto        progress   = label.GetPropertyIndex("uTextRevealProgress");
  const float       nan        = std::numeric_limits<float>::quiet_NaN();
  const float       infinity   = std::numeric_limits<float>::infinity();
  for(const auto size : {Vector2::ZERO, Vector2(-1.0f, 72.0f), Vector2(nan, 72.0f), Vector2(360.0f, infinity)})
  {
    DALI_TEST_CHECK(!Ui::Internal::CreateRuntimeRevealBlur(label, foreground, size, progress, 16u, 0.5f));
  }
  for(float duration : {0.0f, -1.0f, nan, infinity})
  {
    DALI_TEST_CHECK(!Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f), progress, 16u, duration));
  }
  for(uint32_t radius : {0u, 1u, 201u, std::numeric_limits<uint32_t>::max()})
  {
    DALI_TEST_CHECK(!Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f), progress, radius, 0.5f));
  }
  DALI_TEST_CHECK(!Ui::Internal::CreateRuntimeRevealBlur({}, foreground, Vector2(360.0f, 72.0f), progress, 16u, 0.5f));
  DALI_TEST_CHECK(!Ui::Internal::CreateRuntimeRevealBlur(label, {}, Vector2(360.0f, 72.0f), progress, 16u, 0.5f));
  DALI_TEST_CHECK(!Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f), Property::INVALID_INDEX, 16u, 0.5f));
  for(uint32_t malformed = 0u; malformed < 3u; ++malformed)
  {
    Ui::Internal::RuntimeRevealBlurSequence sequence;
    sequence.textures = foreground.GetTextures();
    if(malformed == 0u) sequence.start = nan;
    if(malformed == 1u) sequence.textureRect.z = 0.0f;
    if(malformed == 2u) sequence.textures.Reset();
    DALI_TEST_CHECK(!Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f), progress, 16u, 0.5f, {sequence}));
  }
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u) == foreground);
  label.Unparent();
  DALI_TEST_CHECK(!Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f), progress, 16u, 0.5f));
  END_TEST;
}

namespace
{
int CheckRuntimeConstructionShutdown(uint32_t trigger, uint32_t kind = 0u)
{
  UiTestApplication application;
  Label             label                = MakeLabel(application);
  Renderer          foreground           = label.GetRendererAt(0u);
  const auto        shader               = foreground.GetShader();
  const auto        textures             = foreground.GetTextures();
  const auto        tasks                = application.GetScene().GetRenderTaskList();
  bool              stopped              = false;
  uint32_t          allocationsAfterStop = 0u;
  uint32_t          actors               = 0u;
  ConnectionTracker tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(stopped)
    {
      ++allocationsAfterStop;
    }
    else if(kind == 3u && Renderer::DownCast(object) && ++actors == trigger)
    {
      Renderer::DownCast(object).PropertySetSignal().Connect(&tracker, [&](Handle, Property::Index, const Property::Value&)
      {
        if(!stopped)
        {
          application.GetAdaptor().Stop();
          stopped = true;
        }
      });
    }
    else if(kind != 3u && (kind == 0u ? static_cast<bool>(Actor::DownCast(object)) : kind == 1u ? static_cast<bool>(Shader::DownCast(object))
                                                                                                : static_cast<bool>(Renderer::DownCast(object))) &&
            ++actors == trigger)
    {
      application.GetAdaptor().Stop();
      stopped = true;
    }
  });
  Actor companion = Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f),
                                                          label.GetPropertyIndex("uTextRevealProgress"), 16u, 0.5f);
  tracker.DisconnectAll();
  DALI_TEST_CHECK(stopped && !companion);
  DALI_TEST_EQUALS(allocationsAfterStop, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(foreground.GetShader() == shader && foreground.GetTextures() == textures);
  label.Unparent();
  END_TEST;
}
} // unnamed namespace

int UtcDaliTextRevealRuntimeGaussianConstructionShutdownP(void)
{
  return CheckRuntimeConstructionShutdown(1u);
}

int UtcDaliTextRevealRuntimeGaussianPassConstructionShutdownP(void)
{
  return CheckRuntimeConstructionShutdown(4u);
}

int UtcDaliTextRevealRuntimeGaussianTaskConstructionShutdownP(void)
{
  return CheckRuntimeConstructionShutdown(7u);
}

int UtcDaliTextRevealRuntimeGaussianShaderConstructionShutdownP(void)
{
  return CheckRuntimeConstructionShutdown(1u, 1u);
}

int UtcDaliTextRevealRuntimeGaussianRendererConstructionShutdownP(void)
{
  return CheckRuntimeConstructionShutdown(1u, 2u);
}

int UtcDaliTextRevealRuntimeGaussianPropertyConstructionShutdownP(void)
{
  return CheckRuntimeConstructionShutdown(1u, 3u);
}

int UtcDaliTextRevealRuntimeGaussianOutputConstructionShutdownP(void)
{
  return CheckRuntimeConstructionShutdown(3u, 3u);
}

int UtcDaliTextRevealRuntimeGaussianDeferredStyleReentryP(void)
{
  UiTestApplication application;
  Gradient::Linear  gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
  for(uint32_t mutation = 0u; mutation < 6u; ++mutation)
  {
    Label             label = MakeLabel(application);
    WeakHandle<Actor> candidate;
    bool              changed = false;
    ConnectionTracker tracker;
    application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
    {
      if(!changed && Actor::DownCast(object))
      {
        auto actor = Actor::DownCast(object);
        candidate  = WeakHandle<Actor>(actor);
        changed    = true;
        switch(mutation)
        {
          case 0u:
            label.SetTextGradient(gradient);
            break;
          case 1u:
            label.SetTextGradientOverlay(gradient);
            break;
          case 2u:
            label.SetTextUnderline(Text::Underline());
            break;
          case 3u:
            label.SetTextShadow(Text::Shadow());
            break;
          case 4u:
            label.SetPadding(8.0f, 4.0f, 2.0f, 6.0f);
            break;
          case 5u:
            label.SetRenderScale(1.5f);
            break;
        }
      }
    });
    auto reveal = label.GetTextReveal();
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
    label.SetTextReveal(reveal);
    tracker.DisconnectAll();
    DALI_TEST_CHECK(changed);
    DALI_TEST_CHECK(!candidate.GetHandle());
    Settle(application);
    DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
    label.Unparent();
    label.Reset();
    Settle(application);
    DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianDeferredSourceReentryP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  WeakHandle<Actor> candidate;
  bool              changed = false;
  ConnectionTracker tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(!changed && Actor::DownCast(object))
    {
      auto actor = Actor::DownCast(object);
      candidate  = WeakHandle<Actor>(actor);
      changed    = true;
      label.SetText("A new source queued for the next relayout");
    }
  });
  auto reveal = label.GetTextReveal();
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
  label.SetTextReveal(reveal);
  tracker.DisconnectAll();
  DALI_TEST_CHECK(changed);
  // Do not hide stale publication behind the next layout's convergence.
  DALI_TEST_CHECK(!candidate.GetHandle());
  Settle(application);
  DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
  END_TEST;
}

int UtcDaliTextRevealImageTimingReentrantSourceP(void)
{
  UiTestApplication application;
  Label             label   = MakeLabel(application);
  Texture           image   = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  auto              url     = Ui::ImageUrl::New(image, true);
  auto              builder = Text::StyledTextBuilder::New("A \xef\xbf\xbc B");
  DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(url.GetUrl(), Vector2(24.0f, 18.0f))), 2u, 3u));
  label.SetStyledText(builder.Build());
  Settle(application);
  auto* data = Ui::Internal::Text::GetInlineReplacementData(label);
  DALI_TEST_CHECK(data);
  using Accessor = Ui::Internal::Text::InlineReplacementManagerTestAccessor;
  auto renderer  = Accessor::GetEntryVisual(data->manager, 1u).GetRenderer();
  DALI_TEST_CHECK(renderer);
  Text::ReplacementRevealTiming timing;
  DALI_TEST_CHECK(Accessor::GetRevealTiming(data->manager, 1u, timing));
  timing.fadeDuration = 0.75f;
  Vector<Text::ReplacementRevealTiming> timings;
  timings.PushBack(timing);
  bool              changed = false;
  ConnectionTracker tracker;
  renderer.PropertySetSignal().Connect(&tracker, [&](Handle, Property::Index, const Property::Value&)
  {
    if(!changed)
    {
      changed = true;
      label.SetText("The latest source has no replacement");
    }
  });
  data->manager.ApplyRevealTimings(timings, Accessor::GetEntrySourceRevision(data->manager), label.GetPropertyIndex("uTextRevealProgress"));
  tracker.DisconnectAll();
  DALI_TEST_CHECK(changed);
  DALI_TEST_CHECK(!Ui::Internal::Text::GetInlineReplacementData(label));
  Settle(application);
  DALI_TEST_EQUALS(label.GetText(), Dali::String("The latest source has no replacement"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianShutdownMutationP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  Label label = MakeLabel(application);
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
  label.SetTextGradientOverlay(gradient);
  auto reveal = label.GetTextReveal();
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  label.SetTextReveal(reveal);
  Settle(application);
  DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
  Actor          companion = Find(label, "TextRevealRuntimeGaussian");
  WeakHandleBase retired(companion);
  companion.Reset();
  const auto tasks = application.GetScene().GetRenderTaskList();

  application.GetAdaptor().Stop();
  // Reproduce the sample destructor's setter, without an app-side None or
  // disconnect beforehand. Cleanup must not recreate blur after shutdown.
  label.SetTextGradientOverlay(Gradient::Base::None());
  DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
  DALI_TEST_CHECK(!retired.GetBaseHandle());
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
  label.SetTextReveal(reveal);
  label.SetTextReveal(Text::Reveal::None());
  label.Unparent();
  label.Reset();
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianPublicationCancelledP(void)
{
  UiTestApplication application;
  Label label = MakeLabel(application);
  Renderer foreground = label.GetRendererAt(0u);
  const auto fade = foreground.GetPropertyIndex("uTextRevealFadeDuration");
  DALI_TEST_CHECK(fade != Property::INVALID_INDEX);
  bool cancelled = false;
  ConnectionTracker tracker;
  foreground.PropertySetSignal().Connect(&tracker, [&](Handle, Property::Index index, const Property::Value&)
  {
    if(!cancelled && index == fade && Find(label, "TextRevealRuntimeGaussian"))
    {
      cancelled = true;
      label.SetTextReveal(Text::Reveal::None());
    }
  });
  auto reveal = label.GetTextReveal();
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
  label.SetTextReveal(reveal);
  tracker.DisconnectAll();
  Settle(application);
  DALI_TEST_CHECK(cancelled);
  DALI_TEST_CHECK(label.GetTextReveal() == Text::Reveal::None());
  DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
  END_TEST;
}

namespace
{
int CheckRuntimePublicationReentry(UiTestApplication& application, uint32_t mutation, uint32_t trigger)
{
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  Label label = MakeLabel(application);
  auto reveal = label.GetTextReveal();
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
  bool              fired         = false;
  bool              asyncFinished = false;
  uint32_t          actorsCreated = 0u;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&asyncFinished](View, float, float)
  {
    asyncFinished = true;
  });
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    // Actors are only created by the runtime factory, not by the preceding
    // CPU preparation or metadata texture upload.
    if(!fired && Actor::DownCast(object) && ++actorsCreated == trigger)
    {
      fired = true;
      std::printf("Reentry at runtime actor creation %u, mutation %u\n", trigger, mutation);
      switch(mutation)
      {
        case 0u:
        {
          label.SetTextReveal(Text::Reveal::None());
          break;
        }
        case 1u:
        {
          auto replacement = reveal;
          Ui::Integration::Text::Reveal::SetBlurRadius(replacement, 48.0f);
          label.SetTextReveal(replacement);
          break;
        }
        case 2u:
        {
          label.SetText("Only the latest source must survive");
          break;
        }
        case 3u:
        {
          label.SetAsyncRendering(true);
          break;
        }
        case 4u:
        {
          label.Unparent();
          break;
        }
      }
    }
  });
  label.SetTextReveal(reveal);
  Settle(application);
  DALI_TEST_CHECK(fired);
  if(mutation == 3u)
  {
    for(uint32_t completion = 0u; completion < 8u && !asyncFinished; ++completion)
    {
      DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
      Settle(application);
    }
    DALI_TEST_CHECK(asyncFinished);
    DALI_TEST_CHECK(label.IsAsyncRendering());
  }
  tracker.DisconnectAll();
  Settle(application);
  if(mutation == 0u || mutation == 4u)
  {
    DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
    DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetRendererCount(), mutation == 0u ? 1u : 0u, TEST_LOCATION);
    if(mutation == 0u)
    {
      DALI_TEST_CHECK(label.GetTextReveal() == Text::Reveal::None());
    }
    else
    {
      application.GetScene().Add(label);
      Settle(application);
      DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
      DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 4u, TEST_LOCATION);
      DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
    }
  }
  else
  {
    DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
    DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 4u, TEST_LOCATION);
    if(mutation == 1u)
    {
      DALI_TEST_EQUALS(Ui::Integration::Text::Reveal::GetBlurRadius(label.GetTextReveal()), 48.0f, 0.001f, TEST_LOCATION);
      const auto shader = Find(label, "RevealGaussianH").GetRendererAt(0u).GetShader();
      // The surviving publication must use the new radius.
      DALI_TEST_EQUALS(shader, Ui::Internal::TextRevealBlurRenderer::Create(48u).GetShader(), TEST_LOCATION);
    }
    if(mutation == 2u)
    {
      DALI_TEST_EQUALS(label.GetText(), Dali::String("Only the latest source must survive"), TEST_LOCATION);
    }
  }
  label.SetTextReveal(Text::Reveal::None());
  label.Unparent();
  label.Reset();
  Settle(application);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int CheckRuntimePublicationReentry(uint32_t mutation)
{
  UiTestApplication application;
  for(uint32_t trigger : {1u, 4u, 6u})
  {
    if(CheckRuntimePublicationReentry(application, mutation, trigger))
    {
      return 1;
    }
  }
  END_TEST;
}
} // unnamed namespace

int UtcDaliTextRevealRuntimeGaussianReentryNoneP(void)
{
  return CheckRuntimePublicationReentry(0u);
}

int UtcDaliTextRevealRuntimeGaussianReentryConfigP(void)
{
  return CheckRuntimePublicationReentry(1u);
}

int UtcDaliTextRevealRuntimeGaussianReentryTextP(void)
{
  return CheckRuntimePublicationReentry(2u);
}

int UtcDaliTextRevealRuntimeGaussianReentryAsyncP(void)
{
  return CheckRuntimePublicationReentry(3u);
}

int UtcDaliTextRevealRuntimeGaussianReentryDisconnectP(void)
{
  return CheckRuntimePublicationReentry(4u);
}

int UtcDaliTextRevealGaussianKernelBoundsP(void)
{
  UiTestApplication application;
  using Gaussian = Ui::Internal::GaussianBlurAlgorithm;
  for(uint32_t radius : {0u, 1u, 201u, 202u, 0x80000000u, std::numeric_limits<uint32_t>::max()})
  {
    DALI_TEST_EQUALS(static_cast<bool>(Gaussian::GetShader(radius)), radius == 0u, TEST_LOCATION);
    DALI_TEST_CHECK(!Gaussian::CreateShader(radius, "", "", Shader::Hint::NONE, "InvalidKernel"));
    DALI_TEST_EQUALS(static_cast<bool>(Gaussian::CreateRenderer(radius)), radius == 0u, TEST_LOCATION);
    DALI_TEST_CHECK(!Ui::Internal::TextRevealBlurRenderer::Create(radius));
    DALI_TEST_CHECK(!Ui::Internal::TextRevealBlurRenderer::CreateBatch(radius, Geometry::New()));
  }
  for(uint32_t radius : {2u, 3u, 4u, 199u, 200u})
  {
    auto shader = Gaussian::GetShader(radius);
    DALI_TEST_CHECK(shader);
    DALI_TEST_CHECK(Gaussian::GetShader(radius) == shader);
    DALI_TEST_CHECK(Gaussian::CreateRenderer(radius));
    DALI_TEST_CHECK(Ui::Internal::TextRevealBlurRenderer::Create(radius));
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianCoverageP(void)
{
  UiTestApplication application;
  Rect<uint32_t>    coverage;
  PixelBuffer       luma = PixelBuffer::New(64u, 48u, Pixel::L8);
  std::fill(luma.GetBuffer(), luma.GetBuffer() + 64u * 48u, 0u);
  luma.GetBuffer()[10u * 64u + 12u] = 255u;
  luma.GetBuffer()[18u * 64u + 30u] = 1u;
  Ui::Internal::AccumulateRuntimeRevealBlurCoverage(PixelBuffer::Convert(luma), coverage);
  DALI_TEST_CHECK(coverage == Rect<uint32_t>(12u, 10u, 19u, 9u));
  PixelBuffer rgba = PixelBuffer::New(64u, 48u, Pixel::RGBA8888);
  std::fill(rgba.GetBuffer(), rgba.GetBuffer() + 64u * 48u * 4u, 0u);
  rgba.GetBuffer()[0u]                         = 255u; // RGB without alpha is not foreground coverage.
  rgba.GetBuffer()[(8u * 64u + 36u) * 4u + 3u] = 1u;
  Ui::Internal::AccumulateRuntimeRevealBlurCoverage(PixelBuffer::Convert(rgba), coverage);
  DALI_TEST_CHECK(coverage == Rect<uint32_t>(12u, 8u, 25u, 11u));
  for(auto format : {Pixel::L8, Pixel::RGBA8888})
  {
    const auto  bytes = Pixel::GetBytesPerPixel(format);
    PixelBuffer odd   = PixelBuffer::New(13u, 7u, format);
    std::fill(odd.GetBuffer(), odd.GetBuffer() + 13u * 7u * bytes, 0u);
    odd.GetBuffer()[13u * 7u * bytes - 1u] = 1u;
    Rect<uint32_t> edge;
    Ui::Internal::AccumulateRuntimeRevealBlurCoverage(PixelBuffer::Convert(odd), edge);
    DALI_TEST_CHECK(edge == Rect<uint32_t>(12u, 6u, 1u, 1u));
  }

  Label    label    = MakeLabel(application);
  Renderer renderer = label.GetRendererAt(0u);
  using P           = VisualRenderer::Property;
  renderer.SetProperty(P::TRANSFORM_OFFSET, Vector2(10.0f, 20.0f));
  renderer.SetProperty(P::TRANSFORM_SIZE, Vector2(64.0f, 48.0f));
  renderer.SetProperty(P::EXTRA_SIZE, Vector2::ZERO);
  renderer.SetProperty(P::TRANSFORM_ORIGIN, Vector2(-0.5f, -0.5f));
  renderer.SetProperty(P::TRANSFORM_PIVOT, Vector2(0.5f, 0.5f));
  renderer.SetProperty(P::TRANSFORM_OFFSET_SIZE_MODE, Vector4::ONE);
  const auto target = Ui::Internal::ResolveRuntimeRevealBlurTarget(renderer, Vector2(400.0f, 200.0f), Vector2(64.0f, 48.0f), coverage, 16u);
  // Source pixel coordinates map through the original visual offset, with
  // the same full-target origin and a 20px kernel/sampling guard.
  DALI_TEST_CHECK(target == Rect<int32_t>(20, 26, 65, 51));
  DALI_TEST_CHECK(Ui::Internal::ResolveRuntimeRevealBlurTarget(renderer, Vector2(400.0f, 200.0f), Vector2(64.0f, 48.0f), {}, 16u) ==
                  Rect<int32_t>(0, 0, 436, 236));
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianHorizontalCoverageP(void)
{
  UiTestApplication application;
  Label             label    = MakeLabel(application);
  Renderer          renderer = label.GetRendererAt(0u);
  using P                    = VisualRenderer::Property;
  renderer.SetProperty(P::EXTRA_SIZE, Vector2::ZERO);
  renderer.SetProperty(P::TRANSFORM_ORIGIN, Vector2(-0.5f, -0.5f));
  renderer.SetProperty(P::TRANSFORM_PIVOT, Vector2(0.5f, 0.5f));
  renderer.SetProperty(P::TRANSFORM_OFFSET_SIZE_MODE, Vector4::ONE);
  const Vector2        control(400.0f, 200.0f);
  const Vector2        texture(64.0f, 48.0f);
  const Rect<uint32_t> coverage(12u, 8u, 25u, 11u);
  for(float scale : {0.5f, 1.0f, 1.25f, 1.5f, 4.0f})
  {
    renderer.SetProperty(P::TRANSFORM_SIZE, texture * scale);
    renderer.SetProperty(P::TRANSFORM_OFFSET, Vector2(10.35f, 20.65f));
    Vector2    band;
    const auto target = Ui::Internal::ResolveRuntimeRevealBlurTarget(renderer, control, texture, coverage, 24u, &band);
    DALI_TEST_CHECK(target == Ui::Internal::ResolveRuntimeRevealBlurTarget(renderer, control, texture, coverage, 24u));
    // Include the half-source-texel LINEAR support at this scale, plus a
    // capture texel; test actual pixel cells, not a nominal font line box.
    const float top    = 26.0f + 20.65f + (8.0f - 0.5f) * scale;
    const float bottom = 26.0f + 20.65f + (19.0f + 0.5f) * scale;
    DALI_TEST_CHECK(band.x <= std::floor(top - 1.0f));
    DALI_TEST_CHECK(band.y >= std::ceil(bottom + 1.0f));
    DALI_TEST_CHECK(band.x >= static_cast<float>(target.y));
    DALI_TEST_CHECK(band.y <= static_cast<float>(target.y + target.height));
    DALI_TEST_CHECK(band.y - band.x < static_cast<float>(target.height));
  }
  Vector2 band;
  Ui::Internal::ResolveRuntimeRevealBlurTarget(renderer, control, texture, {}, 24u, &band);
  DALI_TEST_EQUALS(band, Vector2(0.0f, 252.0f), TEST_LOCATION);
  Ui::Internal::ResolveRuntimeRevealBlurTarget(renderer, control, texture, Rect<uint32_t>(60u, 8u, 20u, 11u), 24u, &band);
  DALI_TEST_EQUALS(band, Vector2(0.0f, 252.0f), TEST_LOCATION);
  renderer.SetProperty(P::TRANSFORM_OFFSET, Vector2(0.0f, std::numeric_limits<float>::quiet_NaN()));
  Ui::Internal::ResolveRuntimeRevealBlurTarget(renderer, control, texture, coverage, 24u, &band);
  DALI_TEST_EQUALS(band, Vector2(0.0f, 252.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianHorizontalGeometryP(void)
{
  UiTestApplication application;
  Label             label      = MakeLabel(application);
  Renderer          foreground = label.GetRendererAt(0u);
  using P                      = VisualRenderer::Property;
  foreground.SetProperty(P::TRANSFORM_SIZE, Vector2(100.0f, 100.0f));
  foreground.SetProperty(P::TRANSFORM_OFFSET, Vector2::ZERO);
  foreground.SetProperty(P::EXTRA_SIZE, Vector2::ZERO);
  foreground.SetProperty(P::TRANSFORM_ORIGIN, Vector2(-0.5f, -0.5f));
  foreground.SetProperty(P::TRANSFORM_PIVOT, Vector2(0.5f, 0.5f));
  foreground.SetProperty(P::TRANSFORM_OFFSET_SIZE_MODE, Vector4::ONE);
  TextureSet textures = TextureSet::New();
  textures.SetTexture(0u, Texture::New(TextureType::TEXTURE_2D, Pixel::L8, 100u, 100u));
  foreground.SetTextures(textures);
  std::vector<Ui::Internal::RuntimeRevealBlurSequence> sequences(2u);
  for(uint32_t line = 0u; line < 2u; ++line)
  {
    sequences[line].textures    = textures;
    sequences[line].textureRect = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    sequences[line].lineIndex   = line;
    sequences[line].start       = static_cast<float>(line) * 0.25f;
  }
  sequences[0u].coverage          = Rect<uint32_t>(20u, 30u, 20u, 10u);
  sequences[1u].hasTextForeground = false;
  auto images                     = std::make_unique<Ui::Internal::RuntimeRevealBlurImages>();
  for(uint32_t line = 0u; line < 2u; ++line)
  {
    Ui::Internal::RevealBlurImage image;
    image.lineIndex = line;
    // Extend both above and below the text; the second line is image-only.
    image.coverage = Rect<uint32_t>(25u, 15u, 15u, 50u);
    images->placements.push_back(image);
  }
  Actor companion = Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(100.0f, 100.0f),
                                                          label.GetPropertyIndex("uTextRevealProgress"), 24u, 0.5f, std::move(sequences), false, {}, std::move(images));
  DALI_TEST_CHECK(companion);
  Settle(application);
  auto horizontal = Passes(companion, "RevealGaussianBatchH");
  auto vertical   = Passes(companion, "RevealGaussianBatchV");
  DALI_TEST_EQUALS(horizontal.size(), static_cast<size_t>(1u), TEST_LOCATION);
  DALI_TEST_CHECK(horizontal[0u].GetGeometry() != vertical[0u].GetGeometry());
  const auto h = UploadedCaptureVertices(application, 2u, true);
  const auto v = UploadedCaptureVertices(application, 2u);
  DALI_TEST_EQUALS(h.size(), static_cast<size_t>(88u), TEST_LOCATION);
  DALI_TEST_EQUALS(v.size(), h.size(), TEST_LOCATION);
  for(size_t line = 0u; line < 2u; ++line)
  {
    const auto* a = h.data() + line * 44u;
    const auto* b = v.data() + line * 44u;
    // Full-target image coverage [41,91] with two filter rows becomes [39,93].
    const float height = b[23u] - b[1u];
    DALI_TEST_EQUALS((a[25u] - a[3u]) * height, 54.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_CHECK(a[3u] >= 0.0f && a[25u] <= 1.0f && a[3u] < a[25u]);
    DALI_TEST_CHECK(a[1u] >= b[1u] && a[23u] <= b[23u]);
    for(uint32_t corner = 0u; corner < 4u; ++corner)
    {
      // X, atlas rectangle, inverse size and line identity are unchanged.
      DALI_TEST_EQUALS(a[corner * 11u], b[corner * 11u], TEST_LOCATION);
      for(uint32_t field = 4u; field < 11u; ++field)
      {
        DALI_TEST_EQUALS(a[corner * 11u + field], b[corner * 11u + field], TEST_LOCATION);
      }
    }
  }
  const auto tasks = application.GetScene().GetRenderTaskList();
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 4u, TEST_LOCATION);
  for(uint32_t task = 1u; task < tasks.GetTaskCount(); ++task)
  {
    DALI_TEST_CHECK(tasks.GetTask(task).GetClearEnabled());
    DALI_TEST_EQUALS(tasks.GetTask(task).GetClearColor(), Color::TRANSPARENT, TEST_LOCATION);
  }
  auto           geometryHandle = horizontal[0u].GetGeometry();
  WeakHandleBase geometry(geometryHandle);
  geometryHandle.Reset();
  for(float progress : {0.4f, 1.0f, 0.2f, 0.0f})
  {
    label.SetTextRevealProgress(progress);
    Settle(application);
    DALI_TEST_CHECK(Passes(companion, "RevealGaussianBatchH")[0u].GetGeometry() == geometry.GetBaseHandle());
  }
  Ui::Internal::RemoveRuntimeRevealBlur(companion, label);
  horizontal.clear();
  vertical.clear();
  Settle(application);
  DALI_TEST_CHECK(!geometry.GetBaseHandle());
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  Ui::Internal::RuntimeRevealBlurSequence unknown;
  unknown.textures = textures;
  companion        = Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(100.0f, 100.0f),
                                                           label.GetPropertyIndex("uTextRevealProgress"), 24u, 0.5f, {unknown});
  DALI_TEST_CHECK(companion);
  Settle(application);
  // Unknown/empty coverage must preserve blur and share the original full quad.
  DALI_TEST_CHECK(Passes(companion, "RevealGaussianBatchH")[0u].GetGeometry() ==
                  Passes(companion, "RevealGaussianBatchV")[0u].GetGeometry());
  Ui::Internal::RemoveRuntimeRevealBlur(companion, label);
  Settle(application);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianHorizontalConstructionShutdownP(void)
{
  UiTestApplication                       application;
  Label                                   label      = MakeLabel(application);
  Renderer                                foreground = label.GetRendererAt(0u);
  Ui::Internal::RuntimeRevealBlurSequence sequence;
  sequence.textures             = foreground.GetTextures();
  sequence.coverage             = Rect<uint32_t>(1u, 2u, 5u, 4u);
  const auto        tasks       = application.GetScene().GetRenderTaskList();
  uint32_t          allocations = 0u;
  uint32_t          afterStop   = 0u;
  bool              stopped     = false;
  ConnectionTracker tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(stopped)
    {
      ++afterStop;
    }
    else if(Renderer::DownCast(object) && ++allocations == 2u)
    {
      // Clone, then H renderer: cancel after both geometries exist.
      application.GetAdaptor().Stop();
      stopped = true;
    }
  });
  Actor companion = Ui::Internal::CreateRuntimeRevealBlur(label, foreground, Vector2(360.0f, 72.0f),
                                                          label.GetPropertyIndex("uTextRevealProgress"), 24u, 0.5f, {sequence});
  tracker.DisconnectAll();
  DALI_TEST_CHECK(stopped && !companion);
  DALI_TEST_EQUALS(afterStop, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererAt(0u) == foreground);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianCroppedGeometryP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetRequestedWidth(600.0f);
  label.SetRequestedHeight(240.0f);
  label.SetPadding(Insets(11.0f, 13.0f, 7.0f, 17.0f));
  label.SetMultiLine(true);
  label.SetText("AVATAR ffi office gjpq\n안녕하세요 DALi UI\nArabic العربية mixed עברית 🌈");
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
  label.SetTextGradient(gradient);
  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::PIXEL);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 32.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
  label.SetTextReveal(reveal);
  for(auto horizontal : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END})
  {
    for(auto vertical : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END})
    {
      label.SetHorizontalTextAlignment(horizontal);
      label.SetVerticalTextAlignment(vertical);
      Settle(application);
      const Actor host = Find(label, "TextRevealRuntimeGaussian");
      DALI_TEST_CHECK(host);
      DALI_TEST_CHECK(!View::DownCast(host));
      const Renderer original    = Find(label, "RevealGaussianForeground").GetRendererAt(0u);
      const auto     foregrounds = Passes(label, "RevealGaussianLineForeground");
      DALI_TEST_EQUALS(foregrounds.size(), static_cast<size_t>(1u), TEST_LOCATION);
      const auto lineCount      = static_cast<uint32_t>(BlurLines(label).size());
      const auto sourceVertices = UploadedSourceVertices(application, lineCount);
      DALI_TEST_EQUALS(sourceVertices.size(), static_cast<size_t>(lineCount) * 48u, TEST_LOCATION);
      for(Renderer foreground : foregrounds)
      {
        DALI_TEST_CHECK(foreground.GetShader() != original.GetShader());
        DALI_TEST_CHECK(foreground.GetGeometry() != original.GetGeometry());
        const auto index = original.GetPropertyIndex("uTextGradientBounds");
        DALI_TEST_CHECK(index != Property::INVALID_INDEX);
        const auto bounds        = original.GetCurrentProperty<Vector4>(index);
        // Shared bounds stay in original coordinates. Each quad supplies its
        // own crop for the shader's existing local gradient calculation.
        DALI_TEST_EQUALS(foreground.GetCurrentProperty<Vector4>(foreground.GetPropertyIndex("uTextGradientBounds")), bounds, TEST_LOCATION);
        for(uint32_t line = 0u; line < lineCount; ++line)
        {
          const auto*   vertex = sourceVertices.data() + static_cast<size_t>(line) * 48u;
          const Vector4 rectangle(vertex[2u], vertex[3u], vertex[4u], vertex[5u]);
          DALI_TEST_CHECK(rectangle.z > 0.0f && rectangle.w > 0.0f);
          const Vector4 croppedBounds((bounds.x - rectangle.x) / rectangle.z, (bounds.y - rectangle.y) / rectangle.w,
                                      bounds.z / rectangle.z, bounds.w / rectangle.w);
          DALI_TEST_EQUALS(Vector4(croppedBounds.x * rectangle.z + rectangle.x, croppedBounds.y * rectangle.w + rectangle.y,
                                   croppedBounds.z * rectangle.z, croppedBounds.w * rectangle.w),
                           bounds, 0.0001f, TEST_LOCATION);
        }
        using P                 = VisualRenderer::Property;
        const auto originalSize = original.GetCurrentProperty<Vector2>(P::TRANSFORM_SIZE);
        DALI_TEST_EQUALS(foreground.GetCurrentProperty<Vector2>(P::TRANSFORM_SIZE), originalSize, TEST_LOCATION);
      }
      auto               tasks      = application.GetScene().GetRenderTaskList();
      uint64_t           actualArea = 0u;
      std::vector<Actor> outputs;
      for(uint32_t child = 0u; child < host.GetChildCount(); ++child)
      {
        auto actor = host.GetChildAt(child);
        if(actor.GetProperty<Dali::String>(Actor::Property::NAME) == "RevealGaussianOutput")
        {
          outputs.push_back(actor);
        }
      }
      size_t outputIndex = 0u;
      const bool lateSmooth  = Ui::Integration::Text::Reveal::GetBlurMode(label.GetTextReveal()) == Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE;
      for(uint32_t taskIndex = 1u; taskIndex < tasks.GetTaskCount(); ++taskIndex)
      {
        auto task    = tasks.GetTask(taskIndex);
        auto source  = task.GetSourceActor();
        auto texture = task.GetFrameBuffer().GetColorTexture();
        actualArea += static_cast<uint64_t>(texture.GetWidth()) * texture.GetHeight();
        // The camera and geometry retain full coordinates even when H/V render
        // into smaller targets. Only Source's pixel dimensions must match here.
        if(!lateSmooth || source.GetProperty<Dali::String>(Actor::Property::NAME) == "RevealGaussianBatchSource")
        {
          DALI_TEST_EQUALS(source.GetCurrentProperty<Vector3>(Actor::Property::SIZE).x, static_cast<float>(texture.GetWidth()), TEST_LOCATION);
          DALI_TEST_EQUALS(source.GetCurrentProperty<Vector3>(Actor::Property::SIZE).y, static_cast<float>(texture.GetHeight()), TEST_LOCATION);
        }
        Actor foreground = Find(source, "RevealGaussianLineForeground");
        if(foreground)
        {
          // Page placement now lives in renderer offsets. Compose it with
          // the actual blur quads and check the original output coordinates.
          // The common actor must retain the original shader size reference.
          DALI_TEST_EQUALS(foreground.GetCurrentProperty<Vector3>(Actor::Property::SIZE),
                           Find(label, "RevealGaussianForeground").GetCurrentProperty<Vector3>(Actor::Property::SIZE), TEST_LOCATION);
          DALI_TEST_EQUALS(foreground.GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3::ZERO, TEST_LOCATION);
          const auto capture = UploadedCaptureVertices(application, lineCount);
          DALI_TEST_EQUALS(capture.size(), static_cast<size_t>(lineCount) * 44u, TEST_LOCATION);
          std::vector<float> expectedVertices;
          for(uint32_t line = 0u; line < lineCount; ++line)
          {
            DALI_TEST_CHECK(outputIndex < outputs.size());
            const auto*   vertex = capture.data() + static_cast<size_t>(line) * 44u;
            const Vector2 center((vertex[0u] + vertex[33u]) * 0.5f, (vertex[1u] + vertex[34u]) * 0.5f);
            const Vector2 lineSize(vertex[11u] - vertex[0u], vertex[23u] - vertex[1u]);
            const Vector4 rectangle(vertex[4u], vertex[5u], vertex[6u], vertex[7u]);
            DALI_TEST_CHECK(lineSize.x > 0.0f && lineSize.y > 0.0f);
            DALI_TEST_CHECK(std::abs(center.x) + lineSize.x * 0.5f <= static_cast<float>(texture.GetWidth()) * 0.5f + 0.001f);
            DALI_TEST_CHECK(std::abs(center.y) + lineSize.y * 0.5f <= static_cast<float>(texture.GetHeight()) * 0.5f + 0.001f);
            const auto*   source = sourceVertices.data() + static_cast<size_t>(line) * 48u;
            const Vector2 captureOffset(source[6u], source[7u]);
            const Vector2 offset = center - captureOffset;
            for(Vector2 uv : {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f)})
            {
              const Vector2 position = offset + (uv - Vector2(0.5f, 0.5f)) * lineSize;
              expectedVertices.insert(expectedVertices.end(), {position.x, position.y, uv.x, uv.y,
                                                               rectangle.x, rectangle.y, rectangle.z, rectangle.w});
              if(lateSmooth)
              {
                expectedVertices.push_back(static_cast<float>(line));
              }
            }
          }
          DALI_TEST_CHECK(HasUploadedBatchVertices(application, expectedVertices));
          DALI_TEST_EQUALS(outputs[outputIndex++].GetCurrentProperty<Vector3>(Actor::Property::POSITION), Vector3::ZERO, 0.0001f, TEST_LOCATION);
        }
      }
      DALI_TEST_EQUALS(outputIndex, outputs.size(), TEST_LOCATION);
      DALI_TEST_CHECK(actualArea < static_cast<uint64_t>(lineCount) * 3u * 668u * 308u);
    }
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianPlaneCropP(void)
{
  UiTestApplication application;
  for(auto format : {Pixel::L8, Pixel::RGBA8888})
  {
    const auto bytes = Pixel::GetBytesPerPixel(format);
    // Non-tight stride exercises row copies, including arbitrary metadata bytes.
    const uint32_t length = 17u * 9u * bytes;
    auto*          buffer = new uint8_t[length];
    for(uint32_t index = 0u; index < length; ++index)
    {
      buffer[index] = static_cast<uint8_t>(index % 251u);
    }
    PixelData            source = PixelData::New(buffer, length, 13u, 9u, 17u * bytes, format, PixelData::DELETE_ARRAY);
    const Rect<uint32_t> rectangle(3u, 2u, 7u, 5u);
    PixelData            cropped = Ui::Internal::CropRuntimeRevealBlurPixels(source, rectangle);
    DALI_TEST_CHECK(cropped);
    DALI_TEST_EQUALS(cropped.GetWidth(), rectangle.width, TEST_LOCATION);
    DALI_TEST_EQUALS(cropped.GetHeight(), rectangle.height, TEST_LOCATION);
    const auto data = Dali::Integration::GetPixelDataBuffer(cropped);
    for(uint32_t row = 0u; row < rectangle.height; ++row)
    {
      for(uint32_t column = 0u; column < rectangle.width * bytes; ++column)
      {
        DALI_TEST_EQUALS(data.buffer[row * cropped.GetStrideBytes() + column],
                         buffer[(row + rectangle.y) * 17u * bytes + rectangle.x * bytes + column], TEST_LOCATION);
      }
    }
    DALI_TEST_CHECK(Ui::Internal::CropRuntimeRevealBlurPixels(source, Rect<uint32_t>(0u, 0u, 13u, 9u)) == source);
    DALI_TEST_CHECK(!Ui::Internal::CropRuntimeRevealBlurPixels(source, Rect<uint32_t>(12u, 0u, 2u, 1u)));
    DALI_TEST_CHECK(!Ui::Internal::CropRuntimeRevealBlurPixels(source, Rect<uint32_t>(0u, 8u, 1u, 2u)));
    DALI_TEST_CHECK(!Ui::Internal::CropRuntimeRevealBlurPixels(source, {}));
  }
  DALI_TEST_CHECK(!Ui::Internal::CropRuntimeRevealBlurPixels({}, Rect<uint32_t>(0u, 0u, 1u, 1u)));
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianSharedScratchP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetRequestedWidth(600.0f);
  label.SetRequestedHeight(400.0f);
  label.SetFontSize(20.0f);
  // Integer baseline increments keep identical line pages equal in pixels.
  label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
  label.SetLineHeight(30.0f);
  label.SetMultiLine(true);
  // Alternating wide/narrow captures exceed the occupancy allowance. Equal
  // pages still share scratch, independently of any fixed line-count limit.
  label.SetText("A complete line\nI\nA complete line\nI\nA complete line");
  Text::Reveal reveal;
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.0f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 32.0f);
  label.SetTextReveal(reveal);
  const auto tasks = application.GetScene().GetRenderTaskList();
  const bool lateSmooth = Ui::Integration::Text::Reveal::GetBlurMode(label.GetTextReveal()) == Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE;
  for(int connection = 0; connection < 2; ++connection)
  {
    Settle(application);
    std::vector<RenderTask> ordered;
    for(uint32_t index = 1u; index < tasks.GetTaskCount(); ++index)
    {
      ordered.push_back(tasks.GetTask(index));
    }
    std::sort(ordered.begin(), ordered.end(), [](RenderTask a, RenderTask b)
    {
      return a.GetOrderIndex() < b.GetOrderIndex();
    });
    DALI_TEST_CHECK(ordered.size() >= 9u && ordered.size() % 3u == 0u);
    bool shared        = false;
    bool differentSize = false;
    for(size_t index = 0u; index < ordered.size(); index += 3u)
    {
      DALI_TEST_EQUALS(ordered[index].GetSourceActor().GetProperty<Dali::String>(Actor::Property::NAME), Dali::String("RevealGaussianBatchSource"), TEST_LOCATION);
      DALI_TEST_EQUALS(ordered[index + 1u].GetSourceActor().GetProperty<Dali::String>(Actor::Property::NAME), Dali::String("RevealGaussianBatchH"), TEST_LOCATION);
      DALI_TEST_EQUALS(ordered[index + 2u].GetSourceActor().GetProperty<Dali::String>(Actor::Property::NAME), Dali::String("RevealGaussianBatchV"), TEST_LOCATION);
      for(size_t pass = 0u; pass < 2u; ++pass)
      {
        DALI_TEST_CHECK(ordered[index + pass].GetOrderIndex() < ordered[index + pass + 1u].GetOrderIndex());
        const auto renderers = Passes(ordered[index + pass + 1u].GetSourceActor(), pass == 0u ? "RevealGaussianBatchH" : "RevealGaussianBatchV");
        DALI_TEST_CHECK(!renderers.empty());
        for(auto renderer : renderers)
        {
          DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) == ordered[index + pass].GetFrameBuffer().GetColorTexture());
        }
      }
      DALI_TEST_CHECK(ordered[index].GetFrameBuffer() != ordered[index + 1u].GetFrameBuffer());
      for(size_t previous = 0u; previous < index; previous += 3u)
      {
        const auto a     = ordered[index].GetFrameBuffer().GetColorTexture();
        const auto b     = ordered[previous].GetFrameBuffer().GetColorTexture();
        const bool equal = a.GetWidth() == b.GetWidth() && a.GetHeight() == b.GetHeight();
        shared |= equal;
        differentSize |= !equal;
        // Late Smooth retains each page's sharp source through composition;
        // the original full-resolution path can reuse source as scratch.
        DALI_TEST_CHECK((a == b) == (equal && !lateSmooth));
        DALI_TEST_CHECK((ordered[index + 1u].GetFrameBuffer() == ordered[previous + 1u].GetFrameBuffer()) == equal);
        DALI_TEST_CHECK(ordered[index + 2u].GetFrameBuffer() != ordered[previous + 2u].GetFrameBuffer());
        // The previous sequence has consumed its scratch before any overwrite.
        DALI_TEST_CHECK(ordered[previous + 2u].GetOrderIndex() < ordered[index].GetOrderIndex());
      }
    }
    DALI_TEST_CHECK(shared);
    DALI_TEST_CHECK(differentSize);
    if(lateSmooth)
    {
      for(auto output : Passes(label, "RevealGaussianOutput"))
      {
        bool matched = false;
        for(size_t index = 0u; index < ordered.size(); index += 3u)
        {
          if(output.GetTextures().GetTexture(0u) == ordered[index + 2u].GetFrameBuffer().GetColorTexture())
          {
            DALI_TEST_EQUALS(output.GetTextures().GetTexture(1u), ordered[index].GetFrameBuffer().GetColorTexture(), TEST_LOCATION);
            matched = true;
          }
        }
        DALI_TEST_CHECK(matched);
      }
    }
    application.GetScene().Remove(label);
    Settle(application);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
    application.GetScene().Add(label);
  }
  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianNestedEffectP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetMultiLine(true);
  label.SetRequestedHeight(180.0f);
  label.SetText("A complete line\nA second line");
  label.SetRenderEffect(GaussianBlurEffect::New(16u));
  Settle(application);
  const auto   tasks      = application.GetScene().GetRenderTaskList();
  const auto   outerCount = tasks.GetTaskCount();
  Text::Reveal reveal;
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  for(int iteration = 0; iteration < 3; ++iteration)
  {
    label.SetTextReveal(reveal);
    Settle(application);
    DALI_TEST_CHECK(tasks.GetTaskCount() > outerCount);
    RenderTask outerSource;
    for(uint32_t index = 0u; index < tasks.GetTaskCount(); ++index)
    {
      if(tasks.GetTask(index).GetSourceActor() == label)
      {
        outerSource = tasks.GetTask(index);
      }
    }
    DALI_TEST_CHECK(outerSource);
    uint32_t found = 0u;
    for(uint32_t index = 0u; index < tasks.GetTaskCount(); ++index)
    {
      auto       task = tasks.GetTask(index);
      const auto name = task.GetSourceActor().GetProperty<Dali::String>(Actor::Property::NAME);
      if(name == "RevealGaussianBatchSource" || name == "RevealGaussianBatchH" || name == "RevealGaussianBatchV")
      {
        DALI_TEST_CHECK(task.GetOrderIndex() < outerSource.GetOrderIndex());
        ++found;
      }
    }
    DALI_TEST_CHECK(found > 0u);
    label.SetTextReveal(Text::Reveal::None());
    Settle(application);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), outerCount, TEST_LOCATION);
  }
  label.ClearRenderEffect();
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianCustomShaderP(void)
{
  UiTestApplication application;
  using Dali::Integration::ToDaliStringView;
  using Ui::Internal::GaussianBlurAlgorithm;
  const std::string vertexSource   = "// Custom vertex\n" + std::string(SHADER_CONTROL_RENDERERS_VERT);
  const std::string fragmentSource = "// Custom fragment\n" + std::string(SHADER_BLUR_EFFECT_FRAG);
  const auto        vertex         = ToDaliStringView(vertexSource);
  const auto        fragment       = ToDaliStringView(fragmentSource);
  for(uint32_t radius : {2u, 16u, 17u, 32u, 64u, 200u})
  {
    const auto shader = GaussianBlurAlgorithm::CreateShader(radius, vertex, fragment, Shader::Hint::OUTPUT_IS_TRANSPARENT, "CustomGaussian");
    const auto other  = GaussianBlurAlgorithm::CreateShader(radius, vertex, fragment, Shader::Hint::OUTPUT_IS_TRANSPARENT, "CustomGaussian");
    DALI_TEST_CHECK(shader && other && shader != other);
    const auto  program = shader.GetProperty(Shader::Property::PROGRAM);
    const auto* map     = program.GetMap();
    DALI_TEST_CHECK(map && map->Find("vertex") && map->Find("fragment") && map->Find("name") && map->Find("hints"));
    Dali::String source;
    DALI_TEST_CHECK(map->Find("vertex")->Get(source));
    DALI_TEST_CHECK(std::string(source.CStr()) == vertexSource);
    DALI_TEST_CHECK(map->Find("fragment")->Get(source));
    const std::string expected = "#define NUM_SAMPLES " + std::to_string(radius >> 1) + "\n" + fragmentSource;
    DALI_TEST_CHECK(std::string(source.CStr()) == expected);
    DALI_TEST_CHECK(map->Find("name")->Get(source));
    DALI_TEST_EQUALS(source, Dali::String("CustomGaussian"), TEST_LOCATION);
    DALI_TEST_CHECK(map->Find("hints")->Get(source));
    DALI_TEST_EQUALS(source, Dali::String("OUTPUT_IS_TRANSPARENT"), TEST_LOCATION);

    // Custom shaders remain separate from the ordinary shader cache.
    const auto ordinary = GaussianBlurAlgorithm::GetShader(radius);
    DALI_TEST_CHECK(shader != ordinary && other != ordinary);
    DALI_TEST_CHECK(ordinary == GaussianBlurAlgorithm::GetShader(radius));
  }
  Settle(application);
  DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianKernelSharingP(void)
{
  UiTestApplication application;
  using Dali::Integration::ToDaliStringView;
  using Ui::Internal::GaussianBlurAlgorithm;
  auto&                                        graphics = application.GetGraphicsController();
  constexpr uint32_t                           count    = 16u;
  constexpr uint32_t                           stride   = sizeof(float);
  TestGraphicsReflection::TestUniformBlockInfo block;
  block.name = "GaussianBlurSampleBlock";
  graphics.AddMemberToUniformBlock(block, "uSampleOffsets", Property::FLOAT, count, stride);
  graphics.AddMemberToUniformBlock(block, "uSampleWeights", Property::FLOAT, count, stride);
  graphics.AddCustomUniformBlock(block);

  // Inspect the real graphics binding, without exposing the private UBO handle.
  Renderer   renderer = GaussianBlurAlgorithm::CreateRenderer(32u);
  const auto ordinary = renderer.GetShader();
  Actor      actor    = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(64.0f, 64.0f));
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);
  Settle(application);
  const auto originalBinding = graphics.mLastUniformBinding;
  DALI_TEST_CHECK(originalBinding.buffer && !originalBinding.emulated);
  DALI_TEST_CHECK(originalBinding.offset + block.size <= originalBinding.buffer->memory.size());

  const auto custom = GaussianBlurAlgorithm::CreateShader(
    32u, ToDaliStringView(SHADER_CONTROL_RENDERERS_VERT), ToDaliStringView(SHADER_BLUR_EFFECT_FRAG),
    Shader::Hint::NONE, "CustomGaussianKernel");
  const auto oddRadius = GaussianBlurAlgorithm::CreateShader(
    33u, ToDaliStringView(SHADER_CONTROL_RENDERERS_VERT), ToDaliStringView(SHADER_BLUR_EFFECT_FRAG),
    Shader::Hint::NONE, "CustomGaussianOddRadius");

  // Ordinary shader properties cannot overwrite the shared block's constants.
  // Copying the custom shader handle still does not expose a mutable kernel.
  Shader copy = custom;
  copy.RegisterProperty("uSampleWeights[0]", 99.0f);
  copy.RegisterProperty("uSampleOffsets[0]", 99.0f);
  for(auto shader : {custom, oddRadius, ordinary})
  {
    renderer.SetShader(shader);
    graphics.mLastUniformBinding = {};
    Settle(application);
    const auto binding = graphics.mLastUniformBinding;
    DALI_TEST_CHECK(binding.buffer && !binding.emulated);
    DALI_TEST_CHECK(binding.buffer == originalBinding.buffer);
    DALI_TEST_EQUALS(binding.offset, originalBinding.offset, TEST_LOCATION);
    DALI_TEST_CHECK(binding.offset + block.size <= binding.buffer->memory.size());
    const auto* data = binding.buffer->memory.data() + binding.offset;
    float       sum  = 0.0f;
    for(uint32_t sample = 0u; sample < count; ++sample)
    {
      float weight;
      float offset;
      std::memcpy(&offset, data + sample * stride, sizeof(float));
      std::memcpy(&weight, data + (count + sample) * stride, sizeof(float));
      DALI_TEST_CHECK(std::isfinite(weight) && weight > 0.0f);
      DALI_TEST_CHECK(std::isfinite(offset) && offset >= static_cast<float>(2u * sample) && offset <= static_cast<float>(2u * sample + 1u));
      sum += weight * 2.0f;
    }
    DALI_TEST_EQUALS(sum, 1.0f, 0.00001f, TEST_LOCATION);
    // Pre-extraction values, with tolerance for platform math rounding.
    float weight;
    float offset;
    std::memcpy(&offset, data, sizeof(float));
    std::memcpy(&weight, data + count * stride, sizeof(float));
    DALI_TEST_EQUALS(offset, 0.665543258f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(weight, 0.0600374341f, 0.000001f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianShaderIsolationP(void)
{
  UiTestApplication application;
  const auto        ordinary = Ui::Internal::GaussianBlurAlgorithm::CreateRenderer(16u).GetShader();
  Label             label    = MakeLabel(application);
  Text::Reveal      reveal;
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  label.SetTextReveal(reveal);
  Settle(application);

  const auto horizontal = Passes(label, "RevealGaussianH");
  const auto vertical   = Passes(label, "RevealGaussianV");
  DALI_TEST_EQUALS(horizontal.size(), static_cast<size_t>(1u), TEST_LOCATION);
  DALI_TEST_EQUALS(vertical.size(), static_cast<size_t>(1u), TEST_LOCATION);
  const auto optimized = horizontal.front().GetShader();
  DALI_TEST_CHECK(optimized != ordinary);
  DALI_TEST_CHECK(optimized == vertical.front().GetShader());
  DALI_TEST_CHECK(ordinary == Ui::Internal::GaussianBlurAlgorithm::CreateRenderer(16u).GetShader());
  const auto batched = Ui::Internal::TextRevealBlurRenderer::CreateBatch(16u, Geometry::New()).GetShader();
  DALI_TEST_CHECK(batched != ordinary && batched != optimized);
  DALI_TEST_CHECK(batched == Ui::Internal::TextRevealBlurRenderer::CreateBatch(16u, Geometry::New()).GetShader());
  for(auto shader : {ordinary, optimized, batched})
  {
    const auto  program = shader.GetProperty(Shader::Property::PROGRAM);
    const auto* map     = program.GetMap();
    DALI_TEST_CHECK(map && map->Find("fragment") && map->Find("vertex"));
    Dali::String fragment;
    DALI_TEST_CHECK(map->Find("fragment")->Get(fragment));
    DALI_TEST_EQUALS(std::string(fragment.CStr()).find("uTextRevealBlurProgress") != std::string::npos,
                     shader != ordinary, TEST_LOCATION);
    DALI_TEST_EQUALS(std::string(fragment.CStr()).find("#define TEXT_REVEAL_DRAW_BATCH") != std::string::npos,
                     shader == batched, TEST_LOCATION);
    Dali::String vertex;
    DALI_TEST_CHECK(map->Find("vertex")->Get(vertex));
    DALI_TEST_EQUALS(std::string(vertex.CStr()).find("uRevealBlurState[") != std::string::npos,
                     shader == batched, TEST_LOCATION);
    if(shader == ordinary)
    {
      DALI_TEST_CHECK(std::string(fragment.CStr()).find("REVEAL") == std::string::npos);
      DALI_TEST_CHECK(std::string(fragment.CStr()).find("uReveal") == std::string::npos);
    }
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianMinimumRadiusP(void)
{
  UiTestApplication application;
  const auto        tasks = application.GetScene().GetRenderTaskList();
  for(auto mode : {Ui::Integration::Text::Reveal::BlurMode::QUALITY, Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE})
  {
    for(bool perLine : {false, true})
    {
      Label label = MakeLabel(application);
      label.SetText("First line\nSecond line");
      label.SetFontSize(16.0f);
      label.SetMultiLine(true);
      Text::Reveal reveal;
      reveal.SetSequence(perLine ? Text::Reveal::Sequence::PER_LINE : Text::Reveal::Sequence::WHOLE_TEXT);
      Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 1.0f);
      Ui::Integration::Text::Reveal::SetBlurMode(reveal, mode);
      std::vector<Vector2> minimumTargets;
      // Start with the supported reference, then exercise rounding, disabling
      // and re-enabling. Small radii must share its shader AND capture bounds.
      for(float radius : {4.0f, 0.5f, 1.0f, 2.0f, 3.0f, 5.0f, 0.0f, 2.0f})
      {
        Ui::Integration::Text::Reveal::SetBlurRadius(reveal, radius);
        label.SetTextReveal(reveal);
        Settle(application);
        DALI_TEST_EQUALS(Ui::Integration::Text::Reveal::GetBlurRadius(label.GetTextReveal()), radius, TEST_LOCATION);
        const Actor host = Find(label, "TextRevealRuntimeGaussian");
        if(radius == 0.0f)
        {
          DALI_TEST_CHECK(!host);
          DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
          continue;
        }
        DALI_TEST_CHECK(host);
        const uint32_t kernelRadius = radius > 4.0f ? 6u : 4u;
        // The public mode choice is retained even below the handoff anchors;
        // PERFORMANCE must not silently select full-resolution targets.
        for(auto output : Passes(label, "RevealGaussianOutput"))
        {
          DALI_TEST_EQUALS(output.GetPropertyIndex("uQuarterAuthoredRadius") != Property::INVALID_INDEX,
                           mode == Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE, TEST_LOCATION);
        }
        const auto    contentSize = label.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
        const Vector3 targetSize(std::ceil(contentSize.x) + 2.0f * static_cast<float>(kernelRadius + 2u),
                                 std::ceil(contentSize.y) + 2.0f * static_cast<float>(kernelRadius + 2u), 0.0f);
        DALI_TEST_EQUALS(host.GetCurrentProperty<Vector3>(Actor::Property::SIZE), targetSize, TEST_LOCATION);
        for(bool horizontal : {true, false})
        {
          const auto lines = BlurLines(label, horizontal);
          DALI_TEST_CHECK(!lines.empty());
          for(const auto& line : lines)
          {
            const auto  program = line.renderer.GetShader().GetProperty(Shader::Property::PROGRAM);
            const auto* map     = program.GetMap();
            DALI_TEST_CHECK(map && map->Find("fragment"));
            Dali::String fragment;
            DALI_TEST_CHECK(map->Find("fragment")->Get(fragment));
            const std::string expected = "#define NUM_SAMPLES " + std::to_string(kernelRadius / 2u) + "\n";
            DALI_TEST_CHECK(std::string(fragment.CStr()).find(expected) != std::string::npos);
          }
        }
        std::vector<Vector2> targets;
        for(uint32_t index = 1u; index < tasks.GetTaskCount(); ++index)
        {
          const auto texture = tasks.GetTask(index).GetFrameBuffer().GetColorTexture();
          targets.emplace_back(static_cast<float>(texture.GetWidth()), static_cast<float>(texture.GetHeight()));
        }
        DALI_TEST_CHECK(!targets.empty());
        if(minimumTargets.empty())
        {
          minimumTargets = targets;
        }
        if(kernelRadius == 4u)
        {
          DALI_TEST_EQUALS(targets.size(), minimumTargets.size(), TEST_LOCATION);
          for(size_t index = 0u; index < targets.size(); ++index)
          {
            DALI_TEST_EQUALS(targets[index], minimumTargets[index], TEST_LOCATION);
          }
        }
      }
      application.GetScene().Remove(label);
      Settle(application);
      DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
    }
  }
  END_TEST;
}

namespace
{
int CheckRuntimeGaussianQualityPublication(bool async)
{
  UiTestApplication application;
  const auto        tasks = application.GetScene().GetRenderTaskList();
  for(bool perLine : {false, true})
  {
    for(bool gradientText : {false, true})
    {
      Label label = Label::New("A bright line\nAnother short line\nEnd.");
      label.SetLayoutMode(LayoutMode::STANDALONE);
      label.SetRequestedWidth(360.0f);
      label.SetRequestedHeight(180.0f);
      label.SetFontSize(18.0f);
      label.SetMultiLine(true);
      label.SetAsyncRendering(async);
      if(gradientText)
      {
        Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
        gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
        label.SetTextGradient(gradient);
      }
      Text::Reveal reveal;
      reveal.SetUnit(Text::Reveal::Unit::PIXEL);
      reveal.SetSequence(perLine ? Text::Reveal::Sequence::PER_LINE : Text::Reveal::Sequence::WHOLE_TEXT);
      reveal.SetSequenceStaggerRatio(0.25f);
      reveal.SetFadeDurationRatio(0.25f);
      Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
      Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
      label.SetTextReveal(reveal);
      label.SetTextRevealProgress(0.4f);
      const auto        progressIndex = label.GetPropertyIndex("uTextRevealProgress");
      uint32_t          completed     = 0u;
      ConnectionTracker tracker;
      label.AsyncRenderFinishedSignal().Connect(&tracker, [&completed](View, float, float)
      {
        ++completed;
      });
      application.GetScene().Add(label);
      auto settlePublication = [&](uint32_t previous)
      {
        Settle(application);
        if(async)
        {
          for(uint32_t attempt = 0u; attempt < 12u && completed == previous; ++attempt)
          {
            DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
            Settle(application);
          }
          DALI_TEST_CHECK(completed > previous);
        }
      };
      std::vector<float> referenceStrengths;
      uint32_t           referenceTaskCount = 0u;
      for(auto mode : {Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE, Ui::Integration::Text::Reveal::BlurMode::QUALITY, static_cast<Ui::Integration::Text::Reveal::BlurMode>(255u), Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE})
      {
        const auto previous = completed;
        Ui::Integration::Text::Reveal::SetBlurMode(reveal, mode);
        label.SetTextReveal(reveal);
        settlePublication(previous);
        DALI_TEST_CHECK(label.GetTextReveal() == reveal);
        DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
        DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.4f, 0.0001f, TEST_LOCATION);
        if(referenceTaskCount == 0u)
        {
          referenceTaskCount = tasks.GetTaskCount();
        }
        DALI_TEST_EQUALS(tasks.GetTaskCount(), referenceTaskCount, TEST_LOCATION);
        DALI_TEST_CHECK(referenceTaskCount > 1u && (referenceTaskCount - 1u) % 3u == 0u);
        std::vector<RenderTask> ordered;
        for(uint32_t index = 1u; index < tasks.GetTaskCount(); ++index)
        {
          ordered.push_back(tasks.GetTask(index));
        }
        std::sort(ordered.begin(), ordered.end(), [](RenderTask a, RenderTask b)
        {
          return a.GetOrderIndex() < b.GetOrderIndex();
        });
        for(const auto& task : ordered)
        {
          const auto texture = task.GetFrameBuffer().GetColorTexture();
          DALI_TEST_EQUALS(texture.GetPixelFormat(), gradientText ? Pixel::RGBA8888 : Pixel::A8, TEST_LOCATION);
          DALI_TEST_EQUALS(task.GetRefreshRate(), static_cast<uint32_t>(RenderTask::REFRESH_ALWAYS), TEST_LOCATION);
        }
        const bool reduced = mode != Ui::Integration::Text::Reveal::BlurMode::QUALITY;
        for(size_t index = 0u; index < ordered.size(); index += 3u)
        {
          const auto source     = ordered[index].GetFrameBuffer().GetColorTexture();
          const auto horizontal = ordered[index + 1u].GetFrameBuffer().GetColorTexture();
          const auto vertical   = ordered[index + 2u].GetFrameBuffer().GetColorTexture();
          DALI_TEST_EQUALS(horizontal.GetWidth(), reduced ? (source.GetWidth() + 3u) / 4u : source.GetWidth(), TEST_LOCATION);
          DALI_TEST_EQUALS(horizontal.GetHeight(), source.GetHeight(), TEST_LOCATION);
          DALI_TEST_EQUALS(vertical.GetWidth(), horizontal.GetWidth(), TEST_LOCATION);
          DALI_TEST_EQUALS(vertical.GetHeight(), reduced ? (source.GetHeight() + 3u) / 4u : source.GetHeight(), TEST_LOCATION);
        }
        const auto lines = BlurLines(label);
        if(referenceStrengths.empty())
        {
          for(const auto& line : lines)
          {
            referenceStrengths.push_back(line.Strength());
          }
        }
        DALI_TEST_EQUALS(lines.size(), referenceStrengths.size(), TEST_LOCATION);
        for(size_t index = 0u; index < lines.size(); ++index)
        {
          DALI_TEST_EQUALS(lines[index].Strength(), referenceStrengths[index], 0.0001f, TEST_LOCATION);
        }
      }
      // Coalesce a mode change back to PERFORMANCE before an async request
      // completes. Only the final authored configuration may be published.
      const auto previous = completed;
      Ui::Integration::Text::Reveal::SetBlurMode(reveal, Ui::Integration::Text::Reveal::BlurMode::QUALITY);
      label.SetTextReveal(reveal);
      application.SendNotification();
      Ui::Integration::Text::Reveal::SetBlurMode(reveal, Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE);
      label.SetTextReveal(reveal);
      settlePublication(previous);
      DALI_TEST_CHECK(label.GetTextReveal() == reveal);
      for(auto output : Passes(label, "RevealGaussianOutput"))
      {
        DALI_TEST_EQUALS(output.GetTextures().GetTextureCount(), 2u, TEST_LOCATION);
      }
      DALI_TEST_EQUALS(tasks.GetTaskCount(), referenceTaskCount, TEST_LOCATION);
      auto              host = Find(label, "TextRevealRuntimeGaussian");
      WeakHandle<Actor> companion(host);
      host.Reset();
      label.SetTextReveal(Text::Reveal::None());
      label.Unparent();
      Settle(application);
      DALI_TEST_CHECK(!companion.GetHandle());
      DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
    }
  }
  END_TEST;
}
} // namespace

int UtcDaliTextRevealRuntimeGaussianQualityPublicationP(void)
{
  return CheckRuntimeGaussianQualityPublication(false);
}

int UtcDaliTextRevealRuntimeGaussianQualityPublicationAsyncP(void)
{
  return CheckRuntimeGaussianQualityPublication(true);
}

namespace
{
int CheckRuntimeGaussianPublicTiming(Text::Reveal::Unit unit, Text::Reveal::Sequence sequence)
{
  UiTestApplication application;
  // Actual completion includes PIXEL's spatial progression as well as fade.
  Text::Internal::Reveal::Plan plan;
  plan.unitStart    = {0.0f, 0.4f, 0.8f};
  plan.fadeDuration = 0.1f;
  DALI_TEST_EQUALS(Ui::Internal::GetRuntimeRevealEndProgress(plan), 0.9f, 0.0001f, TEST_LOCATION);
  plan.pixelUnitTiming.resize(3u);
  plan.pixelUnitTiming.back().progressionSpan = 0.1f;
  DALI_TEST_EQUALS(Ui::Internal::GetRuntimeRevealEndProgress(plan), 1.0f, 0.0001f, TEST_LOCATION);

  Label label = MakeLabel(application);
  label.SetMultiLine(true);
  label.SetRequestedWidth(600.0f);
  label.SetRequestedHeight(180.0f);
  label.SetFontSize(20.0f);
  label.SetText("Alpha beta gamma.\nAlpha beta gamma.\nAlpha beta gamma.");
  const auto progressIndex = label.GetPropertyIndex("uTextRevealProgress");
  for(float stagger : {0.0f, 0.25f, 1.0f})
  {
    for(float fade : {0.0f, 0.25f})
    {
      for(float ratio : {0.25f, 0.5f, 1.0f})
      {
        Text::Reveal reveal;
        reveal.SetUnit(unit);
        reveal.SetSequence(sequence);
        reveal.SetSequenceStaggerRatio(stagger);
        reveal.SetFadeDurationRatio(fade);
        Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
        Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, ratio);
        label.SetTextReveal(reveal);
        Settle(application);
        auto       horizontal = BlurLines(label);
        auto       vertical   = BlurLines(label, false);
        const bool perLine    = sequence == Text::Reveal::Sequence::PER_LINE;
        auto       foreground = Passes(label, perLine ? "RevealGaussianLineForeground" : "RevealGaussianForeground");
        DALI_TEST_EQUALS(horizontal.size(), perLine ? static_cast<size_t>(3u) : static_cast<size_t>(1u), TEST_LOCATION);
        DALI_TEST_EQUALS(vertical.size(), horizontal.size(), TEST_LOCATION);
        DALI_TEST_EQUALS(foreground.size(), static_cast<size_t>(1u), TEST_LOCATION);
        // Identical explicit lines share a canonical reference interval.
        // LINE/STEP has zero visible transition time, but its blur/stagger
        // reference remains meaningful and must not become zero.
        const float revealSpan   = unit == Text::Reveal::Unit::LINE ? fade : 1.0f;
        const float end          = perLine ? 2.0f * stagger + std::max(revealSpan, ratio) : 1.0f;
        const float blurDuration = ratio / end;
        auto&       gl           = application.GetGlAbstraction();
        gl.EnableTextureCallTrace(true);
        gl.ResetTextureCallStack();
        for(float progress : {0.0f, 0.15f, 0.55f, 1.0f, 0.3f, 0.0f})
        {
          label.SetTextRevealProgress(progress);
          Settle(application);
          for(size_t line = 0u; line < horizontal.size(); ++line)
          {
            const float start = perLine ? static_cast<float>(line) * stagger / end : 0.0f;
            const float q     = progress >= 1.0f ? 1.0f : std::clamp((progress - start) / blurDuration, 0.0f, 1.0f);
            for(auto pass : {horizontal[line], vertical[line]})
            {
              DALI_TEST_EQUALS(pass.Start(), start, 0.0002f, TEST_LOCATION);
              DALI_TEST_EQUALS(pass.Progress(), progress, 0.0001f, TEST_LOCATION);
              DALI_TEST_EQUALS(pass.Strength(),
                               1.0f - q * q * (3.0f - 2.0f * q), 0.0002f, TEST_LOCATION);
            }
            DALI_TEST_EQUALS(foreground[0u].GetCurrentProperty<float>(foreground[0u].GetPropertyIndex("uTextRevealProgress")),
                             progress, 0.0001f, TEST_LOCATION);
          }
        }
        DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
        DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);
        DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
      }
    }
  }
  END_TEST;
}
} // namespace

// Keep every timing combination, but bound the work in each runner process.
int UtcDaliTextRevealRuntimeGaussianPublicTimingP(void)
{
  return CheckRuntimeGaussianPublicTiming(Text::Reveal::Unit::CHARACTER, Text::Reveal::Sequence::WHOLE_TEXT);
}

int UtcDaliTextRevealRuntimeGaussianPublicTimingWordP(void)
{
  return CheckRuntimeGaussianPublicTiming(Text::Reveal::Unit::WORD, Text::Reveal::Sequence::WHOLE_TEXT);
}

int UtcDaliTextRevealRuntimeGaussianPublicTimingLineP(void)
{
  return CheckRuntimeGaussianPublicTiming(Text::Reveal::Unit::LINE, Text::Reveal::Sequence::WHOLE_TEXT);
}

int UtcDaliTextRevealRuntimeGaussianPublicTimingPixelP(void)
{
  return CheckRuntimeGaussianPublicTiming(Text::Reveal::Unit::PIXEL, Text::Reveal::Sequence::WHOLE_TEXT);
}

int UtcDaliTextRevealRuntimeGaussianPublicTimingPerLineP(void)
{
  return CheckRuntimeGaussianPublicTiming(Text::Reveal::Unit::CHARACTER, Text::Reveal::Sequence::PER_LINE);
}

int UtcDaliTextRevealRuntimeGaussianPublicTimingWordPerLineP(void)
{
  return CheckRuntimeGaussianPublicTiming(Text::Reveal::Unit::WORD, Text::Reveal::Sequence::PER_LINE);
}

int UtcDaliTextRevealRuntimeGaussianPublicTimingLinePerLineP(void)
{
  return CheckRuntimeGaussianPublicTiming(Text::Reveal::Unit::LINE, Text::Reveal::Sequence::PER_LINE);
}

int UtcDaliTextRevealRuntimeGaussianPublicTimingPixelPerLineP(void)
{
  return CheckRuntimeGaussianPublicTiming(Text::Reveal::Unit::PIXEL, Text::Reveal::Sequence::PER_LINE);
}

namespace
{
int CheckRuntimeGaussianPublicAnimation(AlphaFunction::BuiltinFunction alpha)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetMultiLine(true);
  label.SetRequestedHeight(180.0f);
  label.SetText("Alpha beta.\nAlpha beta.\nAlpha beta.");
  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::LINE);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  reveal.SetFadeDurationRatio(0.25f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
  label.SetTextReveal(reveal);
  Settle(application);
  const auto horizontal = BlurLines(label);
  DALI_TEST_EQUALS(horizontal.size(), static_cast<size_t>(3u), TEST_LOCATION);
  for(float duration : {1.0f, 2.0f, 4.0f})
  {
    for(float target : {1.0f, 0.0f})
    {
      Animation animation = Animation::New(duration);
      label.Animate(animation).TextRevealProgress(target, Duration(duration), AlphaFunction(alpha));
      ConnectionTracker tracker;
      bool              finished = false;
      animation.FinishedSignal().Connect(&tracker, [&finished](Animation)
      {
        finished = true;
      });
      animation.Play();
      // Advance mock time at fixed normalized intervals. Rendering every
      // 20 ms repeats the same contract more often for longer animations.
      constexpr uint32_t SAMPLE_COUNT            = 20u;
      const auto         interval                = static_cast<uint32_t>(duration * 1000.0f) / SAMPLE_COUNT;
      bool               sawIntermediateProgress = false;
      for(uint32_t frame = 0u; frame < SAMPLE_COUNT + 3u; ++frame)
      {
        application.SendNotification();
        // Allow only a few milliseconds for endpoint rounding and the
        // finished notification, independent of the animation duration.
        application.Render(frame < SAMPLE_COUNT ? interval : 1u);
        const float progress = label.GetTextRevealProgress();
        sawIntermediateProgress |= progress > 0.0f && progress < 1.0f;
        if(frame + 1u < SAMPLE_COUNT)
        {
          DALI_TEST_CHECK(!finished);
        }
        for(size_t line = 0u; line < horizontal.size(); ++line)
        {
          const float q = progress >= 1.0f ? 1.0f : std::clamp((progress - static_cast<float>(line) * 0.25f) / 0.5f, 0.0f, 1.0f);
          DALI_TEST_EQUALS(horizontal[line].Strength(),
                           1.0f - q * q * (3.0f - 2.0f * q), 0.0002f, TEST_LOCATION);
          DALI_TEST_EQUALS(horizontal[line].Progress(),
                           progress, 0.0001f, TEST_LOCATION);
        }
      }
      DALI_TEST_CHECK(sawIntermediateProgress);
      DALI_TEST_CHECK(finished);
      DALI_TEST_EQUALS(animation.GetDuration(), duration, TEST_LOCATION);
      DALI_TEST_EQUALS(label.GetTextRevealProgress(), target, 0.0001f, TEST_LOCATION);
      animation.Stop();
    }
  }
  END_TEST;
}
} // namespace

int UtcDaliTextRevealRuntimeGaussianPublicAnimationP(void)
{
  return CheckRuntimeGaussianPublicAnimation(AlphaFunction::LINEAR);
}

int UtcDaliTextRevealRuntimeGaussianPublicAnimationEaseInP(void)
{
  return CheckRuntimeGaussianPublicAnimation(AlphaFunction::EASE_IN_SQUARE);
}

int UtcDaliTextRevealRuntimeGaussianPublicAnimationEaseOutP(void)
{
  return CheckRuntimeGaussianPublicAnimation(AlphaFunction::EASE_OUT_SQUARE);
}

int UtcDaliTextRevealRuntimeGaussianProgressEndpointP(void)
{
  using Ui::Text::Internal::Reveal::ResolveRenderProgress;
  const float complete = 1.0f - std::numeric_limits<float>::epsilon();
  const float interior = std::nextafter(complete, 0.0f);
  DALI_TEST_EQUALS(ResolveRenderProgress(complete), 1.0f, TEST_LOCATION);
  DALI_TEST_CHECK(ResolveRenderProgress(interior) == interior);
  DALI_TEST_CHECK(ResolveRenderProgress(0.00001f) == 0.00001f);
  DALI_TEST_EQUALS(ResolveRenderProgress(0.0f), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(ResolveRenderProgress(-1.0f), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(ResolveRenderProgress(2.0f), 1.0f, TEST_LOCATION);

  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetMultiLine(true);
  label.SetRequestedHeight(180.0f);
  label.SetText("A completed line.\nAnother completed line.\nA short tail.");
  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::CHARACTER);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetFadeDurationRatio(0.0f);
  reveal.SetSequenceStaggerRatio(0.25f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
  label.SetTextReveal(reveal);
  Settle(application);

  // Exercise both directions through values that float property equality may
  // retain. Mock GL does not execute fragments: real-GLES image comparisons
  // additionally verify that STEP units scheduled at one are fully present.
  for(float progress : {0.0f, interior, complete, 1.0f, complete, 0.5f, 0.00001f, 0.0f})
  {
    label.SetTextRevealProgress(progress);
    Settle(application);
    for(const auto& line : BlurLines(label))
    {
      DALI_TEST_CHECK(std::isfinite(line.Strength()));
      if(progress >= complete)
      {
        DALI_TEST_EQUALS(line.Strength(), 0.0f, TEST_LOCATION);
      }
    }
  }
  const auto   original = Find(label, "RevealGaussianForeground").GetRendererAt(0u);
  const auto   program  = original.GetShader().GetProperty(Shader::Property::PROGRAM);
  Dali::String fragment;
  DALI_TEST_CHECK(program.GetMap()->Find("fragment")->Get(fragment));
  // Keep the CPU and shader endpoint policies in lockstep.
  DALI_TEST_CHECK(std::string(fragment.CStr()).find("completeProgress = 0.99999988079071044921875") != std::string::npos);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianPublicLifecycleP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetMultiLine(true);
  label.SetRequestedHeight(180.0f);
  label.SetText("A full line of text.\nZ");
  const auto   tasks         = application.GetScene().GetRenderTaskList();
  const auto   baseline      = tasks.GetTaskCount();
  const auto   progressIndex = label.GetPropertyIndex("uTextRevealProgress");
  Text::Reveal reveal;
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
  label.SetTextRevealProgress(0.4f);
  label.SetTextReveal(reveal);
  Settle(application);
  const auto firstCount = tasks.GetTaskCount();
  DALI_TEST_CHECK(firstCount > baseline && firstCount <= baseline + 6u);
  // Disable by either public setting, then recover without a private clock.
  for(bool disableRadius : {false, true})
  {
    Text::Reveal disabled(reveal);
    if(disableRadius)
    {
      Ui::Integration::Text::Reveal::SetBlurRadius(disabled, 0.0f);
    }
    else
    {
      Ui::Integration::Text::Reveal::SetBlurDurationRatio(disabled, 0.0f);
    }
    label.SetTextReveal(disabled);
    Settle(application);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
    DALI_TEST_CHECK(!Find(label, "RevealGaussianForeground"));
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    label.SetTextReveal(reveal);
    Settle(application);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), firstCount, TEST_LOCATION);
  }
  label.SetTextRevealProgress(1.0f);
  label.SetText("Different full text.\nShort tail.");
  Settle(application);
  const auto updatedCount = tasks.GetTaskCount();
  DALI_TEST_CHECK(updatedCount > baseline && updatedCount <= baseline + 6u);
  for(auto pass : BlurLines(label))
  {
    DALI_TEST_EQUALS(pass.Strength(), 0.0f, TEST_LOCATION);
  }
  // Allocation fallback must not retain normalized source timing or tasks.
  label.SetRequestedWidth(4096.0f);
  label.SetRequestedHeight(4096.0f);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  DALI_TEST_CHECK(!Find(label, "RevealGaussianForeground"));
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(180.0f);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), updatedCount, TEST_LOCATION);
  application.GetScene().Remove(label);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  application.GetScene().Add(label);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), updatedCount, TEST_LOCATION);
  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianPublicImageTimingP(void)
{
  UiTestApplication application;
  Label             label    = MakeLabel(application);
  const auto        tasks    = application.GetScene().GetRenderTaskList();
  const auto        baseline = tasks.GetTaskCount();
  label.SetMultiLine(true);
  label.SetRequestedHeight(180.0f);
  Texture      image      = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  Ui::ImageUrl url        = Ui::ImageUrl::New(image, true);
  auto         builder    = Text::StyledTextBuilder::New("A full text line.\n");
  const auto   imageIndex = builder.GetUtf32Length();
  builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
  DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(url.GetUrl(), Vector2(32.0f, 24.0f))),
                                  imageIndex, imageIndex + 1u));
  label.SetStyledText(builder.Build());
  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::LINE);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  reveal.SetFadeDurationRatio(0.0f);
  label.SetTextReveal(reveal);
  Settle(application);
  auto* data = Ui::Internal::Text::GetInlineReplacementData(label);
  DALI_TEST_CHECK(data);
  using Accessor = Ui::Internal::Text::InlineReplacementManagerTestAccessor;
  Text::ReplacementRevealTiming ordinary;
  DALI_TEST_CHECK(Accessor::GetRevealTiming(data->manager, 1u, ordinary));
  DALI_TEST_EQUALS(ordinary.start, 0.2f, 0.0001f, TEST_LOCATION);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 1.0f);
  label.SetTextReveal(reveal);
  Settle(application);
  Text::ReplacementRevealTiming combined;
  // The image-only sequence receives the same blur interval as the text line.
  // A backend supporting A8 may put text and image into separate pages.
  DALI_TEST_CHECK(tasks.GetTaskCount() > baseline && tasks.GetTaskCount() <= baseline + 6u);
  DALI_TEST_CHECK(Accessor::GetRevealTiming(data->manager, 1u, combined));
  // End is max(revealEnd, lastStart + blurDuration) = max(.2, .2 + .8).
  DALI_TEST_EQUALS(combined.start, 0.2f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(combined.fadeDuration, 0.0f, TEST_LOCATION);
  label.SetTextRevealProgress(0.5f);
  Settle(application);
  auto horizontal = BlurLines(label);
  DALI_TEST_CHECK(!horizontal.empty());
  DALI_TEST_EQUALS(horizontal.front().Strength(),
                   0.31640625f, 0.0001f, TEST_LOCATION);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 0.0f);
  label.SetTextReveal(reveal);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  DALI_TEST_CHECK(Accessor::GetRevealTiming(data->manager, 1u, combined));
  DALI_TEST_EQUALS(combined.start, ordinary.start, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.5f, TEST_LOCATION);
  END_TEST;
}

namespace
{
int CheckAsyncImageRevealPending(bool blur)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  Label label = MakeLabel(application);
  label.SetTextReveal(Text::Reveal::None());
  Texture      image      = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  Ui::ImageUrl url        = Ui::ImageUrl::New(image, true);
  auto         builder    = Text::StyledTextBuilder::New("Text before ");
  const auto   imageIndex = builder.GetUtf32Length();
  builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
  builder.AppendText(" and after.");
  DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(url.GetUrl(), Vector2(32.0f, 24.0f))),
                                  imageIndex, imageIndex + 1u));
  label.SetStyledText(builder.Build());
  Settle(application);
  auto* data = Ui::Internal::Text::GetInlineReplacementData(label);
  DALI_TEST_CHECK(data);
  using Accessor = Ui::Internal::Text::InlineReplacementManagerTestAccessor;
  Renderer imageRenderer;
  for(uint32_t index = 0u; index < label.GetRendererCount(); ++index)
  {
    auto renderer = label.GetRendererAt(index);
    auto textures = renderer.GetTextures();
    if(textures && textures.GetTextureCount() > 0u && textures.GetTexture(0u) == image)
    {
      imageRenderer = renderer;
    }
  }
  DALI_TEST_CHECK(imageRenderer && Accessor::IsEntryVisible(data->manager, 1u));
  uint32_t          completions = 0u;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&completions](View, float, float)
  {
    ++completions;
  });
  auto complete = [&](uint32_t previous)
  {
    Settle(application);
    for(int attempt = 0; attempt < 8 && completions == previous; ++attempt)
    {
      if(!Test::WaitForEventThreadTrigger(1, 5))
      {
        return false;
      }
      Settle(application);
    }
    return completions > previous;
  };
  label.SetAsyncRendering(true);
  DALI_TEST_CHECK(complete(0u));
  for(auto unit : {Text::Reveal::Unit::PIXEL, Text::Reveal::Unit::CHARACTER,
                   Text::Reveal::Unit::WORD, Text::Reveal::Unit::LINE})
  {
    for(bool hidden : {false, true})
    {
      label.SetProperty(Actor::Property::VISIBLE, !hidden);
      Settle(application);
      Text::Reveal reveal;
      reveal.SetUnit(unit);
      reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
      reveal.SetFadeDurationRatio(0.0f);
      Ui::Integration::Text::Reveal::SetBlurRadius(reveal, blur ? 24.0f : 0.0f);
      Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
      const auto before = completions;
      label.SetTextReveal(reveal);
      label.SetTextRevealProgress(0.0f);
      label.SetProperty(Actor::Property::VISIBLE, true);
      // No worker callbacks are dispatched here. An image resource-ready
      // refresh must not expose the old ordinary image while timing is pending.
      data->manager.Refresh();
      Settle(application);
      DALI_TEST_EQUALS(completions, before, TEST_LOCATION);
      DALI_TEST_CHECK(!Accessor::IsEntryVisible(data->manager, 1u));
      DALI_TEST_EQUALS(imageRenderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), 0.0f, TEST_LOCATION);
      DALI_TEST_CHECK(complete(before));
      DALI_TEST_EQUALS(Accessor::GetRevealConstraintCount(data->manager), 1u, TEST_LOCATION);
      DALI_TEST_CHECK(Accessor::GetRevealConstraint(data->manager, 1u).GetTargetObject() == imageRenderer);
      const auto hiddenProperty = Accessor::IsRevealPixelSpatial(data->manager, 1u)
                                    ? imageRenderer.GetPropertyIndex("uInlineReplacementRevealProgress")
                                    : static_cast<Property::Index>(DevelRenderer::Property::OPACITY);
      DALI_TEST_EQUALS(imageRenderer.GetCurrentProperty<float>(hiddenProperty), 0.0f, TEST_LOCATION);
      label.SetTextRevealProgress(1.0f);
      Settle(application);
      DALI_TEST_EQUALS(imageRenderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), 1.0f, TEST_LOCATION);

      // A valid prior binding remains usable during enabled reconfiguration.
      auto       constraint  = Accessor::GetRevealConstraint(data->manager, 1u);
      const auto reconfigure = completions;
      reveal.SetFadeDurationRatio(0.25f);
      label.SetTextReveal(reveal);
      label.SetTextRevealProgress(0.0f);
      data->manager.Refresh();
      Settle(application);
      DALI_TEST_CHECK(Accessor::GetRevealConstraint(data->manager, 1u) == constraint);
      DALI_TEST_CHECK(complete(reconfigure));

      // Entry-to-exit reconfiguration keeps progress at one until the new
      // result is published. PIXEL with fade ratio one uses scalar opacity;
      // async placement publication must not put its spatial shader back.
      for(auto sequence : {Text::Reveal::Sequence::WHOLE_TEXT, Text::Reveal::Sequence::PER_LINE})
      {
        label.SetTextRevealProgress(1.0f);
        Settle(application);
        const auto exit = completions;
        reveal.SetSequence(sequence);
        reveal.SetSequenceStaggerRatio(0.0f);
        reveal.SetFadeDurationRatio(1.0f);
        Ui::Integration::Text::Reveal::SetBlurRadius(reveal, blur ? 48.0f : 0.0f);
        Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 1.0f);
        label.SetTextReveal(reveal);
        DALI_TEST_CHECK(complete(exit));
        DALI_TEST_CHECK(Accessor::IsEntryVisible(data->manager, 1u));
        DALI_TEST_CHECK(!Accessor::IsRevealPixelSpatial(data->manager, 1u));
        auto scalar = Accessor::GetRevealConstraint(data->manager, 1u);
        DALI_TEST_CHECK(scalar.GetTargetObject() == imageRenderer);
        DALI_TEST_EQUALS(scalar.GetTargetProperty(), static_cast<Property::Index>(DevelRenderer::Property::OPACITY), TEST_LOCATION);
        const auto program = imageRenderer.GetShader().GetProperty(Shader::Property::PROGRAM);
        DALI_TEST_CHECK(program.GetMap());
        const auto fragmentValue = program.GetMap()->Find("fragment");
        DALI_TEST_CHECK(fragmentValue);
        Dali::String fragment;
        DALI_TEST_CHECK(fragmentValue->Get(fragment));
        DALI_TEST_CHECK(std::string(fragment.CStr()).find("uInlineReplacementRevealProgress") == std::string::npos);
        for(float progress : {1.0f, 0.5f, 0.0f})
        {
          label.SetTextRevealProgress(progress);
          data->manager.Refresh();
          Settle(application);
          DALI_TEST_EQUALS(imageRenderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY),
                           progress, 0.001f, TEST_LOCATION);
        }
      }
      const auto disabled = completions;
      label.SetTextReveal(Text::Reveal::None());
      data->manager.Refresh();
      Settle(application);
      DALI_TEST_CHECK(Accessor::IsEntryVisible(data->manager, 1u));
      DALI_TEST_EQUALS(Accessor::GetRevealConstraintCount(data->manager), 0u, TEST_LOCATION);
      DALI_TEST_CHECK(complete(disabled));
    }
  }

  // None -> full fade can reuse the current image occurrence immediately.
  // Start and finish an exit without dispatching any worker completion: the
  // application does not need a readiness callback to keep the image visible.
  Text::Reveal fullFade;
  fullFade.SetUnit(Text::Reveal::Unit::PIXEL);
  fullFade.SetFadeDurationRatio(1.0f);
  Ui::Integration::Text::Reveal::SetBlurRadius(fullFade, blur ? 48.0f : 0.0f);
  const auto fullFadePending = completions;
  label.SetTextReveal(fullFade);
  for(float progress : {1.0f, 0.75f, 0.25f, 0.0f})
  {
    label.SetTextRevealProgress(progress);
    data->manager.Refresh();
    Settle(application);
    DALI_TEST_EQUALS(completions, fullFadePending, TEST_LOCATION);
    DALI_TEST_CHECK(Accessor::IsEntryVisible(data->manager, 1u));
    DALI_TEST_EQUALS(imageRenderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), progress, 0.001f, TEST_LOCATION);
  }
  DALI_TEST_CHECK(complete(fullFadePending));
  DALI_TEST_EQUALS(imageRenderer.GetCurrentProperty<float>(DevelRenderer::Property::OPACITY), 0.0f, TEST_LOCATION);
  const auto fullFadeDisabled = completions;
  label.SetTextReveal(Text::Reveal::None());
  DALI_TEST_CHECK(complete(fullFadeDisabled));

  // Cancel a submitted enable before its timing can be published. None must
  // restore the already loaded image immediately, including after Refresh().
  Text::Reveal pending;
  pending.SetUnit(Text::Reveal::Unit::PIXEL);
  Ui::Integration::Text::Reveal::SetBlurRadius(pending, blur ? 24.0f : 0.0f);
  const auto cancelled = completions;
  label.SetTextReveal(pending);
  Settle(application);
  DALI_TEST_CHECK(!Accessor::IsEntryVisible(data->manager, 1u));
  label.SetTextReveal(Text::Reveal::None());
  data->manager.Refresh();
  Settle(application);
  DALI_TEST_CHECK(Accessor::IsEntryVisible(data->manager, 1u));
  DALI_TEST_CHECK(complete(cancelled));
  DALI_TEST_CHECK(label.GetTextReveal() == Text::Reveal::None());
  DALI_TEST_EQUALS(Accessor::GetRevealConstraintCount(data->manager), 0u, TEST_LOCATION);

  // Disconnect while an unbound image is hidden, then reconnect and accept
  // a new result. The visibility gate adds no owner or task lifetime.
  const auto disconnected = completions;
  label.SetTextReveal(pending);
  Settle(application);
  label.Unparent();
  Settle(application);
  application.GetScene().Add(label);
  DALI_TEST_CHECK(complete(disconnected));
  DALI_TEST_EQUALS(Accessor::GetRevealConstraintCount(data->manager), 1u, TEST_LOCATION);

  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  label.SetTextReveal(pending);
  Settle(application);
  const auto        destroyed = completions;
  WeakHandle<Label> weak(label);
  label.Unparent();
  label.Reset();
  Settle(application);
  DALI_TEST_CHECK(!weak.GetHandle());
  // Dispatch a stale worker wakeup after destruction; it cannot call the
  // former Label's completion signal or recreate its image binding.
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  Settle(application);
  DALI_TEST_EQUALS(completions, destroyed, TEST_LOCATION);
  END_TEST;
}
} // unnamed namespace

int UtcDaliTextRevealAsyncImagePendingP(void)
{
  return CheckAsyncImageRevealPending(false);
}

int UtcDaliTextRevealAsyncBlurImagePendingP(void)
{
  return CheckAsyncImageRevealPending(true);
}

int UtcDaliTextRevealRuntimeGaussianAsyncImageTimingP(void)
{
  UiTestApplication application;
  Label             label    = MakeLabel(application);
  const auto        tasks    = application.GetScene().GetRenderTaskList();
  const auto        baseline = tasks.GetTaskCount();
  label.SetMultiLine(true);
  label.SetRequestedHeight(180.0f);
  Texture      image      = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  Ui::ImageUrl url        = Ui::ImageUrl::New(image, true);
  auto         builder    = Text::StyledTextBuilder::New("A full text line.\n");
  const auto   imageIndex = builder.GetUtf32Length();
  builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
  DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(url.GetUrl(), Vector2(32.0f, 24.0f))),
                                  imageIndex, imageIndex + 1u));
  uint32_t          completions = 0u;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&completions](View, float, float)
  {
    ++completions;
  });
  auto complete = [&](uint32_t previous)
  {
    Settle(application);
    for(int attempt = 0; attempt < 8 && completions == previous; ++attempt)
    {
      if(!Test::WaitForEventThreadTrigger(1, 5))
      {
        return false;
      }
      Settle(application);
    }
    return completions > previous;
  };
  label.SetAsyncRendering(true);
  label.SetStyledText(builder.Build());
  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::LINE);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  reveal.SetFadeDurationRatio(0.0f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 1.0f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.5f);
  DALI_TEST_CHECK(complete(0u));
  auto* data = Ui::Internal::Text::GetInlineReplacementData(label);
  DALI_TEST_CHECK(data);
  using Accessor = Ui::Internal::Text::InlineReplacementManagerTestAccessor;
  Text::ReplacementRevealTiming timing;
  DALI_TEST_CHECK(Accessor::GetRevealTiming(data->manager, 1u, timing));
  // The image-only line participates in the shared blur clock; its blur
  // completion determines the normalized endpoint, just like text lines.
  DALI_TEST_EQUALS(timing.start, 0.2f, 0.0001f, TEST_LOCATION);
  DALI_TEST_CHECK(tasks.GetTaskCount() > baseline && tasks.GetTaskCount() <= baseline + 6u);
  auto horizontal = BlurLines(label);
  DALI_TEST_CHECK(!horizontal.empty());
  DALI_TEST_EQUALS(horizontal.front().Strength(), 0.31640625f, 0.0001f, TEST_LOCATION);
  const auto before = completions;
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 0.0f);
  label.SetTextReveal(reveal);
  DALI_TEST_CHECK(complete(before));
  DALI_TEST_CHECK(Accessor::GetRevealTiming(data->manager, 1u, timing));
  DALI_TEST_EQUALS(timing.start, 0.2f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianPublicShortTailP(void)
{
  UiTestApplication application;
  Label             label = MakeLabel(application);
  label.SetMultiLine(true);
  label.SetFontSize(20.0f);
  label.SetRequestedWidth(600.0f);
  label.SetRequestedHeight(320.0f);
  for(auto unit : {Text::Reveal::Unit::CHARACTER, Text::Reveal::Unit::WORD,
                   Text::Reveal::Unit::LINE, Text::Reveal::Unit::PIXEL})
  {
    Text::Reveal reveal;
    reveal.SetUnit(unit);
    reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
    reveal.SetSequenceStaggerRatio(0.25f);
    reveal.SetFadeDurationRatio(0.0f);
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
    Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
    label.SetTextReveal(reveal);
    for(const char* text : {"Alpha beta gamma.\nAlpha beta gamma.\nAlpha beta gamma.",
                            "Alpha beta gamma.\nAlpha beta gamma.\nZ",
                            "Alpha beta gamma.\nAlpha beta gamma.\nAlpha beta gamma.\nAlpha beta gamma.\nAlpha beta gamma.\nZ"})
    {
      label.SetText(text);
      Settle(application);
      const auto passes = BlurLines(label);
      DALI_TEST_CHECK(passes.size() >= static_cast<size_t>(2u));
      const float firstStart  = passes.front().Start();
      const float secondStart = passes[1u].Start();
      const float reference   = (secondStart - firstStart) / reveal.GetSequenceStaggerRatio();
      const float blur        = reference * Ui::Integration::Text::Reveal::GetBlurDurationRatio(reveal);
      DALI_TEST_CHECK(blur > 0.0f);
      for(auto pass : passes)
      {
        const float start    = pass.Start();
        const float midpoint = start + blur * 0.5f;
        DALI_TEST_CHECK(midpoint < 1.0f);
        label.SetTextRevealProgress(midpoint);
        Settle(application);
        // A short tail and a longer paragraph must use the same reference
        // as stagger, not the whole paragraph or this line's own glyph span.
        DALI_TEST_EQUALS(pass.Strength(),
                         0.5f, 0.0002f, TEST_LOCATION);
      }
    }
  }
  END_TEST;
}

namespace
{
uint32_t CountCapturedImageDraws(Actor actor, Texture image)
{
  if(!actor.GetProperty<bool>(Actor::Property::VISIBLE))
  {
    return 0u;
  }
  uint32_t count = 0u;
  for(uint32_t r = 0u; r < actor.GetRendererCount(); ++r)
  {
    const auto textures = actor.GetRendererAt(r).GetTextures();
    count += textures && textures.GetTextureCount() && textures.GetTexture(0u) == image ? 1u : 0u;
  }
  for(uint32_t c = 0u; c < actor.GetChildCount(); ++c)
  {
    count += CountCapturedImageDraws(actor.GetChildAt(c), image);
  }
  return count;
}

int CheckImageBlurCapture(UiTestApplication& application, bool async, bool imageOnly,
                          Text::Reveal::Unit           unit      = Text::Reveal::Unit::PIXEL,
                          Text::Reveal::Sequence       sequence  = Text::Reveal::Sequence::PER_LINE,
                          bool                         readiness = false,
                          std::vector<WeakHandleBase>* resources = nullptr)
{
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TextAbstraction::FontClient::Get();
  Texture    image   = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  ImageUrl   url     = ImageUrl::New(image, true);
  auto       builder = Text::StyledTextBuilder::New(imageOnly ? "" : "Before ");
  const auto index   = builder.GetUtf32Length();
  builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
  DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(url.GetUrl(), Vector2(64.0f, 64.0f))), index, index + 1u));
  if(!imageOnly)
  {
    builder.AppendText(" after\nSecond line");
  }
  Label label = Label::New();
  label.SetLayoutMode(LayoutMode::STANDALONE);
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(180.0f);
  label.SetMultiLine(true);
  label.SetStyledText(builder.Build());
  label.SetAsyncRendering(async);
  Text::Reveal reveal;
  reveal.SetUnit(unit);
  reveal.SetSequence(sequence);
  reveal.SetSequenceStaggerRatio(0.25f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 1.0f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.4f);
  uint32_t          completed = 0u;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&completed](View, float, float)
  {
    ++completed;
  });
  const auto tasks    = application.GetScene().GetRenderTaskList();
  const auto baseline = tasks.GetTaskCount();
  application.GetScene().Add(label);
  Settle(application);
  if(async)
  {
    for(uint32_t attempt = 0u; attempt < 12u && completed == 0u; ++attempt)
    {
      DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
      Settle(application);
    }
    DALI_TEST_CHECK(completed > 0u);
  }
  // Desired support contract: the resolved image participates in the capture,
  // including a fresh async result and an image-only visual line.
  DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
  DALI_TEST_EQUALS(CountCapturedImageDraws(Find(label, "TextRevealRuntimeGaussian"), image), 1u, TEST_LOCATION);
  const uint32_t maximumTasks = imageOnly || sequence == Text::Reveal::Sequence::WHOLE_TEXT ? 3u : 6u;
  DALI_TEST_CHECK(tasks.GetTaskCount() > baseline && tasks.GetTaskCount() <= baseline + maximumTasks);
  auto                 companion = Find(label, "TextRevealRuntimeGaussian");
  std::vector<Geometry> horizontalGeometries;
  for(auto renderer : Passes(companion, "RevealGaussianBatchH"))
  {
    auto geometry = renderer.GetGeometry();
    horizontalGeometries.push_back(geometry);
    if(resources)
    {
      resources->emplace_back(geometry);
    }
  }
  std::vector<Texture> targets;
  for(uint32_t i = baseline; i < tasks.GetTaskCount(); ++i)
  {
    targets.push_back(tasks.GetTask(i).GetFrameBuffer().GetColorTexture());
    if(resources)
    {
      auto buffer = tasks.GetTask(i).GetFrameBuffer();
      resources->emplace_back(buffer);
      resources->emplace_back(targets.back());
    }
  }
  auto* replacements = Ui::Internal::Text::GetInlineReplacementData(label);
  DALI_TEST_CHECK(replacements);
  auto imageVisual = Ui::Internal::Text::InlineReplacementManagerTestAccessor::GetEntryVisual(replacements->manager, 1u);
  DALI_TEST_CHECK(imageVisual);
  if(readiness)
  {
    // Deterministic delayed readiness; no network, sleeps or resource workload.
    // The Label callback must reconnect a proxy without replacing blur pages.
    Ui::GetImplementation(imageVisual).ResourceReady(Ui::Visual::ResourceStatus::PREPARING);
    label.ResourceReadySignal().Emit(label);
    Settle(application);
    DALI_TEST_EQUALS(CountCapturedImageDraws(companion, image), 0u, TEST_LOCATION);
    Ui::GetImplementation(imageVisual).ResourceReady(Ui::Visual::ResourceStatus::READY);
    label.ResourceReadySignal().Emit(label);
    Settle(application);
    DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian") == companion);
    DALI_TEST_EQUALS(CountCapturedImageDraws(companion, image), 1u, TEST_LOCATION);
  }
  for(float progress : {0.0f, 0.4f, 1.0f, 0.0f})
  {
    label.SetTextRevealProgress(progress);
    Settle(application);
    DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
    DALI_TEST_EQUALS(CountCapturedImageDraws(Find(label, "TextRevealRuntimeGaussian"), image), 1u, TEST_LOCATION);
    label.ResourceReadySignal().Emit(label);
    Settle(application);
    DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian") == companion);
    const auto horizontal = Passes(companion, "RevealGaussianBatchH");
    DALI_TEST_EQUALS(horizontal.size(), horizontalGeometries.size(), TEST_LOCATION);
    for(size_t draw = 0u; draw < horizontal.size(); ++draw)
    {
      DALI_TEST_CHECK(horizontal[draw].GetGeometry() == horizontalGeometries[draw]);
    }
    auto       original      = imageVisual.GetRenderer();
    const auto imageProgress = original.GetPropertyIndex("uInlineReplacementRevealProgress");
    if(imageProgress != Property::INVALID_INDEX)
    {
      // Mirroring a detached source renderer leaves update-side progress stale.
      DALI_TEST_EQUALS(original.GetCurrentProperty<float>(imageProgress), progress, 0.0001f, TEST_LOCATION);
    }
    for(uint32_t i = baseline; i < tasks.GetTaskCount(); ++i)
    {
      DALI_TEST_CHECK(tasks.GetTask(i).GetFrameBuffer().GetColorTexture() == targets[i - baseline]);
    }
  }
  label.SetAsyncRendering(false);
  Settle(application);
  application.GetScene().Remove(label);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  application.GetScene().Add(label);
  Settle(application);
  DALI_TEST_EQUALS(CountCapturedImageDraws(Find(label, "TextRevealRuntimeGaussian"), image), 1u, TEST_LOCATION);
  if(resources)
  {
    for(uint32_t i = baseline; i < tasks.GetTaskCount(); ++i)
    {
      auto buffer  = tasks.GetTask(i).GetFrameBuffer();
      auto texture = buffer.GetColorTexture();
      resources->emplace_back(buffer);
      resources->emplace_back(texture);
    }
  }
  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
  bool restored = false;
  for(uint32_t i = 0u; i < label.GetRendererCount(); ++i)
  {
    const auto textures = label.GetRendererAt(i).GetTextures();
    restored |= textures && textures.GetTextureCount() && textures.GetTexture(0u) == image;
  }
  DALI_TEST_CHECK(restored);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  Ui::GetImplementation(imageVisual).ResourceReady(Ui::Visual::ResourceStatus::READY);
  label.ResourceReadySignal().Emit(label);
  Settle(application);
  DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  if(readiness)
  {
    label.SetTextReveal(reveal);
    Settle(application);
    companion          = Find(label, "TextRevealRuntimeGaussian");
    auto currentVisual = Ui::Internal::Text::InlineReplacementManagerTestAccessor::GetEntryVisual(replacements->manager, 1u);
    DALI_TEST_CHECK(companion && currentVisual);
    Ui::GetImplementation(currentVisual).ResourceReady(Ui::Visual::ResourceStatus::FAILED);
    label.ResourceReadySignal().Emit(label);
    Settle(application);
    DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian") == companion);
    DALI_TEST_EQUALS(CountCapturedImageDraws(companion, image), 0u, TEST_LOCATION);
    label.SetTextReveal(Text::Reveal::None());
    Settle(application);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  }
  application.GetScene().Remove(label);
  Settle(application);
  END_TEST;
}
} // namespace

int UtcDaliTextRevealRuntimeOutputBatchImageOrderP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  Texture  image   = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
  ImageUrl url     = ImageUrl::New(image, true);
  auto     builder = Text::StyledTextBuilder::New("");
  for(uint32_t line = 0u; line < 6u; ++line)
  {
    if(line != 0u)
    {
      builder.AppendText("\n");
    }
    builder.AppendText("A ");
    if(line == 1u || line == 4u)
    {
      const auto index = builder.GetUtf32Length();
      builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
      DALI_TEST_CHECK(builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(url.GetUrl(), Vector2(24.0f, 24.0f))), index, index + 1u));
    }
  }
  Label label = Label::New();
  label.SetLayoutMode(LayoutMode::STANDALONE);
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(300.0f);
  label.SetMultiLine(true);
  label.SetFontSize(16.0f);
  label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
  label.SetLineHeight(40.0f);
  label.SetStyledText(builder.Build());
  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::PIXEL);
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  reveal.SetSequenceStaggerRatio(0.25f);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
  label.SetTextReveal(reveal);
  application.GetScene().Add(label);
  Settle(application);
  const auto tasks   = application.GetScene().GetRenderTaskList();
  auto       outputs = Passes(label, "RevealGaussianOutput");
  // A8 lines 0 / 2,3 / 5 interleave with RGBA lines 1 / 4. Only the
  // contiguous 2,3 range may combine, with indices 1,2 in the A8 H/V draw.
  DALI_TEST_EQUALS(outputs.size(), static_cast<size_t>(5u), TEST_LOCATION);
  DALI_TEST_CHECK(outputs[2u].GetPropertyIndex("uRevealBlurStrength[0]") == Property::INVALID_INDEX);
  DALI_TEST_CHECK(outputs[2u].GetPropertyIndex("uRevealBlurStrength[1]") != Property::INVALID_INDEX);
  DALI_TEST_CHECK(outputs[2u].GetPropertyIndex("uRevealBlurStrength[2]") != Property::INVALID_INDEX);
  for(float progress : {0.0f, 0.2f, 0.6f, 1.0f, 0.4f, 0.0f})
  {
    label.SetTextRevealProgress(progress);
    Settle(application);
    float previousStart = -1.0f;
    for(auto output : outputs)
    {
      Renderer vertical;
      for(uint32_t task = 1u; task < tasks.GetTaskCount(); ++task)
      {
        auto candidate = tasks.GetTask(task);
        if(candidate.GetFrameBuffer().GetColorTexture() == output.GetTextures().GetTexture(0u))
        {
          vertical = candidate.GetSourceActor().GetRendererAt(0u);
          break;
        }
      }
      DALI_TEST_CHECK(vertical);
      for(uint32_t line = 0u; line < Ui::Internal::TextRevealBlurRenderer::MAX_LINES_PER_DRAW; ++line)
      {
        const std::string suffix = "[" + std::to_string(line) + "]";
        const auto        index  = output.GetPropertyIndex(("uRevealBlurStrength" + suffix).c_str());
        if(index != Property::INVALID_INDEX)
        {
          const auto state = vertical.GetCurrentProperty<Vector2>(vertical.GetPropertyIndex(("uRevealBlurState" + suffix).c_str()));
          DALI_TEST_EQUALS(output.GetCurrentProperty<float>(index), state.x, 0.0001f, TEST_LOCATION);
          DALI_TEST_CHECK(state.y >= previousStart);
          previousStart = state.y;
        }
      }
    }
  }
  std::vector<WeakHandleBase> retired;
  for(auto output : outputs)
  {
    retired.emplace_back(output);
    auto geometry = output.GetGeometry();
    retired.emplace_back(geometry);
  }
  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  outputs.clear();
  for(const auto& object : retired)
  {
    DALI_TEST_CHECK(!object.GetBaseHandle());
  }
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealBlurImageCaptureReadyP(void)
{
  UiTestApplication application;
  return CheckImageBlurCapture(application, false, false);
}

int UtcDaliTextRevealBlurImageCaptureAsyncP(void)
{
  UiTestApplication application;
  return CheckImageBlurCapture(application, true, false);
}

int UtcDaliTextRevealBlurImageCaptureImageOnlyP(void)
{
  UiTestApplication application;
  for(bool async : {false, true})
  {
    for(auto sequence : {Text::Reveal::Sequence::WHOLE_TEXT, Text::Reveal::Sequence::PER_LINE})
    {
      if(CheckImageBlurCapture(application, async, true, Text::Reveal::Unit::PIXEL, sequence))
      {
        return 1;
      }
    }
  }
  END_TEST;
}

int UtcDaliTextRevealBlurImageCaptureReadinessP(void)
{
  UiTestApplication application;
  for(auto unit : {Text::Reveal::Unit::CHARACTER, Text::Reveal::Unit::WORD, Text::Reveal::Unit::LINE, Text::Reveal::Unit::PIXEL})
  {
    if(CheckImageBlurCapture(application, true, false, unit, Text::Reveal::Sequence::PER_LINE, true))
    {
      return 1;
    }
  }
  END_TEST;
}

namespace
{
enum class OwnershipStressCase
{
  A8,
  FORMAT,
  BOUNDARY
};

int CheckRuntimeGaussianOwnershipStress(OwnershipStressCase testCase)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TextAbstraction::FontClient::Get();
  const auto                 tasks = application.GetScene().GetRenderTaskList();
  std::vector<WeakHandleBase> objects;
  auto                        cycle = [&](bool churn, uint32_t lineCount) -> int
  {
    const bool  multiPage = lineCount == 0u;
    std::string text;
    for(uint32_t line = 0u; line < lineCount; ++line)
    {
      if(line != 0u)
      {
        text += '\n';
      }
      text += lineCount > 6u ? "A" : "A complete line";
    }
    if(multiPage)
    {
      text = "A complete line\nI\nA complete line\nI\nA complete line";
    }
    Label label = Label::New(text.c_str());
    label.SetLayoutMode(LayoutMode::STANDALONE);
    // Keep the 64/65-line ownership fixture below the preparation memory cap.
    label.SetRequestedWidth(multiPage ? 600.0f : (lineCount > 6u ? 64.0f : 360.0f));
    label.SetRequestedHeight(multiPage ? 400.0f : static_cast<float>(lineCount) * 12.0f + 24.0f);
    label.SetFontSize(multiPage ? 20.0f : 8.0f);
    label.SetMultiLine(true);
    label.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
    label.SetLineHeight(multiPage ? 30.0f : 12.0f);
    application.GetScene().Add(label);
    Text::Reveal reveal;
    reveal.SetUnit(Text::Reveal::Unit::PIXEL);
    reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
    reveal.SetSequenceStaggerRatio(0.25f);
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, multiPage ? 32.0f : (lineCount > 6u ? 8.0f : 24.0f));
    Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
    gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
    for(uint32_t state = 0u; state < (churn ? 8u : 1u); ++state)
    {
      auto              previousHandle = Find(label, "TextRevealRuntimeGaussian");
      WeakHandle<Actor> previous(previousHandle);
      previousHandle.Reset();
      label.SetTextGradient(state == 1u ? gradient : Gradient::Base::None());
      label.SetTextGradientOverlay(state == 3u ? gradient : Gradient::Base::None());
      if(state == 5u)
      {
        label.SetStyledText(Text::StyledText::FromMarkup("Plain <color value='red'>colored</color> text\nAnother line"));
      }
      else if(state == 7u)
      {
        // Change one live publication from six lines through two to 65.
        std::string boundaryText("A");
        for(uint32_t line = 1u; line < 65u; ++line)
        {
          boundaryText += "\nA";
        }
        label.SetRequestedWidth(64.0f);
        label.SetRequestedHeight(804.0f);
        label.SetText(boundaryText.c_str());
      }
      else
      {
        label.SetText(text.c_str());
      }
      // Exercise every colored format in PERFORMANCE; interleave QUALITY A8.
      Ui::Integration::Text::Reveal::SetBlurMode(reveal, state != 0u && state % 2u == 0u ? Ui::Integration::Text::Reveal::BlurMode::QUALITY : Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE);
      reveal.SetSequence(state == 6u ? Text::Reveal::Sequence::WHOLE_TEXT : Text::Reveal::Sequence::PER_LINE);
      label.SetTextReveal(reveal);
      label.SetTextRevealProgress(state % 2u ? 0.0f : 0.4f);
      Settle(application);
      application.RunIdles();
      Settle(application);
      DALI_TEST_CHECK(!previous.GetHandle());
      DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
      DALI_TEST_CHECK(tasks.GetTaskCount() > 1u);
      if(multiPage)
      {
        DALI_TEST_CHECK(tasks.GetTaskCount() > 4u);
      }
      for(uint32_t index = 1u; index < tasks.GetTaskCount(); ++index)
      {
        auto texture = tasks.GetTask(index).GetFrameBuffer().GetColorTexture();
        objects.emplace_back(texture);
      }
      label.SetTextRevealProgress(1.0f);
      Settle(application);
      label.SetTextRevealProgress(0.4f);
      Settle(application);
    }
    auto              retiredHandle = Find(label, "TextRevealRuntimeGaussian");
    WeakHandle<Actor> retired(retiredHandle);
    retiredHandle.Reset();
    label.SetTextReveal(Text::Reveal::None());
    Settle(application);
    DALI_TEST_CHECK(!retired.GetHandle());
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    label.Unparent();
    label.Reset();
    application.RunIdles();
    Settle(application);
    return 0;
  };
  // Warm shared shaders, quad geometry and gradient resources before tracking.
  DALI_TEST_EQUALS(cycle(true, 6u), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(cycle(false, 65u), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(cycle(false, 0u), 0, TEST_LOCATION);
  objects.clear();
  const auto        textureBaseline = application.GetGlAbstraction().GetNumGeneratedTextures();
  const auto        bufferBaseline  = application.GetGraphicsController().mAllocatedBuffers.size();
  ConnectionTracker tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(Actor::DownCast(object) || Renderer::DownCast(object) || Geometry::DownCast(object) ||
       VertexBuffer::DownCast(object) || FrameBuffer::DownCast(object) || Constraint::DownCast(object))
    {
      objects.emplace_back(object);
    }
  });
  // Bound repeated work per UTC without dropping any format or page transition.
  const uint32_t iterations = testCase == OwnershipStressCase::A8 ? 4u : (testCase == OwnershipStressCase::FORMAT ? 2u : 5u);
  for(uint32_t iteration = 0u; iteration < iterations; ++iteration)
  {
    const uint32_t boundaries[] = {1u, 2u, 64u, 65u, 0u};
    DALI_TEST_EQUALS(cycle(testCase == OwnershipStressCase::FORMAT,
                           testCase == OwnershipStressCase::BOUNDARY ? boundaries[iteration] : 6u),
                     0, TEST_LOCATION);
    DALI_TEST_CHECK(!objects.empty());
    for(const auto& object : objects)
    {
      DALI_TEST_CHECK(!object.GetBaseHandle());
    }
    objects.clear();
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().GetNumGeneratedTextures(), textureBaseline, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGraphicsController().mAllocatedBuffers.size(), bufferBaseline, TEST_LOCATION);
  }
  std::printf("%u ownership cycles: textures %u -> %u; buffers %zu -> %zu; weak objects 0; tasks 1\n",
              iterations, textureBaseline, application.GetGlAbstraction().GetNumGeneratedTextures(),
              bufferBaseline, application.GetGraphicsController().mAllocatedBuffers.size());
  END_TEST;
}
} // namespace

int UtcDaliTextRevealRuntimeGaussianOwnershipStressP(void)
{
  return CheckRuntimeGaussianOwnershipStress(OwnershipStressCase::A8);
}

int UtcDaliTextRevealRuntimeGaussianFormatOwnershipStressP(void)
{
  return CheckRuntimeGaussianOwnershipStress(OwnershipStressCase::FORMAT);
}

int UtcDaliTextRevealRuntimeGaussianBoundaryOwnershipStressP(void)
{
  return CheckRuntimeGaussianOwnershipStress(OwnershipStressCase::BOUNDARY);
}

namespace
{
int CheckImageBlurCaptureLifecycleStress(bool async, Text::Reveal::Sequence sequence)
{
  UiTestApplication application;
  if(CheckImageBlurCapture(application, true, false, Text::Reveal::Unit::PIXEL,
                           Text::Reveal::Sequence::PER_LINE, true))
  {
    return 1;
  }
  if(CheckImageBlurCapture(application, false, false, Text::Reveal::Unit::PIXEL,
                           Text::Reveal::Sequence::WHOLE_TEXT, true))
  {
    return 1;
  }
  application.RunIdles();
  Settle(application);
  const auto                  textureBaseline = application.GetGlAbstraction().GetNumGeneratedTextures();
  const auto                  bufferBaseline  = application.GetGraphicsController().mAllocatedBuffers.size();
  std::vector<WeakHandleBase> objects;
  ConnectionTracker           tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle object)
  {
    if(Actor::DownCast(object) || Renderer::DownCast(object) || FrameBuffer::DownCast(object) || Constraint::DownCast(object) ||
       Geometry::DownCast(object) || VertexBuffer::DownCast(object))
    {
      objects.emplace_back(object);
    }
  });
  // Preserve the former twelve-cycle matrix across four independent UTCs.
  const uint32_t cycles = sequence == Text::Reveal::Sequence::WHOLE_TEXT ? 2u : 4u;
  for(uint32_t cycle = 0u; cycle < cycles; ++cycle)
  {
    if(CheckImageBlurCapture(application, async, false, Text::Reveal::Unit::PIXEL, sequence, true, &objects))
    {
      return 1;
    }
    application.RunIdles();
    Settle(application);
    DALI_TEST_CHECK(!objects.empty());
    for(const auto& object : objects)
    {
      DALI_TEST_CHECK(!object.GetBaseHandle());
    }
    objects.clear();
    DALI_TEST_EQUALS(application.GetScene().GetRenderTaskList().GetTaskCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGlAbstraction().GetNumGeneratedTextures(), textureBaseline, TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetGraphicsController().mAllocatedBuffers.size(), bufferBaseline, TEST_LOCATION);
  }
  std::printf("%u lifecycle cycles: textures %u -> %u; graphics buffers %zu -> %zu; tracked objects 0; tasks 1\n",
              cycles, textureBaseline, application.GetGlAbstraction().GetNumGeneratedTextures(),
              bufferBaseline, application.GetGraphicsController().mAllocatedBuffers.size());
  END_TEST;
}
} // namespace

int UtcDaliTextRevealBlurImageCaptureLifecycleStressP(void)
{
  return CheckImageBlurCaptureLifecycleStress(false, Text::Reveal::Sequence::WHOLE_TEXT);
}

int UtcDaliTextRevealBlurImageCaptureLifecyclePerLineStressP(void)
{
  return CheckImageBlurCaptureLifecycleStress(false, Text::Reveal::Sequence::PER_LINE);
}

int UtcDaliTextRevealBlurImageCaptureAsyncLifecycleStressP(void)
{
  return CheckImageBlurCaptureLifecycleStress(true, Text::Reveal::Sequence::WHOLE_TEXT);
}

int UtcDaliTextRevealBlurImageCaptureAsyncLifecyclePerLineStressP(void)
{
  return CheckImageBlurCaptureLifecycleStress(true, Text::Reveal::Sequence::PER_LINE);
}

int UtcDaliTextRevealBlurImageCaptureOwnershipP(void)
{
  UiTestApplication application;
  // Warm shared framework resources before tracking per-Label ownership.
  if(CheckImageBlurCapture(application, false, false))
  {
    return 1;
  }
  application.RunIdles();
  Settle(application);

  std::vector<WeakHandleBase> objects;
  ConnectionTracker           tracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&objects](BaseHandle object)
  {
    if(Actor::DownCast(object) || Renderer::DownCast(object) || FrameBuffer::DownCast(object))
    {
      objects.emplace_back(object);
    }
  });
  for(bool async : {false, true})
  {
    for(auto sequence : {Text::Reveal::Sequence::WHOLE_TEXT, Text::Reveal::Sequence::PER_LINE})
    {
      if(CheckImageBlurCapture(application, async, false, Text::Reveal::Unit::PIXEL, sequence, true))
      {
        return 1;
      }
      // VisualFactory discards old visuals on idle, not on Render alone.
      // After the helper's handles leave scope, only weak references remain.
      application.RunIdles();
      Settle(application);
      DALI_TEST_CHECK(!objects.empty());
      for(const auto& object : objects)
      {
        DALI_TEST_CHECK(!object.GetBaseHandle());
      }
      objects.clear();
    }
  }
  END_TEST;
}

namespace
{
int CheckImageBlurDirectDestruction(bool async, bool shutdown)
{
  UiTestApplication application;
  // Warm only shared resources; everything tracked below belongs to the
  // new Label. No remote server or explicit Reveal::None() is involved.
  if(CheckImageBlurCapture(application, false, false))
  {
    return 1;
  }
  application.RunIdles();
  Settle(application);
  std::vector<WeakHandleBase> objects;
  ConnectionTracker           objectTracker;
  application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&objectTracker, [&objects](BaseHandle object)
  {
    if(Actor::DownCast(object) || Renderer::DownCast(object) || FrameBuffer::DownCast(object) ||
       Geometry::DownCast(object) || VertexBuffer::DownCast(object) || Constraint::DownCast(object))
    {
      objects.emplace_back(object);
    }
  });
  const auto textureBaseline = application.GetGlAbstraction().GetNumGeneratedTextures();
  const auto bufferBaseline  = application.GetGraphicsController().mAllocatedBuffers.size();
  auto       destroy         = [&](bool imageSpan) -> int
  {
    ConnectionTracker tracker;
    Label             label = Label::New();
    label.SetLayoutMode(LayoutMode::STANDALONE);
    label.SetRequestedWidth(360.0f);
    label.SetRequestedHeight(180.0f);
    label.SetMultiLine(true);
    label.SetAsyncRendering(async);
    Texture  image;
    ImageUrl url;
    if(imageSpan)
    {
      image            = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
      url              = ImageUrl::New(image, true);
      auto       text  = Text::StyledTextBuilder::New("Before ");
      const auto index = text.GetUtf32Length();
      text.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
      text.AppendText(" after\nSecond line");
      DALI_TEST_CHECK(text.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(url.GetUrl(), Vector2(64.0f, 48.0f))), index, index + 1u));
      label.SetStyledText(text.Build());
    }
    else
    {
      label.SetText("Plain text\nSecond line");
    }
    Text::Reveal reveal;
    reveal.SetUnit(Text::Reveal::Unit::PIXEL);
    reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
    reveal.SetSequenceStaggerRatio(0.25f);
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
    label.SetTextReveal(reveal);
    label.SetTextRevealProgress(0.4f);
    uint32_t completions = 0u;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&completions](View, float, float)
    {
      ++completions;
    });
    application.GetScene().Add(label);
    Settle(application);
    for(int attempt = 0; async && completions == 0u && attempt < 8; ++attempt)
    {
      DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
      Settle(application);
    }
    DALI_TEST_CHECK(!async || completions > 0u);
    DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
    const auto tasks = application.GetScene().GetRenderTaskList();
    DALI_TEST_CHECK(tasks.GetTaskCount() > 1u);
    if(async)
    {
      // Leave a replacement request pending while the active image capture
      // and its owner are destroyed; the old publication still owns tasks.
      Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 48.0f);
      label.SetTextReveal(reveal);
      Settle(application);
    }
    const auto completedBeforeDestruction = completions;
    if(shutdown)
    {
      application.GetAdaptor().Stop();
    }
    label.Unparent();
    label.Reset();
    tracker.DisconnectAll();
    if(!shutdown)
    {
      application.RunIdles();
      Settle(application);
    }
    // Shutdown cleanup cannot rely on an idle or a finished callback.
    DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(completions, completedBeforeDestruction, TEST_LOCATION);
    DALI_TEST_CHECK(!objects.empty());
    for(const auto& object : objects)
    {
      DALI_TEST_CHECK(!object.GetBaseHandle());
    }
    return 0;
  };
  // Repeated plain/image destruction covers pending replacement and resource
  // recovery without turning a functional UTC into a long-running soak test.
  const uint32_t cycles = shutdown ? 1u : (async ? 8u : 2u);
  for(uint32_t cycle = 0u; cycle < cycles; ++cycle)
  {
    // Also destroy a plain-text publication directly, without None first.
    DALI_TEST_EQUALS(destroy(shutdown || cycle % 2u != 0u), 0, TEST_LOCATION);
    objects.clear();
    if(!shutdown)
    {
      application.RunIdles();
      Settle(application);
      DALI_TEST_EQUALS(application.GetGlAbstraction().GetNumGeneratedTextures(), textureBaseline, TEST_LOCATION);
      DALI_TEST_EQUALS(application.GetGraphicsController().mAllocatedBuffers.size(), bufferBaseline, TEST_LOCATION);
    }
  }
  std::printf("%u direct destruction cycles (async=%d, shutdown=%d): weak objects 0; tasks 1; textures %u -> %u; buffers %zu -> %zu\n",
              cycles, async, shutdown, textureBaseline, application.GetGlAbstraction().GetNumGeneratedTextures(),
              bufferBaseline, application.GetGraphicsController().mAllocatedBuffers.size());
  END_TEST;
}
} // unnamed namespace

int UtcDaliTextRevealBlurImageCaptureDirectDestructionP(void)
{
  return CheckImageBlurDirectDestruction(false, false);
}

int UtcDaliTextRevealBlurImageCaptureShutdownP(void)
{
  return CheckImageBlurDirectDestruction(false, true);
}

int UtcDaliTextRevealBlurImageCaptureAsyncDestructionP(void)
{
  return CheckImageBlurDirectDestruction(true, false);
}

int UtcDaliTextRevealBlurImageCaptureAsyncShutdownP(void)
{
  return CheckImageBlurDirectDestruction(true, true);
}

int UtcDaliTextRevealBlurImageCaptureReplacementOwnershipP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  Label label = Label::New();
  label.SetLayoutMode(LayoutMode::STANDALONE);
  label.SetRequestedWidth(360.0f);
  label.SetRequestedHeight(180.0f);
  label.SetMultiLine(true);
  application.GetScene().Add(label);
  const auto tasks    = application.GetScene().GetRenderTaskList();
  const auto baseline = tasks.GetTaskCount();

  uint32_t          completed = 0u;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&completed](View, float, float)
  {
    ++completed;
  });
  auto complete = [&](uint32_t before)
  {
    Settle(application);
    for(uint32_t attempt = 0u; attempt < 12u && completed == before; ++attempt)
    {
      if(!Test::WaitForEventThreadTrigger(1, 5))
      {
        return false;
      }
      Settle(application);
    }
    return completed > before;
  };

  std::vector<WeakHandleBase> retired;
  for(uint32_t iteration = 0u; iteration < 4u; ++iteration)
  {
    const bool async = iteration >= 2u;
    Texture    image = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 4u, 4u);
    ImageUrl   url   = ImageUrl::New(image, true);
    auto       text  = Text::StyledTextBuilder::New("Before ");
    const auto index = text.GetUtf32Length();
    text.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
    const Vector2 imageSize(iteration % 2u ? 96.0f : 64.0f, 64.0f);
    DALI_TEST_CHECK(text.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(url.GetUrl(), imageSize)), index, index + 1u));
    text.AppendText(iteration % 2u ? "\nAfter replacement" : " after\nSecond line");
    Text::Reveal reveal;
    reveal.SetUnit(Text::Reveal::Unit::PIXEL);
    reveal.SetSequence(iteration % 2u ? Text::Reveal::Sequence::PER_LINE : Text::Reveal::Sequence::WHOLE_TEXT);
    reveal.SetSequenceStaggerRatio(iteration % 2u ? 0.25f : 0.0f);
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, iteration % 2u ? 24.0f : 48.0f);
    Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, iteration % 2u ? 0.5f : 1.0f);
    const auto before = completed;
    label.SetAsyncRendering(async);
    label.SetStyledText(text.Build());
    label.SetTextReveal(reveal);
    label.SetTextRevealProgress(0.4f);
    if(async)
    {
      DALI_TEST_CHECK(complete(before));
    }
    else
    {
      Settle(application);
    }
    application.RunIdles();
    Settle(application);
    // Each replacement must retire the previous capture, not merely hide it.
    for(const auto& object : retired)
    {
      DALI_TEST_CHECK(!object.GetBaseHandle());
    }
    auto companion = Find(label, "TextRevealRuntimeGaussian");
    DALI_TEST_CHECK(companion);
    DALI_TEST_EQUALS(CountCapturedImageDraws(companion, image), 1u, TEST_LOCATION);
    retired.emplace_back(companion);
    // Track only capture proxies: original image/text renderers may correctly
    // survive None on the Label. The separate ownership UTC covers destruction.
    auto trackProxies = [&](auto&& visit, Actor actor) -> void
    {
      if(!actor.GetProperty<bool>(Actor::Property::VISIBLE))
      {
        return;
      }
      for(uint32_t i = 0u; i < actor.GetRendererCount(); ++i)
      {
        auto       renderer = actor.GetRendererAt(i);
        const auto textures = renderer.GetTextures();
        if(textures && textures.GetTextureCount() && textures.GetTexture(0u) == image)
        {
          retired.emplace_back(renderer);
        }
      }
      for(uint32_t i = 0u; i < actor.GetChildCount(); ++i)
      {
        visit(visit, actor.GetChildAt(i));
      }
    };
    trackProxies(trackProxies, companion);
    for(uint32_t i = baseline; i < tasks.GetTaskCount(); ++i)
    {
      auto buffer = tasks.GetTask(i).GetFrameBuffer();
      DALI_TEST_CHECK(buffer);
      auto texture = buffer.GetColorTexture();
      DALI_TEST_CHECK(texture);
      retired.emplace_back(buffer);
      retired.emplace_back(texture);
    }
  }

  const auto beforeNone = completed;
  label.SetTextReveal(Text::Reveal::None());
  DALI_TEST_CHECK(complete(beforeNone));
  label.ResourceReadySignal().Emit(label);
  application.RunIdles();
  Settle(application);
  DALI_TEST_CHECK(label.GetTextReveal() == Text::Reveal::None());
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  for(const auto& object : retired)
  {
    DALI_TEST_CHECK(!object.GetBaseHandle());
  }
  WeakHandle<Label> weak(label);
  label.Unparent();
  label.Reset();
  application.RunIdles();
  Settle(application);
  DALI_TEST_CHECK(!weak.GetHandle());
  END_TEST;
}

namespace
{
int CheckRuntimeGaussianDecorations(bool async)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TextAbstraction::FontClient::Get();
  const auto tasks    = application.GetScene().GetRenderTaskList();
  const auto baseline = tasks.GetTaskCount();
  for(auto sequence : {Text::Reveal::Sequence::WHOLE_TEXT, Text::Reveal::Sequence::PER_LINE})
  {
    Label label = Label::New("AVATAR ffi office\nA short final line");
    label.SetLayoutMode(LayoutMode::STANDALONE);
    label.SetRequestedWidth(360.0f);
    label.SetRequestedHeight(160.0f);
    label.SetMultiLine(true);
    label.SetAsyncRendering(async);
    label.SetProperty(Actor::Property::COLOR_ALPHA, 0.35f);
    Text::Reveal reveal;
    reveal.SetUnit(Text::Reveal::Unit::PIXEL);
    reveal.SetSequence(sequence);
    reveal.SetSequenceStaggerRatio(0.25f);
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
    label.SetTextReveal(reveal);
    uint32_t          completed = 0u;
    ConnectionTracker tracker;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&completed](View, float, float)
    {
      ++completed;
    });
    application.GetScene().Add(label);
    for(uint32_t style : {3u, 1u, 2u, 0u, 3u})
    {
      const auto   previous = completed;
      Text::Shadow shadow;
      shadow.SetColor(UiColor(0xEE2255, 0.6f));
      shadow.SetOffset(Vector2(4.0f, 3.0f));
      Text::Underline underline;
      underline.SetColor(UiColor(0x22EE66, 0.7f));
      label.SetTextShadow((style & 1u) ? shadow : Text::Shadow::None());
      label.SetTextUnderline((style & 2u) ? underline : Text::Underline::None());
      Settle(application);
      if(async)
      {
        for(uint32_t attempt = 0u; attempt < 12u && completed == previous; ++attempt)
        {
          DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
          Settle(application);
        }
        DALI_TEST_CHECK(completed > previous);
      }
      const auto composition = Find(label, "RevealGaussianDecorationComposition");
      DALI_TEST_EQUALS(static_cast<bool>(composition), style != 0u, TEST_LOCATION);
      DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + (style ? 4u : 3u), TEST_LOCATION);
      std::vector<Texture> planes;
      for(const char* name : {"RevealGaussianBackground", "RevealGaussianOverlay"})
      {
        auto plane = Find(label, name);
        if(plane)
        {
          DALI_TEST_EQUALS(plane.GetRendererCount(), 1u, TEST_LOCATION);
          const auto texture = plane.GetRendererAt(0u).GetTextures().GetTexture(0u);
          DALI_TEST_EQUALS(texture.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);
          planes.push_back(texture);
        }
      }
      for(float progress : {0.0f, 0.4f, 1.0f, 0.4f, 0.0f})
      {
        label.SetTextRevealProgress(progress);
        Settle(application);
        DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + (style ? 4u : 3u), TEST_LOCATION);
        DALI_TEST_CHECK(Find(label, "RevealGaussianDecorationComposition") == composition);
        size_t index = 0u;
        for(const char* name : {"RevealGaussianBackground", "RevealGaussianOverlay"})
        {
          auto plane = Find(label, name);
          if(plane)
          {
            DALI_TEST_CHECK(plane.GetRendererAt(0u).GetTextures().GetTexture(0u) == planes[index++]);
          }
        }
      }
    }
    label.SetAsyncRendering(false);
    label.SetTextReveal(Text::Reveal::None());
    Settle(application);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
    DALI_TEST_CHECK(!Find(label, "RevealGaussianDecorationComposition"));
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    label.Unparent();
    Settle(application);
  }
  END_TEST;
}
} // namespace

int UtcDaliTextRevealRuntimeGaussianDecorationsP(void)
{
  return CheckRuntimeGaussianDecorations(false);
}

int UtcDaliTextRevealRuntimeGaussianDecorationsAsyncP(void)
{
  return CheckRuntimeGaussianDecorations(true);
}

namespace
{
int CheckRuntimeGaussianScale(bool async)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  TextAbstraction::FontClient::Get();
  const auto tasks    = application.GetScene().GetRenderTaskList();
  const auto baseline = tasks.GetTaskCount();
  for(auto sequence : {Text::Reveal::Sequence::WHOLE_TEXT, Text::Reveal::Sequence::PER_LINE})
  {
    Label label = Label::New("AVATAR ffi office\nA short final line");
    label.SetLayoutMode(LayoutMode::STANDALONE);
    label.SetRequestedWidth(360.0f);
    label.SetRequestedHeight(160.0f);
    label.SetPadding(Insets(7.0f, 11.0f, 5.0f, 9.0f));
    label.SetMultiLine(true);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetAsyncRendering(async);
    Text::Reveal reveal;
    reveal.SetUnit(Text::Reveal::Unit::PIXEL);
    reveal.SetSequence(sequence);
    reveal.SetSequenceStaggerRatio(0.25f);
    reveal.SetFadeDurationRatio(0.0f);
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
    label.SetTextReveal(reveal);
    uint32_t          completed = 0u;
    ConnectionTracker tracker;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&completed](View, float, float)
    {
      ++completed;
    });
    application.GetScene().Add(label);
    for(float scale : {0.8f, 1.0f, 1.2f, 1.25f, 1.4f, 2.0f, 1.0f})
    {
      const auto previous = completed;
      UiScaleManager::Get().SetScale(scale);
      // Include async supersampling with fractional UI scale and return to
      // ordinary sampling. Layout/placement updates retain one blur task group.
      if(async)
      {
        label.SetRenderScale(scale == 1.25f ? 1.5f : 1.0f);
      }
      Settle(application);
      if(async)
      {
        for(uint32_t attempt = 0u; attempt < 12u && completed == previous; ++attempt)
        {
          DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
          Settle(application);
        }
        DALI_TEST_CHECK(completed > previous);
      }
      const auto foreground = Find(label, "RevealGaussianForeground");
      DALI_TEST_CHECK(foreground);
      // Capture must use the owner's actual size, not the async constraint
      // rounded before fractional padding is restored.
      DALI_TEST_EQUALS(foreground.GetCurrentProperty<Vector3>(Actor::Property::SIZE),
                       label.GetCurrentProperty<Vector3>(Actor::Property::SIZE), 0.0001f, TEST_LOCATION);
      DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
      for(auto output : Passes(label, "RevealGaussianOutput"))
      {
        const auto radiusIndex = output.GetPropertyIndex("uQuarterAuthoredRadius");
        if(radiusIndex != Property::INVALID_INDEX)
        {
          // Render scale changes raster density, not the display-pixel handoff.
          DALI_TEST_EQUALS(output.GetProperty<float>(radiusIndex), 16.0f * scale, 0.0001f, TEST_LOCATION);
        }
      }
      for(float progress : {0.0f, 0.4f, 1.0f, 0.4f, 0.0f})
      {
        label.SetTextRevealProgress(progress);
        Settle(application);
        DALI_TEST_EQUALS(label.GetTextRevealProgress(), progress, 0.0001f, TEST_LOCATION);
        DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
      }
    }
    label.SetTextReveal(Text::Reveal::None());
    label.Unparent();
    Settle(application);
    DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  }
  UiScaleManager::Get().SetScale(1.0f);
  END_TEST;
}
} // namespace

int UtcDaliTextRevealRuntimeGaussianScaleP(void)
{
  return CheckRuntimeGaussianScale(false);
}

int UtcDaliTextRevealRuntimeGaussianScaleAsyncP(void)
{
  return CheckRuntimeGaussianScale(true);
}

int UtcDaliTextRevealRuntimeGaussianPublicFallbackP(void)
{
  UiTestApplication application;
  Label             label    = MakeLabel(application);
  const auto        tasks    = application.GetScene().GetRenderTaskList();
  const auto        baseline = tasks.GetTaskCount();
  Text::Reveal      reveal;
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.4f);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  UiScaleManager::Get().SetScale(1.25f);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  DALI_TEST_CHECK(Find(label, "RevealGaussianForeground"));
  DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  UiScaleManager::Get().SetScale(1.0f);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);

  bool              finished = false;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&finished](View, float, float)
  {
    finished = true;
  });
  label.SetAsyncRendering(true);
  label.SetText("Async Reveal blur publication");
  Settle(application);
  for(int completion = 0; completion < 4 && !finished; ++completion)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    Settle(application);
  }
  DALI_TEST_CHECK(finished);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  DALI_TEST_CHECK(Find(label, "RevealGaussianForeground"));
  DALI_TEST_CHECK(label.GetTextReveal() == reveal);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.4f, 0.0001f, TEST_LOCATION);
  label.SetAsyncRendering(false);
  label.SetText("Back to synchronous blur");
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.4f, 0.0001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealBlurHeightTilingFallbackP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  const auto     tasks    = application.GetScene().GetRenderTaskList();
  const auto     baseline = tasks.GetTaskCount();
  const uint32_t maximum  = static_cast<uint32_t>(Dali::GetMaxTextureSize());
  DALI_TEST_CHECK(maximum > 0u);
  std::string text;
  for(uint32_t line = 0u; line < maximum / 4u + 64u; ++line)
  {
    text += "X\n";
  }
  for(bool async : {false, true})
  {
    for(auto unit : {Text::Reveal::Unit::CHARACTER, Text::Reveal::Unit::PIXEL})
    {
      Label label = MakeLabel(application);
      label.SetMultiLine(true);
      label.SetFontSize(32.0f);
      label.SetRequestedWidth(96.0f);
      label.SetRequestedHeight(128.0f);
      label.SetAsyncRendering(async);
      Text::Reveal reveal;
      reveal.SetUnit(unit);
      reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
      reveal.SetFadeDurationRatio(0.25f);
      Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
      label.SetTextReveal(reveal);
      label.SetTextRevealProgress(0.4f);
      const auto        progressIndex = label.GetPropertyIndex("uTextRevealProgress");
      bool              finished      = false;
      ConnectionTracker tracker;
      label.AsyncRenderFinishedSignal().Connect(&tracker, [&](View, float, float)
      {
        finished = true;
      });
      auto complete = [&]()
      {
        Settle(application);
        for(int attempt = 0; async && !finished && attempt < 8; ++attempt)
        {
          if(!Test::WaitForEventThreadTrigger(1, 5))
          {
            return false;
          }
          Settle(application);
        }
        return !async || finished;
      };
      DALI_TEST_CHECK(complete());
      DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
      DALI_TEST_CHECK(tasks.GetTaskCount() > baseline);

      // The same active Label must discard blur and publish complete ordinary
      // Reveal tiles, without losing authored settings or progress bindings.
      finished = false;
      label.SetRequestedHeight(static_cast<float>(maximum + 64u));
      label.SetText(Dali::String(text.c_str()));
      DALI_TEST_CHECK(complete());
      DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
      DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
      DALI_TEST_CHECK(label.GetRendererCount() >= 2u);
      DALI_TEST_CHECK(label.GetTextReveal() == reveal);
      DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
      for(uint32_t index = 0u; index < label.GetRendererCount(); ++index)
      {
        const auto renderer = label.GetRendererAt(index);
        const auto textures = renderer.GetTextures();
        DALI_TEST_CHECK(textures.GetTextureCount() >= 2u);
        const auto foreground = textures.GetTexture(0u);
        const auto metadata   = textures.GetTexture(textures.GetTextureCount() - 1u);
        DALI_TEST_CHECK(foreground && metadata);
        DALI_TEST_EQUALS(metadata.GetWidth(), foreground.GetWidth(), TEST_LOCATION);
        DALI_TEST_EQUALS(metadata.GetHeight(), foreground.GetHeight(), TEST_LOCATION);
        DALI_TEST_CHECK(metadata.GetHeight() > 0u && metadata.GetHeight() <= maximum);
        DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(renderer.GetPropertyIndex("uTextRevealProgress")), 0.4f, 0.0001f, TEST_LOCATION);
      }
      finished = false;
      label.SetText("Short text");
      label.SetRequestedHeight(128.0f);
      DALI_TEST_CHECK(complete());
      DALI_TEST_CHECK(Find(label, "TextRevealRuntimeGaussian"));
      DALI_TEST_CHECK(tasks.GetTaskCount() > baseline);
      label.Unparent();
      Settle(application);
      DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianAsyncLifecycleP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  auto       tasks    = application.GetScene().GetRenderTaskList();
  const auto baseline = tasks.GetTaskCount();
  Label      label    = MakeLabel(application);
  label.SetMultiLine(true);
  label.SetRequestedHeight(160.0f);
  const auto        progressIndex = label.GetPropertyIndex("uTextRevealProgress");
  uint32_t          completions   = 0u;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&completions](View, float, float)
  {
    ++completions;
  });
  auto complete = [&](uint32_t previous)
  {
    Settle(application);
    for(int attempt = 0; attempt < 8 && completions == previous; ++attempt)
    {
      if(!Test::WaitForEventThreadTrigger(1, 5))
      {
        return false;
      }
      Settle(application);
    }
    return completions > previous;
  };
  label.SetAsyncRendering(true);
  for(int iteration = 0; iteration < 6; ++iteration)
  {
    Text::Reveal reveal;
    reveal.SetUnit(iteration % 2 == 0 ? Text::Reveal::Unit::CHARACTER : Text::Reveal::Unit::PIXEL);
    reveal.SetSequence(iteration % 3 == 0 ? Text::Reveal::Sequence::WHOLE_TEXT : Text::Reveal::Sequence::PER_LINE);
    reveal.SetFadeDurationRatio(iteration % 3 == 0 ? 1.0f : 0.0f);
    reveal.SetSequenceStaggerRatio(iteration % 2 == 0 ? 0.25f : 0.0f);
    Ui::Integration::Text::Reveal::SetBlurRadius(reveal, iteration % 3 == 0 ? 48.0f : 24.0f);
    Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, iteration % 2 == 0 ? 0.5f : 1.0f);
    const auto before = completions;
    label.SetTextReveal(reveal);
    label.SetText(iteration % 2 == 0 ? "New text\nAnother line\nShort end" : "Different text\n안녕하세요 DALi UI\nEnd");
    label.SetPadding(iteration % 2 == 0 ? Insets(3.0f, 7.0f, 5.0f, 9.0f) : Insets());
    label.SetTextRevealProgress(0.4f);
    DALI_TEST_CHECK(complete(before));
    DALI_TEST_CHECK(Find(label, "RevealGaussianForeground"));
    DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetTextRevealProgress(), 0.4f, TEST_LOCATION);
  }
  // Submit a request before None, then let the current disable result converge.
  const auto before = completions;
  label.SetText("Pending old blur\nMust not return after None");
  application.SendNotification();
  label.SetTextReveal(Text::Reveal::None());
  DALI_TEST_CHECK(complete(before));
  DALI_TEST_CHECK(label.GetTextReveal() == Text::Reveal::None());
  DALI_TEST_CHECK(!Find(label, "RevealGaussianForeground"));
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  label.Unparent();
  Settle(application);
  label.Reset();
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextRevealBlurDisabledResourcesP(void)
{
  UiTestApplication application;
  const auto        tasks    = application.GetScene().GetRenderTaskList();
  const auto        baseline = tasks.GetTaskCount();
  for(bool async : {false, true})
  {
    Label             label       = MakeLabel(application);
    const auto        children    = label.GetChildCount();
    uint32_t          completions = 0u;
    ConnectionTracker tracker;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&completions](View, float, float)
    {
      ++completions;
    });
    label.SetAsyncRendering(async);
    auto check = [&](const Text::Reveal& reveal)
    {
      const auto before = completions;
      label.SetTextReveal(reveal);
      // Change content as well so an async publication is required even for
      // equivalent default and explicitly disabled configurations.
      label.SetText(completions % 2u == 0u ? "Disabled blur A" : "Disabled blur B");
      Settle(application);
      if(async)
      {
        for(uint32_t attempt = 0u; attempt < 12u && completions == before; ++attempt)
        {
          DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
          Settle(application);
        }
        DALI_TEST_CHECK(completions > before);
      }
      DALI_TEST_CHECK(label.GetTextReveal() == reveal);
      DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
      DALI_TEST_EQUALS(label.GetChildCount(), children, TEST_LOCATION);
      DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
      DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    };
    check(Text::Reveal::None());
    check(Text::Reveal()); // No integration setter was called.
    for(auto mode : {Ui::Integration::Text::Reveal::BlurMode::PERFORMANCE, Ui::Integration::Text::Reveal::BlurMode::QUALITY})
    {
      Text::Reveal reveal;
      Ui::Integration::Text::Reveal::SetBlurMode(reveal, mode);
      Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 0.0f);
      check(reveal);
      Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 24.0f);
      Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.0f);
      check(reveal);
    }
    label.Unparent();
    Settle(application);
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianLifecycleP(void)
{
  UiTestApplication application;
  auto              tasks         = application.GetScene().GetRenderTaskList();
  const auto        baseline      = tasks.GetTaskCount();
  Label             label         = MakeLabel(application);
  const auto        progressIndex = label.GetPropertyIndex("uTextRevealProgress");
  Renderer          original      = label.GetRendererAt(0u);

  Text::Reveal reveal;
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  label.SetTextReveal(reveal);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  RenderTask sourceTask;
  RenderTask horizontalTask;
  RenderTask verticalTask;
  for(uint32_t i = 0u; i < tasks.GetTaskCount(); ++i)
  {
    RenderTask task = tasks.GetTask(i);
    const auto name = task.GetSourceActor().GetProperty<Dali::String>(Actor::Property::NAME);
    if(name == "RevealGaussianSource")
    {
      sourceTask = task;
    }
    else if(name == "RevealGaussianH")
    {
      horizontalTask = task;
    }
    else if(name == "RevealGaussianV")
    {
      verticalTask = task;
    }
  }
  DALI_TEST_CHECK(sourceTask && horizontalTask && verticalTask);
  // Core processes FBO tasks before the separate on-screen sweep. OrderIndex
  // orders these three dependencies, not FBO tasks against the default task.
  DALI_TEST_CHECK(sourceTask.GetOrderIndex() < horizontalTask.GetOrderIndex());
  DALI_TEST_CHECK(horizontalTask.GetOrderIndex() < verticalTask.GetOrderIndex());
  Actor source = Find(label, "RevealGaussianForeground");
  DALI_TEST_CHECK(source);
  DALI_TEST_CHECK(source.GetRendererAt(0u) == original);
  DALI_TEST_EQUALS(label.GetRendererCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), progressIndex, TEST_LOCATION);
  auto size = label.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_EQUALS(size.x, 360.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(size.y, 72.0f, 0.001f, TEST_LOCATION);

  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  DALI_TEST_CHECK(!Find(label, "RevealGaussianForeground"));
  DALI_TEST_CHECK(label.GetRendererCount() > 0u);
  label.SetTextReveal(reveal);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);

  application.GetScene().Remove(label);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  application.GetScene().Add(label);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline + 3u, TEST_LOCATION);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 0.0f);
  label.SetTextReveal(reveal);
  Settle(application);
  DALI_TEST_EQUALS(tasks.GetTaskCount(), baseline, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetRendererCount() > 0u);
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianLineRasterP(void)
{
  UiTestApplication application;
  using PlanUnit = Text::Internal::Reveal::Unit;
  using Plane    = Text::Typesetter::RuntimeBlurPlane;
  const Vector2 size(720.0f, 240.0f);
  for(float lineSize : {1.0f, 0.65f})
  {
    for(auto alignment : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END})
    {
      auto controller = Text::Controller::New();
      controller->SetMultiLineEnabled(true);
      controller->SetText("AVATAR ffi office gjpq\n안녕하세요 DALi UI\nArabic العربية mixed עברית");
      controller->SetDefaultFontSize(24.0f, Text::Controller::PIXEL_SIZE);
      controller->SetRelativeLineSize(lineSize);
      controller->SetVerticalAlignment(alignment);
      controller->Relayout(size);
      auto      typesetter = Text::Typesetter::New(controller->GetRenderTextModel());
      PixelData full       = typesetter->Render(size, Text::Direction::LEFT_TO_RIGHT,
                                                Text::Typesetter::RENDER_NO_STYLES, false, Pixel::L8);
      auto      source     = Text::Internal::Reveal::BuildCharacterPlan(*controller->GetRenderTextModel(), 0.25f);
      auto      plan       = typesetter->CreateFinalRevealPlan(source, PlanUnit::CHARACTER,
                                                               Text::Internal::Reveal::Sequence::PER_LINE, 0.25f);
      auto      sequences  = Ui::Internal::BuildRevealBlurSequences(*typesetter, plan);
      DALI_TEST_CHECK(sequences.size() > 1u);
      const auto           fullPixels         = Dali::Integration::GetPixelDataBuffer(full);
      float                fade               = 0.0f;
      auto                 fullMetadata       = typesetter->RenderTextRevealMetadata(size, Text::Direction::LEFT_TO_RIGHT, plan, fade);
      const auto           fullMetadataPixels = Dali::Integration::GetPixelDataBuffer(fullMetadata);
      std::vector<uint8_t> combined(static_cast<size_t>(full.GetWidth()) * full.GetHeight(), 0u);
      std::vector<uint8_t> combinedMetadata(combined.size(), 0u);
      for(const auto& sequence : sequences)
      {
        auto layer    = typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, Plane::TEXT, Pixel::L8, plan);
        auto metadata = typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, Plane::REVEAL_METADATA, Pixel::RGBA8888, plan);
        DALI_TEST_CHECK(layer && metadata);
        const auto layerPixels    = Dali::Integration::GetPixelDataBuffer(layer);
        const auto metadataPixels = Dali::Integration::GetPixelDataBuffer(metadata);
        bool       hasCoverage    = false;
        for(uint32_t y = 0u; y < full.GetHeight(); ++y)
        {
          for(uint32_t x = 0u; x < full.GetWidth(); ++x)
          {
            const auto alpha               = layerPixels.buffer[static_cast<size_t>(y) * layer.GetStrideBytes() + x];
            auto&      accumulated         = combined[static_cast<size_t>(y) * full.GetWidth() + x];
            accumulated                    = std::max(accumulated, alpha);
            const auto metadataAlpha       = metadataPixels.buffer[static_cast<size_t>(y) * metadata.GetStrideBytes() + x * 4u + 3u];
            auto&      accumulatedMetadata = combinedMetadata[static_cast<size_t>(y) * full.GetWidth() + x];
            accumulatedMetadata            = std::max(accumulatedMetadata, metadataAlpha);
            if(metadataAlpha != 0u)
            {
              hasCoverage = true;
              DALI_TEST_CHECK(alpha != 0u);
            }
          }
        }
        DALI_TEST_CHECK(hasCoverage);
      }
      // Recombining coverage must retain every ordinary raster pixel, even
      // with compressed line spacing and glyphs crossing nominal line boxes.
      // A following ordinary render also proves selection is not sticky.
      auto       again       = typesetter->Render(size, Text::Direction::LEFT_TO_RIGHT,
                                                  Text::Typesetter::RENDER_NO_STYLES, false, Pixel::L8);
      const auto againPixels = Dali::Integration::GetPixelDataBuffer(again);
      for(uint32_t y = 0u; y < full.GetHeight(); ++y)
      {
        for(uint32_t x = 0u; x < full.GetWidth(); ++x)
        {
          const auto expected = fullPixels.buffer[static_cast<size_t>(y) * full.GetStrideBytes() + x];
          DALI_TEST_EQUALS(combined[static_cast<size_t>(y) * full.GetWidth() + x], expected, TEST_LOCATION);
          // The mock font even gives spaces visible bitmaps. Compare metadata
          // with ordinary Reveal's coverage, which deliberately excludes them.
          DALI_TEST_EQUALS(combinedMetadata[static_cast<size_t>(y) * full.GetWidth() + x],
                           fullMetadataPixels.buffer[static_cast<size_t>(y) * fullMetadata.GetStrideBytes() + x * 4u + 3u], TEST_LOCATION);
          DALI_TEST_EQUALS(againPixels.buffer[static_cast<size_t>(y) * again.GetStrideBytes() + x], expected, TEST_LOCATION);
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianSourceRegionP(void)
{
  UiTestApplication application;
  using Plane = Text::Typesetter::RuntimeBlurPlane;
  const Vector2                         size(480.0f, 240.0f);
  const std::pair<Plane, Pixel::Format> planes[] = {
    {Plane::TEXT, Pixel::L8}, {Plane::TEXT, Pixel::RGBA8888}, {Plane::COLOR_MASK, Pixel::L8}, {Plane::GRADIENT_MASK, Pixel::L8}, {Plane::GRADIENT_PRESERVED, Pixel::RGBA8888}};
  for(auto alignment : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END})
  {
    for(float lineSize : {1.0f, 0.65f})
    {
      auto controller = Text::Controller::New();
      controller->SetMultiLineEnabled(true);
      controller->SetDefaultFontSize(24.5f, Text::Controller::PIXEL_SIZE);
      controller->SetDefaultFontSlant(TextAbstraction::FontSlant::ITALIC);
      controller->SetRelativeLineSize(lineSize);
      controller->SetHorizontalAlignment(alignment);
      controller->SetVerticalAlignment(alignment);
      controller->SetVerticalLineAlignment(alignment);
      controller->SetText("AVATAR ffi office gjpq\n안녕하세요 DALi UI\nالعربية עברית 🌈 🔮\nA final short line");
      if(alignment == Text::Alignment::END)
      {
        controller->SetTextElideEnabled(true);
        controller->SetEllipsisPosition(Text::EllipsisPosition::END);
        controller->SetMaximumNumberOfLines(2);
      }
      controller->Relayout(size);
      auto typesetter = Text::Typesetter::New(controller->GetRenderTextModel());
      typesetter->SetFinalElisionResult(controller->GetFinalElisionResult());
      auto ordinary = typesetter->Render(size, Text::Direction::LEFT_TO_RIGHT,
                                         Text::Typesetter::RENDER_NO_STYLES, false, Pixel::RGBA8888);
      DALI_TEST_CHECK(ordinary);
      auto source    = Text::Internal::Reveal::BuildCharacterPlan(*controller->GetRenderTextModel(), 0.25f);
      auto plan      = typesetter->CreateFinalRevealPlan(source, Text::Internal::Reveal::Unit::CHARACTER,
                                                         Text::Internal::Reveal::Sequence::PER_LINE, 0.25f);
      auto sequences = Ui::Internal::BuildRevealBlurSequences(*typesetter, plan);
      DALI_TEST_CHECK(sequences.size() > 1u);
      for(const auto& sequence : sequences)
      {
        auto bounds = typesetter->GetRuntimeBlurLineRasterBounds(size, sequence.lineIndex);
        DALI_TEST_EQUALS(bounds.x, 0u, TEST_LOCATION);
        DALI_TEST_EQUALS(bounds.width, ordinary.GetWidth(), TEST_LOCATION);
        DALI_TEST_CHECK(bounds.height > 0u && bounds.height < ordinary.GetHeight());
        DALI_TEST_CHECK(bounds.y + bounds.height <= ordinary.GetHeight());
        for(const auto& plane : planes)
        {
          auto full = typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, plane.first, plane.second, plan);
          auto band = typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, plane.first, plane.second, plan, bounds.y, bounds.height);
          DALI_TEST_CHECK(full && band);
          DALI_TEST_EQUALS(band.GetHeight(), bounds.height, TEST_LOCATION);
          const auto                 fullPixels = Dali::Integration::GetPixelDataBuffer(full);
          const auto                 bandPixels = Dali::Integration::GetPixelDataBuffer(band);
          const size_t               rowBytes   = static_cast<size_t>(bounds.width) * Pixel::GetBytesPerPixel(plane.second);
          const std::vector<uint8_t> transparentRow(rowBytes, 0u);
          for(uint32_t y = 0u; y < full.GetHeight(); ++y)
          {
            const auto* actual = y >= bounds.y && y < bounds.y + bounds.height
                                   ? bandPixels.buffer + static_cast<size_t>(y - bounds.y) * band.GetStrideBytes()
                                   : transparentRow.data();
            // Compare the entire reference plane: omitted rows must also be
            // transparent, including italic/color-glyph overhang and ellipsis.
            DALI_TEST_CHECK(std::memcmp(fullPixels.buffer + static_cast<size_t>(y) * full.GetStrideBytes(), actual, rowBytes) == 0);
          }
          DALI_TEST_CHECK(!typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, plane.first, plane.second, plan, full.GetHeight(), 1u));
        }
      }
      auto       again  = typesetter->Render(size, Text::Direction::LEFT_TO_RIGHT,
                                             Text::Typesetter::RENDER_NO_STYLES, false, Pixel::RGBA8888);
      const auto before = Dali::Integration::GetPixelDataBuffer(ordinary);
      const auto after  = Dali::Integration::GetPixelDataBuffer(again);
      DALI_TEST_EQUALS(after.bufferSize, before.bufferSize, TEST_LOCATION);
      DALI_TEST_CHECK(std::memcmp(before.buffer, after.buffer, before.bufferSize) == 0);

      // Exercise preparation's cumulative-baseline path, not just the bounds
      // helper's independent legacy traversal. Reconstruct every full line.
      Ui::Internal::RevealBlurPreparationOptions options;
      options.rasterSize       = size;
      options.controlSize      = size;
      options.radius           = 16u;
      options.maxTextureSize   = 4096u;
      options.foregroundFormat = Pixel::RGBA8888;
      options.perLine          = true;
      auto prepared = Ui::Internal::PrepareRevealBlur(*typesetter, plan, {}, options);
      DALI_TEST_CHECK(prepared && prepared->lines.size() == sequences.size());
      for(const auto& line : prepared->lines)
      {
        auto full = typesetter->RenderRuntimeBlurLine(size, line.sequence.lineIndex, Plane::TEXT, Pixel::RGBA8888, plan);
        const auto isolated = line.atlasIndex == Ui::Internal::RevealBlurLineRaster::NO_ATLAS
                                ? line.foreground
                                : Ui::Internal::CropRuntimeRevealBlurPixels(prepared->sourceAtlases[line.atlasIndex].foreground, line.atlasRectangle);
        DALI_TEST_CHECK(full && isolated);
        const auto fullPixels = Dali::Integration::GetPixelDataBuffer(full);
        const auto   cropped    = Dali::Integration::GetPixelDataBuffer(isolated);
        const auto x          = static_cast<uint32_t>(std::round(line.sequence.textureRect.x * size.width));
        const auto y          = static_cast<uint32_t>(std::round(line.sequence.textureRect.y * size.height));
        const size_t rowBytes = static_cast<size_t>(full.GetWidth()) * 4u;
        DALI_TEST_CHECK(x + isolated.GetWidth() <= full.GetWidth());
        DALI_TEST_CHECK(y + isolated.GetHeight() <= full.GetHeight());
        std::vector<uint8_t> reconstructed(rowBytes * full.GetHeight(), 0u);
        for(uint32_t row = 0u; row < isolated.GetHeight(); ++row)
        {
          std::memcpy(reconstructed.data() + static_cast<size_t>(y + row) * rowBytes + x * 4u,
                      cropped.buffer + static_cast<size_t>(row) * isolated.GetStrideBytes(),
                      static_cast<size_t>(isolated.GetWidth()) * 4u);
        }
        for(uint32_t row = 0u; row < full.GetHeight(); ++row)
        {
          DALI_TEST_CHECK(std::memcmp(reconstructed.data() + static_cast<size_t>(row) * rowBytes,
                                      fullPixels.buffer + static_cast<size_t>(row) * full.GetStrideBytes(), rowBytes) == 0);
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianSourceRegionFallbackP(void)
{
  UiTestApplication application;
  using Plane = Text::Typesetter::RuntimeBlurPlane;
  const Vector2 size(480.0f, 240.0f);
  auto          controller = Text::Controller::New();
  controller->SetMultiLineEnabled(true);
  controller->SetDefaultFontSize(24.5f, Text::Controller::PIXEL_SIZE);
  Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
  gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
  auto builder = Text::StyledTextBuilder::New("GradientSpan\nkeeps original paint coordinates");
  DALI_TEST_CHECK(builder.SetSpan(Text::GradientSpan::New(gradient), 0u, 6u));
  controller->SetStyledText(builder.Build());
  controller->Relayout(size);
  auto typesetter = Text::Typesetter::New(controller->GetRenderTextModel());
  DALI_TEST_CHECK(typesetter->Render(size, Text::Direction::LEFT_TO_RIGHT, Text::Typesetter::RENDER_NO_STYLES));
  auto source = Text::Internal::Reveal::BuildCharacterPlan(*controller->GetRenderTextModel(), 0.25f);
  auto plan   = typesetter->CreateFinalRevealPlan(source, Text::Internal::Reveal::Unit::CHARACTER,
                                                  Text::Internal::Reveal::Sequence::PER_LINE, 0.25f);
  for(Text::LineIndex line = 0u; line < typesetter->GetViewModel()->GetNumberOfLines(); ++line)
  {
    const auto bounds = typesetter->GetRuntimeBlurLineRasterBounds(size, line);
    DALI_TEST_EQUALS(bounds.y, 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(bounds.height, static_cast<uint32_t>(size.height), TEST_LOCATION);
    for(auto plane : {Plane::TEXT, Plane::GRADIENT_PRESERVED, Plane::GRADIENT_MASK, Plane::COLOR_MASK})
    {
      // Reject accidental regional paint coordinates, while preserving the
      // full-size reference and the separately validated metadata band path.
      DALI_TEST_CHECK(!typesetter->RenderRuntimeBlurLine(size, line, plane, Pixel::RGBA8888, plan, 1u, 10u));
      DALI_TEST_CHECK(typesetter->RenderRuntimeBlurLine(size, line, plane, Pixel::RGBA8888, plan));
    }
  }

  // Explicitly transparent RGBA foreground has bitmap bounds but no coverage.
  // The production path must retain its full-size transparent publication,
  // rather than fail when the final crop extends beyond the provisional band.
  const char* transparentText    = "Alpha beta.\nGamma delta.";
  auto        transparentBuilder = Text::StyledTextBuilder::New(transparentText);
  DALI_TEST_CHECK(transparentBuilder.SetSpan(Text::ForegroundColorSpan::New(UiColor(Color::TRANSPARENT)),
                                             0u, static_cast<uint32_t>(std::strlen(transparentText))));
  Label label = Label::New();
  label.SetLayoutMode(LayoutMode::STANDALONE);
  label.SetRequestedWidth(size.width);
  label.SetRequestedHeight(size.height);
  label.SetMultiLine(true);
  label.SetStyledText(transparentBuilder.Build());
  Text::Reveal reveal;
  reveal.SetSequence(Text::Reveal::Sequence::PER_LINE);
  label.SetTextReveal(reveal);
  application.GetScene().Add(label);
  Settle(application);
  const auto originalTexture = label.GetRendererAt(0u).GetTextures().GetTexture(0u);
  DALI_TEST_EQUALS(originalTexture.GetPixelFormat(), Pixel::RGBA8888, TEST_LOCATION);
  Ui::Integration::Text::Reveal::SetBlurRadius(reveal, 16.0f);
  Ui::Integration::Text::Reveal::SetBlurDurationRatio(reveal, 0.5f);
  label.SetTextReveal(reveal);
  Settle(application);
  DALI_TEST_CHECK(BlurLines(label).size() > 1u);
  const auto foreground = Passes(label, "RevealGaussianLineForeground");
  DALI_TEST_CHECK(!foreground.empty());
  const auto lineCount      = static_cast<uint32_t>(BlurLines(label).size());
  const auto sourceVertices = UploadedSourceVertices(application, lineCount);
  DALI_TEST_EQUALS(sourceVertices.size(), static_cast<size_t>(lineCount) * 48u, TEST_LOCATION);
  for(auto renderer : foreground)
  {
    auto texture = renderer.GetTextures().GetTexture(0u);
    for(uint32_t line = 0u; line < lineCount; ++line)
    {
      const auto* vertex = sourceVertices.data() + static_cast<size_t>(line) * 48u;
      // Transparent source fallback still retains the entire original plane
      // per entry; batching changes storage, not its crop/coverage contract.
      DALI_TEST_EQUALS(Vector4(vertex[2u], vertex[3u], vertex[4u], vertex[5u]), Vector4(0.0f, 0.0f, 1.0f, 1.0f), TEST_LOCATION);
      DALI_TEST_EQUALS(vertex[10u] * static_cast<float>(texture.GetWidth()), static_cast<float>(originalTexture.GetWidth()), 0.001f, TEST_LOCATION);
      DALI_TEST_EQUALS(vertex[11u] * static_cast<float>(texture.GetHeight()), static_cast<float>(originalTexture.GetHeight()), 0.001f, TEST_LOCATION);
    }
  }
  label.SetTextReveal(Text::Reveal::None());
  Settle(application);
  DALI_TEST_CHECK(!Find(label, "TextRevealRuntimeGaussian"));
  END_TEST;
}

int UtcDaliTextRevealRuntimeGaussianMetadataRegionP(void)
{
  UiTestApplication application;
  using PlanUnit = Text::Internal::Reveal::Unit;
  using Plane    = Text::Typesetter::RuntimeBlurPlane;
  const Vector2 size(480.0f, 180.0f);
  for(auto alignment : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END})
  {
    for(float lineSize : {1.0f, 0.65f})
    {
      auto controller = Text::Controller::New();
      controller->SetMultiLineEnabled(true);
      controller->SetDefaultFontSize(24.5f, Text::Controller::PIXEL_SIZE);
      controller->SetRelativeLineSize(lineSize);
      controller->SetHorizontalAlignment(alignment);
      controller->SetVerticalAlignment(alignment);
      if(alignment == Text::Alignment::END)
      {
        controller->SetTextElideEnabled(true);
        controller->SetEllipsisPosition(Text::EllipsisPosition::END);
        controller->SetMaximumNumberOfLines(2);
      }
      controller->SetDefaultColor(Vector4(0.2f, 0.6f, 0.9f, 0.25f));
      Gradient::Linear gradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f));
      gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(Color::RED)), Gradient::StopNode(1.0f, UiColor(Color::BLUE))});
      auto builder = Text::StyledTextBuilder::New("AVATAR ffi office gjpq\n안녕하세요 DALi UI\nالعربية עברית 🌈 🔮\nA final short line");
      DALI_TEST_CHECK(builder.SetSpan(Text::GradientSpan::New(gradient), 0u, 6u));
      controller->SetStyledText(builder.Build());
      controller->Relayout(size);
      auto typesetter = Text::Typesetter::New(controller->GetRenderTextModel());
      typesetter->SetFinalElisionResult(controller->GetFinalElisionResult());
      // Resolve the same final model that the production source raster uses.
      auto ordinary = typesetter->Render(size, Text::Direction::LEFT_TO_RIGHT,
                                         Text::Typesetter::RENDER_NO_STYLES, false, Pixel::RGBA8888);
      DALI_TEST_CHECK(ordinary);
      for(auto unit : {PlanUnit::CHARACTER, PlanUnit::WORD, PlanUnit::LINE, PlanUnit::PIXEL})
      {
        auto segmentation = TextAbstraction::Segmentation::New();
        auto source       = Text::Internal::Reveal::BuildPlan(*controller->GetRenderTextModel(), unit, 0.25f, segmentation);
        auto plan         = typesetter->CreateFinalRevealPlan(source, unit, Text::Internal::Reveal::Sequence::PER_LINE, 0.25f);
        auto sequences    = Ui::Internal::BuildRevealBlurSequences(*typesetter, plan);
        DALI_TEST_CHECK(sequences.size() > 1u);
        if(alignment == Text::Alignment::END)
        {
          const auto ellipsis = typesetter->GetViewModel()->GetEllipsisFinalGlyphIndex();
          DALI_TEST_CHECK(ellipsis < plan.glyphToUnit.size());
          DALI_TEST_CHECK(plan.glyphToUnit[ellipsis] != Text::Internal::Reveal::NO_UNIT);
        }
        for(const auto& sequence : sequences)
        {
          auto full = typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, Plane::REVEAL_METADATA, Pixel::RGBA8888, plan);
          DALI_TEST_CHECK(full);
          const auto fullPixels = Dali::Integration::GetPixelDataBuffer(full);
          // Deliberately cut through glyphs, not just between nominal lines.
          // A one-row source guard must reproduce every byte of the full plane,
          // including ownership-only halo and PIXEL start encodings.
          for(uint32_t top = 0u; top < full.GetHeight(); top += 17u)
          {
            const uint32_t bottom       = std::min(full.GetHeight(), top + 17u);
            const uint32_t sourceTop    = top > 0u ? top - 1u : 0u;
            const uint32_t sourceBottom = std::min(full.GetHeight(), bottom + 1u);
            auto           tile         = typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, Plane::REVEAL_METADATA, Pixel::RGBA8888,
                                                                            plan, sourceTop, sourceBottom - sourceTop);
            DALI_TEST_CHECK(tile);
            DALI_TEST_EQUALS(tile.GetWidth(), full.GetWidth(), TEST_LOCATION);
            DALI_TEST_EQUALS(tile.GetHeight(), sourceBottom - sourceTop, TEST_LOCATION);
            const auto tilePixels = Dali::Integration::GetPixelDataBuffer(tile);
            for(uint32_t y = top; y < bottom; ++y)
            {
              const bool equal = std::memcmp(fullPixels.buffer + static_cast<size_t>(y) * full.GetStrideBytes(),
                                             tilePixels.buffer + static_cast<size_t>(y - sourceTop) * tile.GetStrideBytes(),
                                             static_cast<size_t>(full.GetWidth()) * 4u) == 0;
              if(!equal)
              {
                std::fprintf(stderr, "Metadata region mismatch: alignment=%d lineSize=%f unit=%d line=%u top=%u y=%u\n",
                             static_cast<int>(alignment), lineSize, static_cast<int>(unit), sequence.lineIndex, top, y);
              }
              DALI_TEST_CHECK(equal);
            }
            // Keep the vertical-band reference above, and also tile both
            // axes through real glyphs. The guard must preserve all four
            // metadata bytes, including PIXEL timing and ownership-only halo.
            for(uint32_t left = 0u; left < full.GetWidth(); left += 97u)
            {
              const uint32_t right       = std::min(full.GetWidth(), left + 97u);
              const uint32_t sourceLeft  = left > 0u ? left - 1u : 0u;
              const uint32_t sourceRight = std::min(full.GetWidth(), right + 1u);
              auto           rectangle   = typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, Plane::REVEAL_METADATA, Pixel::RGBA8888,
                                                                             plan, sourceTop, sourceBottom - sourceTop, sourceLeft, sourceRight - sourceLeft);
              DALI_TEST_CHECK(rectangle);
              DALI_TEST_EQUALS(rectangle.GetWidth(), sourceRight - sourceLeft, TEST_LOCATION);
              DALI_TEST_EQUALS(rectangle.GetHeight(), sourceBottom - sourceTop, TEST_LOCATION);
              const auto rectanglePixels = Dali::Integration::GetPixelDataBuffer(rectangle);
              for(uint32_t y = top; y < bottom; ++y)
              {
                DALI_TEST_CHECK(std::memcmp(fullPixels.buffer + static_cast<size_t>(y) * full.GetStrideBytes() + static_cast<size_t>(left) * 4u,
                                            rectanglePixels.buffer + static_cast<size_t>(y - sourceTop) * rectangle.GetStrideBytes() + static_cast<size_t>(left - sourceLeft) * 4u,
                                            static_cast<size_t>(right - left) * 4u) == 0);
              }
            }
          }
          DALI_TEST_CHECK(!typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, Plane::REVEAL_METADATA, Pixel::RGBA8888,
                                                             plan, full.GetHeight(), 1u));
          DALI_TEST_CHECK(!typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, Plane::REVEAL_METADATA, Pixel::RGBA8888,
                                                             plan, 0u, 0u, full.GetWidth(), 1u));
          DALI_TEST_CHECK(!typesetter->RenderRuntimeBlurLine(size, sequence.lineIndex, Plane::TEXT, Pixel::L8,
                                                             plan, 0u, 0u, 1u, 10u));
        }
      }
      auto       again          = typesetter->Render(size, Text::Direction::LEFT_TO_RIGHT,
                                                     Text::Typesetter::RENDER_NO_STYLES, false, Pixel::RGBA8888);
      const auto originalPixels = Dali::Integration::GetPixelDataBuffer(ordinary);
      const auto againPixels    = Dali::Integration::GetPixelDataBuffer(again);
      DALI_TEST_EQUALS(againPixels.bufferSize, originalPixels.bufferSize, TEST_LOCATION);
      DALI_TEST_CHECK(std::memcmp(originalPixels.buffer, againPixels.buffer, originalPixels.bufferSize) == 0);
    }
  }
  END_TEST;
}
