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
#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/integration-api/text/text-control-interface.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/replacement/editable-inline-replacement-data.h>
#include <dali-ui-foundation/internal/text/replacement/inline-replacement-manager.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-glyph-helper.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <dali/integration-api/system/system-settings.h>
#include <chrono>
#include <sstream>
#include <utility>
#include "resources/image-span-resolution-png.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float EPSILON = 0.01f;

Text::StyledText ImageText(const char* prefix, Vector2 size, bool markup, const char* source = "missing-font-scale.png")
{
  if(markup)
  {
    std::ostringstream text;
    text << prefix << "<img src='" << source << "' width='" << size.x
         << "' height='" << size.y << "'/> DEF";
    return Text::StyledText::FromMarkup(text.str().c_str());
  }
  auto       builder = Text::StyledTextBuilder::New(prefix);
  const auto start   = builder.GetUtf32Length();
  builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
  builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes(source, size)), start, start + 1u);
  builder.AppendText(" DEF");
  return builder.Build();
}

struct UpdateRequests : Ui::Integration::Text::ControlInterface
{
  void RequestTextRelayout() override
  {
    ++relayout;
  }
  void InvalidateTextMeasure() override
  {
    ++measure;
  }
  void RequestAsyncRender() override
  {
    ++async;
  }
  unsigned relayout{0u};
  unsigned measure{0u};
  unsigned async{0u};
};

void CheckBox(const Text::ReplacementRenderState& state, Vector2 expected)
{
  DALI_TEST_CHECK(state.processingModel);
  DALI_TEST_EQUALS(state.placements.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(state.placements[0u].size, expected, EPSILON, TEST_LOCATION);
  const auto& run = state.projection.GetReplacementRuns()[0u];
  DALI_TEST_EQUALS(run.metrics.width, expected.x, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(run.metrics.height, expected.y, EPSILON, TEST_LOCATION);
  const auto& visual         = *state.processingModel->mVisualModel;
  unsigned    syntheticCount = 0u;
  for(const auto& glyph : visual.mGlyphs)
  {
    if(Text::IsSyntheticReplacementGlyph(glyph))
    {
      ++syntheticCount;
      DALI_TEST_EQUALS(glyph.width, expected.x, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(glyph.height, expected.y, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(glyph.advance, expected.x, EPSILON, TEST_LOCATION);
    }
  }
  DALI_TEST_EQUALS(syntheticCount, 1u, TEST_LOCATION);
  const auto& line = visual.mLines[state.placements[0u].lineIndex];
  DALI_TEST_CHECK(line.ascender - line.descender + EPSILON >= expected.y);
}

struct NoopRelayoutContainer : RelayoutContainer
{
  void Add(const Actor&, const Vector2&) override
  {
  }
};

Vector2 VisualSize(Ui::Integration::Visual::Base visual)
{
  Property::Map properties;
  visual.CreatePropertyMap(properties);
  auto* transform = properties.Find(Ui::Integration::Visual::Property::TRANSFORM);
  DALI_TEST_CHECK(transform && transform->GetMap());
  auto* size = transform->GetMap()->Find(Ui::Integration::Visual::Transform::Property::SIZE);
  DALI_TEST_CHECK(size);
  return size->Get<Vector2>();
}
template<typename Predicate>
void WaitUntil(UiTestApplication& application, Predicate ready)
{
  // Use the existing event-thread utility and the async text UTC timeout.
  const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  do
  {
    application.SendNotification();
    application.RunIdles();
    application.Render();
    if(ready()) return;
    Test::WaitForEventThreadTrigger(1, 1);
  } while(std::chrono::steady_clock::now() < deadline);
  DALI_TEST_CHECK(ready());
}

unsigned CountInlineRenderers(Actor actor)
{
  unsigned count = 0u;
  for(unsigned index = 0u; index < actor.GetRendererCount(); ++index)
  {
    auto renderer = actor.GetRendererAt(index);
    // Inline image renderers expose pixelArea; text/background renderers do not.
    if(renderer.GetPropertyIndex("pixelArea") != Property::INVALID_INDEX ||
       renderer.GetShader().GetPropertyIndex("pixelArea") != Property::INVALID_INDEX) ++count;
  }
  return count;
}

Ui::ImageUrl EncodedPng()
{
  EncodedImageBuffer::RawBufferType bytes;
  for(auto byte : IMAGE_SPAN_RESOLUTION_PNG) bytes.PushBack(byte);
  return Ui::ImageUrl::New(EncodedImageBuffer::New(std::move(bytes)));
}

struct InlineImageFixture
{
  explicit InlineImageFixture(UiTestApplication& application, const char* url, bool onScene = true)
  : owner(View::New()),
    host(owner, 1)
  {
    if(onScene) application.GetScene().Add(owner);
    source.sourceRevision = 1u;
    Text::ReplacementRunSnapshot run;
    run.type               = Text::ReplacementType::IMAGE;
    run.occurrenceIdentity = 1u;
    run.metrics.width = run.metrics.height = 24.f;
    run.image.source                       = url;
    source.runs.PushBack(run);
    Text::ReplacementPlacement placement;
    placement.sourceRunIndex     = 0u;
    placement.occurrenceIdentity = 1u;
    placement.visible            = true;
    placements.PushBack(placement);
  }
  ~InlineImageFixture()
  {
    manager.Clear();
    owner.Unparent();
  }
  Ui::Integration::Visual::Base Update(float uiScale, float fontScale, bool clipped = false)
  {
    placements[0].size = Vector2(24.f, 24.f) * (uiScale * fontScale);
    DALI_TEST_CHECK(manager.Update(host, source, placements, Vector2::ZERO,
                                   clipped ? Vector2(12.f, 12.f) : Vector2(150.f, 150.f), Vector2(150.f, 150.f), uiScale, 1u));
    return Visual();
  }
  Ui::Integration::Visual::Base Visual()
  {
    return Ui::Internal::ViewDataImpl::Get(GetImpl(owner)).GetVisual(owner.GetPropertyIndex("__dali_ui_inline_replacement_0"));
  }
  Texture Loaded(UiTestApplication& application)
  {
    WaitUntil(application, [&]()
    {
      auto visual = Visual();
      return visual && Ui::GetImplementation(visual).GetResourceStatus() != Ui::Visual::ResourceStatus::PREPARING;
    });
    auto visual = Visual();
    DALI_TEST_EQUALS(Ui::GetImplementation(visual).GetResourceStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
    manager.Refresh();
    DALI_TEST_EQUALS(owner.GetRendererCount(), 1u, TEST_LOCATION);
    return visual.GetRenderer().GetTextures().GetTexture(0u);
  }
  View                                          owner;
  Ui::Internal::Text::InlineReplacementViewHost host;
  Ui::Internal::Text::InlineReplacementManager  manager;
  Text::ReplacementSourceSnapshot               source;
  Vector<Text::ReplacementPlacement>            placements;
};

void CheckRequested(Ui::Integration::Visual::Base visual, int size)
{
  DALI_TEST_CHECK(visual);
  Property::Map map;
  visual.CreatePropertyMap(map);
  DALI_TEST_EQUALS(map.Find(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH)->Get<int>(), size, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Find(Ui::Integration::ImageVisual::Property::DESIRED_HEIGHT)->Get<int>(), size, TEST_LOCATION);
}
} // namespace

int UtcDaliImageSpanSystemFontScaleMatrixP(void)
{
  UiTestApplication application;
  uint32_t          dpi = 0u, verticalDpi = 0u;
  TextAbstraction::FontClient::Get().GetDpi(dpi, verticalDpi);
  // Fake only the value delivered by SystemSettings. Both production engines
  // consume the controller's resolved scale and immutable authored snapshot.
  for(bool markup : {false, true})
  {
    for(const char* prefix : {"ABC ", "한글 ", "A한B ", "ABC\n한글 "})
    {
      for(Vector2 sourceSize : {Vector2(8.f, 8.f), Vector2(20.f, 20.f), Vector2(48.f, 48.f), Vector2(24.f, 12.f)})
      {
        auto  controller = Text::Controller::New();
        auto& impl       = Text::Controller::Impl::GetImplementation(*controller);
        controller->SetDefaultFontSize(20.f, Text::Controller::PIXEL_SIZE);
        controller->SetSystemFontSizeScaleEnabled(true);
        controller->SetTextElideEnabled(false);
        controller->GetLayoutEngine().SetLayout(Text::Layout::Engine::MULTI_LINE_BOX);
        controller->SetStyledText(ImageText(prefix, sourceSize, markup));
        Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
        for(float uiScale : {1.f, 1.5f})
        {
          // Match the control's OnMeasure contract for a UI scale change.
          if(controller->SetUiScale(uiScale))
          {
            controller->InvalidateFontData();
          }
          // Reuse each model across increases and a return to 1: no accumulated scaling.
          for(float fontScale : {1.f, 1.25f, 1.5f, 2.f, 1.f})
          {
            controller->SetSystemFontSizeScale(fontScale);
            controller->Relayout(Size(600.f, 500.f));
            const auto& sync = impl.GetReplacementRenderState();
            CheckBox(sync, sourceSize * (uiScale * fontScale));
            const auto& source = controller->GetReplacementSourceSnapshot();
            DALI_TEST_EQUALS(source.runs[0u].metrics.width, sourceSize.x, TEST_LOCATION);
            DALI_TEST_EQUALS(source.runs[0u].metrics.height, sourceSize.y, TEST_LOCATION);
            for(float renderScale : {1.f, 2.f})
            {
              Text::AsyncTextParameters p;
              controller->GetText(p.text);
              p.fontSize                  = 20.f * 72.f / static_cast<float>(dpi);
              p.textWidth                 = 600.f;
              p.textHeight                = 500.f;
              p.isMultiLine               = true;
              p.ellipsis                  = false;
              p.relativeLineSize          = controller->GetRelativeLineSize();
              p.effectiveTextScale        = controller->GetEffectiveTextScale();
              p.renderScale               = renderScale;
              p.replacementSourceSnapshot = source;
              bool cached                 = false;
              Size naturalSize            = Size::ZERO;
              if(renderScale > 1.f)
              {
                naturalSize = loader.SetupRenderScale(p, cached);
              }
              const auto  result = loader.RenderText(p, cached, naturalSize);
              const auto* async  = Text::GetImplementation(loader).GetReplacementRenderState();
              DALI_TEST_CHECK(async);
              CheckBox(*async, sourceSize * (uiScale * fontScale * renderScale));
              DALI_TEST_EQUALS(result.replacementPlacements[0u].size, sync.placements[0u].size, EPSILON, TEST_LOCATION);
              DALI_TEST_EQUALS(result.replacementPlacements[0u].lineIndex, sync.placements[0u].lineIndex, TEST_LOCATION);
              if(renderScale == 1.f)
              {
                DALI_TEST_EQUALS(async->layoutSize, sync.layoutSize, EPSILON, TEST_LOCATION);
                DALI_TEST_EQUALS(result.replacementPlacements[0u].position, sync.placements[0u].position, EPSILON, TEST_LOCATION);
              }
            }
          }
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliImageSpanSystemFontScaleInvalidationP(void)
{
  UiTestApplication application;
  UpdateRequests    requests;
  auto              controller = Text::Controller::New();
  controller->SetControlInterface(&requests);
  controller->SetDefaultFontSize(20.f, Text::Controller::PIXEL_SIZE);
  controller->SetStyledText(ImageText("ABC ", Vector2(24.f, 24.f), false));
  controller->SetSystemFontSizeScaleEnabled(true);
  controller->Relayout(Size(500.f, 200.f));
  auto&      impl         = Text::Controller::Impl::GetImplementation(*controller);
  const auto generation   = impl.GetReplacementRenderState().layoutGeneration;
  const auto revision     = controller->GetReplacementSourceSnapshot().sourceRevision;
  const auto originalSize = controller->GetNaturalSize(false);
  requests                = {};
  controller->SetSystemFontSizeScale(1.5f);
  DALI_TEST_CHECK(requests.relayout > 0u && requests.measure > 0u && requests.async > 0u);
  DALI_TEST_CHECK(controller->GetNaturalSize(false).width > originalSize.width);
  controller->Relayout(Size(500.f, 200.f));
  CheckBox(impl.GetReplacementRenderState(), Vector2(36.f, 36.f));
  DALI_TEST_CHECK(impl.GetReplacementRenderState().layoutGeneration > generation);
  DALI_TEST_EQUALS(controller->GetReplacementSourceSnapshot().sourceRevision, revision, TEST_LOCATION);
  requests = {};
  controller->SetSystemFontSizeScale(1.5f);
  DALI_TEST_EQUALS(requests.relayout + requests.measure + requests.async, 0u, TEST_LOCATION);
  controller->SetMaximumFontSizeScale(1.25f);
  controller->Relayout(Size(500.f, 200.f));
  CheckBox(impl.GetReplacementRenderState(), Vector2(30.f, 30.f));
  controller->SetSystemFontSizeScaleEnabled(false);
  controller->Relayout(Size(500.f, 200.f));
  CheckBox(impl.GetReplacementRenderState(), Vector2(24.f, 24.f));
  requests = {};
  controller->SetSystemFontSizeScale(2.f);
  DALI_TEST_EQUALS(requests.relayout + requests.measure + requests.async, 0u, TEST_LOCATION);
  controller->SetSystemFontSizeScaleEnabled(true);
  controller->Relayout(Size(500.f, 200.f));
  CheckBox(impl.GetReplacementRenderState(), Vector2(30.f, 30.f));
  controller->SetControlInterface(nullptr);
  END_TEST;
}

int UtcDaliImageSpanSystemFontScaleControlSignalP(void)
{
  auto config = UiConfig::New();
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::NORMAL, 1.f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::LARGE, 1.25f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::EXTRA_LARGE, 1.5f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::GIANT, 2.f);
  UiTestApplication application(config);
  auto              settings = Dali::Integration::SystemSettings::Get();
  DALI_TEST_CHECK(settings);
  NoopRelayoutContainer container;
  auto                  label = Label::New();
  label.SetAsyncRendering(false);
  auto field    = InputField::New();
  auto editor   = InputEditor::New();
  auto exercise = [&](auto control, auto& impl, auto getVisual)
  {
    control.SetFontSize(20.f);
    control.SetSystemFontSizeScaleEnabled(true);
    control.SetPadding(Insets(0.f, 0.f, 0.f, 0.f));
    control.SetStyledText(ImageText("ABC ", Vector2(24.f, 24.f), false));
    const Dali::Integration::SystemSettings::FontSize values[] = {
      Dali::Integration::SystemSettings::FontSize::NORMAL,
      Dali::Integration::SystemSettings::FontSize::LARGE,
      Dali::Integration::SystemSettings::FontSize::EXTRA_LARGE,
      Dali::Integration::SystemSettings::FontSize::GIANT,
      Dali::Integration::SystemSettings::FontSize::NORMAL};
    const float scales[] = {1.f, 1.25f, 1.5f, 2.f, 1.f};
    for(unsigned i = 0u; i < 5u; ++i)
    {
      // Inject the provider's event; exercise the real control callback,
      // UiConfig mapping, invalidation, layout and inline visual publication.
      settings.FontSizeChangedSignal().Emit(values[i]);
      DALI_TEST_EQUALS(control.GetAdjustedFontSizeScale(), scales[i], EPSILON, TEST_LOCATION);
      impl.OnRelayout(Vector2(600.f, 200.f), container);
      auto visual = getVisual();
      DALI_TEST_CHECK(visual);
      DALI_TEST_EQUALS(VisualSize(visual), Vector2(24.f, 24.f) * scales[i], EPSILON, TEST_LOCATION);
      Property::Map properties;
      visual.CreatePropertyMap(properties);
      const int requested = static_cast<int>(24.f * scales[i]);
      DALI_TEST_EQUALS(properties.Find(Ui::Integration::ImageVisual::Property::DESIRED_WIDTH)->Get<int>(), requested, TEST_LOCATION);
    }
  };
  exercise(label, static_cast<Ui::Integration::LabelImpl&>(GetImpl(label)), [&]()
  {
    auto& data = Ui::Internal::ViewDataImpl::Get(GetImpl(label));
    return data.GetVisual(label.GetPropertyIndex("__dali_ui_inline_replacement_0"));
  });
  const auto editableVisual = [](auto& impl)
  {
    auto* data = Ui::Internal::Text::GetEditableInlineReplacementData(impl);
    DALI_TEST_CHECK(data);
    auto layer = data->visualLayer;
    return Ui::Internal::ViewDataImpl::Get(GetImpl(layer)).GetVisual(layer.GetPropertyIndex("__dali_ui_inline_replacement_0"));
  };
  exercise(field, static_cast<Ui::Integration::InputFieldImpl&>(GetImpl(field)), [&]()
  { return editableVisual(GetImpl(field)); });
  exercise(editor, static_cast<Ui::Integration::InputEditorImpl&>(GetImpl(editor)), [&]()
  { return editableVisual(GetImpl(editor)); });
  END_TEST;
}

int UtcDaliImageSpanSystemFontScaleWrappingP(void)
{
  UiTestApplication application;
  auto              controller = Text::Controller::New();
  auto              builder    = Text::StyledTextBuilder::New();
  for(unsigned i = 0u; i < 2u; ++i)
  {
    builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
    builder.SetSpan(Text::ImageSpan::New(Text::ImageAttributes("missing.png", Vector2(24.f, 60.f))), i, i + 1u);
  }
  controller->SetStyledText(builder.Build());
  controller->SetDefaultFontSize(20.f, Text::Controller::PIXEL_SIZE);
  controller->SetRelativeLineSize(-1.f);
  controller->SetSystemFontSizeScaleEnabled(true);
  controller->SetTextElideEnabled(false);
  controller->GetLayoutEngine().SetLayout(Text::Layout::Engine::MULTI_LINE_BOX);
  controller->SetLineWrapMode(Text::LineWrapMode::CHARACTER);
  auto& impl = Text::Controller::Impl::GetImplementation(*controller);
  for(float scale : {1.f, 1.25f, 1.5f, 2.f, 1.f})
  {
    controller->SetSystemFontSizeScale(scale);
    controller->Relayout(Size(55.f, 400.f));
    const auto& state = impl.GetReplacementRenderState();
    DALI_TEST_EQUALS(state.placements.Count(), 2u, TEST_LOCATION);
    const auto& lines = state.processingModel->mVisualModel->mLines;
    DALI_TEST_EQUALS(lines.Count(), scale == 1.f ? 1u : 2u, TEST_LOCATION);
    DALI_TEST_EQUALS(state.placements[1u].lineIndex, scale == 1.f ? 0u : 1u, TEST_LOCATION);
    for(const auto& line : lines)
    {
      DALI_TEST_EQUALS(line.width, (scale == 1.f ? 48.f : 24.f * scale), EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(line.ascender - line.descender, 60.f * scale, EPSILON, TEST_LOCATION);
    }
  }
  END_TEST;
}

int UtcDaliImageSpanSystemFontScaleLayoutSemanticsP(void)
{
  UiTestApplication application;
  for(float scale : {1.f, 1.25f, 1.5f, 2.f})
  {
    for(auto alignment : {Text::Alignment::START, Text::Alignment::CENTER, Text::Alignment::END})
    {
      for(auto imageAlignment : {Text::ImageAttributes::InlineAlignment::TEXT_BOTTOM,
                                 Text::ImageAttributes::InlineAlignment::TEXT_BASELINE,
                                 Text::ImageAttributes::InlineAlignment::TEXT_CENTER})
      {
        for(bool rtl : {false, true})
        {
          for(bool ellipsis : {false, true})
          {
            Text::ControllerPtr controllers[2];
            for(unsigned index = 0u; index < 2u; ++index)
            {
              // Independently authored reference: same final font and box sizes,
              // with FontSizeScale left at 1. This also checks baseline/offset
              // semantics against the pre-existing unscaled layout contract.
              const float authoredScale = index == 0u ? 1.f : scale;
              auto        builder       = Text::StyledTextBuilder::New(rtl ? "אב A " : "AB 한 ");
              for(unsigned image = 0u; image < 2u; ++image)
              {
                const auto start = builder.GetUtf32Length();
                builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);
                Text::ImageAttributes attributes("missing.png", Vector2(24.f, 36.f) * authoredScale);
                attributes.SetAlignment(imageAlignment);
                attributes.SetVerticalOffset(-3.f * authoredScale);
                builder.SetSpan(Text::ImageSpan::New(attributes), start, start + 1u);
                builder.AppendText(" DEF ");
              }
              builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(Color::RED)), 0u, builder.GetUtf32Length());
              auto controller    = Text::Controller::New();
              controllers[index] = controller;
              controller->SetDefaultFontSize(20.f * authoredScale, Text::Controller::PIXEL_SIZE);
              controller->SetRelativeLineSize(-1.f);
              controller->SetStyledText(builder.Build());
              controller->SetSystemFontSizeScaleEnabled(true);
              controller->SetSystemFontSizeScale(index == 0u ? scale : 1.f);
              controller->SetTextElideEnabled(ellipsis);
              controller->SetEllipsisPosition(Text::EllipsisPosition::END);
              controller->SetHorizontalAlignment(alignment);
              controller->SetVerticalAlignment(alignment);
              controller->GetLayoutEngine().SetLayout(Text::Layout::Engine::MULTI_LINE_BOX);
              controller->Relayout(Size(ellipsis ? 90.f : 600.f, ellipsis ? 60.f : 200.f));
            }
            const auto& actual   = Text::Controller::Impl::GetImplementation(*controllers[0]).GetReplacementRenderState();
            const auto& expected = Text::Controller::Impl::GetImplementation(*controllers[1]).GetReplacementRenderState();
            DALI_TEST_EQUALS(actual.layoutSize, expected.layoutSize, EPSILON, TEST_LOCATION);
            DALI_TEST_EQUALS(actual.finalElision.textElided, expected.finalElision.textElided, TEST_LOCATION);
            DALI_TEST_EQUALS(actual.placements.Count(), 2u, TEST_LOCATION);
            for(unsigned i = 0u; i < 2u; ++i)
            {
              DALI_TEST_EQUALS(actual.placements[i].size, expected.placements[i].size, EPSILON, TEST_LOCATION);
              DALI_TEST_EQUALS(actual.placements[i].position, expected.placements[i].position, EPSILON, TEST_LOCATION);
              DALI_TEST_EQUALS(actual.placements[i].visible, expected.placements[i].visible, TEST_LOCATION);
              DALI_TEST_EQUALS(actual.placements[i].elided, expected.placements[i].elided, TEST_LOCATION);
              DALI_TEST_EQUALS(actual.placements[i].lineIndex, expected.placements[i].lineIndex, TEST_LOCATION);
            }
          }
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliImageSpanSystemFontScaleAsyncPublicationP(void)
{
  auto config = UiConfig::New();
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::NORMAL, 1.f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::LARGE, 1.25f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::EXTRA_LARGE, 1.5f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::GIANT, 2.f);
  UiTestApplication application(config);
  TextAbstraction::FontClient::Get();
  auto       texture  = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 24u, 24u);
  const auto imageUrl = Ui::ImageUrl::New(texture, true);
  {
    const char* source = imageUrl.GetUrl().CStr();
    auto        label  = Label::New();
    label.SetAsyncRendering(true);
    label.SetRenderScale(2.f);
    label.SetSystemFontSizeScaleEnabled(true);
    label.SetFontSize(20.f);
    label.SetRequestedWidth(400.f);
    label.SetRequestedHeight(150.f);
    label.SetPadding(Insets(0.f, 0.f, 0.f, 0.f));
    label.SetStyledText(ImageText("ABC ", Vector2(24.f, 24.f), true, source));
    ConnectionTracker tracker;
    unsigned          completed = 0u;
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
    { ++completed; });
    application.GetScene().Add(label);
    auto       settings = Dali::Integration::SystemSettings::Get();
    const auto drain    = [&]()
    {
      application.SendNotification();
      application.Render();
    };
    const auto waitForPublication = [&]()
    {
      WaitUntil(application, [&]()
      { return completed > 0u; });
    };
    const Dali::Integration::SystemSettings::FontSize values[] = {
      Dali::Integration::SystemSettings::FontSize::NORMAL,
      Dali::Integration::SystemSettings::FontSize::LARGE,
      Dali::Integration::SystemSettings::FontSize::EXTRA_LARGE,
      Dali::Integration::SystemSettings::FontSize::GIANT,
      Dali::Integration::SystemSettings::FontSize::NORMAL};
    const float scales[] = {1.f, 1.25f, 1.5f, 2.f, 1.f};
    auto&       viewData = Ui::Internal::ViewDataImpl::Get(GetImpl(label));
    for(unsigned i = 0u; i < 5u; ++i)
    {
      completed = 0u;
      settings.FontSizeChangedSignal().Emit(values[i]);
      waitForPublication();
      auto visual = viewData.GetVisual(label.GetPropertyIndex("__dali_ui_inline_replacement_0"));
      DALI_TEST_CHECK(visual);
      DALI_TEST_EQUALS(VisualSize(visual), Vector2(24.f, 24.f) * scales[i], EPSILON, TEST_LOCATION);
      const unsigned expected = 24u;
      CheckRequested(visual, static_cast<int>(24.f * scales[i]));
      WaitUntil(application, [&]()
      {
        return Ui::GetImplementation(visual).GetResourceStatus() != Ui::Visual::ResourceStatus::PREPARING;
      });
      DALI_TEST_EQUALS(Ui::GetImplementation(visual).GetResourceStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
      const auto loaded = visual.GetRenderer().GetTextures().GetTexture(0u);
      DALI_TEST_EQUALS(loaded.GetWidth(), expected, TEST_LOCATION);
      DALI_TEST_EQUALS(loaded.GetHeight(), expected, TEST_LOCATION);
    }
    // Queue a scale update, then supersede it before worker publication.
    completed = 0u;
    settings.FontSizeChangedSignal().Emit(Dali::Integration::SystemSettings::FontSize::GIANT);
    drain();
    settings.FontSizeChangedSignal().Emit(Dali::Integration::SystemSettings::FontSize::EXTRA_LARGE);
    drain();
    // Wait for the final generation's placement as well as worker completion;
    // the superseded render may have finished while the second event was queued.
    WaitUntil(application, [&]()
    {
      auto current = viewData.GetVisual(label.GetPropertyIndex("__dali_ui_inline_replacement_0"));
      return completed > 0u && current && VisualSize(current) == Vector2(36.f, 36.f) &&
             Ui::GetImplementation(current).GetResourceStatus() != Ui::Visual::ResourceStatus::PREPARING;
    });
    auto visual = viewData.GetVisual(label.GetPropertyIndex("__dali_ui_inline_replacement_0"));
    CheckRequested(visual, 36);
    DALI_TEST_EQUALS(Ui::GetImplementation(visual).GetResourceStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
    DALI_TEST_EQUALS(visual.GetRenderer().GetTextures().GetTexture(0u).GetWidth(), 24u, TEST_LOCATION);
    DALI_TEST_EQUALS(VisualSize(visual), Vector2(36.f, 36.f), EPSILON, TEST_LOCATION);
    visual.Reset();
    for(bool async : {false, true, false})
    {
      completed = 0u;
      label.SetAsyncRendering(async);
      WaitUntil(application, [&]()
      {
        auto current = viewData.GetVisual(label.GetPropertyIndex("__dali_ui_inline_replacement_0"));
        return (!async || completed > 0u) && current && VisualSize(current) == Vector2(36.f, 36.f) &&
               CountInlineRenderers(label) == 1u;
      });
      CheckRequested(viewData.GetVisual(label.GetPropertyIndex("__dali_ui_inline_replacement_0")), 36);
    }
    label.Unparent();
    WaitUntil(application, [&]()
    { return CountInlineRenderers(label) == 0u; });
    application.GetScene().Add(label);
    WaitUntil(application, [&]()
    { return CountInlineRenderers(label) == 1u; });
    WeakHandle<Ui::View> weakOwner(label);
    auto                 current = viewData.GetVisual(label.GetPropertyIndex("__dali_ui_inline_replacement_0"));
    auto                 renderer = current.GetRenderer();
    DALI_TEST_CHECK(renderer);
    WeakHandle<Renderer> weakRenderer(renderer);
    renderer.Reset();
    current.Reset();
    label.Unparent();
    label.Reset();
    WaitUntil(application, [&]()
    { return !weakOwner.GetHandle() && !weakRenderer.GetHandle(); });
    DALI_TEST_CHECK(!weakOwner.GetHandle());
    DALI_TEST_CHECK(!weakRenderer.GetHandle());
  }
  END_TEST;
}

// Descriptor correctness does not depend on file I/O or decoder availability.
int UtcDaliImageSpanResourceDescriptorP(void)
{
  UiTestApplication application;
  for(float uiScale : {1.f, 2.f})
  {
    InlineImageFixture fixture(application, "not-loaded.png", false);
    for(float scale : {1.f, 1.5f, 2.f, 1.25f, 1.f})
    {
      auto visual = fixture.Update(uiScale, scale / uiScale, true);
      CheckRequested(visual, static_cast<int>(24.f * scale));
      DALI_TEST_EQUALS(fixture.source.runs[0].metrics.width, 24.f, TEST_LOCATION);
      DALI_TEST_EQUALS(fixture.placements[0].size, Vector2(24.f, 24.f) * scale, EPSILON, TEST_LOCATION);
    }
    fixture.placements[0].elided = true;
    DALI_TEST_CHECK(!fixture.Update(uiScale, 2.f / uiScale));
    fixture.placements[0].elided  = false;
    fixture.placements[0].visible = false;
    DALI_TEST_CHECK(!fixture.Update(uiScale, 2.f / uiScale));
    fixture.placements[0].visible       = true;
    fixture.source.runs[0].image.source = "replacement-not-loaded.png";
    CheckRequested(fixture.Update(uiScale, 2.f / uiScale, true), 48);
  }
  END_TEST;
}

// Real PNG decoding/upload sizes, with embedded bytes and mocked graphics.
int UtcDaliImageSpanResourceResolutionP(void)
{
  UiTestApplication  application;
  const auto         png = EncodedPng();
  InlineImageFixture ui(application, png.GetUrl().CStr());
  InlineImageFixture font(application, png.GetUrl().CStr());
  CheckRequested(ui.Update(2.f, 1.f), 48);
  CheckRequested(font.Update(1.f, 2.f), 48);
  const auto uiTexture   = ui.Loaded(application);
  const auto fontTexture = font.Loaded(application);
  DALI_TEST_EQUALS(uiTexture.GetWidth(), 48u, TEST_LOCATION);
  DALI_TEST_EQUALS(uiTexture.GetHeight(), 48u, TEST_LOCATION);
  DALI_TEST_EQUALS(fontTexture.GetWidth(), 48u, TEST_LOCATION);
  DALI_TEST_EQUALS(fontTexture.GetHeight(), 48u, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualSize(ui.Visual()), Vector2(48.f, 48.f), EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(VisualSize(font.Visual()), Vector2(48.f, 48.f), EPSILON, TEST_LOCATION);
  tet_printf("UI=2/Font=1 and UI=1/Font=2: requested=48x48 texture=48x48 display=48x48\n");
  END_TEST;
}

int UtcDaliImageSpanResourceRuntimeP(void)
{
  UiTestApplication application;
  auto              texture = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 24u, 24u);
  const auto        fixed   = Ui::ImageUrl::New(texture, true);
  for(bool uiScale : {false, true})
  {
    InlineImageFixture fixture(application, fixed.GetUrl().CStr());
    for(float scale : {1.f, 2.f, 1.f})
    {
      CheckRequested(fixture.Update(uiScale ? scale : 1.f, uiScale ? 1.f : scale), static_cast<int>(24.f * scale));
      const auto loaded = fixture.Loaded(application);
      DALI_TEST_EQUALS(loaded.GetWidth(), 24u, TEST_LOCATION);
      DALI_TEST_EQUALS(loaded.GetHeight(), 24u, TEST_LOCATION);
      DALI_TEST_EQUALS(VisualSize(fixture.Visual()), Vector2(24.f, 24.f) * scale, EPSILON, TEST_LOCATION);
      CheckRequested(fixture.Update(uiScale ? scale : 1.f, uiScale ? 1.f : scale, true), static_cast<int>(24.f * scale));
      fixture.Loaded(application); // Also checks that exactly one renderer is attached.
    }
  }

  // Use the existing ReplacementProjection UTC ResourceReady injection pattern.
  // Keep the old visual pending off scene: no decoder, files or worker ordering.
  for(bool destroy : {false, true})
  {
    Ui::Integration::Visual::Base pending;
    WeakHandle<Ui::View>          weakOwner;
    {
      InlineImageFixture fixture(application, "not-loaded.png", false);
      pending = fixture.Update(1.f, 2.f);
      DALI_TEST_EQUALS(Ui::GetImplementation(pending).GetResourceStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
      weakOwner = WeakHandle<Ui::View>(fixture.owner);
      if(!destroy)
      {
        fixture.source.runs[0].image.source = fixed.GetUrl().CStr();
        fixture.Update(1.f, 1.f);
        application.GetScene().Add(fixture.owner);
        fixture.Loaded(application);
        Ui::GetImplementation(pending).ResourceReady(Ui::Visual::ResourceStatus::READY);
        fixture.manager.Refresh();
        Property::Map properties;
        fixture.Visual().CreatePropertyMap(properties);
        DALI_TEST_EQUALS(properties.Find(Ui::Integration::ImageVisual::Property::URL)->Get<Dali::String>(),
                         fixed.GetUrl(), TEST_LOCATION);
        CheckRequested(fixture.Visual(), 24);
        DALI_TEST_EQUALS(fixture.owner.GetRendererCount(), 1u, TEST_LOCATION);
      }
    }
    // A delayed completion after owner/manager destruction must stay detached.
    Ui::GetImplementation(pending).ResourceReady(Ui::Visual::ResourceStatus::READY);
    application.RunIdles();
    DALI_TEST_CHECK(!weakOwner.GetHandle());
  }
  END_TEST;
}
