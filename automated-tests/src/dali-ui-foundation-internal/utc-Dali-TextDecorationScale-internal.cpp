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
#include <dali-ui/ui-event-thread-callback.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/cursor-helper-functions.h>
#include <dali-ui-foundation/internal/text/decoration-scale.h>
#include <dali-ui-foundation/internal/text/rendering/styles/underline-helper-functions.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/replacement/replacement-processing-source.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_text_decoration_scale_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_decoration_scale_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
// Explicit instantiation permits naming private members without rewriting class
// declarations. Keep production access levels (and MSVC symbol names) intact.
template<typename Tag, typename Tag::Type member>
struct TestMemberAccess
{
  friend typename Tag::Type GetMember(Tag)
  {
    return member;
  }
};

struct LabelController
{
  using Type = Text::ControllerPtr Ui::Integration::LabelImpl::*;
  friend Type GetMember(LabelController);
};
struct FieldController
{
  using Type = Text::ControllerPtr Ui::Integration::InputFieldImpl::*;
  friend Type GetMember(FieldController);
};
struct EditorController
{
  using Type = Text::ControllerPtr Ui::Integration::InputEditorImpl::*;
  friend Type GetMember(EditorController);
};
struct LabelParameters
{
  using Type = Text::AsyncTextParameters (Ui::Integration::LabelImpl::*)(Ui::Integration::Text::Async::RequestType,
                                                                       const Vector2&, const Insets&, Dali::LayoutDirection::Type);
  friend Type GetMember(LabelParameters);
};
struct WorkerModel
{
  using Type = const Text::Model* (Text::Internal::AsyncTextLoader::*)() const;
  friend Type GetMember(WorkerModel);
};

template struct TestMemberAccess<LabelController, &Ui::Integration::LabelImpl::mController>;
template struct TestMemberAccess<FieldController, &Ui::Integration::InputFieldImpl::mController>;
template struct TestMemberAccess<EditorController, &Ui::Integration::InputEditorImpl::mController>;
template struct TestMemberAccess<LabelParameters, &Ui::Integration::LabelImpl::GetAsyncTextParameters>;
template struct TestMemberAccess<WorkerModel, &Text::Internal::AsyncTextLoader::GetRenderTextModel>;

unsigned asyncCompletions = 0u;
void     OnDecorationRenderFinished(View, float, float)
{
  ++asyncCompletions;
}

constexpr float    EPSILON          = 0.001f;
constexpr float    SCALES[]         = {1.f, 1.25f, 1.5f, 2.f, 1.f};
constexpr uint16_t OUTLINE_WIDTHS[] = {2u, 3u, 3u, 4u, 2u};
const Size         BOX(320.f, 160.f);

struct ScaleGuard
{
  ScaleGuard()
  : scale(UiScaleManager::Get().GetScale()),
    scalable(UiScaleManager::Get().IsScalable())
  {
    UiScaleManager::Get().SetScalable(true);
    UiScaleManager::Get().SetScale(1.f);
  }
  ~ScaleGuard()
  {
    UiScaleManager::Get().SetScale(scale);
    UiScaleManager::Get().SetScalable(scalable);
  }
  float scale;
  bool  scalable;
};

Ui::Integration::LabelImpl& Impl(Label control)
{
  return static_cast<Ui::Integration::LabelImpl&>(GetImpl(control));
}
Ui::Integration::InputFieldImpl& Impl(InputField control)
{
  return static_cast<Ui::Integration::InputFieldImpl&>(GetImpl(control));
}
Ui::Integration::InputEditorImpl& Impl(InputEditor control)
{
  return static_cast<Ui::Integration::InputEditorImpl&>(GetImpl(control));
}
Text::ControllerPtr ControllerFor(Label control)
{
  return Impl(control).*GetMember(LabelController{});
}
Text::ControllerPtr ControllerFor(InputField control)
{
  return Impl(control).*GetMember(FieldController{});
}
Text::ControllerPtr ControllerFor(InputEditor control)
{
  return Impl(control).*GetMember(EditorController{});
}
const Text::Model& RenderModel(Text::AsyncTextLoader loader)
{
  return *(Text::GetImplementation(loader).*GetMember(WorkerModel{}))();
}
Text::Model& Model(Text::ControllerPtr controller)
{
  return *Text::Controller::Impl::GetImplementation(*controller).mModel;
}

Text::Underline Underline(Text::Underline::Type type, float multiplier = 1.f, float height = 2.f)
{
  Text::Underline style;
  style.SetColor(Color::RED);
  style.SetType(type);
  style.SetThickness(height * multiplier);
  style.SetDashLength(4.f * multiplier);
  style.SetDashGap(2.f * multiplier);
  return style;
}

Text::LineThrough LineThrough(float multiplier = 1.f)
{
  Text::LineThrough style;
  style.SetColor(Color::GREEN);
  style.SetThickness(2.f * multiplier);
  return style;
}

Text::StyledText Styled(int authoring, Text::Underline::Type type, float multiplier = 1.f, bool overlap = false)
{
  if(authoring == 2)
  {
    const char*        token = type == Text::Underline::Type::SOLID ? "solid" : type == Text::Underline::Type::DASHED ? "dashed"
                                                                                                                      : "double";
    std::ostringstream markup;
    markup << "<u color='red' type='" << token << "' height='" << 2.f * multiplier
           << "' dash-width='" << 4.f * multiplier << "' dash-gap='" << 2.f * multiplier
           << "'><s color='green' height='" << 2.f * multiplier << "'>ABCD</s></u>";
    return Text::StyledText::FromMarkup(markup.str().c_str());
  }
  auto builder = Text::StyledTextBuilder::New("ABCD");
  builder.SetSpan(Text::UnderlineSpan::New(Underline(type, multiplier)), 0u, 4u);
  builder.SetSpan(Text::LineThroughSpan::New(LineThrough(multiplier)), 0u, 4u);
  if(overlap)
  {
    builder.SetSpan(Text::UnderlineSpan::New(Underline(type, multiplier, 3.f)), 1u, 3u);
    auto line = LineThrough(multiplier);
    line.SetThickness(3.f * multiplier);
    builder.SetSpan(Text::LineThroughSpan::New(line), 1u, 3u);
  }
  return builder.Build();
}

template<typename Control>
void Configure(Control control, int authoring, Text::Underline::Type type)
{
  control.SetFontSize(24.f);
  control.SetSystemFontSizeScaleEnabled(false);
  control.SetRequestedWidth(BOX.x);
  control.SetRequestedHeight(BOX.y);
  control.SetPadding(Insets(0.f, 0.f, 0.f, 0.f));
  control.SetTextColor(Color::BLACK);
  if(authoring == 0)
  {
    control.SetText("ABCD");
    control.SetTextUnderline(Underline(type));
    control.SetTextLineThrough(LineThrough());
  }
  else
  {
    control.SetTextUnderline(Text::Underline::None());
    control.SetTextLineThrough(Text::LineThrough::None());
    control.SetStyledText(Styled(authoring, type));
  }
  Text::Shadow shadow;
  shadow.SetOffset(Vector2(-2.f, 3.f));
  shadow.SetBlurRadius(1.25f);
  shadow.SetColor(Color::BLUE);
  control.SetTextShadow(shadow);
  Text::Outline outline;
  outline.SetWidth(2.f);
  outline.SetOffset(Vector2(2.f, -3.f));
  outline.SetBlurRadius(0.5f);
  outline.SetColor(Color::YELLOW);
  control.SetTextOutline(outline);
}

std::vector<uint8_t> Bytes(PixelData data)
{
  if(!data) return {};
  auto       buffer = Dali::Integration::GetPixelDataBuffer(data);
  const auto count  = data.GetWidth() * data.GetHeight() * Pixel::GetBytesPerPixel(data.GetPixelFormat());
  return std::vector<uint8_t>(buffer.buffer, buffer.buffer + count);
}

PixelData Render(Text::Model& model)
{
  return Text::Typesetter::New(&model)->Render(BOX, Text::Direction::LEFT_TO_RIGHT);
}

void EqualPixels(PixelData actual, PixelData expected)
{
  DALI_TEST_EQUALS(static_cast<bool>(actual), static_cast<bool>(expected), TEST_LOCATION);
  if(actual && expected)
  {
    DALI_TEST_EQUALS(actual.GetWidth(), expected.GetWidth(), TEST_LOCATION);
    DALI_TEST_EQUALS(actual.GetHeight(), expected.GetHeight(), TEST_LOCATION);
    DALI_TEST_EQUALS(actual.GetPixelFormat(), expected.GetPixelFormat(), TEST_LOCATION);
    DALI_TEST_CHECK(Bytes(actual) == Bytes(expected));
  }
}

void CheckGeometry(const Text::Model& model, float scale, uint16_t width, bool spans)
{
  auto& visual = *model.mVisualModel;
  DALI_TEST_EQUALS(visual.GetShadowOffset(), Vector2(-2.f, 3.f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOutlineOffset(), Vector2(2.f, -3.f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOutlineWidth(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(model.GetShadowOffset(), Vector2(-2.f, 3.f) * scale, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(model.GetOutlineOffset(), Vector2(2.f, -3.f) * scale, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(model.GetOutlineWidth(), width, TEST_LOCATION);
  DALI_TEST_EQUALS(model.GetShadowBlurRadius(), 1.25f, TEST_LOCATION);
  DALI_TEST_EQUALS(model.GetOutlineBlurRadius(), 0.5f, TEST_LOCATION);
  if(spans)
  {
    DALI_TEST_CHECK(!visual.mUnderlineRuns.Empty());
    DALI_TEST_CHECK(!visual.mStrikethroughRuns.Empty());
    for(const auto& run : visual.mUnderlineRuns)
    {
      DALI_TEST_EQUALS(run.properties.height, 2.f * scale, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(run.properties.dashWidth, 4.f * scale, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(run.properties.dashGap, 2.f * scale, EPSILON, TEST_LOCATION);
    }
    for(const auto& run : visual.mStrikethroughRuns)
    {
      DALI_TEST_EQUALS(run.properties.height, 2.f * scale, EPSILON, TEST_LOCATION);
    }
    for(const auto& run : model.mLogicalModel->mUnderlinedCharacterRuns)
    {
      DALI_TEST_EQUALS(run.properties.height, 2.f, TEST_LOCATION);
      DALI_TEST_EQUALS(run.properties.dashWidth, 4.f, TEST_LOCATION);
      DALI_TEST_EQUALS(run.properties.dashGap, 2.f, TEST_LOCATION);
    }
    for(const auto& run : model.mLogicalModel->mStrikethroughCharacterRuns)
    {
      DALI_TEST_EQUALS(run.properties.height, 2.f, TEST_LOCATION);
    }
  }
  else
  {
    DALI_TEST_EQUALS(visual.GetUnderlineHeight(), 2.f, TEST_LOCATION);
    DALI_TEST_EQUALS(model.GetUnderlineHeight(), 2.f * scale, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(model.GetDashedUnderlineWidth(), 4.f * scale, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(model.GetDashedUnderlineGap(), 2.f * scale, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(model.GetStrikethroughHeight(), 2.f * scale, EPSILON, TEST_LOCATION);
  }
}

template<typename Control>
void ControlMatrix(UiTestApplication& application, Control control)
{
  ScaleGuard guard;
  application.GetScene().Add(control);
  for(auto type : {Text::Underline::Type::SOLID, Text::Underline::Type::DASHED, Text::Underline::Type::DOUBLE})
  {
    std::vector<std::vector<uint8_t>> reference;
    for(int authoring : {0, 1, 2})
    {
      Configure(control, authoring, type);
      std::vector<uint8_t> baseline;
      unsigned             combination = 0u;
      for(float fontScale : {1.f, 2.f})
      {
        control.SetMinimumFontSizeScale(fontScale);
        control.SetMaximumFontSizeScale(fontScale);
        for(unsigned index = 0u; index < 5u; ++index, ++combination)
        {
          const float scale = SCALES[index];
          UiScaleManager::Get().SetScale(scale);
          application.SendNotification();
          application.Render();
          auto  controller = ControllerFor(control);
          auto& model      = Model(controller);
          DALI_TEST_EQUALS(controller->GetUiScale(), scale, EPSILON, TEST_LOCATION);
          DALI_TEST_EQUALS(controller->GetEffectiveTextScale(), scale * fontScale, EPSILON, TEST_LOCATION);
          CheckGeometry(model, scale, OUTLINE_WIDTHS[index], authoring != 0);
          DALI_TEST_EQUALS(control.GetTextOutline().GetWidth(), 2.f, TEST_LOCATION);
          DALI_TEST_EQUALS(control.GetTextShadow().GetOffset(), Vector2(-2.f, 3.f), TEST_LOCATION);
          auto& view = controller->GetView();
          DALI_TEST_EQUALS(view.GetOutlineWidth(), OUTLINE_WIDTHS[index], TEST_LOCATION);
          DALI_TEST_EQUALS(view.GetShadowOffset(), model.GetShadowOffset(), TEST_LOCATION);
          if(authoring == 0)
          {
            DALI_TEST_EQUALS(control.GetTextUnderline().GetThickness(), 2.f, TEST_LOCATION);
            DALI_TEST_EQUALS(view.GetUnderlineHeight(), 2.f * scale, EPSILON, TEST_LOCATION);
          }
          const auto pixels = Bytes(Render(model));
          DALI_TEST_CHECK(!pixels.empty());
          if(authoring == 0)
            reference.push_back(pixels);
          else
          {
            DALI_TEST_CHECK(pixels == reference[combination]);
          }
          if(index == 0u) baseline = pixels;
          if(index == 4u)
          {
            DALI_TEST_CHECK(pixels == baseline);
          }
        }
      }
      control.SetText("ABCD");
      control.SetTextUnderline(Text::Underline::None());
      control.SetTextLineThrough(Text::LineThrough::None());
      control.SetTextShadow(Text::Shadow::None());
      control.SetTextOutline(Text::Outline::None());
      UiScaleManager::Get().SetScale(1.5f);
      application.SendNotification();
      application.Render();
      auto& plain = Model(ControllerFor(control));
      DALI_TEST_EQUALS(plain.mVisualModel->mUnderlineRuns.Count(), 0u, TEST_LOCATION);
      DALI_TEST_EQUALS(plain.mVisualModel->mStrikethroughRuns.Count(), 0u, TEST_LOCATION);
      DALI_TEST_CHECK(!plain.IsUnderlineEnabled() && !plain.IsStrikethroughEnabled());
      DALI_TEST_CHECK(!plain.IsShadowEnabled() && !plain.IsOutlineEnabled());
      Configure(control, authoring, type);
      application.SendNotification();
      application.Render();
      auto& reapplied = Model(ControllerFor(control));
      CheckGeometry(reapplied, 1.5f, 3u, authoring != 0);
      // The matrix's F=2, U=1.5 entry is the independent pre-removal result.
      DALI_TEST_CHECK(Bytes(Render(reapplied)) == reference[7u]);
    }
  }
  control.Unparent();
}

Text::AsyncTextParameters Parameters(Label label)
{
  return (Impl(label).*GetMember(LabelParameters{}))(Ui::Integration::Text::Async::RENDER_FIXED_SIZE, BOX,
                                                    Insets(0.f, 0.f, 0.f, 0.f), Dali::LayoutDirection::LEFT_TO_RIGHT);
}

template<typename Control>
void PolicyMatrix(UiTestApplication& application, Control control)
{
  ScaleGuard guard;
  auto       enabledRoot = View::New();
  enabledRoot.SetUiScalePolicy(UiScalePolicy::ENABLED);
  enabledRoot.SetRequestedWidth(1000.f);
  enabledRoot.SetRequestedHeight(1000.f);
  auto disabledRoot = View::New();
  disabledRoot.SetUiScalePolicy(UiScalePolicy::DISABLED);
  disabledRoot.SetRequestedWidth(1000.f);
  disabledRoot.SetRequestedHeight(1000.f);
  auto branch = View::New();
  branch.SetRequestedWidth(800.f);
  branch.SetRequestedHeight(800.f);
  enabledRoot.Add(branch);
  branch.Add(control);
  application.GetScene().Add(enabledRoot);
  application.GetScene().Add(disabledRoot);
  auto                  label = Label::DownCast(control);
  Text::AsyncTextLoader worker;
  if(label)
  {
    label.SetAsyncRendering(false);
    worker = Text::AsyncTextLoader::New();
  }
  for(int authoring : {0, 1, 2})
  {
    Configure(control, authoring, Text::Underline::Type::DASHED);
    const auto                        source = label ? label.GetStyledText() : ControllerFor(control)->GetStyledText();
    std::vector<std::vector<uint8_t>> baselines(3u);
    auto                              check = [&](float scale)
    {
      application.SendNotification();
      application.Render();
      application.SendNotification();
      application.Render();
      auto           controller = ControllerFor(control);
      const uint16_t width      = scale == 1.f ? 2u : scale == 2.f ? 4u
                                                                   : 3u;
      DALI_TEST_EQUALS(GetImpl(control).GetEffectiveScale(), scale, EPSILON, TEST_LOCATION);
      DALI_TEST_EQUALS(controller->GetUiScale(), scale, EPSILON, TEST_LOCATION);
      CheckGeometry(Model(controller), scale, width, authoring != 0);
      DALI_TEST_EQUALS(controller->GetView().GetOutlineWidth(), width, TEST_LOCATION);
      DALI_TEST_EQUALS(control.GetTextShadow().GetOffset(), Vector2(-2.f, 3.f), TEST_LOCATION);
      DALI_TEST_EQUALS(control.GetTextOutline().GetWidth(), 2.f, TEST_LOCATION);
      if(authoring != 0)
      {
        if(label)
        {
          DALI_TEST_CHECK(label.GetStyledText() == source);
        }
        for(uint32_t spanIndex = 0u; spanIndex < source.GetSpanCount(); ++spanIndex)
        {
          auto underline = Text::UnderlineSpan::DownCast(source.GetSpanAt(spanIndex));
          if(underline)
          {
            DALI_TEST_EQUALS(underline.GetUnderline().GetThickness(), 2.f, TEST_LOCATION);
            DALI_TEST_EQUALS(underline.GetUnderline().GetDashLength(), 4.f, TEST_LOCATION);
            DALI_TEST_EQUALS(underline.GetUnderline().GetDashGap(), 2.f, TEST_LOCATION);
          }
          auto lineThrough = Text::LineThroughSpan::DownCast(source.GetSpanAt(spanIndex));
          if(lineThrough)
          {
            DALI_TEST_EQUALS(lineThrough.GetLineThrough().GetThickness(), 2.f, TEST_LOCATION);
          }
        }
      }
      const auto pixels   = Bytes(Render(Model(controller)));
      auto&      baseline = baselines[scale == 1.f ? 0u : scale == 2.f ? 2u
                                                                       : 1u];
      if(baseline.empty())
        baseline = pixels;
      else
      {
        DALI_TEST_CHECK(pixels == baseline);
      }
      if(label)
      {
        auto parameters = Parameters(label);
        DALI_TEST_EQUALS(parameters.decorationUiScale, scale, TEST_LOCATION);
        worker.RenderText(parameters, false, Size::ZERO);
        CheckGeometry(RenderModel(worker), scale, width, authoring != 0);
      }
    };
    UiScaleManager::Get().SetScale(2.f);
    control.SetUiScalePolicy(UiScalePolicy::INHERIT);
    check(2.f);
    control.SetUiScalePolicy(UiScalePolicy::DISABLED);
    check(1.f);
    control.SetUiScalePolicy(UiScalePolicy::ENABLED);
    check(2.f);
    enabledRoot.SetUiScalePolicy(UiScalePolicy::DISABLED);
    check(2.f); // Explicit ENABLED overrides the disabled ancestor.
    control.SetUiScalePolicy(UiScalePolicy::INHERIT);
    check(1.f);
    enabledRoot.SetUiScalePolicy(UiScalePolicy::ENABLED);
    check(2.f);
    UiScaleManager::Get().SetScale(1.5f);
    check(1.5f);
    branch.Unparent();
    disabledRoot.Add(branch);
    check(1.f);
    UiScaleManager::Get().SetScale(2.f);
    check(1.f);
    branch.SetUiScalePolicy(UiScalePolicy::ENABLED);
    check(2.f);
    branch.SetUiScalePolicy(UiScalePolicy::DISABLED);
    check(1.f);
    branch.SetUiScalePolicy(UiScalePolicy::INHERIT);
    branch.Unparent();
    enabledRoot.Add(branch);
    check(2.f);
    UiScaleManager::Get().SetScale(1.f);
    check(1.f);
  }
  control.Unparent();
  enabledRoot.Unparent();
  disabledRoot.Unparent();
}
} // namespace

int UtcDaliTextDecorationScaleLabelP(void)
{
  UiTestApplication application;
  auto              label = Label::New();
  label.SetAsyncRendering(false);
  ControlMatrix(application, label);
  END_TEST;
}

int UtcDaliTextDecorationScaleInputFieldP(void)
{
  UiTestApplication application;
  ControlMatrix(application, InputField::New());
  END_TEST;
}

int UtcDaliTextDecorationScaleInputEditorP(void)
{
  UiTestApplication application;
  ControlMatrix(application, InputEditor::New());
  END_TEST;
}

int UtcDaliTextDecorationScaleAsyncP(void)
{
  UiTestApplication application;
  ScaleGuard        guard;
  auto              label = Label::New();
  label.SetAsyncRendering(false);
  application.GetScene().Add(label);
  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  Text::AsyncTextLoader oracle = Text::AsyncTextLoader::New();
  for(auto type : {Text::Underline::Type::SOLID, Text::Underline::Type::DASHED, Text::Underline::Type::DOUBLE})
  {
    for(int authoring : {0, 1, 2})
    {
      Configure(label, authoring, type);
      for(float fontScale : {1.f, 2.f})
      {
        label.SetMinimumFontSizeScale(fontScale);
        label.SetMaximumFontSizeScale(fontScale);
        std::vector<uint8_t> baseline;
        for(unsigned index = 0u; index < 5u; ++index)
        {
          const float scale = SCALES[index];
          UiScaleManager::Get().SetScale(scale);
          application.SendNotification();
          application.Render();
          auto p = Parameters(label);
          DALI_TEST_EQUALS(p.decorationUiScale, scale, TEST_LOCATION);
          DALI_TEST_EQUALS(p.effectiveTextScale, scale * fontScale, EPSILON, TEST_LOCATION);
          DALI_TEST_EQUALS(p.renderScale, 1.f, TEST_LOCATION);
          auto result = loader.RenderText(p, false, Size::ZERO);
          CheckGeometry(RenderModel(loader), scale, OUTLINE_WIDTHS[index], authoring != 0);
          // Independent scale-1 oracle: author final distances with the same
          // effective font size. Compare actual worker raster output, not just getters.
          auto expected                 = p;
          expected.decorationUiScale    = 1.f;
          expected.underlineHeight      = 2.f * scale;
          expected.dashedUnderlineWidth = 4.f * scale;
          expected.dashedUnderlineGap   = 2.f * scale;
          expected.strikethroughHeight  = 2.f * scale;
          expected.shadowOffset         = Vector2(-2.f, 3.f) * scale;
          expected.outlineOffset        = Vector2(2.f, -3.f) * scale;
          expected.outlineWidth         = OUTLINE_WIDTHS[index];
          if(authoring != 0)
          {
            uint32_t dpi = 0u, verticalDpi = 0u;
            TextAbstraction::FontClient::Get().GetDpi(dpi, verticalDpi);
            expected.styledTextStyleSnapshot = Ui::Internal::Text::StyledTextApplier::BuildTextStyleRunSnapshot(Styled(authoring, type, scale), static_cast<float>(dpi));
          }
          auto wanted = oracle.RenderText(expected, false, Size::ZERO);
          EqualPixels(result.textPixelData, wanted.textPixelData);
          EqualPixels(result.stylePixelData, wanted.stylePixelData);
          EqualPixels(result.overlayStylePixelData, wanted.overlayStylePixelData);
          if(index == 0u) baseline = Bytes(result.overlayStylePixelData);
          if(index == 4u)
          {
            DALI_TEST_CHECK(Bytes(result.overlayStylePixelData) == baseline);
          }
        }
      }
      UiScaleManager::Get().SetScale(1.5f);
      application.SendNotification();
      application.Render();
      auto beforeParameters = Parameters(label);
      auto beforeRemoval    = loader.RenderText(beforeParameters, false, Size::ZERO);
      label.SetText("ABCD");
      label.SetTextUnderline(Text::Underline::None());
      label.SetTextLineThrough(Text::LineThrough::None());
      label.SetTextShadow(Text::Shadow::None());
      label.SetTextOutline(Text::Outline::None());
      UiScaleManager::Get().SetScale(2.f);
      application.SendNotification();
      application.Render();
      auto p     = Parameters(label);
      auto plain = loader.RenderText(p, false, Size::ZERO);
      DALI_TEST_CHECK(!plain.overlayStylePixelData);
      DALI_TEST_CHECK(!plain.stylePixelData);
      Configure(label, authoring, type);
      UiScaleManager::Get().SetScale(1.5f);
      application.SendNotification();
      application.Render();
      p              = Parameters(label);
      auto reapplied = loader.RenderText(p, false, Size::ZERO);
      CheckGeometry(RenderModel(loader), 1.5f, 3u, authoring != 0);
      EqualPixels(reapplied.textPixelData, beforeRemoval.textPixelData);
      EqualPixels(reapplied.stylePixelData, beforeRemoval.stylePixelData);
      EqualPixels(reapplied.overlayStylePixelData, beforeRemoval.overlayStylePixelData);
    }
  }
  END_TEST;
}

int UtcDaliTextDecorationScaleAutomaticP(void)
{
  UiTestApplication application;
  auto              scaled = Text::Controller::New();
  auto              oracle = Text::Controller::New();
  for(auto controller : {scaled, oracle})
  {
    controller->SetText("ABCD");
    controller->SetTextElideEnabled(false);
    controller->SetUnderlineEnabled(true);
    controller->SetUnderlineHeight(0.f);
    controller->SetStrikethroughEnabled(true);
    controller->SetStrikethroughHeight(0.f);
  }
  for(float fontScale : {1.f, 2.f})
  {
    for(float scale : SCALES)
    {
      scaled->SetDefaultFontSize(24.f, Text::Controller::PIXEL_SIZE);
      scaled->SetMinimumFontSizeScale(1.f);
      scaled->SetMaximumFontSizeScale(2.f);
      scaled->SetFontSizeScale(fontScale);
      if(scaled->SetUiScale(scale)) scaled->InvalidateFontData();
      oracle->SetDefaultFontSize(24.f * scale * fontScale, Text::Controller::PIXEL_SIZE);
      scaled->Relayout(BOX);
      oracle->Relayout(BOX);
      DALI_TEST_EQUALS(Model(scaled).GetUnderlineHeight(), 0.f, TEST_LOCATION);
      DALI_TEST_EQUALS(Model(scaled).GetStrikethroughHeight(), 0.f, TEST_LOCATION);
      EqualPixels(Render(Model(scaled)), Render(Model(oracle)));
    }
  }
  END_TEST;
}

int UtcDaliTextDecorationScaleOverlapReplacementP(void)
{
  UiTestApplication application;
  auto              controller = Text::Controller::New();
  auto              source     = Styled(1, Text::Underline::Type::DASHED, 1.f, true);
  controller->SetStyledText(source);
  controller->SetDefaultFontSize(24.f, Text::Controller::PIXEL_SIZE);
  controller->SetTextElideEnabled(false);
  for(float scale : SCALES)
  {
    if(controller->SetUiScale(scale)) controller->InvalidateFontData();
    controller->Relayout(BOX);
    auto& model = Model(controller);
    auto& runs  = model.mVisualModel->mUnderlineRuns;
    DALI_TEST_EQUALS(runs.Count(), 2u, TEST_LOCATION);
    DALI_TEST_EQUALS(runs[0].properties.height, 2.f * scale, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(runs[1].properties.height, 3.f * scale, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(model.mVisualModel->mStrikethroughRuns[1].properties.height, 3.f * scale, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(model.mLogicalModel->mStrikethroughCharacterRuns[1].properties.height, 3.f, TEST_LOCATION);
    Vector<Text::UnderlinedGlyphRun>::ConstIterator it = runs.End();
    // IsGlyphUnderlined establishes the range iterator used by the renderer.
    DALI_TEST_CHECK(Text::IsGlyphUnderlined(1u, runs, it));
    auto resolved = Text::GetCurrentUnderlineProperties(1u, true, runs, it, runs[0].properties);
    DALI_TEST_EQUALS(resolved.height, 3.f * scale, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(Text::UnderlineSpan::DownCast(source.GetSpanAt(0u)).GetUnderline().GetThickness(), 2.f, TEST_LOCATION);
    DALI_TEST_EQUALS(model.mLogicalModel->mUnderlinedCharacterRuns[1].properties.height, 3.f, TEST_LOCATION);
  }

  // A replacement uses a second processing model. Its source and glyph styles
  // must carry the same UI context without scaling already resolved defaults.
  auto label = Label::New();
  label.SetAsyncRendering(false);
  Configure(label, 0, Text::Underline::Type::DASHED);
  auto builder = Text::StyledTextBuilder::New("ABCD");
  builder.SetSpan(Text::UnderlineSpan::New(Underline(Text::Underline::Type::DASHED)), 0u, 4u);
  builder.SetSpan(Text::LineThroughSpan::New(LineThrough()), 0u, 4u);
  Text::ImageAttributes image("missing-decoration-scale.png", Vector2(12.f, 8.f));
  image.SetVerticalOffset(-2.f);
  builder.SetSpan(Text::ImageSpan::New(image), 1u, 2u);
  label.SetStyledText(builder.Build());
  ScaleGuard guard;
  application.GetScene().Add(label);
  Text::AsyncTextLoader loader = Text::AsyncTextLoader::New();
  for(unsigned index = 0; index < 5u; ++index)
  {
    float scale = SCALES[index];
    UiScaleManager::Get().SetScale(scale);
    application.SendNotification();
    application.Render();
    auto& state = Text::Controller::Impl::GetImplementation(*ControllerFor(label)).GetReplacementRenderState();
    DALI_TEST_CHECK(state.processingModel);
    CheckGeometry(*state.processingModel, scale, OUTLINE_WIDTHS[index], true);
    DALI_TEST_EQUALS(state.placements[0].size, Vector2(12.f, 8.f) * scale, EPSILON, TEST_LOCATION);
    auto p = Parameters(label);
    loader.RenderText(p, false, Size::ZERO);
    const auto* asyncState = Text::GetImplementation(loader).GetReplacementRenderState();
    DALI_TEST_CHECK(asyncState && asyncState->processingModel);
    CheckGeometry(*asyncState->processingModel, scale, OUTLINE_WIDTHS[index], true);
  }
  END_TEST;
}

int UtcDaliTextDecorationScaleOutlineLayoutP(void)
{
  UiTestApplication application;
  ScaleGuard        guard;
  auto              field  = InputField::New();
  auto              oracle = InputField::New();
  Configure(field, 0, Text::Underline::Type::SOLID);
  Configure(oracle, 0, Text::Underline::Type::SOLID);
  oracle.SetUiScalePolicy(UiScalePolicy::DISABLED);
  application.GetScene().Add(field);
  application.GetScene().Add(oracle);
  for(unsigned index = 0; index < 5u; ++index)
  {
    float scale = SCALES[index];
    UiScaleManager::Get().SetScale(scale);
    oracle.SetFontSize(24.f * scale);
    oracle.SetRequestedWidth(BOX.x * scale);
    oracle.SetRequestedHeight(BOX.y * scale);
    auto outline = oracle.GetTextOutline();
    outline.SetWidth(OUTLINE_WIDTHS[index]);
    oracle.SetTextOutline(outline);
    application.SendNotification();
    application.Render();
    auto&       actual    = Text::Controller::Impl::GetImplementation(*ControllerFor(field));
    auto&       expected  = Text::Controller::Impl::GetImplementation(*ControllerFor(oracle));
    const auto& positions = actual.mModel->mVisualModel->mGlyphPositions;
    const auto& wanted    = expected.mModel->mVisualModel->mGlyphPositions;
    DALI_TEST_EQUALS(positions.Count(), wanted.Count(), TEST_LOCATION);
    for(unsigned glyph = 0; glyph < positions.Count(); ++glyph)
    {
      DALI_TEST_EQUALS(positions[glyph], wanted[glyph], EPSILON, TEST_LOCATION);
    }
    for(unsigned character : {0u, 2u, 4u})
    {
      Text::CursorInfo cursor, expectedCursor;
      actual.GetCursorPosition(character, cursor);
      expected.GetCursorPosition(character, expectedCursor);
      DALI_TEST_EQUALS(cursor.primaryPosition, expectedCursor.primaryPosition, EPSILON, TEST_LOCATION);
    }
    // Preserve the atlas View's existing unsupported outline-offset behavior.
    DALI_TEST_EQUALS(ControllerFor(field)->GetView().GetOutlineOffset(), Vector2::ZERO, TEST_LOCATION);
  }
  auto model = Text::Model::New();
  model->mVisualModel->SetOutlineWidth(std::numeric_limits<uint16_t>::max());
  model->mVisualModel->SetDecorationUiScale(2.f);
  DALI_TEST_EQUALS(model->GetOutlineWidth(), std::numeric_limits<uint16_t>::max(), TEST_LOCATION);
  model->mVisualModel->SetOutlineWidth(1u);
  model->mVisualModel->SetDecorationUiScale(1.25f);
  DALI_TEST_EQUALS(model->GetOutlineWidth(), 1u, TEST_LOCATION);
  model->mVisualModel->SetDecorationUiScale(1.5f);
  DALI_TEST_EQUALS(model->GetOutlineWidth(), 2u, TEST_LOCATION);
  model->mVisualModel->SetOutlineWidth(2u);
  model->mVisualModel->SetDecorationUiScale(1.25f);
  DALI_TEST_EQUALS(model->GetOutlineWidth(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextDecorationScaleInvalidScaleP(void)
{
  UiTestApplication application;
  auto              visual = Text::VisualModel::New();
  visual->SetShadowOffset(Vector2(-2.f, 3.f));
  visual->SetOutlineWidth(2u);
  visual->SetDecorationUiScale(1.5f);
  for(float invalid : {0.f, -1.f, std::numeric_limits<float>::infinity(),
                       -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::quiet_NaN()})
  {
    // Stateless resolution falls back to authored values; stateful model
    // setters reject invalid input and retain the previous valid context.
    DALI_TEST_EQUALS(Text::ResolveDecorationOutlineWidth(2u, invalid), 2u, TEST_LOCATION);
    DALI_TEST_EQUALS(Text::ResolveDecorationDistance(2.f, invalid), 2.f, TEST_LOCATION);
    DALI_TEST_EQUALS(Text::ResolveDecorationOffset(Vector2(-2.f, 3.f), invalid), Vector2(-2.f, 3.f), TEST_LOCATION);
    DALI_TEST_CHECK(std::signbit(Text::ResolveDecorationDistance(-0.f, invalid)));
    visual->SetDecorationUiScale(invalid);
    DALI_TEST_EQUALS(visual->GetDecorationUiScale(), 1.5f, TEST_LOCATION);
    DALI_TEST_EQUALS(visual->GetEffectiveShadowOffset(), Vector2(-3.f, 4.5f), TEST_LOCATION);
    DALI_TEST_EQUALS(visual->GetEffectiveOutlineWidth(), 3u, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(Text::ResolveDecorationOutlineWidth(0u, std::numeric_limits<float>::max()), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(Text::ResolveDecorationOutlineWidth(2u, std::numeric_limits<float>::max()), std::numeric_limits<uint16_t>::max(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextDecorationScaleLifecycleP(void)
{
  UiTestApplication application;
  auto              visual = Text::VisualModel::New();
  for(bool scaleFirst : {false, true})
  {
    visual = Text::VisualModel::New();
    DALI_TEST_EQUALS(visual->GetDecorationUiScale(), 1.f, TEST_LOCATION);
    DALI_TEST_EQUALS(visual->GetEffectiveShadowOffset(), Vector2::ZERO, TEST_LOCATION);
    DALI_TEST_EQUALS(visual->GetEffectiveOutlineOffset(), Vector2::ZERO, TEST_LOCATION);
    if(scaleFirst) visual->SetDecorationUiScale(1.5f);
    visual->SetShadowOffset(Vector2(-2.f, 3.f));
    visual->SetOutlineOffset(Vector2(2.f, -3.f));
    if(!scaleFirst)
    {
      DALI_TEST_EQUALS(visual->GetEffectiveShadowOffset(), Vector2(-2.f, 3.f), TEST_LOCATION);
      DALI_TEST_EQUALS(visual->GetEffectiveOutlineOffset(), Vector2(2.f, -3.f), TEST_LOCATION);
      visual->SetDecorationUiScale(1.5f);
    }
    DALI_TEST_EQUALS(visual->GetEffectiveShadowOffset(), Vector2(-3.f, 4.5f), TEST_LOCATION);
    DALI_TEST_EQUALS(visual->GetEffectiveOutlineOffset(), Vector2(3.f, -4.5f), TEST_LOCATION);

    visual->SetShadowEnabled(true);
    visual->SetOutlineEnabled(true);
    visual->SetShadowOffset(Vector2(-4.f, 1.f));
    visual->SetOutlineOffset(Vector2(1.f, -4.f));
    visual->SetDecorationUiScale(2.f);
    visual->SetShadowEnabled(false);
    visual->SetOutlineEnabled(false);
    visual->SetDecorationUiScale(1.25f);
    visual->SetShadowEnabled(true);
    visual->SetOutlineEnabled(true);
    DALI_TEST_EQUALS(visual->GetShadowOffset(), Vector2(-4.f, 1.f), TEST_LOCATION);
    DALI_TEST_EQUALS(visual->GetOutlineOffset(), Vector2(1.f, -4.f), TEST_LOCATION);
    DALI_TEST_EQUALS(visual->GetEffectiveShadowOffset(), Vector2(-5.f, 1.25f), TEST_LOCATION);
    DALI_TEST_EQUALS(visual->GetEffectiveOutlineOffset(), Vector2(1.25f, -5.f), TEST_LOCATION);
    visual->SetShadowOffset(Vector2::ZERO);
    visual->SetOutlineOffset(Vector2::ZERO);
    visual->SetDecorationUiScale(1.f);
    DALI_TEST_EQUALS(visual->GetEffectiveShadowOffset(), Vector2::ZERO, TEST_LOCATION);
    DALI_TEST_EQUALS(visual->GetEffectiveOutlineOffset(), Vector2::ZERO, TEST_LOCATION);
  }
  // Allocating style data for a color first must leave its default offset zero.
  visual = Text::VisualModel::New();
  visual->SetDecorationUiScale(2.f);
  visual->SetShadowColor(Color::RED);
  visual->SetOutlineColor(Color::BLUE);
  DALI_TEST_EQUALS(visual->GetEffectiveShadowOffset(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visual->GetEffectiveOutlineOffset(), Vector2::ZERO, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextDecorationScaleParametersP(void)
{
  Text::AsyncTextParameters defaults;
  DALI_TEST_EQUALS(defaults.decorationUiScale, 1.f, TEST_LOCATION);
  defaults.decorationUiScale = 1.5f;
  auto copied = defaults;
  DALI_TEST_EQUALS(copied.decorationUiScale, 1.5f, TEST_LOCATION);
  auto moved = std::move(copied);
  DALI_TEST_EQUALS(moved.decorationUiScale, 1.5f, TEST_LOCATION);
  Text::AsyncTextParameters reused;
  reused = moved;
  DALI_TEST_EQUALS(reused.decorationUiScale, 1.5f, TEST_LOCATION);
  reused = Text::AsyncTextParameters{};
  DALI_TEST_EQUALS(reused.decorationUiScale, 1.f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextDecorationScalePolicyP(void)
{
  UiTestApplication application;
  PolicyMatrix(application, Label::New());
  PolicyMatrix(application, InputField::New());
  PolicyMatrix(application, InputEditor::New());
  END_TEST;
}

int UtcDaliTextDecorationScalePolicyAsyncP(void)
{
  UiTestApplication application;
  ScaleGuard        guard;
  auto              parent = View::New();
  parent.SetRequestedWidth(1000.f);
  parent.SetRequestedHeight(1000.f);
  auto label = Label::New();
  Configure(label, 2, Text::Underline::Type::DASHED);
  const auto source = label.GetStyledText();
  label.SetAsyncRendering(true);
  label.AsyncRenderFinishedSignal().Connect(&OnDecorationRenderFinished);
  parent.Add(label);
  application.GetScene().Add(parent);
  UiScaleManager::Get().SetScale(2.f);
  auto awaitScale = [&](float scale)
  {
    const unsigned before   = asyncCompletions;
    const auto     deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    do
    {
      application.SendNotification();
      application.RunIdles();
      application.Render();
      if(asyncCompletions > before) break;
      Test::WaitForEventThreadTrigger(1, 1);
    } while(std::chrono::steady_clock::now() < deadline);
    DALI_TEST_CHECK(asyncCompletions > before);
    DALI_TEST_EQUALS(ControllerFor(label)->GetUiScale(), scale, TEST_LOCATION);
    DALI_TEST_EQUALS(Parameters(label).decorationUiScale, scale, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetStyledText() == source);
    DALI_TEST_EQUALS(label.GetTextShadow().GetOffset(), Vector2(-2.f, 3.f), TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetRenderScale(), 1.f, TEST_LOCATION);
  };
  awaitScale(2.f);
  label.SetUiScalePolicy(UiScalePolicy::DISABLED);
  awaitScale(1.f);
  label.SetUiScalePolicy(UiScalePolicy::ENABLED);
  awaitScale(2.f);
  label.SetUiScalePolicy(UiScalePolicy::INHERIT);
  parent.SetUiScalePolicy(UiScalePolicy::DISABLED);
  awaitScale(1.f);
  parent.SetUiScalePolicy(UiScalePolicy::ENABLED);
  awaitScale(2.f);
  // Supersede a pending request with the final disabled policy.
  UiScaleManager::Get().SetScale(1.25f);
  application.SendNotification();
  application.Render();
  label.SetUiScalePolicy(UiScalePolicy::DISABLED);
  awaitScale(1.f);
  END_TEST;
}

int UtcDaliTextDecorationScaleLabelAsyncRuntimeP(void)
{
  UiTestApplication application;
  ScaleGuard        guard;
  auto              label = Label::New();
  Configure(label, 2, Text::Underline::Type::DASHED);
  label.SetAsyncRendering(true);
  label.AsyncRenderFinishedSignal().Connect(&OnDecorationRenderFinished);
  application.GetScene().Add(label);
  for(float scale : SCALES)
  {
    const unsigned previous = asyncCompletions;
    UiScaleManager::Get().SetScale(scale);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    do
    {
      application.SendNotification();
      application.RunIdles();
      application.Render();
      if(asyncCompletions > previous) break;
      Test::WaitForEventThreadTrigger(1, 1);
    } while(std::chrono::steady_clock::now() < deadline);
    DALI_TEST_CHECK(asyncCompletions > previous);
    DALI_TEST_EQUALS(ControllerFor(label)->GetUiScale(), scale, EPSILON, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetRenderScale(), 1.f, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetTextShadow().GetOffset(), Vector2(-2.f, 3.f), TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetTextOutline().GetWidth(), 2.f, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    auto texture = label.GetRendererAt(0u).GetTextures().GetTexture(0u);
    DALI_TEST_CHECK(texture && texture.GetWidth() > 0u);
  }
  END_TEST;
}
