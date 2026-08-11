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

#include <stdlib.h>
#include <iostream>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

// ---- Test state ----

Dali::String gLastAppliedString;
BaseHandle    gLastAppliedTarget;
int           gApplyCallCount = 0;

// ---- Test apply functions ----

void TestApplyFunc(BaseHandle target, const Dali::String& localized)
{
  gLastAppliedTarget = target;
  gLastAppliedString = localized;
  gApplyCallCount++;
}

void TestApplyFunc2(BaseHandle target, const Dali::String& localized)
{
  // Second apply function for testing multiple bindings
}

// ---- StringView comparison helper ----

bool EqualsStringView(StringView view, const char* text)
{
  const size_t len = strlen(text);
  return view.Size() == len &&
         view.Data() != nullptr &&
         strncmp(view.Data(), text, len) == 0;
}

// ---- Override functions ----

bool OverrideSpecificResourceId(StringView resourceId, StringView domain, Dali::String& outString)
{
  if(EqualsStringView(resourceId, "IDS_TEST"))
  {
    outString = Dali::String("Localized Test");
    return true;
  }
  return false;
}

bool OverrideTestAndOtherResourceId(StringView resourceId, StringView domain, Dali::String& outString)
{
  if(EqualsStringView(resourceId, "IDS_TEST"))
  {
    outString = Dali::String("Localized Test");
    return true;
  }
  if(EqualsStringView(resourceId, "IDS_OTHER"))
  {
    outString = Dali::String("Other Localized");
    return true;
  }
  return false;
}

bool OverrideAll(StringView resourceId, StringView domain, Dali::String& outString)
{
  outString = Dali::String("Overridden");
  return true;
}

bool OverrideByDomain(StringView resourceId, StringView domain, Dali::String& outString)
{
  if(EqualsStringView(domain, "domainA"))
  {
    outString = Dali::String("DomainA Result");
    return true;
  }
  if(EqualsStringView(domain, "domainB"))
  {
    outString = Dali::String("DomainB Result");
    return true;
  }
  return false;
}

bool OverrideAlwaysFalse(StringView resourceId, StringView domain, Dali::String& outString)
{
  return false;
}

// ---- Reentrant test state ----

UiLocalizationManager*  gReentrantManager     = nullptr;
bool                    gReentrantTriggered   = false;

void ReentrantApplyFunc(BaseHandle target, const Dali::String& localized)
{
  gLastAppliedTarget = target;
  gLastAppliedString = localized;
  gApplyCallCount++;

  // Trigger a mutation from within the callback
  if(gReentrantManager && target && !gReentrantTriggered)
  {
    gReentrantTriggered = true;
    gReentrantManager->SetBindingResource(target, "Text", "IDS_REENTRANT",
                                           LocalizedStringCallback::New(ReentrantApplyFunc));
  }
}

void ReentrantClearFunc(BaseHandle target, const Dali::String& localized)
{
  gLastAppliedTarget = target;
  gLastAppliedString = localized;
  gApplyCallCount++;

  // Clear binding from within the callback
  if(gReentrantManager && target && !gReentrantTriggered)
  {
    gReentrantTriggered = true;
    gReentrantManager->ClearBinding(target, "Text");
  }
}

// ---- Cleanup helper ----

void CleanupManager(UiLocalizationManager& manager, BaseHandle target = BaseHandle())
{
  if(target)
  {
    manager.ClearBindings(target);
  }

  manager.ClearLocalizedStringOverride();
  manager.SetBypassEnabled(false);
  manager.SetDefaultDomain("");
}

} // namespace

void utc_dali_uilocalizationmanager_startup(void)
{
  test_return_value = TET_UNDEF;
  gLastAppliedString = Dali::String();
  gLastAppliedTarget = BaseHandle();
  gApplyCallCount    = 0;
}

void utc_dali_uilocalizationmanager_cleanup(void)
{
  test_return_value = TET_PASS;
}

// === Constructor ===

int UtcDaliUiLocalizationManagerConstructorP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager;
  DALI_TEST_CHECK(!manager);

  END_TEST;
}

// === Get Singleton ===

int UtcDaliUiLocalizationManagerGetP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  DALI_TEST_CHECK(manager);

  CleanupManager(manager);

  END_TEST;
}

int UtcDaliUiLocalizationManagerGetSingletonP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager1 = UiLocalizationManager::Get();
  UiLocalizationManager manager2 = UiLocalizationManager::Get();

  DALI_TEST_CHECK(manager1);
  DALI_TEST_CHECK(manager2);
  DALI_TEST_CHECK(manager1 == manager2);

  CleanupManager(manager1);

  END_TEST;
}

// === DownCast ===

int UtcDaliUiLocalizationManagerDownCastP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  BaseHandle           handle(manager);

  UiLocalizationManager downCasted = UiLocalizationManager::DownCast(handle);
  DALI_TEST_CHECK(downCasted);

  CleanupManager(manager);

  END_TEST;
}

int UtcDaliUiLocalizationManagerDownCastN(void)
{
  UiTestApplication application;

  BaseHandle handle;
  UiLocalizationManager downCasted = UiLocalizationManager::DownCast(handle);
  DALI_TEST_CHECK(!downCasted);

  END_TEST;
}

// === Default Domain ===

int UtcDaliUiLocalizationManagerDefaultDomainP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  manager.SetDefaultDomain("myapp");
  DALI_TEST_EQUALS(manager.GetDefaultDomain(), Dali::String("myapp"), TEST_LOCATION);

  // Change default domain
  manager.SetDefaultDomain("otherapp");
  DALI_TEST_EQUALS(manager.GetDefaultDomain(), Dali::String("otherapp"), TEST_LOCATION);

  // Clear default domain
  manager.SetDefaultDomain("");
  DALI_TEST_EQUALS(manager.GetDefaultDomain(), Dali::String(""), TEST_LOCATION);

  END_TEST;
}

// === GetLocalizedString No Domain ===

int UtcDaliUiLocalizationManagerGetLocalizedStringNoDomainP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  // No default domain, no override -> resourceId returned as-is
  Dali::String result = manager.GetLocalizedString("IDS_TEST");
  DALI_TEST_EQUALS(result, Dali::String("IDS_TEST"), TEST_LOCATION);

  END_TEST;
}

// === GetLocalizedString Empty ResourceId ===

int UtcDaliUiLocalizationManagerGetLocalizedStringEmptyP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  // Empty resourceId -> empty string
  Dali::String result = manager.GetLocalizedString("");
  DALI_TEST_EQUALS(result, Dali::String(""), TEST_LOCATION);

  END_TEST;
}

// === GetLocalizedPluralString ===

int UtcDaliUiLocalizationManagerGetLocalizedPluralStringNoDomainP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  // Without an effective domain, plural rules are not applied.
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 1u), Dali::String("IDS_MESSAGE"), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 2u), Dali::String("IDS_MESSAGE"), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 5u), Dali::String("IDS_MESSAGE"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiLocalizationManagerGetLocalizedPluralStringDefaultDomainP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  // A missing catalog lets dngettext provide its documented source-string
  // fallback while still exercising default-domain resolution.
  manager.SetDefaultDomain("dali-ui-plural-test-missing-default-domain");

  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 1u), Dali::String("IDS_MESSAGE"), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 2u), Dali::String("IDS_PLURAL_MESSAGE"), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 5u), Dali::String("IDS_PLURAL_MESSAGE"), TEST_LOCATION);

  CleanupManager(manager);
  END_TEST;
}

int UtcDaliUiLocalizationManagerGetLocalizedPluralStringExplicitDomainP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);
  manager.SetDefaultDomain("dali-ui-plural-test-unused-default-domain");

  constexpr const char* explicitDomain = "dali-ui-plural-test-missing-explicit-domain";
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 1u, explicitDomain), Dali::String("IDS_MESSAGE"), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 2u, explicitDomain), Dali::String("IDS_PLURAL_MESSAGE"), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 5u, explicitDomain), Dali::String("IDS_PLURAL_MESSAGE"), TEST_LOCATION);

  CleanupManager(manager);
  END_TEST;
}

int UtcDaliUiLocalizationManagerGetLocalizedPluralStringNonNullTerminatedP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  constexpr char resourceIdStorage[]       = "IDS_MESSAGE_TRAILING";
  constexpr char pluralResourceIdStorage[] = "IDS_PLURAL_MESSAGE_TRAILING";
  StringView     resourceId(resourceIdStorage, sizeof("IDS_MESSAGE") - 1u);
  StringView     pluralResourceId(pluralResourceIdStorage, sizeof("IDS_PLURAL_MESSAGE") - 1u);

  Dali::String result = manager.GetLocalizedPluralString(resourceId,
                                                         pluralResourceId,
                                                         2u,
                                                         "dali-ui-plural-test-missing-domain");
  DALI_TEST_EQUALS(result, Dali::String("IDS_PLURAL_MESSAGE"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiLocalizationManagerGetLocalizedPluralStringEmptyP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("", "IDS_PLURAL_MESSAGE", 2u), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "", 2u), Dali::String(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiLocalizationManagerGetLocalizedPluralStringBypassP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);
  manager.SetDefaultDomain("dali-ui-plural-test-missing-domain");
  manager.SetBypassEnabled(true);

  // Bypass returns the primary resource id without selecting a plural form.
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 1u), Dali::String("IDS_MESSAGE"), TEST_LOCATION);
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 5u), Dali::String("IDS_MESSAGE"), TEST_LOCATION);

  CleanupManager(manager);
  END_TEST;
}

int UtcDaliUiLocalizationManagerGetLocalizedPluralStringDoesNotUseOverrideP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);
  manager.SetDefaultDomain("dali-ui-plural-test-missing-domain");
  manager.SetLocalizedStringOverride(OverrideAll);

  // The existing override cannot receive pluralResourceId or quantity.
  DALI_TEST_EQUALS(manager.GetLocalizedPluralString("IDS_MESSAGE", "IDS_PLURAL_MESSAGE", 2u), Dali::String("IDS_PLURAL_MESSAGE"), TEST_LOCATION);

  CleanupManager(manager);
  END_TEST;
}

// === Bypass ===

int UtcDaliUiLocalizationManagerBypassP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  // Set override that would return "Overridden"
  manager.SetLocalizedStringOverride(OverrideAll);

  // Without bypass, override should work
  Dali::String result = manager.GetLocalizedString("IDS_TEST");
  DALI_TEST_EQUALS(result, Dali::String("Overridden"), TEST_LOCATION);

  // Enable bypass -> resourceId returned as-is, override ignored
  manager.SetBypassEnabled(true);
  result = manager.GetLocalizedString("IDS_TEST");
  DALI_TEST_EQUALS(result, Dali::String("IDS_TEST"), TEST_LOCATION);

  // Disable bypass -> override works again
  manager.SetBypassEnabled(false);
  result = manager.GetLocalizedString("IDS_TEST");
  DALI_TEST_EQUALS(result, Dali::String("Overridden"), TEST_LOCATION);

  // Cleanup
  CleanupManager(manager);

  END_TEST;
}

// === Override ===

int UtcDaliUiLocalizationManagerOverrideP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  // Set override that handles "IDS_TEST"
  manager.SetLocalizedStringOverride(OverrideSpecificResourceId);

  // "IDS_TEST" should be overridden
  Dali::String result = manager.GetLocalizedString("IDS_TEST");
  DALI_TEST_EQUALS(result, Dali::String("Localized Test"), TEST_LOCATION);

  // "IDS_OTHER" should fall through (override returns false)
  result = manager.GetLocalizedString("IDS_OTHER");
  // No domain -> resourceId returned
  DALI_TEST_EQUALS(result, Dali::String("IDS_OTHER"), TEST_LOCATION);

  // Clear override -> fallback
  manager.ClearLocalizedStringOverride();
  result = manager.GetLocalizedString("IDS_TEST");
  DALI_TEST_EQUALS(result, Dali::String("IDS_TEST"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiLocalizationManagerOverrideFallbackP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  // Override that always returns false -> dgettext fallback
  manager.SetLocalizedStringOverride(OverrideAlwaysFalse);

  // No domain -> resourceId returned
  Dali::String result = manager.GetLocalizedString("IDS_TEST");
  DALI_TEST_EQUALS(result, Dali::String("IDS_TEST"), TEST_LOCATION);

  // Cleanup
  CleanupManager(manager);

  END_TEST;
}

int UtcDaliUiLocalizationManagerOverrideNullptrP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  // Set override
  manager.SetLocalizedStringOverride(OverrideAll);
  Dali::String result = manager.GetLocalizedString("IDS_TEST");
  DALI_TEST_EQUALS(result, Dali::String("Overridden"), TEST_LOCATION);

  // Pass nullptr to clear
  manager.SetLocalizedStringOverride(nullptr);
  result = manager.GetLocalizedString("IDS_TEST");
  DALI_TEST_EQUALS(result, Dali::String("IDS_TEST"), TEST_LOCATION);

  END_TEST;
}

// === SetBindingResource ===

int UtcDaliUiLocalizationManagerSetBindingResourceP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();
  gApplyCallCount    = 0;
  gLastAppliedString = Dali::String();

  // Set override to provide predictable results
  manager.SetLocalizedStringOverride(OverrideSpecificResourceId);

  // SetBindingResource should immediately invoke the callback
  manager.SetBindingResource(view, "Text", "IDS_TEST",
                              LocalizedStringCallback::New(TestApplyFunc));

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("Localized Test"), TEST_LOCATION);
  DALI_TEST_CHECK(gLastAppliedTarget == view);
  DALI_TEST_CHECK(manager.HasBinding(view, "Text"));

  // Cleanup
  CleanupManager(manager, view);

  END_TEST;
}

// === SetBindingResource Explicit Domain ===

int UtcDaliUiLocalizationManagerSetBindingResourceExplicitDomainP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();
  gApplyCallCount    = 0;
  gLastAppliedString = Dali::String();

  // Set override that checks domain
  manager.SetLocalizedStringOverride(OverrideByDomain);

  // SetBindingResource with explicit domain "domainA"
  manager.SetBindingResource(view, "Text", "IDS_TEST", "domainA",
                              LocalizedStringCallback::New(TestApplyFunc));

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("DomainA Result"), TEST_LOCATION);
  DALI_TEST_CHECK(gLastAppliedTarget == view);

  // Change default domain to "domainB" -> explicit domain should still be "domainA"
  gApplyCallCount = 0;
  manager.SetDefaultDomain("domainB");

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("DomainA Result"), TEST_LOCATION);
  DALI_TEST_CHECK(gLastAppliedTarget == view);

  // Cleanup
  CleanupManager(manager, view);

  END_TEST;
}

// === SetBindingResource Update ResourceId ===

int UtcDaliUiLocalizationManagerSetBindingResourceUpdateResourceIdP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();
  gApplyCallCount    = 0;
  gLastAppliedString = Dali::String();

  manager.SetLocalizedStringOverride(OverrideTestAndOtherResourceId);

  // Create initial binding
  manager.SetBindingResource(view, "Text", "IDS_TEST",
                              LocalizedStringCallback::New(TestApplyFunc));

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("Localized Test"), TEST_LOCATION);

  // Update with different resourceId - same target+bindingId replaces existing
  gApplyCallCount = 0;
  manager.SetBindingResource(view, "Text", "IDS_OTHER",
                              LocalizedStringCallback::New(TestApplyFunc));

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("Other Localized"), TEST_LOCATION);
  DALI_TEST_CHECK(manager.HasBinding(view, "Text"));

  // Cleanup
  CleanupManager(manager, view);

  END_TEST;
}

// === SetBindingResource Update Domain ===

int UtcDaliUiLocalizationManagerSetBindingResourceUpdateDomainP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();
  gApplyCallCount    = 0;
  gLastAppliedString = Dali::String();

  manager.SetLocalizedStringOverride(OverrideByDomain);

  // Create binding with domainA
  manager.SetBindingResource(view, "Text", "IDS_TEST", "domainA",
                              LocalizedStringCallback::New(TestApplyFunc));

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("DomainA Result"), TEST_LOCATION);

  // Update domain to domainB using SetBindingResource again
  gApplyCallCount = 0;
  manager.SetBindingResource(view, "Text", "IDS_TEST", "domainB",
                              LocalizedStringCallback::New(TestApplyFunc));

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("DomainB Result"), TEST_LOCATION);

  // Cleanup
  CleanupManager(manager, view);

  END_TEST;
}

// === Binding Default Domain ===

int UtcDaliUiLocalizationManagerBindingDefaultDomainP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();
  gApplyCallCount    = 0;
  gLastAppliedString = Dali::String();

  // Set override that checks domain
  manager.SetLocalizedStringOverride(OverrideByDomain);

  // SetBindingResource with empty domain (uses default domain)
  manager.SetBindingResource(view, "Text", "IDS_TEST",
                              LocalizedStringCallback::New(TestApplyFunc));

  // No default domain set -> override gets empty domain -> returns false -> resourceId
  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("IDS_TEST"), TEST_LOCATION);

  // Set default domain to "domainA" and refresh
  gApplyCallCount    = 0;
  manager.SetDefaultDomain("domainA");

  // RefreshBindings should apply with domainA
  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("DomainA Result"), TEST_LOCATION);

  // Change default domain to "domainB" and refresh
  gApplyCallCount = 0;
  manager.SetDefaultDomain("domainB");

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("DomainB Result"), TEST_LOCATION);

  // Cleanup
  CleanupManager(manager, view);

  END_TEST;
}

// === HasBinding ===

int UtcDaliUiLocalizationManagerHasBindingP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();

  // No binding initially
  DALI_TEST_CHECK(!manager.HasBinding(view, "Text"));

  // Set override for predictable results
  manager.SetLocalizedStringOverride(OverrideSpecificResourceId);

  // Create binding
  manager.SetBindingResource(view, "Text", "IDS_TEST",
                              LocalizedStringCallback::New(TestApplyFunc));

  // Verify HasBinding
  DALI_TEST_CHECK(manager.HasBinding(view, "Text"));

  // Clear binding
  manager.ClearBinding(view, "Text");
  DALI_TEST_CHECK(!manager.HasBinding(view, "Text"));

  // Cleanup
  CleanupManager(manager, view);

  END_TEST;
}

// === ClearBinding ===

int UtcDaliUiLocalizationManagerClearBindingP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();
  gApplyCallCount    = 0;
  gLastAppliedString = Dali::String();

  manager.SetLocalizedStringOverride(OverrideSpecificResourceId);

  manager.SetBindingResource(view, "Text", "IDS_TEST",
                              LocalizedStringCallback::New(TestApplyFunc));

  DALI_TEST_CHECK(manager.HasBinding(view, "Text"));

  // Clear binding
  manager.ClearBinding(view, "Text");
  DALI_TEST_CHECK(!manager.HasBinding(view, "Text"));

  // RefreshBindings should not invoke callback for cleared binding
  gApplyCallCount = 0;
  manager.RefreshBindings();
  DALI_TEST_EQUALS(gApplyCallCount, 0, TEST_LOCATION);

  // Cleanup
  CleanupManager(manager, view);

  END_TEST;
}

// === ClearBindings ===

int UtcDaliUiLocalizationManagerClearBindingsP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();

  // Set override for predictable results
  manager.SetLocalizedStringOverride(OverrideSpecificResourceId);

  // Register multiple bindings
  manager.SetBindingResource(view, "Text", "IDS_TEST",
                              LocalizedStringCallback::New(TestApplyFunc));
  manager.SetBindingResource(view, "ImageUrl", "IDS_IMAGE",
                              LocalizedStringCallback::New(TestApplyFunc2));

  DALI_TEST_CHECK(manager.HasBinding(view, "Text"));
  DALI_TEST_CHECK(manager.HasBinding(view, "ImageUrl"));

  // Clear all bindings
  manager.ClearBindings(view);

  DALI_TEST_CHECK(!manager.HasBinding(view, "Text"));
  DALI_TEST_CHECK(!manager.HasBinding(view, "ImageUrl"));

  // RefreshBindings should not invoke any callback
  gApplyCallCount = 0;
  manager.RefreshBindings();
  DALI_TEST_EQUALS(gApplyCallCount, 0, TEST_LOCATION);

  // Cleanup
  CleanupManager(manager);

  END_TEST;
}

// === Reentrant Refresh ===

int UtcDaliUiLocalizationManagerReentrantRefreshP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();
  gApplyCallCount     = 0;
  gLastAppliedString  = Dali::String();
  gReentrantTriggered = false;
  gReentrantManager   = &manager;

  // Set override for predictable results.
  manager.SetLocalizedStringOverride(OverrideSpecificResourceId);

  // SetBindingResource invokes the callback immediately.
  // ReentrantApplyFunc calls SetBindingResource("IDS_REENTRANT") from inside
  // the callback. This should not recursively apply during the same callback,
  // but it should schedule a pending refresh after the initial apply completes.
  manager.SetBindingResource(view,
                              "Text",
                              "IDS_TEST",
                              LocalizedStringCallback::New(ReentrantApplyFunc));

  // Initial apply + pending refresh apply.
  // 1st callback: IDS_TEST -> "Localized Test"
  // 2nd callback: IDS_REENTRANT -> fallback "IDS_REENTRANT"
  DALI_TEST_EQUALS(gApplyCallCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("IDS_REENTRANT"), TEST_LOCATION);

  // A normal explicit refresh should now apply the current resourceId once.
  gApplyCallCount    = 0;
  gLastAppliedString = Dali::String();

  manager.RefreshBindings();

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("IDS_REENTRANT"), TEST_LOCATION);

  // Cleanup.
  gReentrantManager   = nullptr;
  gReentrantTriggered = false;
  CleanupManager(manager, view);

  END_TEST;
}

int UtcDaliUiLocalizationManagerReentrantClearP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();
  gApplyCallCount    = 0;
  gLastAppliedString = Dali::String();
  gReentrantTriggered = false;
  gReentrantManager   = &manager;

  // Set override for predictable results
  manager.SetLocalizedStringOverride(OverrideSpecificResourceId);

  // SetBindingResource with a callback that calls ClearBinding from within
  manager.SetBindingResource(view, "Text", "IDS_TEST",
                              LocalizedStringCallback::New(ReentrantClearFunc));

  // First apply should have happened
  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);

  // No crash occurred
  DALI_TEST_CHECK(true);

  // The binding should have been cleared by the reentrant ClearBinding
  DALI_TEST_CHECK(!manager.HasBinding(view, "Text"));

  // Cleanup
  gReentrantManager  = nullptr;
  gReentrantTriggered = false;
  CleanupManager(manager);

  END_TEST;
}

// === Bypass with bindings ===

int UtcDaliUiLocalizationManagerBypassBindingP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  View view = View::New();
  gApplyCallCount    = 0;
  gLastAppliedString = Dali::String();

  // Set override
  manager.SetLocalizedStringOverride(OverrideSpecificResourceId);

  // Create binding
  manager.SetBindingResource(view, "Text", "IDS_TEST",
                              LocalizedStringCallback::New(TestApplyFunc));

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("Localized Test"), TEST_LOCATION);
  DALI_TEST_CHECK(gLastAppliedTarget == view);

  // Enable bypass -> RefreshBindings should apply resourceId as-is
  gApplyCallCount = 0;
  manager.SetBypassEnabled(true);

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("IDS_TEST"), TEST_LOCATION);
  DALI_TEST_CHECK(gLastAppliedTarget == view);

  // Disable bypass -> RefreshBindings should apply override result again
  gApplyCallCount = 0;
  manager.SetBypassEnabled(false);

  DALI_TEST_EQUALS(gApplyCallCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gLastAppliedString, Dali::String("Localized Test"), TEST_LOCATION);
  DALI_TEST_CHECK(gLastAppliedTarget == view);

  // Cleanup
  CleanupManager(manager, view);

  END_TEST;
}

// === IsBypassEnabled ===

int UtcDaliUiLocalizationManagerIsBypassEnabledP(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  DALI_TEST_CHECK(!manager.IsBypassEnabled());

  manager.SetBypassEnabled(true);
  DALI_TEST_CHECK(manager.IsBypassEnabled());

  manager.SetBypassEnabled(false);
  DALI_TEST_CHECK(!manager.IsBypassEnabled());

  END_TEST;
}

// === RegisterDomain with empty args ===

int UtcDaliUiLocalizationManagerRegisterDomainEmptyN(void)
{
  UiTestApplication application;

  UiLocalizationManager manager = UiLocalizationManager::Get();
  CleanupManager(manager);

  // Empty domain should fail
  DALI_TEST_CHECK(!manager.RegisterDomain("", "/some/path"));

  // Empty locale path should fail
  DALI_TEST_CHECK(!manager.RegisterDomain("myapp", ""));

  // Both empty should fail
  DALI_TEST_CHECK(!manager.RegisterDomain("", ""));

  END_TEST;
}
