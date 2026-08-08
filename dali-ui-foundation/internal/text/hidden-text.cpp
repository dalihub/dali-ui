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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/hidden-text.h>

// EXTERNAL INCLUDES
#include <algorithm>

namespace Dali
{
namespace Ui
{
namespace Text
{

HiddenText::HiddenText(Observer* observer)
: mObserver(observer),
  mMode(Mode::NONE),
  mPasswordMaskCharacter(DEFAULT_PASSWORD_MASK_CHARACTER),
  mPasswordRevealDuration(DEFAULT_PASSWORD_REVEAL_DURATION),
  mSubstituteCount(0u),
  mPreviousTextCount(0u),
  mIsLastCharacterRevealed(false)
{
  mTimer = Timer::New(mPasswordRevealDuration);
  mTimer.TickSignal().Connect(this, &HiddenText::OnTick);
}

void HiddenText::SetPasswordMode(PasswordMode mode)
{
  mMode = ToHiddenTextMode(mode);
}

PasswordMode HiddenText::GetPasswordMode() const
{
  return ToPasswordMode(mMode);
}

void HiddenText::SetPasswordMaskCharacter(uint32_t character)
{
  mPasswordMaskCharacter = character;
}

uint32_t HiddenText::GetPasswordMaskCharacter() const
{
  return mPasswordMaskCharacter;
}

void HiddenText::SetPasswordRevealDuration(int duration)
{
  mPasswordRevealDuration = duration;
}

int HiddenText::GetPasswordRevealDuration() const
{
  return mPasswordRevealDuration;
}

void HiddenText::ClearHiddenText()
{
  mMode = Mode::NONE;
}

void HiddenText::HideAll()
{
  mMode = Mode::HIDE_ALL;
}

void HiddenText::HideFirstCharacters(uint32_t count)
{
  mMode            = Mode::HIDE_COUNT;
  mSubstituteCount = count;
}

void HiddenText::ShowFirstCharacters(uint32_t count)
{
  mMode            = Mode::SHOW_COUNT;
  mSubstituteCount = count;
}

void HiddenText::RevealLastCharacter()
{
  mMode = Mode::REVEAL_LAST_CHARACTER;
}

HiddenText::Mode HiddenText::GetMode() const
{
  return mMode;
}

uint32_t HiddenText::GetSubstituteCount() const
{
  return mSubstituteCount;
}

void HiddenText::Substitute(const Vector<Character>& source, Vector<Character>& destination, Length cursorPos)
{
  const Length characterCount = static_cast<Dali::Ui::Text::Length>(source.Count());

  destination.Resize(characterCount);

  uint32_t* begin     = destination.Begin();
  uint32_t* end       = begin + characterCount;
  uint32_t* hideStart = nullptr;
  uint32_t* hideEnd   = nullptr;
  uint32_t* sourcePos = source.Begin();

  switch(mMode)
  {
    case Mode::NONE:
    {
      hideStart = nullptr;
      hideEnd   = nullptr;
      break;
    }

    case Mode::HIDE_ALL:
    {
      hideStart = begin;
      hideEnd   = end;
      break;
    }

    case Mode::HIDE_COUNT:
    {
      const Length count = std::min<Length>(static_cast<Length>(mSubstituteCount), characterCount);
      hideStart          = begin;
      hideEnd            = begin + count;
      break;
    }

    case Mode::SHOW_COUNT:
    {
      const Length count = std::min<Length>(static_cast<Length>(mSubstituteCount), characterCount);
      hideStart          = begin + count;
      hideEnd            = end;
      break;
    }

    case Mode::REVEAL_LAST_CHARACTER:
    {
      hideStart = begin;
      hideEnd   = end;

      if(mPreviousTextCount < characterCount)
      {
        if(mPasswordRevealDuration > 0)
        {
          mTimer.SetInterval(mPasswordRevealDuration);
          mTimer.Start();
          mIsLastCharacterRevealed = true;
        }
        else
        {
          OnTick();
        }
      }
      else
      {
        mIsLastCharacterRevealed = false;
      }
      break;
    }
  }

  if(mMode == Mode::REVEAL_LAST_CHARACTER)
  {
    Length currentPos = 0u;

    for(; begin < end; ++begin)
    {
      if(begin >= hideStart && begin < hideEnd && cursorPos > 0u && currentPos != cursorPos - 1u)
      {
        *begin = mPasswordMaskCharacter;
      }
      else
      {
        *begin = mIsLastCharacterRevealed ? *sourcePos : mPasswordMaskCharacter;
      }

      sourcePos++;
      currentPos++;
    }
  }
  else
  {
    for(; begin < end; ++begin)
    {
      if(begin >= hideStart && begin < hideEnd)
      {
        *begin = mPasswordMaskCharacter;
        sourcePos++;
      }
      else
      {
        *begin = *sourcePos++;
      }
    }
  }

  mPreviousTextCount = characterCount;
}

void HiddenText::InitPreviousTextCount()
{
  mPreviousTextCount = 0u;
}

bool HiddenText::OnTick()
{
  if(mObserver != nullptr)
  {
    mObserver->DisplayTimeExpired();
  }

  mIsLastCharacterRevealed = false;
  return false;
}

HiddenText::Mode HiddenText::ToHiddenTextMode(PasswordMode mode) const
{
  switch(mode)
  {
    case PasswordMode::NONE:
    {
      return Mode::NONE;
    }

    case PasswordMode::HIDE_ALL:
    {
      return Mode::HIDE_ALL;
    }

    case PasswordMode::REVEAL_LAST_CHARACTER:
    {
      return Mode::REVEAL_LAST_CHARACTER;
    }
  }

  return Mode::NONE;
}

PasswordMode HiddenText::ToPasswordMode(Mode mode) const
{
  switch(mode)
  {
    case Mode::NONE:
    {
      return PasswordMode::NONE;
    }

    case Mode::HIDE_ALL:
    {
      return PasswordMode::HIDE_ALL;
    }

    case Mode::REVEAL_LAST_CHARACTER:
    {
      return PasswordMode::REVEAL_LAST_CHARACTER;
    }

    case Mode::HIDE_COUNT:
    case Mode::SHOW_COUNT:
    {
      // These modes are preserved internally, but cannot be represented by
      // the public PasswordMode enum.
      return PasswordMode::NONE;
    }
  }

  return PasswordMode::NONE;
}

} // namespace Text
} // namespace Ui
} // namespace Dali
