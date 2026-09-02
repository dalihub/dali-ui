/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include DALI_TEST_HEADER

#include <cstring>
#include <exception>
#include <iostream>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#include <dbghelp.h>
#endif

namespace
{
#if defined(_WIN32)
LONG WINAPI ReportUnhandledException(EXCEPTION_POINTERS* exceptionPointers)
{
  HANDLE process = GetCurrentProcess();
  SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
  SymInitialize(process, nullptr, TRUE);

  CONTEXT context = *exceptionPointers->ContextRecord;
  STACKFRAME64 frame{};
#if defined(_M_X64)
  DWORD machineType      = IMAGE_FILE_MACHINE_AMD64;
  frame.AddrPC.Offset    = context.Rip;
  frame.AddrFrame.Offset = context.Rbp;
  frame.AddrStack.Offset = context.Rsp;
#else
  DWORD machineType      = IMAGE_FILE_MACHINE_I386;
  frame.AddrPC.Offset    = context.Eip;
  frame.AddrFrame.Offset = context.Ebp;
  frame.AddrStack.Offset = context.Esp;
#endif
  frame.AddrPC.Mode = frame.AddrFrame.Mode = frame.AddrStack.Mode = AddrModeFlat;

  std::cerr << "Unhandled SEH exception 0x" << std::hex
            << exceptionPointers->ExceptionRecord->ExceptionCode << std::dec << '\n';
  for(unsigned int index = 0u; index < 64u && frame.AddrPC.Offset != 0u; ++index)
  {
    alignas(SYMBOL_INFO) char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME]{};
    auto* symbol         = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen   = MAX_SYM_NAME;
    DWORD64 displacement = 0u;
    std::cerr << "  #" << index << " 0x" << std::hex << frame.AddrPC.Offset << std::dec;
    if(SymFromAddr(process, frame.AddrPC.Offset, &displacement, symbol))
    {
      std::cerr << " " << symbol->Name << "+0x" << std::hex << displacement << std::dec;
      IMAGEHLP_LINE64 line{};
      line.SizeOfStruct      = sizeof(line);
      DWORD lineDisplacement = 0u;
      if(SymGetLineFromAddr64(process, frame.AddrPC.Offset, &lineDisplacement, &line))
      {
        std::cerr << " (" << line.FileName << ':' << line.LineNumber << ")";
      }
    }
    std::cerr << '\n';
    if(!StackWalk64(machineType, process, GetCurrentThread(), &frame, &context, nullptr,
                    SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
    {
      break;
    }
  }
  SymCleanup(process);
  return EXCEPTION_EXECUTE_HANDLER;
}
#endif

struct UnsupportedTest
{
  const char* name;
  const char* reason;
};

constexpr const char* WINDOWS_ACCESSIBILITY_REASON =
  "The Windows adaptor uses the dummy accessibility backend; Accessible::Get() is unavailable.";

constexpr const char* WINDOWS_PRIVATE_ACCESS_REASON =
  "This TC relies on Linux private/protected symbol interposition, which is incompatible with the MSVC ABI.";

constexpr const char* WINDOWS_OVERLAY_EFFECT_INTERNAL_REASON =
  "This TC requires a non-exported OverlayEffect internal method.";

constexpr UnsupportedTest WINDOWS_UNSUPPORTED_TESTS[] = {
  {"UtcDaliWindowsWarningCoverageNPatchP", WINDOWS_PRIVATE_ACCESS_REASON},
  {"UtcDaliWindowsWarningCoverageImageVisualsP", WINDOWS_PRIVATE_ACCESS_REASON},
  {"UtcDaliWindowsWarningCoverageAsyncTextP", WINDOWS_PRIVATE_ACCESS_REASON},
  {"UtcDaliWindowsWarningCoverageTextVisualMaxTextureP", WINDOWS_PRIVATE_ACCESS_REASON},
  {"UtcDaliInteractiveViewOverlayEffectRecoilRestoreReleaseWaitsForFinishP", WINDOWS_OVERLAY_EFFECT_INTERNAL_REASON},
  {"UtcDaliViewAccessibilityActiveDescendantNotificationP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliViewAccessibilityReadingInfoInternalP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliViewAccessibilityDirectApiDefaultsAndMetadataP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliViewAccessibilityReadingInfoAndLanguageSpansP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliViewAccessibilityTranslationAndRequestedVirtualsP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliViewAccessibilityRelationsActionsAndSignalsP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliViewAccessibilityHighlightCommandsP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliViewAccessibilityActionCallbacksP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliViewAccessibilityRequestCallbacksP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliViewAccessibilityExtensionNotificationsP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliAccessibilityTextControlsInterfacesInternalP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliAccessibilityTextUnicodeCharacterOffsetsInternalP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliAccessibilityTextUnicodeRangeGeometryInternalP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliAccessibilityTextRangeGeometryLayoutOffsetInternalP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliAccessibilityEditableTextToolkitCompatibilityInternalP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliAccessibilityEditableTextUnicodeCharacterOffsetsInternalP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliAccessibilityInputFieldPasswordRoleInternalP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliAccessibilityInputFieldPasswordClipboardProtectionInternalP", WINDOWS_ACCESSIBILITY_REASON},
  {"UtcDaliAccessibilityEditableTextReadOnlyInternalP", WINDOWS_ACCESSIBILITY_REASON},
};

const char* GetUnsupportedReason(const char* name)
{
  for(const auto& unsupported : WINDOWS_UNSUPPORTED_TESTS)
  {
    if(std::strcmp(name, unsupported.name) == 0)
    {
      return unsupported.reason;
    }
  }
  return nullptr;
}

testcase* FindTest(const std::string& name)
{
  for(testcase* test = DALI_TEST_ARRAY; test->name; ++test)
  {
    if(name == test->name)
    {
      return test;
    }
  }
  return nullptr;
}

int RunTest(testcase& test)
{
  int result = 1;
  if(test.startup)
  {
    test.startup();
  }
  try
  {
    result = test.function();
  }
  catch(const char*)
  {
    result = 1;
  }
  catch(const std::exception& exception)
  {
    std::cerr << "Unhandled exception: " << exception.what() << '\n';
    result = 1;
  }
  catch(...)
  {
    std::cerr << "Unknown unhandled exception\n";
    result = 1;
  }
  if(test.cleanup)
  {
    test.cleanup();
  }
  return result;
}
} // unnamed namespace

int main(int argc, char** argv)
{
#if defined(_WIN32)
  SetUnhandledExceptionFilter(ReportUnhandledException);
#endif
  if(argc == 2 && std::strcmp(argv[1], "--list") == 0)
  {
    for(testcase* test = DALI_TEST_ARRAY; test->name; ++test)
    {
      const char* reason = GetUnsupportedReason(test->name);
      if(reason)
      {
        std::cout << test->name << "|common,mock," DALI_TEST_SUITE ",windows-unsupported|" << reason << '\n';
      }
      else
      {
        std::cout << test->name << "|common,mock," DALI_TEST_SUITE "|Existing DALi UI common UTC.\n";
      }
    }
    return 0;
  }

  std::string testName;
  for(int index = 1; index < argc; ++index)
  {
    if(std::strcmp(argv[index], "--test") == 0 && index + 1 < argc)
    {
      testName = argv[++index];
    }
    else if(std::strcmp(argv[index], "--timeout-ms") == 0 && index + 1 < argc)
    {
      ++index;
    }
    else
    {
      std::cerr << "Unknown or incomplete argument: " << argv[index] << '\n';
      return 2;
    }
  }

  if(const char* reason = GetUnsupportedReason(testName.c_str()))
  {
    std::cout << "[  SKIPPED ] " << testName << ": " << reason << '\n';
    return 77;
  }

  testcase* test = FindTest(testName);
  if(!test)
  {
    std::cerr << "Unknown test: " << testName << '\n';
    return 2;
  }

  std::cout << "[ RUN      ] " << test->name << '\n';
  const int result = RunTest(*test);
  std::cout << (result == 0 ? "[       OK ] " : "[  FAILED  ] ") << test->name << '\n';
  return result;
}
