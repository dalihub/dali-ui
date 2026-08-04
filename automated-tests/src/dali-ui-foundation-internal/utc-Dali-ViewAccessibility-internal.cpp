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

#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/view-accessibility.h>
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/internal/views/view/view-accessibility-data.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/object/type-registry-helper.h>

#include <limits>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
namespace UiAccessibility = Dali::Ui::Accessibility;

bool EqualsStringView(StringView value, const char* text)
{
  const std::size_t length = strlen(text);
  return value.Size() == length && value.Data() && strncmp(value.Data(), text, length) == 0;
}

bool ViewAccessibilityLocalizationOverride(StringView resourceId, StringView domain, Dali::String& result)
{
  if(EqualsStringView(resourceId, "IDS_NAME"))
  {
    result = EqualsStringView(domain, "domainA") ? "Name A" : "Name Default";
    return true;
  }
  if(EqualsStringView(resourceId, "IDS_DESCRIPTION"))
  {
    result = EqualsStringView(domain, "domainB") ? "Description B" : "Description Default";
    return true;
  }
  return false;
}

void CleanupLocalization(View view)
{
  view.ClearTranslatableAccessibilityName();
  view.ClearTranslatableAccessibilityDescription();
  auto manager = UiLocalizationManager::Get();
  manager.ClearLocalizedStringOverride();
  manager.SetDefaultDomain("");
}

class TestAccessibilityViewAccessible : public ViewAccessible
{
public:
  explicit TestAccessibilityViewAccessible(Actor self)
  : ViewAccessible(self)
  {
  }

  bool GrabHighlight() override
  {
    ++grabHighlightCount;
    return grabHighlightResult;
  }

  bool ClearHighlight() override
  {
    ++clearHighlightCount;
    return clearHighlightResult;
  }

  std::string GetDescriptionRaw() const override
  {
    return "Raw fallback description";
  }

  int  grabHighlightCount{0};
  int  clearHighlightCount{0};
  bool grabHighlightResult{true};
  bool clearHighlightResult{true};
};

class TestAccessibilityViewImpl : public ViewImpl
{
public:
  using Ptr = IntrusivePtr<TestAccessibilityViewImpl>;

  enum class RequestedMode
  {
    DYNAMIC,
    EMPTY,
    FALLBACK
  };

  enum class DefaultNameMode
  {
    DYNAMIC,
    EMPTY,
    FALLBACK
  };

  enum class DefaultDescriptionMode
  {
    DYNAMIC,
    EMPTY,
    FALLBACK
  };

  static Ptr New()
  {
    return Ptr(new TestAccessibilityViewImpl());
  }

  void SetRequestedMode(RequestedMode mode)
  {
    mRequestedMode = mode;
  }

  void SetDefaultNameMode(DefaultNameMode mode)
  {
    mDefaultNameMode = mode;
  }

  void SetDefaultDescriptionMode(DefaultDescriptionMode mode)
  {
    mDefaultDescriptionMode = mode;
  }

  bool OnAccessibilityActivate() override
  {
    ++activateCount;
    return true;
  }

  bool OnAccessibilityEscape() override
  {
    ++escapeCount;
    return true;
  }

  bool OnAccessibilityValueChange(bool isIncreased) override
  {
    ++valueChangeCount;
    valueChangeBalance += isIncreased ? 1 : -1;
    return true;
  }

  bool OnAccessibilityScrollToChild(View child) override
  {
    ++scrollToChildCount;
    lastScrolledChild = child;
    return static_cast<bool>(child);
  }

  bool OnAccessibilityPan(PanGesture gesture) override
  {
    ++panCount;
    return true;
  }

  bool OnAccessibilityZoom() override
  {
    ++zoomCount;
    return true;
  }

  bool OnAccessibilityRequestName(Dali::String& value) override
  {
    return ResolveRequestedValue(value, "Requested name");
  }

  bool OnAccessibilityRequestDefaultName(Dali::String& value) override
  {
    if(mDefaultNameMode == DefaultNameMode::FALLBACK)
    {
      value = "Ignored default name";
      return false;
    }
    value = mDefaultNameMode == DefaultNameMode::DYNAMIC ? "Requested default name" : "";
    return true;
  }

  bool OnAccessibilityRequestDescription(Dali::String& value) override
  {
    return ResolveRequestedValue(value, "Requested description");
  }

  bool OnAccessibilityRequestDefaultDescription(Dali::String& value) override
  {
    if(mDefaultDescriptionMode == DefaultDescriptionMode::FALLBACK)
    {
      value = "Ignored default description";
      return false;
    }
    value = mDefaultDescriptionMode == DefaultDescriptionMode::DYNAMIC ? "Requested default description" : "";
    return true;
  }

  bool OnAccessibilityRequestValue(Dali::String& value) override
  {
    return ResolveRequestedValue(value, "Requested value");
  }

  int  activateCount{0};
  int  escapeCount{0};
  int  valueChangeCount{0};
  int  valueChangeBalance{0};
  int  scrollToChildCount{0};
  int  panCount{0};
  int  zoomCount{0};
  View lastScrolledChild;

protected:
  ~TestAccessibilityViewImpl() override = default;

private:
  TestAccessibilityViewImpl()
  {
    Dali::Ui::Integration::ViewAccessibility::SetAccessibleObjectCreator(
      *this,
      [](Dali::Ui::View view) -> ViewAccessible*
    {
      return new TestAccessibilityViewAccessible(view);
    });
  }

  bool ResolveRequestedValue(Dali::String& value, const char* dynamicValue)
  {
    if(mRequestedMode == RequestedMode::FALLBACK)
    {
      value = "Ignored value";
      return false;
    }
    value = mRequestedMode == RequestedMode::DYNAMIC ? dynamicValue : "";
    return true;
  }

  RequestedMode          mRequestedMode{RequestedMode::FALLBACK};
  DefaultNameMode        mDefaultNameMode{DefaultNameMode::FALLBACK};
  DefaultDescriptionMode mDefaultDescriptionMode{DefaultDescriptionMode::FALLBACK};
};

View CreateTestAccessibilityView(TestAccessibilityViewImpl*& implementation)
{
  auto impl      = TestAccessibilityViewImpl::New();
  implementation = impl.Get();
  View view(*impl);
  impl->Initialize();
  return view;
}

BaseHandle CreateRegisteredTestAccessibilityView()
{
  TestAccessibilityViewImpl* implementation = nullptr;
  return CreateTestAccessibilityView(implementation);
}

DALI_TYPE_REGISTRATION_BEGIN(TestAccessibilityViewImpl, ViewImpl, CreateRegisteredTestAccessibilityView)
DALI_TYPE_REGISTRATION_END()
} // namespace

void utc_dali_view_accessibility_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_accessibility_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliViewAccessibilityReadingInfoInternalP(void)
{
  UiTestApplication application;

  View view       = View::New();
  auto accessible = Dali::Accessibility::Accessible::Get(view);
  DALI_TEST_CHECK(accessible);

  auto& viewData = Dali::Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));

  auto defaultTypes = viewData.GetAccessibilityReadingInfoType();
  DALI_TEST_EQUALS(static_cast<bool>(defaultTypes[Dali::Integration::Accessibility::ReadingInfoType::NAME]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(defaultTypes[Dali::Integration::Accessibility::ReadingInfoType::ROLE]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(defaultTypes[Dali::Integration::Accessibility::ReadingInfoType::DESCRIPTION]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(defaultTypes[Dali::Integration::Accessibility::ReadingInfoType::STATE]), true, TEST_LOCATION);

  Dali::Integration::Accessibility::ReadingInfoTypes internalTypes;
  internalTypes[Dali::Integration::Accessibility::ReadingInfoType::NAME]        = true;
  internalTypes[Dali::Integration::Accessibility::ReadingInfoType::ROLE]        = true;
  internalTypes[Dali::Integration::Accessibility::ReadingInfoType::DESCRIPTION] = true;
  internalTypes[Dali::Integration::Accessibility::ReadingInfoType::STATE]       = true;
  viewData.SetAccessibilityReadingInfoType(internalTypes);

  auto storedTypes = viewData.GetAccessibilityReadingInfoType();
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::NAME]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::ROLE]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::DESCRIPTION]), true, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::STATE]), true, TEST_LOCATION);

  viewData.SetAccessibilityReadingInfoType({});
  storedTypes = viewData.GetAccessibilityReadingInfoType();
  DALI_TEST_EQUALS(static_cast<bool>(storedTypes[Dali::Integration::Accessibility::ReadingInfoType::NAME]), false, TEST_LOCATION);

  view.AppendAccessibilityAttribute("reading_info_type", "name|description");

  auto exportedAttributes = accessible->GetAttributes();
  DALI_TEST_EQUALS(exportedAttributes["reading_info_type"], "name|description", TEST_LOCATION);

  view.AppendAccessibilityAttribute("reading_info_type", "name|role|description|state");

  exportedAttributes = accessible->GetAttributes();
  DALI_TEST_EQUALS(exportedAttributes["reading_info_type"], "name|role|description|state", TEST_LOCATION);

  view.ClearAccessibilityAttributes();

  exportedAttributes = accessible->GetAttributes();
  DALI_TEST_CHECK(exportedAttributes.find("reading_info_type") == exportedAttributes.end());

  END_TEST;
}

int UtcDaliViewAccessibilityDirectApiDefaultsAndMetadataP(void)
{
  UiTestApplication application;

  View view = View::New();
  DALI_TEST_EQUALS(view.GetAccessibilityName(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityDescription(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityValue(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityRole(), UiAccessibility::Role::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsAccessibilityHidden(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsAccessibilityHighlightable(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsAccessibilityScrollable(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsAccessibilityModal(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAutomationId(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetTranslatableAccessibilityName(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetTranslatableAccessibilityDescription(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsInitialAccessibilityHighlightRequested(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsAccessibilityCollectionContainer(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityCollectionIndex(), -1, TEST_LOCATION);

  view.SetAccessibilityName("");
  view.SetAccessibilityDescription("");
  view.SetAccessibilityValue("");
  view.SetAccessibilityScrollable(false);
  view.SetAccessibilityModal(false);
  view.SetAutomationId("");

  view.SetAccessibilityName("Name");
  view.SetAccessibilityName("Name");
  view.SetAccessibilityDescription("Description");
  view.SetAccessibilityDescription("Description");
  view.SetAccessibilityValue("Value");
  view.SetAccessibilityValue("Value");
  view.SetAccessibilityRole(UiAccessibility::Role::BUTTON);
  view.SetAccessibilityRole(static_cast<UiAccessibility::Role>(UiAccessibility::Role::MAX_COUNT));
  view.SetAccessibilityHidden(true);
  view.SetAccessibilityHidden(true);
  view.SetAccessibilityScrollable(true);
  view.SetAccessibilityScrollable(false);
  view.SetAccessibilityModal(true);
  view.SetAccessibilityModal(false);
  view.SetAutomationId("view-id");
  view.SetAutomationId("");

  DALI_TEST_EQUALS(view.GetAccessibilityName(), "Name", TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityDescription(), "Description", TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityValue(), "Value", TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityRole(), UiAccessibility::Role::BUTTON, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsAccessibilityHidden(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsAccessibilityScrollable(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsAccessibilityModal(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAutomationId(), Dali::String(), TEST_LOCATION);

  view.SetAccessibilityHighlightable(false);
  DALI_TEST_EQUALS(view.IsAccessibilityHighlightable(), false, TEST_LOCATION);
  view.SetAccessibilityHighlightable(true);
  DALI_TEST_EQUALS(view.IsAccessibilityHighlightable(), true, TEST_LOCATION);
  view.ResetAccessibilityHighlightable();
  DALI_TEST_EQUALS(view.IsAccessibilityHighlightable(), true, TEST_LOCATION);
  view.SetAccessibilityRole(UiAccessibility::Role::NONE);
  DALI_TEST_EQUALS(view.IsAccessibilityHighlightable(), false, TEST_LOCATION);

  DALI_TEST_CHECK(view.HasAccessibilityState(UiAccessibility::State::ENABLED));
  view.AddAccessibilityState(UiAccessibility::State::BUSY);
  view.RemoveAccessibilityState(UiAccessibility::State::ENABLED);
  DALI_TEST_CHECK(view.HasAccessibilityState(UiAccessibility::State::BUSY));
  DALI_TEST_CHECK(!view.HasAccessibilityState(UiAccessibility::State::ENABLED));
  view.AddAccessibilityState(static_cast<UiAccessibility::State>(UiAccessibility::State::MAX_COUNT));
  view.RemoveAccessibilityState(static_cast<UiAccessibility::State>(UiAccessibility::State::MAX_COUNT));
  DALI_TEST_CHECK(!view.HasAccessibilityState(static_cast<UiAccessibility::State>(UiAccessibility::State::MAX_COUNT)));
  view.ClearAccessibilityStates();
  DALI_TEST_CHECK(!view.HasAccessibilityState(UiAccessibility::State::BUSY));

  view.SetRequestInitialAccessibilityHighlight(true);
  view.SetAccessibilityCollectionContainer(true);
  view.SetAccessibilityCollectionIndex(7);
  view.AppendAccessibilityAttribute("custom", "first");
  view.AppendAccessibilityAttribute("custom", "second");
  DALI_TEST_CHECK(view.IsInitialAccessibilityHighlightRequested());
  DALI_TEST_CHECK(view.IsAccessibilityCollectionContainer());
  DALI_TEST_EQUALS(view.GetAccessibilityCollectionIndex(), 7, TEST_LOCATION);

  auto* accessible = Dali::Accessibility::Accessible::Get(view);
  DALI_TEST_CHECK(accessible);
  auto attributes = accessible->GetAttributes();
  DALI_TEST_EQUALS(attributes["initial-a11y-highlight"], "true", TEST_LOCATION);
  DALI_TEST_EQUALS(attributes["collection_container"], "true", TEST_LOCATION);
  DALI_TEST_EQUALS(attributes["collection_index"], "7", TEST_LOCATION);
  DALI_TEST_EQUALS(attributes["custom"], "second", TEST_LOCATION);

  view.SetRequestInitialAccessibilityHighlight(false);
  view.SetAccessibilityCollectionContainer(false);
  view.SetAccessibilityCollectionIndex(-1);
  view.RemoveAccessibilityAttribute("custom");
  DALI_TEST_CHECK(!view.IsInitialAccessibilityHighlightRequested());
  DALI_TEST_CHECK(!view.IsAccessibilityCollectionContainer());
  DALI_TEST_EQUALS(view.GetAccessibilityCollectionIndex(), -1, TEST_LOCATION);

  view.AppendAccessibilityAttribute("collection_index", "invalid");
  DALI_TEST_EQUALS(view.GetAccessibilityCollectionIndex(), -1, TEST_LOCATION);
  view.AppendAccessibilityAttribute("collection_index", "-1");
  DALI_TEST_EQUALS(view.GetAccessibilityCollectionIndex(), -1, TEST_LOCATION);
  view.AppendAccessibilityAttribute("collection_index", "2147483648");
  DALI_TEST_EQUALS(view.GetAccessibilityCollectionIndex(), -1, TEST_LOCATION);
  view.ClearAccessibilityCollectionIndex();
  view.ClearAccessibilityAttributes();

  END_TEST;
}

int UtcDaliViewAccessibilityReadingInfoAndLanguageSpansP(void)
{
  UiTestApplication application;

  View view = View::New();
  DALI_TEST_CHECK(view.HasAccessibilityReadingInfo(UiAccessibility::ReadingInfo::NAME));
  DALI_TEST_CHECK(view.HasAccessibilityReadingInfo(UiAccessibility::ReadingInfo::ROLE));
  DALI_TEST_CHECK(view.HasAccessibilityReadingInfo(UiAccessibility::ReadingInfo::DESCRIPTION));
  DALI_TEST_CHECK(view.HasAccessibilityReadingInfo(UiAccessibility::ReadingInfo::STATE));

  view.RemoveAccessibilityReadingInfo(UiAccessibility::ReadingInfo::ROLE);
  view.AddAccessibilityReadingInfo(UiAccessibility::ReadingInfo::ROLE);
  view.AddAccessibilityReadingInfo(UiAccessibility::ReadingInfo::ROLE);
  DALI_TEST_CHECK(view.HasAccessibilityReadingInfo(UiAccessibility::ReadingInfo::ROLE));
  view.AddAccessibilityReadingInfo(static_cast<UiAccessibility::ReadingInfo>(UiAccessibility::ReadingInfo::MAX_COUNT));
  view.RemoveAccessibilityReadingInfo(static_cast<UiAccessibility::ReadingInfo>(UiAccessibility::ReadingInfo::MAX_COUNT));
  DALI_TEST_CHECK(!view.HasAccessibilityReadingInfo(static_cast<UiAccessibility::ReadingInfo>(UiAccessibility::ReadingInfo::MAX_COUNT)));
  view.ClearAccessibilityReadingInfo();
  DALI_TEST_CHECK(!view.HasAccessibilityReadingInfo(UiAccessibility::ReadingInfo::NAME));

  View empty = View::New();
  DALI_TEST_CHECK(!empty.AddAccessibilityNameLanguageSpan(0u, 1u, "en"));
  DALI_TEST_CHECK(!empty.AddAccessibilityDescriptionLanguageSpan(0u, 1u, "en"));
  empty.ClearAccessibilityNameLanguageSpans();
  empty.ClearAccessibilityDescriptionLanguageSpans();

  view.SetAccessibilityName(
    "A\xEA\xB0\x80"
    "BC");
  view.SetAccessibilityDescription("wxyz");
  DALI_TEST_CHECK(view.AddAccessibilityNameLanguageSpan(2u, 2u, "ko-KR"));
  DALI_TEST_CHECK(view.AddAccessibilityNameLanguageSpan(0u, 1u, "en-US"));
  DALI_TEST_CHECK(!view.AddAccessibilityNameLanguageSpan(0u, 2u, "overlap"));
  DALI_TEST_CHECK(!view.AddAccessibilityNameLanguageSpan(0u, 0u, "en"));
  DALI_TEST_CHECK(!view.AddAccessibilityNameLanguageSpan(0u, 1u, ""));
  DALI_TEST_CHECK(!view.AddAccessibilityNameLanguageSpan(std::numeric_limits<uint32_t>::max(), 1u, "en"));
  DALI_TEST_CHECK(!view.AddAccessibilityNameLanguageSpan(4u, 1u, "en"));

  DALI_TEST_CHECK(view.AddAccessibilityDescriptionLanguageSpan(2u, 2u, "ko-KR"));
  DALI_TEST_CHECK(view.AddAccessibilityDescriptionLanguageSpan(0u, 1u, "a\b\f\n\r\t\\\""));
  DALI_TEST_CHECK(!view.AddAccessibilityDescriptionLanguageSpan(0u, 2u, "overlap"));
  DALI_TEST_CHECK(!view.AddAccessibilityDescriptionLanguageSpan(0u, 0u, "en"));
  DALI_TEST_CHECK(!view.AddAccessibilityDescriptionLanguageSpan(0u, 1u, ""));
  DALI_TEST_CHECK(!view.AddAccessibilityDescriptionLanguageSpan(std::numeric_limits<uint32_t>::max(), 1u, "en"));
  DALI_TEST_CHECK(!view.AddAccessibilityDescriptionLanguageSpan(4u, 1u, "en"));

  auto* accessible = Dali::Accessibility::Accessible::Get(view);
  DALI_TEST_CHECK(accessible);
  auto attributes = accessible->GetAttributes();
  DALI_TEST_EQUALS(attributes["a11y.name.spans"],
                   "[{\"start\":0,\"length\":1,\"locale\":\"en-US\"},{\"start\":2,\"length\":2,\"locale\":\"ko-KR\"}]",
                   TEST_LOCATION);
  DALI_TEST_CHECK(attributes["a11y.description.spans"].find("\\b\\f\\n\\r\\t\\\\\\\"") != std::string::npos);

  view.ClearAccessibilityNameLanguageSpans();
  view.ClearAccessibilityDescriptionLanguageSpans();
  attributes = accessible->GetAttributes();
  DALI_TEST_CHECK(attributes.find("a11y.name.spans") == attributes.end());
  DALI_TEST_CHECK(attributes.find("a11y.description.spans") == attributes.end());

  view.AddAccessibilityNameLanguageSpan(0u, 1u, "en");
  view.AddAccessibilityDescriptionLanguageSpan(0u, 1u, "en");
  view.SetAccessibilityName("replacement");
  view.SetAccessibilityDescription("replacement");
  attributes = accessible->GetAttributes();
  DALI_TEST_CHECK(attributes.find("a11y.name.spans") == attributes.end());
  DALI_TEST_CHECK(attributes.find("a11y.description.spans") == attributes.end());

  END_TEST;
}

int UtcDaliViewAccessibilityTranslationAndRequestedVirtualsP(void)
{
  UiTestApplication application;

  TestAccessibilityViewImpl* implementation = nullptr;
  View                       view           = CreateTestAccessibilityView(implementation);
  auto                       manager        = UiLocalizationManager::Get();
  manager.SetLocalizedStringOverride(ViewAccessibilityLocalizationOverride);

  view.SetTranslatableAccessibilityName("IDS_NAME", "domainA");
  view.SetTranslatableAccessibilityDescription("IDS_DESCRIPTION", "domainB");
  DALI_TEST_EQUALS(view.GetTranslatableAccessibilityName(), "IDS_NAME", TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetTranslatableAccessibilityDescription(), "IDS_DESCRIPTION", TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityName(), "Name A", TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityDescription(), "Description B", TEST_LOCATION);

  manager.SetDefaultDomain("domainDefault");
  view.SetTranslatableAccessibilityName("IDS_NAME");
  view.SetTranslatableAccessibilityDescription("IDS_DESCRIPTION");
  manager.RefreshBindings();
  DALI_TEST_EQUALS(view.GetAccessibilityName(), "Name Default", TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetAccessibilityDescription(), "Description Default", TEST_LOCATION);

  view.SetTranslatableAccessibilityName("");
  view.SetTranslatableAccessibilityDescription("");
  DALI_TEST_EQUALS(view.GetTranslatableAccessibilityName(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetTranslatableAccessibilityDescription(), Dali::String(), TEST_LOCATION);

  view.SetTranslatableAccessibilityName("IDS_NAME", "domainA");
  view.SetTranslatableAccessibilityDescription("IDS_DESCRIPTION", "domainB");
  view.SetAccessibilityName("Static name");
  view.SetAccessibilityDescription("Static description");
  DALI_TEST_EQUALS(view.GetTranslatableAccessibilityName(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetTranslatableAccessibilityDescription(), Dali::String(), TEST_LOCATION);

  view.SetAccessibilityValue("Static value");
  auto* accessible = Dali::Accessibility::Accessible::Get(view);
  DALI_TEST_CHECK(accessible);
  DALI_TEST_EQUALS(accessible->GetName(), "Static name", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetDescription(), "Static description", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetValue(), "Static value", TEST_LOCATION);

  implementation->SetRequestedMode(TestAccessibilityViewImpl::RequestedMode::DYNAMIC);
  DALI_TEST_EQUALS(accessible->GetName(), "Requested name", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetDescription(), "Requested description", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetValue(), "Requested value", TEST_LOCATION);

  implementation->SetRequestedMode(TestAccessibilityViewImpl::RequestedMode::EMPTY);
  DALI_TEST_EQUALS(accessible->GetName(), std::string(), TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetDescription(), std::string(), TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetValue(), std::string(), TEST_LOCATION);

  implementation->SetRequestedMode(TestAccessibilityViewImpl::RequestedMode::FALLBACK);
  DALI_TEST_EQUALS(accessible->GetName(), "Static name", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetDescription(), "Static description", TEST_LOCATION);
  DALI_TEST_EQUALS(accessible->GetValue(), "Static value", TEST_LOCATION);

  // Explicit names win over the component-default hook.
  implementation->SetDefaultNameMode(TestAccessibilityViewImpl::DefaultNameMode::DYNAMIC);
  DALI_TEST_EQUALS(accessible->GetName(), "Static name", TEST_LOCATION);

  // The default hook runs after an empty explicit name and before Actor::NAME.
  view.SetAccessibilityName("");
  view.SetProperty(Actor::Property::NAME, "Actor fallback name");
  DALI_TEST_EQUALS(accessible->GetName(), "Requested default name", TEST_LOCATION);

  // true + empty is an intentional final default and suppresses later fallbacks.
  implementation->SetDefaultNameMode(TestAccessibilityViewImpl::DefaultNameMode::EMPTY);
  DALI_TEST_EQUALS(accessible->GetName(), std::string(), TEST_LOCATION);

  // false resumes the legacy raw-name and Actor::NAME fallback chain.
  implementation->SetDefaultNameMode(TestAccessibilityViewImpl::DefaultNameMode::FALLBACK);
  DALI_TEST_EQUALS(accessible->GetName(), "Actor fallback name", TEST_LOCATION);

  // Explicit descriptions win over the component-default hook.
  implementation->SetDefaultDescriptionMode(TestAccessibilityViewImpl::DefaultDescriptionMode::DYNAMIC);
  DALI_TEST_EQUALS(accessible->GetDescription(), "Static description", TEST_LOCATION);

  // The default hook runs after an empty explicit description and before GetDescriptionRaw().
  view.SetAccessibilityDescription("");
  DALI_TEST_EQUALS(accessible->GetDescription(), "Requested default description", TEST_LOCATION);

  // true + empty is an intentional final default and suppresses the raw fallback.
  implementation->SetDefaultDescriptionMode(TestAccessibilityViewImpl::DefaultDescriptionMode::EMPTY);
  DALI_TEST_EQUALS(accessible->GetDescription(), std::string(), TEST_LOCATION);

  // false resumes the legacy raw-description fallback.
  implementation->SetDefaultDescriptionMode(TestAccessibilityViewImpl::DefaultDescriptionMode::FALLBACK);
  DALI_TEST_EQUALS(accessible->GetDescription(), "Raw fallback description", TEST_LOCATION);

  View fallback = View::New();
  fallback.SetAccessibilityName("Default virtual fallback name");
  fallback.SetAccessibilityDescription("Default virtual fallback description");
  fallback.SetAccessibilityValue("Default virtual fallback value");
  auto* fallbackAccessible = Dali::Accessibility::Accessible::Get(fallback);
  DALI_TEST_EQUALS(fallbackAccessible->GetName(), "Default virtual fallback name", TEST_LOCATION);
  DALI_TEST_EQUALS(fallbackAccessible->GetDescription(), "Default virtual fallback description", TEST_LOCATION);
  DALI_TEST_EQUALS(fallbackAccessible->GetValue(), "Default virtual fallback value", TEST_LOCATION);

  // The base default-name hook declines, then Actor::NAME supplies the name.
  View actorNamedFallback = View::New();
  actorNamedFallback.SetProperty(Actor::Property::NAME, "Actor-only fallback name");
  auto* actorNamedAccessible = Dali::Accessibility::Accessible::Get(actorNamedFallback);
  DALI_TEST_CHECK(actorNamedAccessible);
  DALI_TEST_EQUALS(actorNamedAccessible->GetName(), "Actor-only fallback name", TEST_LOCATION);
  DALI_TEST_EQUALS(actorNamedAccessible->GetDescription(), std::string(), TEST_LOCATION);

  // The integration getter delegates non-View Actors to the adaptor.
  Actor rawActor = Actor::New();
  DALI_TEST_CHECK(Dali::Accessibility::Accessible::Get(rawActor));

  // Disabling creation prevents both ViewAccessible and adaptor fallback.
  View creationDisabled = View::New();
  Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(creationDisabled)).EnableCreateAccessible(false);
  DALI_TEST_CHECK(!Dali::Accessibility::Accessible::Get(creationDisabled));

  CleanupLocalization(view);
  END_TEST;
}

int UtcDaliViewAccessibilityRelationsActionsAndSignalsP(void)
{
  UiTestApplication application;

  TestAccessibilityViewImpl* implementation = nullptr;
  View                       source         = CreateTestAccessibilityView(implementation);
  View                       first          = View::New();
  View                       second         = View::New();
  View                       empty;

  source.RemoveAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, first);
  source.ClearAccessibilityRelations();
  DALI_TEST_CHECK(!source.HasAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, first));
  source.AddAccessibilityRelation(UiAccessibility::RelationType::MAX_COUNT, first);
  source.AddAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, empty);
  source.AddAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, first);
  source.AddAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, first);
  source.AddAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, second);
  {
    View expired = View::New();
    source.AddAccessibilityRelation(UiAccessibility::RelationType::DESCRIBED_BY, expired);
  }
  View liveDescription = View::New();
  source.AddAccessibilityRelation(UiAccessibility::RelationType::DESCRIBED_BY, liveDescription);
  DALI_TEST_CHECK(source.HasAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, first));
  DALI_TEST_CHECK(source.HasAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, second));
  DALI_TEST_CHECK(source.HasAccessibilityRelation(UiAccessibility::RelationType::DESCRIBED_BY, liveDescription));
  DALI_TEST_CHECK(!source.HasAccessibilityRelation(UiAccessibility::RelationType::MAX_COUNT, first));
  DALI_TEST_CHECK(!source.HasAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, empty));

  auto* accessible = Dali::Accessibility::Accessible::Get(source);
  Dali::Accessibility::Accessible::Get(first);
  Dali::Accessibility::Accessible::Get(second);
  Dali::Accessibility::Accessible::Get(liveDescription);
  DALI_TEST_EQUALS(accessible->GetRelationSet().size(), 2u, TEST_LOCATION);

  source.RemoveAccessibilityRelation(UiAccessibility::RelationType::DESCRIBED_BY, first);
  source.RemoveAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, first);
  DALI_TEST_CHECK(!source.HasAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, first));
  source.ClearAccessibilityRelations();
  DALI_TEST_CHECK(!source.HasAccessibilityRelation(UiAccessibility::RelationType::LABEL_FOR, second));

  int                                         highlightedCount = 0;
  ConnectionTracker                           tracker;
  std::vector<UiAccessibility::ReadingStatus> readingStatuses;
  bool                                        readingSourceMatches   = true;
  bool                                        highlightSourceMatches = true;
  source.AccessibilityReadingStatusChangedSignal().Connect(&tracker, [&readingStatuses, &readingSourceMatches, source](View view, UiAccessibility::ReadingStatus status)
  {
    readingSourceMatches = readingSourceMatches && view == source;
    readingStatuses.push_back(status);
  });
  source.AccessibilityHighlightedSignal().Connect(&tracker, [&highlightedCount, &highlightSourceMatches, source](View view, bool highlighted)
  {
    highlightSourceMatches = highlightSourceMatches && view == source;
    highlightedCount += highlighted ? 1 : -1;
  });

  Property::Map actionAttributes;
  DALI_TEST_CHECK(source.DoAction("activate", actionAttributes));
  DALI_TEST_CHECK(source.DoAction("escape", actionAttributes));
  DALI_TEST_CHECK(source.DoAction("increment", actionAttributes));
  DALI_TEST_CHECK(source.DoAction("decrement", actionAttributes));
  DALI_TEST_CHECK(source.DoAction("ReadingSkipped", actionAttributes));
  DALI_TEST_CHECK(source.DoAction("ReadingPaused", actionAttributes));
  DALI_TEST_CHECK(source.DoAction("ReadingResumed", actionAttributes));
  DALI_TEST_CHECK(source.DoAction("ReadingCancelled", actionAttributes));
  DALI_TEST_CHECK(source.DoAction("ReadingStopped", actionAttributes));
  DALI_TEST_CHECK(!source.DoAction("unknown-accessibility-action", actionAttributes));
  DALI_TEST_EQUALS(implementation->activateCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(implementation->escapeCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(implementation->valueChangeCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(implementation->valueChangeBalance, 0, TEST_LOCATION);

  const std::vector<UiAccessibility::ReadingStatus> expectedStatuses{
    UiAccessibility::ReadingStatus::SKIPPED,
    UiAccessibility::ReadingStatus::PAUSED,
    UiAccessibility::ReadingStatus::RESUMED,
    UiAccessibility::ReadingStatus::CANCELLED,
    UiAccessibility::ReadingStatus::STOPPED};
  DALI_TEST_CHECK(readingStatuses == expectedStatuses);
  DALI_TEST_CHECK(readingSourceMatches);

  auto* viewAccessible = dynamic_cast<Dali::Ui::ViewAccessible*>(accessible);
  DALI_TEST_CHECK(viewAccessible);
  DALI_TEST_CHECK(viewAccessible->ScrollToChild(first));
  DALI_TEST_EQUALS(implementation->scrollToChildCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(implementation->lastScrolledChild == first);
  DALI_TEST_CHECK(!viewAccessible->ScrollToChild(Actor::New()));
  DALI_TEST_EQUALS(implementation->scrollToChildCount, 1, TEST_LOCATION);

  DALI_TEST_CHECK(implementation->OnAccessibilityPan(PanGesture{}));
  DALI_TEST_CHECK(implementation->OnAccessibilityZoom());
  DALI_TEST_EQUALS(implementation->panCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(implementation->zoomCount, 1, TEST_LOCATION);

  auto& data = Dali::Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(source)).GetOrCreateAccessibilityData();
  data.mAccessibilityHighlightedSignal.Emit(source, true);
  data.mAccessibilityHighlightedSignal.Emit(source, false);
  DALI_TEST_EQUALS(highlightedCount, 0, TEST_LOCATION);
  DALI_TEST_CHECK(highlightSourceMatches);

  View defaultActions = View::New();
  DALI_TEST_CHECK(!defaultActions.DoAction("escape", actionAttributes));
  DALI_TEST_CHECK(!defaultActions.DoAction("increment", actionAttributes));
  DALI_TEST_CHECK(!defaultActions.DoAction("decrement", actionAttributes));
  DALI_TEST_CHECK(!Ui::GetImpl(defaultActions).OnAccessibilityPan(PanGesture{}));
  DALI_TEST_CHECK(!Ui::GetImpl(defaultActions).OnAccessibilityZoom());

  auto* defaultAccessible = dynamic_cast<Dali::Ui::ViewAccessible*>(Dali::Accessibility::Accessible::Get(defaultActions));
  DALI_TEST_CHECK(defaultAccessible);
  DALI_TEST_CHECK(!defaultAccessible->ScrollToChild(first));

  END_TEST;
}

int UtcDaliViewAccessibilityHighlightCommandsP(void)
{
  UiTestApplication application;

  TestAccessibilityViewImpl* implementation = nullptr;
  View                       view           = CreateTestAccessibilityView(implementation);
  auto*                      accessible     = dynamic_cast<TestAccessibilityViewAccessible*>(Dali::Accessibility::Accessible::Get(view));
  DALI_TEST_CHECK(accessible);

  DALI_TEST_CHECK(Extension::View::GrabAccessibilityHighlight(view));
  DALI_TEST_EQUALS(accessible->grabHighlightCount, 1, TEST_LOCATION);

  accessible->grabHighlightResult = false;
  DALI_TEST_CHECK(!Extension::View::GrabAccessibilityHighlight(view));
  DALI_TEST_EQUALS(accessible->grabHighlightCount, 2, TEST_LOCATION);

  DALI_TEST_CHECK(Extension::View::ClearAccessibilityHighlight(view));
  DALI_TEST_EQUALS(accessible->clearHighlightCount, 1, TEST_LOCATION);

  accessible->clearHighlightResult = false;
  DALI_TEST_CHECK(!Extension::View::ClearAccessibilityHighlight(view));
  DALI_TEST_EQUALS(accessible->clearHighlightCount, 2, TEST_LOCATION);

  END_TEST;
}
