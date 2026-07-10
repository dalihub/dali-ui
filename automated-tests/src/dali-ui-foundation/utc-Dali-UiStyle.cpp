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
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/dali-ui-foundation-extension.h>
#include <dali/public-api/common/intrusive-ptr.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

class TestStyleImpl : public Extension::UiStyleImpl
{
public:
  explicit TestStyleImpl(int value)
  : mValue(value)
  {
  }

  int GetValue() const
  {
    return mValue;
  }

protected:
  ~TestStyleImpl() override = default;

private:
  int mValue;
};

class TestStyle : public UiStyle
{
public:
  TestStyle() = default;

  static TestStyle New(int value)
  {
    IntrusivePtr<TestStyleImpl> impl(new TestStyleImpl(value));
    return TestStyle(impl.Get());
  }

  static TestStyle DownCast(BaseHandle handle)
  {
    return TestStyle(dynamic_cast<TestStyleImpl*>(handle.GetObjectPtr()));
  }

  static TestStyle StaticDownCast(UiStyle style)
  {
    return TestStyle(static_cast<TestStyleImpl*>(style.GetObjectPtr()));
  }

  int GetValue() const
  {
    return static_cast<const TestStyleImpl&>(GetBaseObject()).GetValue();
  }

private:
  explicit TestStyle(TestStyleImpl* impl)
  : UiStyle(impl)
  {
  }
};

class OtherStyleImpl : public Extension::UiStyleImpl
{
protected:
  ~OtherStyleImpl() override = default;
};

class OtherStyle : public UiStyle
{
public:
  static OtherStyle New()
  {
    IntrusivePtr<OtherStyleImpl> impl(new OtherStyleImpl());
    return OtherStyle(impl.Get());
  }

private:
  explicit OtherStyle(OtherStyleImpl* impl)
  : UiStyle(impl)
  {
  }
};

int gPrimaryStyleCreateCount = 0;
int gEmptyStyleCreateCount   = 0;
UiStyleSheet gRecursiveStyleSheet;
UiStyleKey<> gRecursiveKey = UiStyleKey<>::Alloc();

UiStyle CreatePrimaryStyle()
{
  ++gPrimaryStyleCreateCount;
  return TestStyle::New(7);
}

UiStyle CreateOverrideStyle()
{
  return TestStyle::New(19);
}

UiStyle CreateEmptyStyle()
{
  ++gEmptyStyleCreateCount;
  return UiStyle();
}

UiStyle CreateWrongStyle()
{
  return OtherStyle::New();
}

UiStyle CreateRecursiveStyle()
{
  return gRecursiveStyleSheet.GetStyle(gRecursiveKey);
}

} // namespace

void utc_dali_ui_style_startup(void)
{
  gPrimaryStyleCreateCount = 0;
  gEmptyStyleCreateCount   = 0;
  gRecursiveStyleSheet     = UiStyleSheet();
  test_return_value = TET_UNDEF;
}

void utc_dali_ui_style_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliUiStyleKeyAllocP(void)
{
  UiStyleKey<> key1 = UiStyleKey<>::Alloc();
  UiStyleKey<> key2 = UiStyleKey<>::Alloc();

  DALI_TEST_CHECK(key1 != key2);
  END_TEST;
}

int UtcDaliUiStyleSheetSetStyleGetStyleP(void)
{
  UiStyleKey<> key     = UiStyleKey<>::Alloc();
  UiStyleKey<> missing = UiStyleKey<>::Alloc();

  UiStyleSheet styleSheet = UiStyleSheet::New();
  DALI_TEST_CHECK(styleSheet);
  DALI_TEST_CHECK(!styleSheet.IsFrozen());

  styleSheet.SetStyle(key, CreatePrimaryStyle);

  UiConfig config = UiConfig::New();
  config.ResetStyleSheet(styleSheet);
  config.Apply();

  TestStyle style = TestStyle::DownCast(styleSheet.GetStyle(key));
  TestStyle cachedStyle = TestStyle::DownCast(styleSheet.GetStyle(key));
  DALI_TEST_CHECK(style);
  DALI_TEST_CHECK(cachedStyle);
  DALI_TEST_EQUALS(style.GetValue(), 7, TEST_LOCATION);
  DALI_TEST_CHECK(style.GetObjectPtr() == cachedStyle.GetObjectPtr());
  DALI_TEST_EQUALS(gPrimaryStyleCreateCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!styleSheet.GetStyle(missing));
  END_TEST;
}

int UtcDaliUiStyleSheetEmptyCreatorMaterializedP(void)
{
  UiStyleKey<> key = UiStyleKey<>::Alloc();

  UiStyleSheet styleSheet = UiStyleSheet::New();
  styleSheet.SetStyle(key, CreateEmptyStyle);

  UiConfig config = UiConfig::New();
  config.ResetStyleSheet(styleSheet);
  config.Apply();

  DALI_TEST_CHECK(!styleSheet.GetStyle(key));
  DALI_TEST_CHECK(!styleSheet.GetStyle(key));
  DALI_TEST_EQUALS(gEmptyStyleCreateCount, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliUiStyleSheetWrongTypeCachedAsEmptyP(void)
{
  UiStyleKey<TestStyle> key = UiStyleKey<TestStyle>::Alloc();

  UiStyleSheet styleSheet = UiStyleSheet::New();
  styleSheet.SetStyle(key, CreateWrongStyle);

  UiConfig config = UiConfig::New();
  config.ResetStyleSheet(styleSheet);
  config.Apply();

  DALI_TEST_CHECK(!styleSheet.GetStyle(key));
  DALI_TEST_CHECK(!styleSheet.GetStyle(key));
  END_TEST;
}

int UtcDaliUiStyleSheetFreezeViaConfigP(void)
{
  UiStyleKey<> key = UiStyleKey<>::Alloc();

  UiStyleSheet styleSheet = UiStyleSheet::New();
  styleSheet.SetStyle(key, CreatePrimaryStyle);

  UiConfig config = UiConfig::New();
  config.ResetStyleSheet(styleSheet);
  config.Apply();

  DALI_TEST_CHECK(styleSheet.IsFrozen());
  DALI_TEST_ASSERTION(styleSheet.SetStyle(key, CreateOverrideStyle), "UiStyleSheet is frozen");

  TestStyle style = TestStyle::DownCast(styleSheet.GetStyle(key));
  DALI_TEST_CHECK(style);
  DALI_TEST_EQUALS(style.GetValue(), 7, TEST_LOCATION);
  END_TEST;
}

int UtcDaliUiStyleSheetRecursiveResolutionN(void)
{
  gRecursiveStyleSheet = UiStyleSheet::New();
  gRecursiveStyleSheet.SetStyle(gRecursiveKey, CreateRecursiveStyle);

  UiConfig config = UiConfig::New();
  config.ResetStyleSheet(gRecursiveStyleSheet);
  config.Apply();

  DALI_TEST_ASSERTION(gRecursiveStyleSheet.GetStyle(gRecursiveKey), "UiStyleSheet detected recursive style resolution");
  END_TEST;
}

int UtcDaliUiConfigStyleSheetP(void)
{
  UiStyleKey<> key = UiStyleKey<>::Alloc();

  UiStyleSheet styleSheet = UiStyleSheet::New();
  styleSheet.SetStyle(key, CreatePrimaryStyle);

  UiConfig config = UiConfig::New();
  config.ResetStyleSheet(styleSheet);
  config.Apply();

  DALI_TEST_CHECK(styleSheet.IsFrozen());

  TestStyle style = TestStyle::DownCast(UiConfig::GetCurrent().GetStyle(key));
  DALI_TEST_CHECK(style);
  DALI_TEST_EQUALS(style.GetValue(), 7, TEST_LOCATION);

  DALI_TEST_ASSERTION(styleSheet.SetStyle(key, CreateOverrideStyle), "UiStyleSheet is frozen");
  DALI_TEST_ASSERTION(config.ResetStyleSheet(styleSheet), "UiConfig is frozen after UiConfig::Apply()");
  END_TEST;
}

int UtcDaliUiConfigStyleSheetAccessorP(void)
{
  UiStyleKey<> key = UiStyleKey<>::Alloc();

  UiConfig config     = UiConfig::New();
  UiStyleSheet sheet1 = config.StyleSheet();
  UiStyleSheet sheet2 = config.StyleSheet();

  DALI_TEST_CHECK(sheet1);
  DALI_TEST_CHECK(sheet1.GetObjectPtr() == sheet2.GetObjectPtr());

  config.StyleSheet().SetStyle(key, CreatePrimaryStyle);
  config.Apply();

  DALI_TEST_CHECK(sheet1.IsFrozen());

  TestStyle style = TestStyle::DownCast(UiConfig::GetCurrent().GetStyle(key));
  DALI_TEST_CHECK(style);
  DALI_TEST_EQUALS(style.GetValue(), 7, TEST_LOCATION);
  END_TEST;
}

int UtcDaliUiConfigDefaultStyleSheetP(void)
{
  UiStyleKey<> key = UiStyleKey<>::Alloc();

  UiConfig config = UiConfig::New();
  config.Apply();

  DALI_TEST_CHECK(!UiConfig::GetCurrent().GetStyle(key));
  END_TEST;
}
