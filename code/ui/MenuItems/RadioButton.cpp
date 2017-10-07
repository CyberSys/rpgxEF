#include "RadioButton.h"
#include "../Menus/MenuBase.h"
#include "../../common_game/FlagSupport.h"
#include "../ui_local.h"


using namespace Common;

namespace UI::MenuItems
{
  void RadioButton::Draw()
  {
    float* Color;
    int32_t Style;
    auto Focus = m_Parent->Cursor() == m_MenuPosition;

    if(IsSet(m_Flags, QMF_GRAYED))
    {
      Color = text_color_disabled;
      Style = UI_LEFT | UI_SMALLFONT;
    }
    else if(Focus)
    {
      Color = text_color_highlight;
      Style = UI_LEFT | UI_PULSE | UI_SMALLFONT;
    }
    else
    {
      Color = text_color_normal;
      Style = UI_LEFT | UI_SMALLFONT;
    }

    if(Focus)
    {
      FillRect({ m_Rect.Left(), m_Rect.Top(), m_Rect.Right() - m_Rect.Left() + 1, m_Rect.Bottom() - m_Rect.Top() + 1 }, listbar_color);
      DrawChar(m_Position, 13, UI_CENTER | UI_BLINK | UI_SMALLFONT, Color);
    }

    if(!m_Name.empty())
      DrawString({ X() - SMALLCHAR_WIDTH, Y() }, m_Name, UI_RIGHT | UI_SMALLFONT, Color, true);

    if(!m_CurrentValue)
    {
      DrawPicture({ X() + SMALLCHAR_WIDTH, Y() + 2 , 16, 16 }, uis.rb_off);
      DrawString({ X() + SMALLCHAR_WIDTH + 16, Y() }, "off", Style, Color, true);
    }
    else
    {
      DrawPicture({ X() + SMALLCHAR_WIDTH, Y() + 2, 16, 16 }, uis.rb_on);
      DrawString({ X() + SMALLCHAR_WIDTH + 16, Y() }, "on", Style, Color, true);
    }
  }

  void RadioButton::Init()
  {
    auto Len = !m_Name.empty() ? m_Name.length() : 0;

    m_Rect.Left() = X() - (Len + 1) * SMALLCHAR_WIDTH;
    m_Rect.Right() = X() + 6 * SMALLCHAR_WIDTH;
    m_Rect.Top() = Y();
    m_Rect.Bottom() = Y() + SMALLCHAR_HEIGHT;
  }

  sfxHandle_t RadioButton::Key(int32_t& Key)
  {
    switch(Key)
    {
    case K_MOUSE1:
      {
        if(!IsSet(m_Flags, QMF_HASMOUSEFOCUS))
          break;
      }
    case K_JOY1:
    case K_JOY2:
    case K_JOY3:
    case K_JOY4:
    case K_ENTER:
    case K_KP_ENTER:
    case K_KP_LEFTARROW:
    case K_LEFTARROW:
    case K_KP_RIGHTARROW:
    case K_RIGHTARROW:
      {
        m_CurrentValue = !m_CurrentValue;
        if(OnCallback)
          OnCallback(this, QM_ACTIVATED);

        return menu_move_sound;
      }
    default:
      break;
    }

    return 0;
  }
}
