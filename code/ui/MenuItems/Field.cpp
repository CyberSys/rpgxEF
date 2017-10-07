#include "Field.h"
#include "../../common_game/FlagSupport.h"

using namespace BG;
using namespace Common;

namespace UI::MenuItems
{
  Field::Field(MenuItemID ID, MenuPosition Position, size_t Flags, size_t MaxChars, size_t WidthInChars, int32_t Style, StringID TitleID, const Color& TitleColor, const Color& TextColor, const Color& TextColor2, Menus::MenuBase* Parent, const String& InitialValue, const String& Name)
    : MenuItem(EMenuItemType::Slider, ID, Position, Flags, Parent, Name)
    , m_MaxChars(MaxChars)
    , m_Style(Style)
    , m_WidthInChars(WidthInChars)
    , m_TitleID(TitleID)
    , m_TitleColor(TitleColor)
    , m_TextColor(TextColor)
    , m_TextColor2(TextColor2)
  {
    if(!InitialValue.empty())
    {
      if(m_MaxChars)
        m_Buffer = InitialValue.substr(0, m_MaxChars);
      m_Buffer.shrink_to_fit();
    }
  }

  void Field::Draw()
  {
    const float X = m_Position.X();
    const float Y = m_Position.Y();
    const auto Offset = m_Name.empty() ? 0 : MENU_BUTTON_MED_HEIGHT + 4;
    auto W = SMALLCHAR_WIDTH;
    auto Style = m_Style;
    int32_t Color;

    if(IsSet(m_Style, UI_TINYFONT))
      W = TINYCHAR_WIDTH;
    else if(IsSet(m_Style, UI_BIGFONT))
      W = BIGCHAR_WIDTH;
    else if(IsSet(m_Style, UI_GIANTFONT))
      W = GIANTCHAR_WIDTH;

    bool Focus;
    if(m_Parent->ItemAtCursor() == this)
    {
      Focus = true;
      Style |= UI_SHOWCOLOR;
    }
    else
      Focus = false;

    if(!m_Name.empty() && IsSet(m_Style, UI_CENTER))
    {
      const auto Width = MENU_BUTTON_MED_HEIGHT * 2 - 1 + m_Buffer.length() * W + 2 * W;
      if(Focus)
        SetColor(EColors::LightPurple1);
      else if(IsSet(m_Flags, QMF_GRAYED))
        SetColor(colorMdGrey);
      else
        SetColor(EColors::DarkPurple1);

      DrawPicture({ X - (Width >> 1), Y, 19.f, 19.f }, uis.graphicButtonLeftEnd);
      DrawPicture({ X + (Width >> 1) - 19.f, Y, -19.f, 19.f }, uis.graphicButtonLeftEnd);
      DrawPicture({ X - (Width >> 1) + (19.f - 8.f), Y, Width - (19.f * 2.f - 16.f), 19.f }, uis.whiteShader);

      Color = Focus ? CT_WHITE : CT_BLACK;

      DrawProportionalString({ static_cast<int32_t>(X), static_cast<int32_t>(Y) + 2 }, m_Name, UI_CENTER | UI_SMALLFONT, colorTable[Color]);
    }

    if(IsSet(m_Flags, QMF_GRAYED))
      Color = CT_DKGREY;
    else if(Focus)
      Color = m_TextColor2;
    else
      Color = m_TextColor;

    if(Focus)
    {
      FillRect({ m_Rect.Left(), m_Rect.Top() + Offset, m_Rect.Width(), m_Rect.Height() + 1 - Offset }, listbar_color);


      if(menu_button_text[m_TitleID][1])
      {
        DrawProportionalString(m_Parent->GetDescriptionPosition(), menu_button_text[m_TitleID][1], UI_LEFT | UI_TINYFONT, colorTable[CT_BLACK]);
      }
    }

    if(m_TitleID)
    {
      auto TitleColor = m_TitleColor ? m_TitleColor : EColors::Black;

      if(IsSet(m_Flags, QMF_GRAYED))
        TitleColor = EColors::DarkGrey;

      DrawProportionalString({ static_cast<int32_t>(X) - 5, static_cast<int32_t>(Y) }, menu_button_text[m_TitleID][0], UI_RIGHT | UI_SMALLFONT, colorTable[TitleColor]);
    }

    Draw(X, Y + Offset, Style, colorTable[Color], Focus ? 1 : 0);
  }

  void Field::Init()
  {
    int32_t Width;
    int32_t Height;

    m_Cursor = 0;
    m_Scroll = 0;

    if(IsSet(m_Style, UI_TINYFONT))
    {
      Width = TINYCHAR_WIDTH;
      Height = TINYCHAR_HEIGHT;
    }
    else if(IsSet(m_Style, UI_BIGFONT))
    {
      Width = BIGCHAR_WIDTH;
      Height = BIGCHAR_HEIGHT;
    }
    else if(IsSet(m_Style, UI_GIANTFONT))
    {
      Width = GIANTCHAR_WIDTH;
      Height = GIANTCHAR_HEIGHT;
    }
    else
    {
      Width = SMALLCHAR_WIDTH;
      Height = SMALLCHAR_HEIGHT;
    }

    if(IsSet(m_Style, UI_CENTER))
    {
      m_Rect.Right() = m_Position.X() + (Width + m_WidthInChars * Width) / 2;
      m_Rect.Bottom() = m_Position.Y() + Height;
      m_Rect.Left() = m_Position.X() - (m_Rect.Right() - m_Position.X());
      m_Rect.Top() = m_Position.Y();
    }
    else
    {
      m_Rect.Left() = m_Position.X();
      m_Rect.Top() = m_Position.Y();
      m_Rect.Right() = m_Position.X() + Width + m_WidthInChars * Width;
      m_Rect.Bottom() = m_Position.Y() + Height;
    }

    if(!m_Name.empty())
      m_Rect.Bottom() += MENU_BUTTON_MED_HEIGHT + 4;
  }

  void Field::Paste()
  {
    String Buffer(64, String::value_type());

    trap_GetClipboardData(Buffer.data(), 64);
    for(const auto& C : Buffer)
      CharEvent(C);
  }

  void Field::CharEvent(int32_t Char)
  {
    if(Char == 'v' - 'a' + 1)
    {
      Paste();
      return;
    }

    if(Char == 'c' - 'a' + 1)
    {
      Clear();
      return;
    }

    if(Char == 'h' - 'a' + 1) // backspace
    {
      if(m_Cursor > 0)
      {
        m_Buffer.erase(m_Cursor - 1);
        m_Cursor--;
        if(m_Cursor < m_Scroll)
          m_Scroll--;
      }
      return;
    }

    if(Char == 'a' - 'a' + 1) // home
    {
      m_Cursor = 0;
      m_Scroll = 0;
      return;
    }

    if(Char == 'e' - 'a' + 1) // end
    {
      m_Cursor = m_Buffer.length();
      m_Scroll = m_Cursor - m_WidthInChars + 1 >= 0 ? m_Cursor - m_WidthInChars + 1 : 0;
      return;
    }

    // ignore any other non printable chars
    if(Char < 32)
      return;

    if(!trap_Key_GetOverstrikeMode())
    {
      if(m_Cursor == MAX_EDIT_LINE - 1 || m_Cursor >= m_MaxChars)
        return;
    }
    else
    {
      if(m_Buffer.length() == MAX_EDIT_LINE - 1 || m_Buffer.length() >= m_MaxChars)
        return;
    }

    m_Buffer.insert(m_Cursor, ToString(static_cast<String::value_type>(Char)));
    if(m_Cursor < m_MaxChars)
      m_Cursor++;

    if(m_Cursor >= m_WidthInChars)
      m_Scroll++;

    if(m_Cursor == m_Buffer.length() + 1)
      m_Buffer[m_Cursor] = '\0';
  }

  void Field::KeyDownEvent(int32_t Key)
  {
    if((Key == K_INS || Key == K_KP_INS) && trap_Key_IsDown(K_SHIFT))
    {
      Paste();
      return;
    }

    const auto Len = m_Buffer.length();

    if(Key == K_DEL || Key == K_KP_DEL)
    {
      if(m_Cursor < Len)
        m_Buffer.erase(m_Cursor);
      return;
    }

    if(Key == K_RIGHTARROW || Key == K_KP_RIGHTARROW)
    {
      if(m_Cursor < Len)
        m_Cursor++;
      if(m_Cursor >= m_Scroll + m_WidthInChars && m_Cursor <= Len)
        m_Scroll++;
      return;
    }

    if(Key == K_LEFTARROW || Key == K_KP_LEFTARROW)
    {
      if(m_Cursor > 0)
        m_Cursor--;
      if(m_Cursor < m_Scroll)
        m_Scroll--;
      return;
    }

    if(Key == K_HOME || Key == K_KP_HOME || tolower(Key) == 'a' && trap_Key_IsDown(K_CTRL))
    {
      m_Cursor = 0;
      m_Scroll = 0;
      return;
    }

    if(Key == K_END || Key == K_KP_END || _tolower(Key) == 'a' && trap_Key_IsDown(K_CTRL))
    {
      m_Cursor = Len;
      m_Scroll = Len - m_WidthInChars + 1 >= 0 ? Len - m_WidthInChars + 1 : 0;
      return;
    }

    if(Key == K_INS || Key == K_KP_INS)
      trap_Key_SetOverstrikeMode(static_cast<qboolean>(!trap_Key_GetOverstrikeMode()));
  }

  sfxHandle_t Field::Key(int32_t& Key)
  {
    auto KeyCode = Key;

    switch(KeyCode)
    {
    case K_KP_ENTER:
    case K_ENTER:
    case K_JOY1:
    case K_JOY2:
    case K_JOY3:
    case K_JOY4:
      Key = K_TAB;
      break;

    case K_TAB:
    case K_KP_DOWNARROW:
    case K_DOWNARROW:
    case K_KP_UPARROW:
    case K_UPARROW:
      break;

    default:
      {
        if(IsSet(KeyCode, K_CHAR_FLAG))
        {
          KeyCode &= ~K_CHAR_FLAG;

          if(IsSet(m_Flags, QMF_UPPERCASE) && islower(KeyCode))
            KeyCode = 'a' - 'A';
          else if(IsSet(m_Flags, QMF_LOWERCASE) && isupper(KeyCode))
            KeyCode = 'A' - 'a';
          else if(IsSet(m_Flags, QMF_NUMBERSONLY) && isalpha(KeyCode))
            return menu_buzz_sound;

          CharEvent(KeyCode);
        }
        else
          KeyDownEvent(KeyCode);

        if(OnCallback)
          OnCallback(this, QM_ACTIVATED);

        break;
      }
    }

    return 0;
  }

  void Field::Clear()
  {
    m_Buffer.clear();
    m_Cursor = 0;
    m_Scroll = 0;
  }

  void Field::Draw(MenuPosition::value_type X, MenuPosition::value_type Y, int32_t Style, const Color& Color, int32_t Cursor)
  {
    int32_t PrepStep;
    auto DrawLen = m_WidthInChars;

    if(m_Buffer.length() < DrawLen)
      PrepStep = 0;
    else
    {
      if(m_Scroll + DrawLen > m_Buffer.length())
      {
        m_Scroll = m_Buffer.length() - DrawLen >= 0 ? m_Buffer.length() - DrawLen : 0;
      }
      PrepStep = m_Scroll;
    }

    if(PrepStep + DrawLen > m_Buffer.length())
      DrawLen = m_Buffer.length() - PrepStep;

    if(DrawLen >= MAX_STRING_CHARS)
      trap_Error("drawLen >= MAX_STRING_CHARS");
    auto Str = m_Buffer.substr(PrepStep, DrawLen);

    DrawString({ X, Y }, Str, Style, Color, true);

    if(!Cursor)
      return;

    const auto CursorChar = trap_Key_GetOverstrikeMode() ? 11 : 10;
    auto CharWidth = BIGCHAR_WIDTH;

    if(IsSet(Style, UI_SMALLFONT))
      CharWidth = SMALLCHAR_WIDTH;

    if(IsSet(Style, UI_GIANTFONT))
      CharWidth = GIANTCHAR_WIDTH;

    Str = Str.substr(0, m_Cursor - PrepStep);
    const auto Len = ProportionalStringWidth(Str.c_str(), Style);

    if(IsSet(Style, UI_CENTER))
      X = X + Len / 2;
    else if(IsSet(Style, UI_RIGHT))
      X = X - Str.length() * CharWidth;
    else
      X = X + Str.length();

    if(NotSet(uis.realtime / BLINK_DIVISOR, 1))
      DrawChar({ X, Y + 2 }, CursorChar, Style & ~(UI_CENTER | UI_RIGHT), Color);
  }
}
