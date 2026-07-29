/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <array>
#include <cstddef>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int      WINDOW_WIDTH                                  = 920;
constexpr int      WINDOW_HEIGHT                                 = 880;
constexpr float    ROOT_PADDING                                  = 18.0f;
constexpr float    STACK_SPACING                                 = 10.0f;
constexpr float    HEADER_HEIGHT                                 = 132.0f;
constexpr float    FOOTER_HEIGHT                                 = 112.0f;
constexpr float    INPUT_FIELD_HEIGHT                            = 92.0f;
constexpr float    NORMAL_ANIMATION_DURATION_SECONDS             = 2.6f;
constexpr float    PLACEHOLDER_ANIMATION_DURATION_SECONDS        = 2.2f;
constexpr float    NORMAL_GRADIENT_START_OFFSET                  = -0.75f;
constexpr float    NORMAL_GRADIENT_END_OFFSET                    = 1.25f;
constexpr float    PLACEHOLDER_GRADIENT_START_OFFSET             = 0.85f;
constexpr float    PLACEHOLDER_GRADIENT_END_OFFSET               = -1.15f;

enum class GradientKind
{
  LINEAR,
  RADIAL,
  CONIC
};

enum class GradientPalette
{
  NORMAL,
  PLACEHOLDER
};

struct CaseDefinition
{
  const char* title;
  const char* expected;
  const char* fieldText;
  const char* editorText;
  const char* alternateFieldText;
  const char* alternateEditorText;
  const char* fieldPlaceholder;
  const char* editorPlaceholder;
  bool        password;
  bool        styledText;
  bool        normalTextGradient;
};

constexpr std::array<CaseDefinition, 4u> CASES{{
  {
    "Basic normal text",
    "Expected: InputField and InputEditor both show the same animated text gradient.",
    "InputField gradient sample",
    "InputEditor gradient sample\nsecond line shares the same gradient state",
    "Field text swapped while animation runs",
    "Editor text swapped while animation runs\nrenderer recreation should preserve animation",
    "Field placeholder gradient",
    "Editor placeholder gradient",
    false,
    false,
    true,
  },
  {
    "Placeholder only",
    "Expected: empty controls show placeholder gradients; swapped normal text stays plain black.",
    "",
    "",
    "Plain black field text",
    "Plain black editor text\nplaceholder hidden by normal text",
    "Animated field placeholder gradient",
    "Animated editor placeholder gradient",
    false,
    false,
    false,
  },
  {
    "Password field",
    "Expected: InputField password glyphs use the gradient; InputEditor remains normal multiline comparison.",
    "InputGradientPassword123",
    "Editor comparison text remains readable\nwhile the field uses password mode",
    "PasswordSwap987654321",
    "Editor comparison swapped\nnormal gradient still animates",
    "Password field placeholder",
    "Password case editor placeholder",
    true,
    false,
    true,
  },
  {
    "StyledText mixed",
    "Expected: default glyphs use gradient; explicit color/background spans and emoji keep authored rendering.",
    "Gradient <color value='#111827'>black</color> <background color='#FEF3C7'>BG</background> 😀",
    "Gradient default text with <color value='#DC2626'>red span</color> and <background color='#DCFCE7'>green background</background>\nEmoji 😀 🌈 keep color while gradient animates",
    "Swapped <color value='#2563EB'>blue</color> <background color='#E0E7FF'>BG span</background> 🚀",
    "Swapped StyledText mix\n<color value='#047857'>green span</color> <background color='#FDE68A'>warm background</background> 🙂",
    "StyledText field placeholder",
    "StyledText editor placeholder",
    false,
    true,
    true,
  },
}};

constexpr std::array<GradientKind, 3u> GRADIENT_KINDS{{
  GradientKind::LINEAR,
  GradientKind::RADIAL,
  GradientKind::CONIC,
}};

constexpr std::array<Gradient::SpreadMethod, 3u> SPREAD_METHODS{{
  Gradient::SpreadMethod::PAD,
  Gradient::SpreadMethod::REFLECT,
  Gradient::SpreadMethod::REPEAT,
}};

Label CreateLabel(const char* text, float fontSize, const UiColor& color)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(color);
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

void ConfigureBand(Label label, float height, const UiColor& background, const UiColor& border, const UiColor& color)
{
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetBackgroundColor(background);
  label.SetBorderlineWidth(1.0f);
  label.SetBorderlineOffset(-1.0f);
  label.SetBorderlineColor(border);
  label.SetCornerRadius(6.0f);
  label.SetTextColor(color);
  label.SetPadding(Extents(static_cast<int16_t>(10.0f), static_cast<int16_t>(10.0f), static_cast<int16_t>(0.0f), static_cast<int16_t>(0.0f)));
}

void StyleInputControl(View control, float height)
{
  control.SetRequestedWidth(MATCH_PARENT);
  control.SetRequestedHeight(height);
  control.SetPadding(Extents(static_cast<int16_t>(22.0f), static_cast<int16_t>(22.0f), static_cast<int16_t>(14.0f), static_cast<int16_t>(14.0f)));
  control.SetBackgroundColor(UiColor(0xF8FAFC));
  control.SetBorderlineWidth(1.0f);
  control.SetBorderlineOffset(-1.0f);
  control.SetBorderlineColor(UiColor(0xCBD5E1));
  control.SetCornerRadius(8.0f);

  if(auto field = InputField::DownCast(control))
  {
    field.SetCursorColor(UiColor(0x111827));
    field.SetCursorWidth(static_cast<int>(2.0f));
    field.SetMaximumLength(2000);
    field.SetPlaceholderColor(UiColor(0x64748B));
  }
  else if(auto editor = InputEditor::DownCast(control))
  {
    editor.SetCursorColor(UiColor(0x111827));
    editor.SetCursorWidth(static_cast<int>(2.0f));
    editor.SetMaximumLength(4000);
    editor.SetPlaceholderColor(UiColor(0x64748B));
  }
}

void SetNormalGradientStops(Gradient::Base& gradient)
{
  gradient.SetStopNodes({
    Gradient::StopNode(0.00f, UiColor(0xEC4899)), // Pink
    Gradient::StopNode(0.16f, UiColor(0xC084FC)), // Soft purple
    Gradient::StopNode(0.34f, UiColor(0x60A5FA)), // Light blue
    Gradient::StopNode(0.52f, UiColor(0x22D3EE)), // Cyan
    Gradient::StopNode(0.68f, UiColor(0x34D399)), // Mint green
    Gradient::StopNode(0.84f, UiColor(0xFACC15)), // Warm yellow
    Gradient::StopNode(1.00f, UiColor(0xFB923C)), // Soft orange
  });
}

void SetPlaceholderGradientStops(Gradient::Base& gradient)
{
  gradient.SetStopNodes({
    Gradient::StopNode(0.00f, UiColor(0x8B5CF6)), // Violet
    Gradient::StopNode(0.20f, UiColor(0xA78BFA)), // Lavender
    Gradient::StopNode(0.40f, UiColor(0x818CF8)), // Periwinkle
    Gradient::StopNode(0.60f, UiColor(0x38BDF8)), // Sky blue
    Gradient::StopNode(0.78f, UiColor(0x67E8F9)), // Pale cyan
    Gradient::StopNode(1.00f, UiColor(0xFDE047)), // Yellow
  });
}

void SetGradientStops(Gradient::Base& gradient, GradientPalette palette)
{
  if(palette == GradientPalette::PLACEHOLDER)
  {
    SetPlaceholderGradientStops(gradient);
  }
  else
  {
    SetNormalGradientStops(gradient);
  }
}

Gradient::Linear MakeLinear(float startOffset, Gradient::SpreadMethod spreadMethod, GradientPalette palette)
{
  Gradient::Linear gradient(Vector2(-0.18f, 0.0f), Vector2(0.18f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  gradient.SetStartOffset(startOffset);
  SetGradientStops(gradient, palette);
  return gradient;
}

Gradient::Radial MakeRadial(float startOffset, Gradient::SpreadMethod spreadMethod, GradientPalette palette)
{
  Gradient::Radial gradient(Vector2::ZERO, 0.26f);
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  gradient.SetStartOffset(startOffset);
  SetGradientStops(gradient, palette);
  return gradient;
}

Gradient::Conic MakeConic(float startOffset, Gradient::SpreadMethod spreadMethod, GradientPalette palette)
{
  Gradient::Conic gradient(Vector2::ZERO, Radian(0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(spreadMethod);
  gradient.SetStartOffset(startOffset);
  SetGradientStops(gradient, palette);
  return gradient;
}

Gradient::Base MakeGradient(GradientKind kind, float startOffset, Gradient::SpreadMethod spreadMethod, GradientPalette palette)
{
  switch(kind)
  {
    case GradientKind::RADIAL:
    {
      return MakeRadial(startOffset, spreadMethod, palette);
    }
    case GradientKind::CONIC:
    {
      return MakeConic(startOffset, spreadMethod, palette);
    }
    case GradientKind::LINEAR:
    default:
    {
      return MakeLinear(startOffset, spreadMethod, palette);
    }
  }
}

const char* GetGradientName(GradientKind kind)
{
  switch(kind)
  {
    case GradientKind::RADIAL:
    {
      return "Radial";
    }
    case GradientKind::CONIC:
    {
      return "Conic";
    }
    case GradientKind::LINEAR:
    default:
    {
      return "Linear";
    }
  }
}

const char* GetSpreadMethodName(Gradient::SpreadMethod spreadMethod)
{
  switch(spreadMethod)
  {
    case Gradient::SpreadMethod::REFLECT:
    {
      return "REFLECT";
    }
    case Gradient::SpreadMethod::REPEAT:
    {
      return "REPEAT";
    }
    case Gradient::SpreadMethod::PAD:
    default:
    {
      return "PAD";
    }
  }
}

} // namespace

class InputGradientController : public ConnectionTracker
{
public:
  explicit InputGradientController(Application& application)
  : mApplication(application)
  {
    application.InitSignal().Connect(this, &InputGradientController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0x0B1120));

    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetPadding(Extents(static_cast<int16_t>(ROOT_PADDING), static_cast<int16_t>(ROOT_PADDING), static_cast<int16_t>(ROOT_PADDING), static_cast<int16_t>(ROOT_PADDING)));
    mRoot.SetSpacing(STACK_SPACING);
    mRoot.SetBackgroundColor(UiColor(0x0B1120));

    mHeader = StackLayout::New(StackOrientation::VERTICAL);
    mHeader.SetRequestedWidth(MATCH_PARENT);
    mHeader.SetRequestedHeight(HEADER_HEIGHT);
    mHeader.SetSpacing(6.0f);

    mCaseLabel = CreateLabel("", 18.0f, UiColor(0xF8FAFC));
    ConfigureBand(mCaseLabel, 34.0f, UiColor(0x1D4ED8), UiColor(0x93C5FD), UiColor(0xF8FAFC));
    mStateLabel = CreateLabel("", 13.0f, UiColor(0xD1FAE5));
    ConfigureBand(mStateLabel, 30.0f, UiColor(0x064E3B), UiColor(0x34D399), UiColor(0xD1FAE5));
    mExpectedLabel = CreateLabel("", 13.0f, UiColor(0xCBD5E1));
    ConfigureBand(mExpectedLabel, 46.0f, UiColor(0x111827), UiColor(0x334155), UiColor(0xCBD5E1));

    mHeader.Add(mCaseLabel);
    mHeader.Add(mStateLabel);
    mHeader.Add(mExpectedLabel);

    mContent = StackLayout::New(StackOrientation::VERTICAL);
    mContent.SetRequestedWidth(MATCH_PARENT);
    mContent.SetRequestedHeight(MATCH_PARENT);
    mContent.SetSpacing(STACK_SPACING);
    mContent.SetPadding(Extents(static_cast<int16_t>(16.0f), static_cast<int16_t>(16.0f), static_cast<int16_t>(16.0f), static_cast<int16_t>(16.0f)));
    mContent.SetBackgroundColor(UiColor(0xFFFFFF));
    mContent.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    mField = InputField::New();
    StyleInputControl(mField, INPUT_FIELD_HEIGHT);
    mField.SetFontSize(31.0f);
    mField.SetTextColor(UiColor(0x111827));
    mField.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mContent.Add(mField);

    mEditor = InputEditor::New();
    StyleInputControl(mEditor, MATCH_PARENT);
    mEditor.SetFontSize(28.0f);
    mEditor.SetTextColor(UiColor(0x111827));
    mEditor.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mContent.Add(mEditor);

    mFooter = StackLayout::New(StackOrientation::VERTICAL);
    mFooter.SetRequestedWidth(MATCH_PARENT);
    mFooter.SetRequestedHeight(FOOTER_HEIGHT);
    mFooter.SetSpacing(6.0f);

    mCaseHelpLabel = CreateLabel("CASES  Left/Right or 1-4  Basic | Placeholder | Password | StyledText", 12.0f, UiColor(0xCBD5E1));
    ConfigureBand(mCaseHelpLabel, 30.0f, UiColor(0x111827), UiColor(0x334155), UiColor(0xCBD5E1));
    mActionHelpLabel = CreateLabel("ACTIONS  G Type | S Spread | B Bounds | A/P Anim | C Base | O Placeholder", 12.0f, UiColor(0xCBD5E1));
    ConfigureBand(mActionHelpLabel, 30.0f, UiColor(0x111827), UiColor(0x334155), UiColor(0xCBD5E1));
    mDiagHelpLabel = CreateLabel("VIEW  Left/Right Case | T Text swap/recreate | X Reset options | ESC Quit", 12.0f, UiColor(0xCBD5E1));
    ConfigureBand(mDiagHelpLabel, 30.0f, UiColor(0x111827), UiColor(0x334155), UiColor(0xCBD5E1));

    mFooter.Add(mCaseHelpLabel);
    mFooter.Add(mActionHelpLabel);
    mFooter.Add(mDiagHelpLabel);

    mRoot.Add(mHeader);
    mRoot.Add(mContent);
    mRoot.Add(mFooter);

    window.Add(mRoot);
    window.KeyEventSignal().Connect(this, &InputGradientController::OnKeyEvent);

    ShowCase(0u);
  }

  void ShowCase(std::size_t index)
  {
    StopNormalAnimation();
    StopPlaceholderAnimation();
    mCaseIndex       = index % CASES.size();
    mTextSwapEnabled = false;
    ApplyCurrentCase();
  }

  const CaseDefinition& CurrentCase() const
  {
    return CASES[mCaseIndex];
  }

  Text::GradientBoundsMode CurrentBoundsMode() const
  {
    return mViewBounds ? Text::GradientBoundsMode::VIEW_BOUND : Text::GradientBoundsMode::CONTENT_BOUND;
  }

  Gradient::SpreadMethod CurrentSpreadMethod() const
  {
    return SPREAD_METHODS[mSpreadMethodIndex % SPREAD_METHODS.size()];
  }

  bool ShouldApplyNormalTextGradient() const
  {
    return mGradientApplied && CurrentCase().normalTextGradient;
  }

  void ApplyCurrentCase()
  {
    StopNormalAnimation();
    StopPlaceholderAnimation();

    const CaseDefinition& item = CurrentCase();
    ResetControlsForCase(item);
    ApplyCaseText(item);
    ApplyGradients();

    if(mNormalAnimationEnabled)
    {
      StartNormalAnimation();
    }
    if(mPlaceholderAnimationEnabled)
    {
      StartPlaceholderAnimation();
    }

    UpdateStatus();
  }

  void ResetControlsForCase(const CaseDefinition& item)
  {
    mField.SetPasswordMode(item.password ? Text::PasswordMode::HIDE_ALL : Text::PasswordMode::NONE);
    mField.SetTextUnderline(Text::Underline::None());
    mField.SetTextShadow(Text::Shadow::None());
    mField.SetTextGradient(Gradient::Base::None());
    mField.SetPlaceholderTextGradient(Gradient::Base::None());
    mField.SetTextColor(UiColor(0x111827));
    mField.SetFontSize(item.password ? 30.0f : 31.0f);
    mField.SetPlaceholder(item.fieldPlaceholder);

    mEditor.SetTextUnderline(Text::Underline::None());
    mEditor.SetTextShadow(Text::Shadow::None());
    mEditor.SetTextGradient(Gradient::Base::None());
    mEditor.SetPlaceholderTextGradient(Gradient::Base::None());
    mEditor.SetTextColor(UiColor(0x111827));
    mEditor.SetFontSize(28.0f);
    mEditor.SetPlaceholder(item.editorPlaceholder);
  }

  void ApplyCaseText(const CaseDefinition& item)
  {
    const char* fieldText  = mTextSwapEnabled ? item.alternateFieldText : item.fieldText;
    const char* editorText = mTextSwapEnabled ? item.alternateEditorText : item.editorText;

    if(item.styledText)
    {
      mField.SetStyledText(Text::StyledText::FromMarkup(fieldText));
      mEditor.SetStyledText(Text::StyledText::FromMarkup(editorText));
    }
    else
    {
      mField.SetText(fieldText);
      mEditor.SetText(editorText);
    }
  }

  void ApplyGradients()
  {
    mField.SetTextGradientBoundsMode(CurrentBoundsMode());
    mEditor.SetTextGradientBoundsMode(CurrentBoundsMode());
    ApplyNormalGradient();
    ApplyPlaceholderGradient();
  }

  void ApplyNormalGradient()
  {
    if(ShouldApplyNormalTextGradient())
    {
      ApplyNormalAnimationBaseGradient();
    }
    else
    {
      mField.SetTextGradient(Gradient::Base::None());
      mEditor.SetTextGradient(Gradient::Base::None());
    }
  }

  void ApplyPlaceholderGradient()
  {
    if(mPlaceholderGradientApplied)
    {
      ApplyPlaceholderAnimationBaseGradient();
    }
    else
    {
      mField.SetPlaceholderTextGradient(Gradient::Base::None());
      mEditor.SetPlaceholderTextGradient(Gradient::Base::None());
    }
  }

  void ApplyNormalAnimationBaseGradient()
  {
    const Gradient::Base gradient = MakeGradient(mGradientKind, NORMAL_GRADIENT_START_OFFSET, CurrentSpreadMethod(), GradientPalette::NORMAL);
    mField.SetTextGradient(gradient);
    mEditor.SetTextGradient(gradient);
  }

  void ApplyPlaceholderAnimationBaseGradient()
  {
    const Gradient::Base placeholderGradient = MakeGradient(mGradientKind, PLACEHOLDER_GRADIENT_START_OFFSET, CurrentSpreadMethod(), GradientPalette::PLACEHOLDER);
    mField.SetPlaceholderTextGradient(placeholderGradient);
    mEditor.SetPlaceholderTextGradient(placeholderGradient);
  }

  void StartNormalAnimation()
  {
    if(!ShouldApplyNormalTextGradient())
    {
      StopNormalAnimation();
      return;
    }

    StopNormalAnimation();
    ApplyNormalAnimationBaseGradient();

    mNormalAnimation = Animation::New(NORMAL_ANIMATION_DURATION_SECONDS);
    mField.Animate(mNormalAnimation)
      .TextGradientStartOffset(NORMAL_GRADIENT_END_OFFSET, Duration(NORMAL_ANIMATION_DURATION_SECONDS), AlphaFunction::LINEAR);
    mEditor.Animate(mNormalAnimation)
      .TextGradientStartOffset(NORMAL_GRADIENT_END_OFFSET, Duration(NORMAL_ANIMATION_DURATION_SECONDS), AlphaFunction::LINEAR);
    mNormalAnimation.SetLooping(true);
    mNormalAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mNormalAnimation.Play();
  }

  void StopNormalAnimation()
  {
    if(mNormalAnimation)
    {
      mNormalAnimation.Stop();
      mNormalAnimation.Reset();
    }
  }

  void StartPlaceholderAnimation()
  {
    if(!mPlaceholderGradientApplied)
    {
      StopPlaceholderAnimation();
      return;
    }

    StopPlaceholderAnimation();
    ApplyPlaceholderAnimationBaseGradient();

    mPlaceholderAnimation = Animation::New(PLACEHOLDER_ANIMATION_DURATION_SECONDS);
    mField.Animate(mPlaceholderAnimation)
      .PlaceholderTextGradientStartOffset(PLACEHOLDER_GRADIENT_END_OFFSET, Duration(PLACEHOLDER_ANIMATION_DURATION_SECONDS), AlphaFunction::LINEAR);
    mEditor.Animate(mPlaceholderAnimation)
      .PlaceholderTextGradientStartOffset(PLACEHOLDER_GRADIENT_END_OFFSET, Duration(PLACEHOLDER_ANIMATION_DURATION_SECONDS), AlphaFunction::LINEAR);
    mPlaceholderAnimation.SetLooping(true);
    mPlaceholderAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mPlaceholderAnimation.Play();
  }

  void StopPlaceholderAnimation()
  {
    if(mPlaceholderAnimation)
    {
      mPlaceholderAnimation.Stop();
      mPlaceholderAnimation.Reset();
    }
  }

  void RestartAnimationsForOptionChange()
  {
    const bool normalEnabled      = mNormalAnimationEnabled;
    const bool placeholderEnabled = mPlaceholderAnimationEnabled;
    StopNormalAnimation();
    StopPlaceholderAnimation();
    ApplyGradients();
    if(normalEnabled)
    {
      StartNormalAnimation();
    }
    if(placeholderEnabled)
    {
      StartPlaceholderAnimation();
    }
    UpdateStatus();
  }

  void ToggleNormalAnimation()
  {
    mNormalAnimationEnabled = !mNormalAnimationEnabled;
    if(mNormalAnimationEnabled)
    {
      StartNormalAnimation();
    }
    else
    {
      StopNormalAnimation();
    }
    UpdateStatus();
  }

  void TogglePlaceholderAnimation()
  {
    mPlaceholderAnimationEnabled = !mPlaceholderAnimationEnabled;
    if(mPlaceholderAnimationEnabled)
    {
      StartPlaceholderAnimation();
    }
    else
    {
      StopPlaceholderAnimation();
    }
    UpdateStatus();
  }

  void ToggleTextSwap()
  {
    mTextSwapEnabled = !mTextSwapEnabled;
    ApplyCurrentCase();
  }

  void UpdateStatus()
  {
    const CaseDefinition& item = CurrentCase();

    std::string caseText = "Case " + std::to_string(mCaseIndex + 1u) + "/" + std::to_string(CASES.size()) + "  ";
    caseText += item.title;
    mCaseLabel.SetText(Dali::String(caseText.c_str()));

    std::string state = "G ";
    state += GetGradientName(mGradientKind);
    state += " | S ";
    state += GetSpreadMethodName(CurrentSpreadMethod());
    state += mViewBounds ? " | B VIEW" : " | B CONTENT";
    if(CurrentCase().normalTextGradient)
    {
      state += mGradientApplied ? " | C Base ON" : " | C Base OFF";
    }
    else
    {
      state += " | C Base BLACK";
    }
    state += mPlaceholderGradientApplied ? " | O Placeholder ON" : " | O Placeholder OFF";
    state += mNormalAnimationEnabled ? " | A ON" : " | A OFF";
    state += mPlaceholderAnimationEnabled ? "/P ON" : "/P OFF";
    state += mTextSwapEnabled ? " | T Swap" : " | T Primary";
    mStateLabel.SetText(Dali::String(state.c_str()));

    mExpectedLabel.SetText(Dali::String(item.expected));
  }

  void CycleGradientKind()
  {
    for(std::size_t index = 0u; index < GRADIENT_KINDS.size(); ++index)
    {
      if(GRADIENT_KINDS[index] == mGradientKind)
      {
        mGradientKind = GRADIENT_KINDS[(index + 1u) % GRADIENT_KINDS.size()];
        RestartAnimationsForOptionChange();
        return;
      }
    }
    mGradientKind = GradientKind::LINEAR;
    RestartAnimationsForOptionChange();
  }

  void CycleSpreadMethod()
  {
    mSpreadMethodIndex = (mSpreadMethodIndex + 1u) % SPREAD_METHODS.size();
    RestartAnimationsForOptionChange();
  }

  void ToggleBoundsMode()
  {
    mViewBounds = !mViewBounds;
    RestartAnimationsForOptionChange();
  }

  void ToggleBaseGradient()
  {
    mGradientApplied = !mGradientApplied;
    RestartAnimationsForOptionChange();
  }

  void TogglePlaceholderGradient()
  {
    mPlaceholderGradientApplied = !mPlaceholderGradientApplied;
    RestartAnimationsForOptionChange();
  }

  void ResetCurrentOptions()
  {
    mTextSwapEnabled              = false;
    mGradientApplied              = true;
    mPlaceholderGradientApplied   = true;
    mNormalAnimationEnabled       = true;
    mPlaceholderAnimationEnabled  = true;
    mSpreadMethodIndex            = 0u;
    mViewBounds                   = false;
    ApplyCurrentCase();
  }

  bool IsNumberKey(const KeyEvent& event, std::size_t& index) const
  {
    const std::string keyName = event.GetKeyName().CStr();
    if(keyName.size() == 1u && keyName[0] >= '1' && keyName[0] <= '4')
    {
      index = static_cast<std::size_t>(keyName[0] - '1');
      return true;
    }
    return false;
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    std::size_t numberIndex = 0u;
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(IsNumberKey(event, numberIndex))
    {
      ShowCase(numberIndex);
    }
    else if(event.GetKeyName() == "Left")
    {
      ShowCase((mCaseIndex + CASES.size() - 1u) % CASES.size());
    }
    else if(event.GetKeyName() == "Right")
    {
      ShowCase((mCaseIndex + 1u) % CASES.size());
    }
    else if(event.GetKeyName() == "g" || event.GetKeyName() == "G")
    {
      CycleGradientKind();
    }
    else if(event.GetKeyName() == "s" || event.GetKeyName() == "S")
    {
      CycleSpreadMethod();
    }
    else if(event.GetKeyName() == "b" || event.GetKeyName() == "B")
    {
      ToggleBoundsMode();
    }
    else if(event.GetKeyName() == "a" || event.GetKeyName() == "A")
    {
      ToggleNormalAnimation();
    }
    else if(event.GetKeyName() == "p" || event.GetKeyName() == "P")
    {
      TogglePlaceholderAnimation();
    }
    else if(event.GetKeyName() == "c" || event.GetKeyName() == "C")
    {
      ToggleBaseGradient();
    }
    else if(event.GetKeyName() == "o" || event.GetKeyName() == "O")
    {
      TogglePlaceholderGradient();
    }
    else if(event.GetKeyName() == "t" || event.GetKeyName() == "T")
    {
      ToggleTextSwap();
    }
    else if(event.GetKeyName() == "x" || event.GetKeyName() == "X" || event.GetKeyName() == "r" || event.GetKeyName() == "R")
    {
      ResetCurrentOptions();
    }
  }

private:
  Application mApplication;
  StackLayout mRoot;
  StackLayout mHeader;
  StackLayout mContent;
  StackLayout mFooter;
  Label       mCaseLabel;
  Label       mStateLabel;
  Label       mExpectedLabel;
  Label       mCaseHelpLabel;
  Label       mActionHelpLabel;
  Label       mDiagHelpLabel;
  InputField  mField;
  InputEditor mEditor;
  Animation   mNormalAnimation;
  Animation   mPlaceholderAnimation;

  std::size_t mCaseIndex{0u};
  std::size_t mSpreadMethodIndex{0u};
  GradientKind mGradientKind{GradientKind::LINEAR};
  bool         mViewBounds{false};
  bool         mGradientApplied{true};
  bool         mPlaceholderGradientApplied{true};
  bool         mNormalAnimationEnabled{true};
  bool         mPlaceholderAnimationEnabled{true};
  bool         mTextSwapEnabled{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  InputGradientController controller(application);
  application.MainLoop();
  return 0;
}
