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

#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/devel-api/object/type-info.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_view_type_registry_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_view_type_registry_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
/// Every dali-ui view type registers under its *handle* name, with the *impl* class
/// name kept as an alternative lookup key (DALI_TYPE_REGISTRATION_BEGIN_FULL).
/// { handle name, impl name, expected base (handle) name }
struct TypeExpectation
{
  const char* handleName;
  const char* implName;
  const char* baseName;
};

const TypeExpectation EXPECTATIONS[] = {
  {"View", "ViewImpl", "CustomActor"},
  {"Layout", "LayoutImpl", "View"},
  {"StackLayout", "StackLayoutImpl", "Layout"},
  {"AbsoluteLayout", "AbsoluteLayoutImpl", "Layout"},
  {"FlexLayout", "FlexLayoutImpl", "Layout"},
  {"GridLayout", "GridLayoutImpl", "Layout"},
  {"ScrollBar", "ScrollBarImpl", "AbsoluteLayout"},
  {"ScrollView", "ScrollViewImpl", "View"},
  {"PageScrollView", "PageScrollViewImpl", "ScrollView"},
  {"RecyclerView", "RecyclerViewImpl", "View"},
  {"InteractiveView", "InteractiveViewImpl", "View"},
  {"SelectableView", "SelectableViewImpl", "InteractiveView"},
  {"GroupSelectableView", "GroupSelectableViewImpl", "SelectableView"},
  {"ImageView", "ImageViewImpl", "View"},
  {"AnimatedImageView", "AnimatedImageViewImpl", "View"},
  {"LottieAnimationView", "LottieAnimationViewImpl", "View"},
  {"VideoView", "VideoViewImpl", "View"},
  {"WebView", "WebViewImpl", "View"},
  {"Label", "LabelImpl", "View"},
  {"InputField", "InputFieldImpl", "View"},
  {"InputEditor", "InputEditorImpl", "View"},
  {"CanvasView", "CanvasViewImpl", "View"},
  {"TextAnchor", "TextAnchorImpl", "View"},
};

constexpr size_t EXPECTATION_COUNT = sizeof(EXPECTATIONS) / sizeof(EXPECTATIONS[0]);

/// Walks the registered base-name chain upwards, collecting each name.
bool ChainContains(const char* startName, const char* soughtName)
{
  TypeRegistry registry = TypeRegistry::Get();

  Dali::String current(startName);
  for(int depth = 0; depth < 16; ++depth) // guard against a malformed cycle
  {
    if(current == soughtName)
    {
      return true;
    }

    TypeInfo typeInfo = registry.GetTypeInfo(current);
    if(!typeInfo)
    {
      return false;
    }

    Dali::String base = typeInfo.GetBaseName();
    if(base.Empty() || base == current)
    {
      return false;
    }
    current = base;
  }
  return false;
}

} // namespace

// Every view type must be reachable by its public handle name, and report that
// name back. Before the handle/impl name unification the registry only knew the
// impl names ("ViewImpl", "ImageViewImpl", ...).
int UtcDaliViewTypeRegistryHandleNameP(void)
{
  UiTestApplication application;

  TypeRegistry registry = TypeRegistry::Get();
  DALI_TEST_CHECK(registry);

  for(size_t i = 0u; i < EXPECTATION_COUNT; ++i)
  {
    const TypeExpectation& expectation = EXPECTATIONS[i];

    tet_printf("Looking up handle name '%s'\n", expectation.handleName);

    TypeInfo typeInfo = registry.GetTypeInfo(Dali::String(expectation.handleName));
    DALI_TEST_CHECK(typeInfo);
    DALI_TEST_CHECK(typeInfo.GetName() == expectation.handleName);
  }

  END_TEST;
}

// The impl class name stays usable as an alternative key, so that existing
// registrations declaring e.g. typeid(ViewImpl) as their base keep resolving.
int UtcDaliViewTypeRegistryImplNameAliasP(void)
{
  UiTestApplication application;

  TypeRegistry registry = TypeRegistry::Get();

  for(size_t i = 0u; i < EXPECTATION_COUNT; ++i)
  {
    const TypeExpectation& expectation = EXPECTATIONS[i];

    tet_printf("Looking up impl name '%s'\n", expectation.implName);

    TypeInfo viaImpl = registry.GetTypeInfo(Dali::String(expectation.implName));
    DALI_TEST_CHECK(viaImpl);

    // The alias resolves to the very same TypeInfo, which still reports the handle name.
    DALI_TEST_CHECK(viaImpl.GetName() == expectation.handleName);

    TypeInfo viaHandle = registry.GetTypeInfo(Dali::String(expectation.handleName));
    DALI_TEST_CHECK(viaHandle == viaImpl);
  }

  END_TEST;
}

// The base name recorded for each type must describe the *handle* hierarchy,
// which is what scripts and bindings navigate.
int UtcDaliViewTypeRegistryBaseNameP(void)
{
  UiTestApplication application;

  TypeRegistry registry = TypeRegistry::Get();

  for(size_t i = 0u; i < EXPECTATION_COUNT; ++i)
  {
    const TypeExpectation& expectation = EXPECTATIONS[i];

    tet_printf("Checking base of '%s', expecting '%s'\n", expectation.handleName, expectation.baseName);

    TypeInfo typeInfo = registry.GetTypeInfo(Dali::String(expectation.handleName));
    DALI_TEST_CHECK(typeInfo);
    DALI_TEST_CHECK(typeInfo.GetBaseName() == expectation.baseName);
  }

  END_TEST;
}

// ScrollView and RecyclerView reuse LayoutImpl for their implementation but their
// handles derive from View, not Layout. The registry must follow the handles.
int UtcDaliViewTypeRegistryScrollViewBaseIsViewP(void)
{
  UiTestApplication application;

  TypeRegistry registry = TypeRegistry::Get();

  TypeInfo scrollView = registry.GetTypeInfo(Dali::String("ScrollView"));
  DALI_TEST_CHECK(scrollView);
  DALI_TEST_CHECK(scrollView.GetBaseName() == "View");

  TypeInfo recyclerView = registry.GetTypeInfo(Dali::String("RecyclerView"));
  DALI_TEST_CHECK(recyclerView);
  DALI_TEST_CHECK(recyclerView.GetBaseName() == "View");

  END_TEST;
}

// The whole chain must terminate at the dali-core types, so that Actor's default
// properties stay reachable from any dali-ui view.
int UtcDaliViewTypeRegistryChainReachesActorP(void)
{
  UiTestApplication application;

  for(size_t i = 0u; i < EXPECTATION_COUNT; ++i)
  {
    const TypeExpectation& expectation = EXPECTATIONS[i];

    tet_printf("Walking chain of '%s'\n", expectation.handleName);

    DALI_TEST_CHECK(ChainContains(expectation.handleName, "View"));
    DALI_TEST_CHECK(ChainContains(expectation.handleName, "CustomActor"));
    DALI_TEST_CHECK(ChainContains(expectation.handleName, "Actor"));
  }

  END_TEST;
}

// A live instance must report its handle name. This is what leaks out through
// BaseHandle::GetTypeName(), the AT-SPI "class" attribute and the feedback style
// lookup key, so it used to read "ImageViewImpl" instead of "ImageView".
int UtcDaliViewTypeRegistryInstanceTypeNameP(void)
{
  UiTestApplication application;

  View view = View::New();
  DALI_TEST_CHECK(view.GetTypeName() == "View");

  Layout layout = Layout::New();
  DALI_TEST_CHECK(layout.GetTypeName() == "Layout");

  StackLayout stackLayout = StackLayout::New();
  DALI_TEST_CHECK(stackLayout.GetTypeName() == "StackLayout");

  ImageView imageView = ImageView::New();
  DALI_TEST_CHECK(imageView.GetTypeName() == "ImageView");

  Label label = Label::New();
  DALI_TEST_CHECK(label.GetTypeName() == "Label");

  ScrollView scrollView = ScrollView::New();
  DALI_TEST_CHECK(scrollView.GetTypeName() == "ScrollView");

  InteractiveView interactiveView = InteractiveView::New();
  DALI_TEST_CHECK(interactiveView.GetTypeName() == "InteractiveView");

  END_TEST;
}

// BaseHandle::GetTypeInfo() must resolve to the same TypeInfo the registry hands out.
int UtcDaliViewTypeRegistryInstanceTypeInfoP(void)
{
  UiTestApplication application;

  ImageView imageView = ImageView::New();

  TypeInfo fromInstance;
  DALI_TEST_CHECK(imageView.GetTypeInfo(fromInstance));
  DALI_TEST_CHECK(fromInstance);

  TypeInfo fromRegistry = TypeRegistry::Get().GetTypeInfo(Dali::String("ImageView"));
  DALI_TEST_CHECK(fromRegistry);
  DALI_TEST_CHECK(fromInstance == fromRegistry);

  END_TEST;
}

// Property lookup by name must find both the type's own registered properties and
// the ones inherited through the registered base chain.
int UtcDaliViewTypeRegistryInheritedPropertyP(void)
{
  UiTestApplication application;

  ImageView imageView = ImageView::New();

  // ImageView's own registered property.
  Property::Index ownIndex = imageView.GetPropertyIndex(Property::Key("image"));
  DALI_TEST_CHECK(ownIndex != Property::INVALID_INDEX);

  // Inherited from View's registration.
  Property::Index inheritedIndex = imageView.GetPropertyIndex(Property::Key("background"));
  DALI_TEST_CHECK(inheritedIndex != Property::INVALID_INDEX);

  // Inherited from Actor's default properties, i.e. the chain reaches dali-core.
  Property::Index coreIndex = imageView.GetPropertyIndex(Property::Key("position"));
  DALI_TEST_CHECK(coreIndex != Property::INVALID_INDEX);

  END_TEST;
}

// Signals registered on View, and on dali-core's Actor, must be visible from a
// derived type's TypeInfo, i.e. the base chain is walked when enumerating.
int UtcDaliViewTypeRegistryInheritedSignalP(void)
{
  UiTestApplication application;

  TypeInfo imageViewTypeInfo = TypeRegistry::Get().GetTypeInfo(Dali::String("ImageView"));
  DALI_TEST_CHECK(imageViewTypeInfo);

  const size_t signalCount = imageViewTypeInfo.GetSignalCount();
  DALI_TEST_CHECK(signalCount > 0u);

  bool keyEventFound     = false; // registered on View
  bool focusChangedFound = false; // registered on View
  bool touchEventFound   = false; // registered on Actor, i.e. reached dali-core

  for(size_t i = 0u; i < signalCount; ++i)
  {
    const Dali::String name = imageViewTypeInfo.GetSignalName(i);
    if(name == "keyEvent")
    {
      keyEventFound = true;
    }
    else if(name == "focusChanged")
    {
      focusChangedFound = true;
    }
    else if(name == "touchEvent")
    {
      touchEventFound = true;
    }
  }

  DALI_TEST_CHECK(keyEventFound);
  DALI_TEST_CHECK(focusChangedFound);
  DALI_TEST_CHECK(touchEventFound);

  END_TEST;
}

// Same for actions.
int UtcDaliViewTypeRegistryInheritedActionP(void)
{
  UiTestApplication application;

  TypeInfo imageViewTypeInfo = TypeRegistry::Get().GetTypeInfo(Dali::String("ImageView"));
  DALI_TEST_CHECK(imageViewTypeInfo);

  const size_t actionCount = imageViewTypeInfo.GetActionCount();
  DALI_TEST_CHECK(actionCount > 0u);

  bool activateFound = false; // registered on View
  bool escapeFound   = false; // registered on View
  bool showFound     = false; // registered on Actor

  for(size_t i = 0u; i < actionCount; ++i)
  {
    const Dali::String name = imageViewTypeInfo.GetActionName(i);
    if(name == "activate")
    {
      activateFound = true;
    }
    else if(name == "escape")
    {
      escapeFound = true;
    }
    else if(name == "show")
    {
      showFound = true;
    }
  }

  DALI_TEST_CHECK(activateFound);
  DALI_TEST_CHECK(escapeFound);
  DALI_TEST_CHECK(showFound);

  END_TEST;
}

// Connecting an inherited signal by name must be accepted on a derived type.
int UtcDaliViewTypeRegistryConnectInheritedSignalP(void)
{
  UiTestApplication application;

  ImageView imageView = ImageView::New();

  ConnectionTracker tracker;
  bool              called = false;
  DALI_TEST_CHECK(imageView.ConnectSignal(&tracker, "keyEvent", [&called]() { called = true; }));

  // Nothing has been emitted yet; the point of this test is that the connection
  // was accepted through the inherited registration.
  DALI_TEST_CHECK(!called);

  END_TEST;
}

// Creating by name must work through the handle name and produce something the
// matching handle can down-cast.
int UtcDaliViewTypeRegistryCreateInstanceP(void)
{
  UiTestApplication application;

  TypeRegistry registry = TypeRegistry::Get();

  TypeInfo viewTypeInfo = registry.GetTypeInfo(Dali::String("View"));
  DALI_TEST_CHECK(viewTypeInfo);
  View createdView = View::DownCast(viewTypeInfo.CreateInstance());
  DALI_TEST_CHECK(createdView);

  TypeInfo layoutTypeInfo = registry.GetTypeInfo(Dali::String("Layout"));
  DALI_TEST_CHECK(layoutTypeInfo);
  Layout createdLayout = Layout::DownCast(layoutTypeInfo.CreateInstance());
  DALI_TEST_CHECK(createdLayout);

  TypeInfo imageViewTypeInfo = registry.GetTypeInfo(Dali::String("ImageView"));
  DALI_TEST_CHECK(imageViewTypeInfo);
  ImageView createdImageView = ImageView::DownCast(imageViewTypeInfo.CreateInstance());
  DALI_TEST_CHECK(createdImageView);

  TypeInfo interactiveTypeInfo = registry.GetTypeInfo(Dali::String("InteractiveView"));
  DALI_TEST_CHECK(interactiveTypeInfo);
  InteractiveView createdInteractive = InteractiveView::DownCast(interactiveTypeInfo.CreateInstance());
  DALI_TEST_CHECK(createdInteractive);

  END_TEST;
}

// Creating through the impl-name alias must yield the same kind of object.
int UtcDaliViewTypeRegistryCreateInstanceViaAliasP(void)
{
  UiTestApplication application;

  TypeInfo viaAlias = TypeRegistry::Get().GetTypeInfo(Dali::String("ImageViewImpl"));
  DALI_TEST_CHECK(viaAlias);

  ImageView created = ImageView::DownCast(viaAlias.CreateInstance());
  DALI_TEST_CHECK(created);
  DALI_TEST_CHECK(created.GetTypeName() == "ImageView");

  END_TEST;
}

// The impl name is an internal lookup key only: enumerating the registry must
// expose the handle names, not the impl names.
int UtcDaliViewTypeRegistryEnumerationExposesHandleNamesP(void)
{
  UiTestApplication application;

  TypeRegistry registry = TypeRegistry::Get();

  const size_t count = registry.GetTypeNameCount();
  DALI_TEST_CHECK(count > 0u);

  for(size_t i = 0u; i < EXPECTATION_COUNT; ++i)
  {
    const TypeExpectation& expectation = EXPECTATIONS[i];

    bool handleNameFound = false;
    bool implNameFound   = false;

    for(size_t index = 0u; index < count; ++index)
    {
      const Dali::String& name = registry.GetTypeName(index);
      if(name == expectation.handleName)
      {
        handleNameFound = true;
      }
      else if(name == expectation.implName)
      {
        implNameFound = true;
      }
    }

    tet_printf("Enumeration check for '%s'\n", expectation.handleName);
    DALI_TEST_CHECK(handleNameFound);
    DALI_TEST_CHECK(!implNameFound);
  }

  END_TEST;
}

// The animation spec types follow the same handle/impl naming.
int UtcDaliViewTypeRegistryAnimationSpecP(void)
{
  UiTestApplication application;

  TypeRegistry registry = TypeRegistry::Get();

  const TypeExpectation specs[] = {
    {"ViewAnimationSpec", "ViewAnimationSpecImpl", "BaseHandle"},
    {"LabelAnimationSpec", "LabelAnimationSpecImpl", "ViewAnimationSpec"},
    {"InputFieldAnimationSpec", "InputFieldAnimationSpecImpl", "ViewAnimationSpec"},
    {"InputEditorAnimationSpec", "InputEditorAnimationSpecImpl", "ViewAnimationSpec"},
  };

  for(size_t i = 0u; i < sizeof(specs) / sizeof(specs[0]); ++i)
  {
    tet_printf("Checking animation spec '%s'\n", specs[i].handleName);

    TypeInfo viaHandle = registry.GetTypeInfo(Dali::String(specs[i].handleName));
    DALI_TEST_CHECK(viaHandle);
    DALI_TEST_CHECK(viaHandle.GetName() == specs[i].handleName);
    DALI_TEST_CHECK(viaHandle.GetBaseName() == specs[i].baseName);

    TypeInfo viaImpl = registry.GetTypeInfo(Dali::String(specs[i].implName));
    DALI_TEST_CHECK(viaImpl);
    DALI_TEST_CHECK(viaImpl == viaHandle);
  }

  END_TEST;
}

// A name nobody registered must return an empty handle rather than assert.
int UtcDaliViewTypeRegistryUnknownNameN(void)
{
  UiTestApplication application;

  TypeRegistry registry = TypeRegistry::Get();

  DALI_TEST_CHECK(!registry.GetTypeInfo(Dali::String("ThereIsNoSuchUiType")));
  DALI_TEST_CHECK(!registry.GetTypeInfo(Dali::String("ViewImplImpl")));

  END_TEST;
}
