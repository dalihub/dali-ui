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

/**
 * UiScale system test sample
 *
 * Verifies all UiScalePolicy requirements in one scrollable screen:
 *
 *  [Control Panel]
 *    - Current scale display
 *    - Preset buttons: x0.8 / x1.0 / x1.2 / x1.5 / x2.0
 *    - Custom scale InputField + Apply button
 *
 *  [Zone A] INHERIT (default)
 *    - Three colored 60×60 boxes, all INHERIT
 *    - All grow/shrink proportionally with system scale
 *    - Nested 2×2 GridLayout also scales
 *
 *  [Zone B] DISABLED container + ENABLED children  ← KEY TEST
 *    - The outer StackLayout is DISABLED (effectiveScale = 1.0 always)
 *    - Red box:   INHERIT  → inherits DISABLED parent → does NOT scale
 *    - Green box: ENABLED  → bypasses DISABLED parent → DOES scale
 *    - Blue box:  ENABLED  → same as green
 *    Observe: Red stays 60×60. Green/Blue grow as scale increases.
 *
 *  [Zone C] DISABLED subtree inside normal INHERIT parent
 *    - Orange box: INHERIT inside INHERIT zone → scales
 *    - Purple sub-container: DISABLED → isolates its children
 *      - Purple child: INHERIT under DISABLED → does NOT scale
 *      - Orange-red child: ENABLED → scales despite DISABLED parent
 *
 * Press Escape / Back to quit.
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

// ─────────────────────────── Color palette ───────────────────────────────────
namespace
{
constexpr uint32_t C_CTRL_BG    = 0x1A1A2E; // Control panel dark bg
constexpr uint32_t C_BTN        = 0x0F3460; // Preset button bg
constexpr uint32_t C_BTN_APPLY  = 0x00695C; // Apply button (teal)
constexpr uint32_t C_WHITE      = 0xFFFFFF;
constexpr uint32_t C_LIGHT_TEXT = 0xF5F5F5;
constexpr uint32_t C_YELLOW_HI  = 0xFFEB3B; // Scale display highlight
constexpr uint32_t C_DARK_TEXT  = 0x212121;
constexpr uint32_t C_GRAY_TEXT  = 0x757575;
constexpr uint32_t C_INPUT_BG   = 0x2D2D4E;

// Zone backgrounds
constexpr uint32_t C_ZONE_A_BG  = 0xE3F2FD; // light blue
constexpr uint32_t C_ZONE_B_BG  = 0xFFF9C4; // light yellow
constexpr uint32_t C_ZONE_C_BG  = 0xE8F5E9; // light green
constexpr uint32_t C_ZONE_TITLE = 0x37474F;

// Box colors
constexpr uint32_t C_RED        = 0xE53935;
constexpr uint32_t C_GREEN      = 0x43A047;
constexpr uint32_t C_BLUE       = 0x1E88E5;
constexpr uint32_t C_ORANGE     = 0xFB8C00;
constexpr uint32_t C_PURPLE     = 0x8E24AA;
constexpr uint32_t C_DEEP_ORG   = 0xE64A19; // "ENABLED in DISABLED subtree"

// Grid cell colors
constexpr uint32_t C_GRID[4]    = {0xEF9A9A, 0x90CAF9, 0xA5D6A7, 0xFFCC80};

// Misc
constexpr float    BOX_SIZE     = 60.0f;  // natural size of each demo box
constexpr float    ZONE_RADIUS  = 10.0f;
constexpr float    BOX_RADIUS   = 6.0f;
constexpr float    BTN_RADIUS   = 8.0f;
constexpr float    BTN_HEIGHT   = 44.0f;
} // namespace

// ──────────────────── float → "1.23" helper ──────────────────────────────────
static std::string Fmt(float v)
{
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << v;
  return oss.str();
}

// ─────────────────────────── Controller ──────────────────────────────────────
class UiScaleController : public ConnectionTracker
{
public:
  explicit UiScaleController(Application& app)
  : mApplication(app)
  {
    app.InitSignal().Connect(this, &UiScaleController::OnInit);
  }

private:
  // ─────────── App lifecycle ───────────────────────────────────────────────

  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    // Root vertical stack — INHERIT (default), fills window
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(0.0f);

    root.Add(BuildControlPanel());
    root.Add(BuildScrollZones());

    window.Add(root);
    window.KeyEventSignal().Connect(this, &UiScaleController::OnKeyEvent);

    UpdateScaleLabel();
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  // ─────────── Scale control ───────────────────────────────────────────────

  void ApplyScale(float s)
  {
    UiScaleManager::Get().SetScale(s);
    UpdateScaleLabel();
  }

  void OnApplyCustomScale()
  {
    Dali::String text = mScaleInput.GetText();
    if(text.Size() == 0)
      return;
    float s = 1.0f;
    try
    {
      s = std::stof(std::string(text.CStr()));
    }
    catch(...)
    {
      return; // ignore invalid input
    }
    if(s > 0.1f && s < 5.0f)
    {
      ApplyScale(s);
    }
  }

  void UpdateScaleLabel()
  {
    std::string text = "System Scale: " + Fmt(UiScaleManager::Get().GetScale());
    mScaleLabel.SetText(text.c_str());
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Build: Control Panel
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildControlPanel()
  {
    StackLayout panel = StackLayout::New(StackOrientation::VERTICAL);
    panel.SetRequestedWidth(MATCH_PARENT);
    panel.SetRequestedHeight(WRAP_CONTENT);
    panel.SetBackgroundColor(UiColor(C_CTRL_BG));
    panel.SetPadding(Extents(16, 16, 16, 16));
    panel.SetSpacing(10.0f);

    // Title
    panel.Add(Label::New("UiScale System Test")
                .SetRequestedWidth(MATCH_PARENT)
                .SetRequestedHeight(WRAP_CONTENT)
                .SetFontSize(22.0f)
                .SetTextColor(UiColor(C_LIGHT_TEXT)));

    // Current scale display (updated dynamically)
    mScaleLabel = Label::New("System Scale: 1.00");
    mScaleLabel.SetRequestedWidth(MATCH_PARENT);
    mScaleLabel.SetRequestedHeight(WRAP_CONTENT);
    mScaleLabel.SetFontSize(16.0f);
    mScaleLabel.SetTextColor(UiColor(C_YELLOW_HI));
    panel.Add(mScaleLabel);

    // Preset buttons row
    StackLayout btnRow = StackLayout::New(StackOrientation::HORIZONTAL);
    btnRow.SetRequestedWidth(MATCH_PARENT);
    btnRow.SetRequestedHeight(WRAP_CONTENT);
    btnRow.SetSpacing(6.0f);

    static constexpr float kPresets[] = {0.8f, 1.0f, 1.2f, 1.5f, 2.0f};
    for(float preset : kPresets)
    {
      std::string label = "x" + Fmt(preset);
      InteractiveView btn = MakeButton(label, UiColor(C_BTN));
      btn.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

      const float capturedScale = preset;
      btn.ConnectClickedSignal(this, [this, capturedScale](View, const InputEvent&) {
        ApplyScale(capturedScale);
      });
      btnRow.Add(btn);
    }
    panel.Add(btnRow);

    // Custom scale: InputField + Apply button
    StackLayout inputRow = StackLayout::New(StackOrientation::HORIZONTAL);
    inputRow.SetRequestedWidth(MATCH_PARENT);
    inputRow.SetRequestedHeight(WRAP_CONTENT);
    inputRow.SetSpacing(6.0f);

    mScaleInput = InputField::New();
    mScaleInput.SetPlaceholder("Custom scale (e.g. 1.3)");
    mScaleInput.SetFontSize(15.0f);
    mScaleInput.SetCursorWidth(2);
    mScaleInput.SetRequestedHeight(BTN_HEIGHT);
    mScaleInput.SetBackgroundColor(UiColor(C_INPUT_BG));
    mScaleInput.SetTextColor(UiColor(C_LIGHT_TEXT));
    mScaleInput.SetCursorColor(UiColor(C_LIGHT_TEXT));
    mScaleInput.SetPlaceholderColor(UiColor(C_GRAY_TEXT));
    mScaleInput.SetVerticalTextAlignment(Text::Alignment::CENTER);
    mScaleInput.SetPadding(Extents(10, 10, 4, 4));
    mScaleInput.SetCornerRadius(Vector4(BTN_RADIUS, BTN_RADIUS, BTN_RADIUS, BTN_RADIUS));
    mScaleInput.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    InteractiveView applyBtn = MakeButton("Apply", UiColor(C_BTN_APPLY));
    applyBtn.SetRequestedWidth(70.0f);
    applyBtn.ConnectClickedSignal(this, [this](View, const InputEvent&) {
      OnApplyCustomScale();
    });

    inputRow.Add(mScaleInput);
    inputRow.Add(applyBtn);
    panel.Add(inputRow);

    return panel;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Build: Scrollable test zones
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildScrollZones()
  {
    // Vertical content stack for all three zones
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(WRAP_CONTENT);
    content.SetSpacing(16.0f);
    content.SetPadding(Extents(14, 14, 16, 24));

    content.Add(BuildZoneA());
    content.Add(BuildZoneB());
    content.Add(BuildZoneC());

    // ScrollView fills remaining height in the root StackLayout
    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetRequestedHeight(WRAP_CONTENT);
    scrollView.SetContent(content);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    return scrollView;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone A: All INHERIT — every view scales with the system scale
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneA()
  {
    StackLayout zone = BuildZoneContainer(UiColor(C_ZONE_A_BG));
    // No SetUiScalePolicy call — INHERIT is the default

    zone.Add(MakeZoneTitle("[Zone A] INHERIT (default)"));
    zone.Add(MakeDesc(
      "All views use the default INHERIT policy. "
      "The three boxes and the nested grid all scale proportionally "
      "as you change the system scale."));

    // Three boxes — all INHERIT
    StackLayout row = MakeHStack();
    row.Add(MakePolicyBox("INHERIT", UiColor(C_RED),   UiScalePolicy::INHERIT, BOX_SIZE));
    row.Add(MakePolicyBox("INHERIT", UiColor(C_GREEN), UiScalePolicy::INHERIT, BOX_SIZE));
    row.Add(MakePolicyBox("INHERIT", UiColor(C_BLUE),  UiScalePolicy::INHERIT, BOX_SIZE));
    zone.Add(row);

    // Nested 2×2 GridLayout (also INHERIT)
    zone.Add(MakeDesc("Nested 2×2 GridLayout — also INHERIT:"));
    zone.Add(BuildDemoGrid());

    return zone;
  }

  View BuildDemoGrid()
  {
    GridLayout grid = GridLayout::New();
    grid.AddColumnDefinition(GridLength::Star(1.0f));
    grid.AddColumnDefinition(GridLength::Star(1.0f));
    grid.AddRowDefinition(GridLength::Absolute(50.0f));
    grid.AddRowDefinition(GridLength::Absolute(50.0f));
    grid.SetRequestedWidth(MATCH_PARENT);
    grid.SetRequestedHeight(WRAP_CONTENT);
    grid.SetRowSpacing(4.0f);
    grid.SetColumnSpacing(4.0f);
    // INHERIT (default) — inherits system scale

    for(int i = 0; i < 4; ++i)
    {
      View cell = View::New();
      cell.SetBackgroundColor(UiColor(C_GRID[i]));
      cell.SetLayoutParams(GridLayoutParams::New());
      grid.Add(cell);
    }
    return grid;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone B: DISABLED container + ENABLED children  ← KEY TEST CASE
  //
  // This demonstrates the requirement: "자신은 scale을 적용받지 않되
  // child는 scale을 적용받는 뷰"
  //
  // The zone container (StackLayout) is DISABLED → effectiveScale = 1.0.
  // Children with INHERIT also don't scale (they inherit the 1.0 from parent).
  // Children with ENABLED bypass the DISABLED parent and use the system scale
  // directly.
  //
  // Visual proof: increase scale to 1.5 or 2.0 and observe:
  //   - Red (INHERIT)  stays 60×60
  //   - Green (ENABLED) grows to 90×90 / 120×120
  //   - Blue  (ENABLED) grows to 90×90 / 120×120
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneB()
  {
    StackLayout zone = BuildZoneContainer(UiColor(C_ZONE_B_BG));

    // ★★★ Key: DISABLED — this container and its INHERIT descendants don't scale
    zone.SetUiScalePolicy(UiScalePolicy::DISABLED);

    zone.Add(MakeZoneTitle("[Zone B] DISABLED container + ENABLED children"));
    zone.Add(MakeDesc(
      "This container is DISABLED (effectiveScale = 1.0 always). "
      "Its INHERIT children also don't scale. "
      "But children with ENABLED policy bypass DISABLED and use the system scale directly."));

    // The three comparison boxes
    StackLayout row = MakeHStack();

    // Box 1: INHERIT → inherits DISABLED parent → effectiveScale = 1.0 → no scale
    row.Add(MakePolicyBox("INHERIT\n(no scale)", UiColor(C_RED), UiScalePolicy::INHERIT, BOX_SIZE));

    // Box 2: ENABLED → directly reads UiScaleManager → effectiveScale = system scale
    row.Add(MakePolicyBox("ENABLED\n(scales!)", UiColor(C_GREEN), UiScalePolicy::ENABLED, BOX_SIZE));

    // Box 3: ENABLED → same as Box 2
    row.Add(MakePolicyBox("ENABLED\n(scales!)", UiColor(C_BLUE), UiScalePolicy::ENABLED, BOX_SIZE));

    zone.Add(row);
    zone.Add(MakeDesc(
      "Red stays 60×60. Green/Blue grow when scale > 1.0. "
      "The container (yellow background) expands to accommodate them but its own "
      "padding/spacing remain at 1.0 scale."));

    return zone;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Zone C: DISABLED subtree nested inside a normal INHERIT parent
  //
  // Shows how a DISABLED sub-container creates a "scale-free island" inside
  // an otherwise scaled layout, and how ENABLED can break out of it.
  // ═══════════════════════════════════════════════════════════════════════════

  View BuildZoneC()
  {
    StackLayout zone = BuildZoneContainer(UiColor(C_ZONE_C_BG));
    // INHERIT (default)

    zone.Add(MakeZoneTitle("[Zone C] DISABLED sub-container inside INHERIT zone"));
    zone.Add(MakeDesc(
      "The outer zone is INHERIT (scales normally). "
      "The purple sub-container is DISABLED, creating a scale-free island. "
      "Inside it: one INHERIT child (fixed) and one ENABLED child (still scales)."));

    // Outer row: INHERIT zone content
    StackLayout outerRow = MakeHStack();

    // Left: normal INHERIT box — scales with the zone
    outerRow.Add(MakePolicyBox("INHERIT\n(scales)", UiColor(C_ORANGE), UiScalePolicy::INHERIT, BOX_SIZE));

    // Right: DISABLED sub-container (purple tint) — creates isolated island
    StackLayout disabledSub = StackLayout::New(StackOrientation::VERTICAL);
    disabledSub.SetRequestedWidth(WRAP_CONTENT);
    disabledSub.SetRequestedHeight(WRAP_CONTENT);
    disabledSub.SetBackgroundColor(UiColor(0xE1BEE7)); // light purple
    disabledSub.SetPadding(Extents(8, 8, 8, 8));
    disabledSub.SetSpacing(6.0f);
    disabledSub.SetCornerRadius(Vector4(ZONE_RADIUS, ZONE_RADIUS, ZONE_RADIUS, ZONE_RADIUS));
    disabledSub.SetUiScalePolicy(UiScalePolicy::DISABLED); // ★ isolated island

    disabledSub.Add(Label::New("DISABLED sub-container")
                      .SetRequestedWidth(WRAP_CONTENT)
                      .SetRequestedHeight(WRAP_CONTENT)
                      .SetFontSize(12.0f)
                      .SetTextColor(UiColor(0x6A1B9A)));

    StackLayout subRow = MakeHStack();
    // INHERIT under DISABLED → fixed size (1.0)
    subRow.Add(MakePolicyBox("INHERIT\n→ 1.0", UiColor(C_PURPLE), UiScalePolicy::INHERIT, BOX_SIZE - 10.0f));
    // ENABLED inside DISABLED → still scales
    subRow.Add(MakePolicyBox("ENABLED\n→ sys", UiColor(C_DEEP_ORG), UiScalePolicy::ENABLED, BOX_SIZE - 10.0f));
    disabledSub.Add(subRow);

    outerRow.Add(disabledSub);
    zone.Add(outerRow);
    zone.Add(MakeDesc(
      "Orange: INHERIT — scales. "
      "Purple/Orange-red (inside purple box): "
      "Purple=INHERIT (fixed at 1.0), Orange-red=ENABLED (scales despite DISABLED parent)."));

    return zone;
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // Shared UI builder helpers
  // ═══════════════════════════════════════════════════════════════════════════

  // Wrapper zone container (vertical stack with background)
  StackLayout BuildZoneContainer(const UiColor& bgColor)
  {
    StackLayout zone = StackLayout::New(StackOrientation::VERTICAL);
    zone.SetRequestedWidth(MATCH_PARENT);
    zone.SetRequestedHeight(WRAP_CONTENT);
    zone.SetBackgroundColor(bgColor);
    zone.SetPadding(Extents(12, 12, 12, 12));
    zone.SetSpacing(10.0f);
    zone.SetCornerRadius(Vector4(ZONE_RADIUS, ZONE_RADIUS, ZONE_RADIUS, ZONE_RADIUS));
    return zone;
  }

  // Horizontal stack for a row of boxes
  StackLayout MakeHStack()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(WRAP_CONTENT);
    row.SetSpacing(8.0f);
    return row;
  }

  // Colored box with centered policy label and explicit UiScalePolicy
  View MakePolicyBox(const char* text, const UiColor& color, UiScalePolicy policy, float size)
  {
    Label lbl = Label::New(text);
    lbl.SetRequestedWidth(MATCH_PARENT);
    lbl.SetRequestedHeight(MATCH_PARENT);
    lbl.SetFontSize(11.0f);
    lbl.SetTextColor(UiColor(C_WHITE));
    lbl.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    lbl.SetVerticalTextAlignment(Text::Alignment::CENTER);
    lbl.SetMultiLine(true);
    lbl.SetLineWrapMode(Text::LineWrapMode::WORD);

    View box = View::New();
    box.SetRequestedWidth(size);
    box.SetRequestedHeight(size);
    box.SetBackgroundColor(color);
    box.SetCornerRadius(Vector4(BOX_RADIUS, BOX_RADIUS, BOX_RADIUS, BOX_RADIUS));
    box.SetUiScalePolicy(policy); // ← apply the policy
    box.Add(lbl);
    return box;
  }

  // Clickable button with centered text label
  InteractiveView MakeButton(const std::string& text, const UiColor& color)
  {
    Label lbl = Label::New(text.c_str());
    lbl.SetRequestedWidth(MATCH_PARENT);
    lbl.SetRequestedHeight(MATCH_PARENT);
    lbl.SetFontSize(15.0f);
    lbl.SetTextColor(UiColor(C_WHITE));
    lbl.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    lbl.SetVerticalTextAlignment(Text::Alignment::CENTER);

    InteractiveView btn = InteractiveView::New();
    btn.SetRequestedWidth(WRAP_CONTENT);
    btn.SetRequestedHeight(BTN_HEIGHT);
    btn.SetBackgroundColor(color);
    btn.SetCornerRadius(Vector4(BTN_RADIUS, BTN_RADIUS, BTN_RADIUS, BTN_RADIUS));
    btn.SetPadding(Extents(12, 12, 6, 6));
    btn.Add(lbl);
    return btn;
  }

  // Zone section title label
  Label MakeZoneTitle(const char* text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFontSize(15.0f)
      .SetTextColor(UiColor(C_ZONE_TITLE));
  }

  // Small description label (multi-line, gray)
  Label MakeDesc(const char* text)
  {
    return Label::New(text)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetFontSize(12.0f)
      .SetTextColor(UiColor(C_GRAY_TEXT))
      .SetMultiLine(true)
      .SetLineWrapMode(Text::LineWrapMode::WORD);
  }

private:
  Application& mApplication;
  Label        mScaleLabel;  // updated on every ApplyScale()
  InputField   mScaleInput;
};

// ─────────────────────────── Entry point ─────────────────────────────────────
int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  UiScaleController controller(application);
  application.MainLoop();
  return 0;
}
