#include "ScrollList.h"
#include "../../common_game/FlagSupport.h"
#include "../Menus/MenuBase.h"

using namespace Common;

namespace UI::MenuItems
{
  void ScrollList::Draw()
  {
    int32_t Style;
    float* Color;
    const auto Focus = m_Parent->Cursor() == m_MenuPosition;
    auto ValX = X();

    for(auto Column = 0; Column < m_Columns; Column++)
    {
      auto ValY = Y();
      const auto Base = m_Top + Column + m_Size.m_Height;
      for(auto i = Base; i < Base + m_Size.m_Height; i++)
      {
        if(i >= ItemCount())
          break;

        if(i == m_CurrentValue)
        {
          auto U = ValX - 2;
          if(IsSet(m_Flags, QMF_CENTER_JUSTIFY))
            U -= m_Size.m_Width * SMALLCHAR_HEIGHT / 2 + 1;

          FillRect({ U, ValY, m_Size.m_Width * SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT + 2 }, listbar_color);
          Color = text_color_highlight;

          Focus ? Style = UI_PULSE | UI_LEFT | UI_SMALLFONT : Style = UI_LEFT | UI_SMALLFONT;
        }
        else
        {
          Color = text_color_normal;
          Style = UI_LEFT | UI_SMALLFONT;
        }

        if(IsSet(m_Flags, QMF_CENTER_JUSTIFY))
          Style |= UI_CENTER;

        DrawString({ ValX, ValY }, m_ItemNames[i], Style, Color, false);

        ValY += SMALLCHAR_HEIGHT;
      }

      ValX += m_Size.m_Width + m_Separation * SMALLCHAR_WIDTH;
    }
  }

  void ScrollList::Init()
  {
    m_OldValue = 0;
    m_CurrentValue = 0;
    m_Top = 0;

    if(!m_Columns)
    {
      m_Columns = 1;
      m_Separation = 0;
    }
    else if(!m_Separation)
      m_Separation = 3;

    const auto W = ((m_Size.m_Width + m_Separation) * m_Columns - m_Separation) * SMALLCHAR_WIDTH;

    m_Rect.Left() = X();
    m_Rect.Top() = Y();
    m_Rect.Right() = X() + W;
    m_Rect.Bottom() = Y() + m_Size.m_Height * SMALLCHAR_HEIGHT;

    if(IsSet(m_Flags, QMF_CENTER_JUSTIFY))
    {
      m_Rect.Left() -= W / 2;
      m_Rect.Right() -= W / 2;
    }
  }

  sfxHandle_t ScrollList::Key(int32_t& Key)
  {
    switch(Key)
    {
    case K_MOUSE1:
      {
        if(IsSet(m_Flags, QMF_HASMOUSEFOCUS))
        {
          auto ValX = X();
          const auto ValY = Y();
          const auto W = ((m_Size.m_Width + m_Separation) * m_Columns - m_Separation) * SMALLCHAR_WIDTH;
          if(IsSet(m_Flags, QMF_CENTER_JUSTIFY))
            ValX -= W / 2;
          if(CursorInRect({ ValX, ValY, W, m_Size.m_Height * SMALLCHAR_HEIGHT }))
          {
            const auto CursorX = (uis.cursorx - ValX) / SMALLCHAR_WIDTH;
            const auto Column = CursorX / (m_Size.m_Width + m_Separation);
            const auto CursorY = (uis.cursory - ValY) / SMALLCHAR_HEIGHT;
            const auto Index = Column * m_Size.m_Height + CursorY;

            if(m_Top + Index < ItemCount())
            {
              m_OldValue = m_CurrentValue;
              m_CurrentValue = m_Top + Index;

              if(m_OldValue != m_CurrentValue && OnCallback)
              {
                OnCallback(this, QM_GOTFOCUS);
                return menu_move_sound;
              }
            }
          }

          return menu_null_sound;
        }
        break;
      }
    case K_KP_HOME:
    case K_HOME:
      {
        m_OldValue = m_CurrentValue;
        m_CurrentValue = 0;
        m_Top = 0;

        if(m_OldValue != m_CurrentValue && OnCallback)
        {
          OnCallback(this, QM_GOTFOCUS);
          return menu_move_sound;
        }

        return menu_buzz_sound;
      }
    case K_KP_END:
    case K_END:
      {
        m_OldValue = m_CurrentValue;
        m_CurrentValue = ItemCount() - 1;
        if(m_Columns > 1)
        {
          const auto C = (m_CurrentValue / m_Size.m_Height + 1) * m_Size.m_Height;
          m_Top = C - m_Columns * m_Size.m_Height;
        }
        else
          m_Top = m_CurrentValue - (m_Size.m_Height - 1);
        if(m_Top < 0)
          m_Top = 0;

        if(m_OldValue != m_CurrentValue && OnCallback)
        {
          OnCallback(this, QM_GOTFOCUS);
          return menu_move_sound;
        }

        return menu_buzz_sound;
      }
    case K_PGUP:
    case K_KP_PGUP:
      {
        if(m_Columns > 1)
          return menu_null_sound;

        if(m_CurrentValue > 0)
        {
          m_OldValue = m_CurrentValue;
          m_CurrentValue -= m_Size.m_Height - 1;
          if(m_CurrentValue < 0)
            m_CurrentValue = 0;
          m_Top = m_CurrentValue;
          if(m_Top < 0)
            m_Top = 0;

          if(OnCallback)
            OnCallback(this, QM_GOTFOCUS);

          return menu_move_sound;
        }

        return menu_buzz_sound;
      }
    case K_PGDN:
    case K_KP_PGDN:
      {
        if(m_Columns > 1)
          return menu_null_sound;

        if(m_CurrentValue < ItemCount() - 1)
        {
          m_OldValue = m_CurrentValue;
          m_CurrentValue += m_Size.m_Height - 1;
          if(m_CurrentValue > ItemCount() - 1)
            m_CurrentValue = ItemCount() - 1;
          m_Top = m_CurrentValue - (m_Size.m_Height - 1);
          if(m_Top < 0)
            m_Top = 0;

          if(OnCallback)
            OnCallback(this, QM_GOTFOCUS);

          return menu_move_sound;
        }

        return menu_buzz_sound;
      }
    case K_KP_UPARROW:
    case K_UPARROW:
      {
        if(m_CurrentValue == 0)
          return menu_buzz_sound;

        m_OldValue = m_CurrentValue;
        m_CurrentValue--;

        if(m_CurrentValue < m_Top)
          if(m_Columns == 1)
            m_Top--;
          else
            m_Top -= m_Size.m_Height;

        if(OnCallback)
          OnCallback(this, QM_GOTFOCUS);

        return menu_move_sound;
      }
    case K_KP_DOWNARROW:
    case K_DOWNARROW:
      {
        if(m_CurrentValue == ItemCount() - 1)
          return menu_buzz_sound;

        m_OldValue = m_CurrentValue;
        m_CurrentValue++;

        if(m_CurrentValue >= m_Top + m_Columns * m_Size.m_Height)
          if(m_Columns == 1)
            m_Top++;
          else
            m_Top += m_Size.m_Height;

        if(OnCallback)
          OnCallback(this, QM_GOTFOCUS);

        return menu_move_sound;
      }
    case K_KP_LEFTARROW:
    case K_LEFTARROW:
      {
        if(m_Columns == 1)
          return menu_null_sound;

        if(m_CurrentValue < m_Size.m_Height)
          return menu_buzz_sound;

        m_OldValue = m_CurrentValue;
        m_CurrentValue -= m_Size.m_Height;

        if(m_CurrentValue < m_Top)
          m_Top -= m_Size.m_Height;

        if(OnCallback)
          OnCallback(this, QM_GOTFOCUS);

        return menu_move_sound;
      }
    case K_KP_RIGHTARROW:
    case K_RIGHTARROW:
      {
        if(m_Columns == 1)
          return menu_null_sound;

        const auto C = m_CurrentValue + m_Size.m_Height;

        if(C >= ItemCount())
          return menu_buzz_sound;

        m_OldValue = m_CurrentValue;
        m_CurrentValue = C;

        if(m_CurrentValue > m_Top + m_Columns * m_Size.m_Height - 1)
          m_Top += m_Size.m_Height;

        if(OnCallback)
          OnCallback(this, QM_GOTFOCUS);

        return menu_move_sound;
      }
    default:
      break;
    }

    if(Q_isprint(Key))
      return 0;

    if(Q_isupper(Key))
      Key -= 'A' - 'a';

    for(auto i = 1; i <= ItemCount(); i++)
    {
      auto J = (m_CurrentValue + i) % ItemCount();
      auto C = m_ItemNames[J][0];
      if(Q_isupper(C))
        C -= 'A' - 'a';

      if(C == Key)
      {
        if(J < m_Top)
          m_Top = J;
        else if(J < m_Top + m_Size.m_Height - 1)
          m_Top = J + 1 - m_Size.m_Height;

        if(m_CurrentValue != J)
        {
          m_OldValue = m_CurrentValue;
          m_CurrentValue = J;

          if(OnCallback)
            OnCallback(this, QM_GOTFOCUS);

          return menu_move_sound;
        }

        return menu_buzz_sound;
      }
    }

    return menu_buzz_sound;
  }
}

