#include "Slider.h"
#include "../../common_game/FlagSupport.h"
#include "../Menus/MenuBase.h"

using namespace Common;
using namespace BG;

namespace UI::MenuItems
{
  void Slider::Draw()
  {
    Color TextColor = EColors::Red;
    Color ThumbColor = EColors::LightGrey;
    Color BackGroundColor = EColors::DarkGrey;
    auto Focus = false;
    Color Color;

    if(IsSet(m_Flags, QMF_GRAYED))
    {
      Color = EColors::DarkGrey;
    }
    else if(m_Parent->ItemAtCursor() == this)
    {
      Color = m_ColorHighlight;
      TextColor = m_Text.ColorHightlight();
      ThumbColor = m_Thumb.ColorHightlight();
      BackGroundColor = EColors::LightGrey;
      Focus = true;
    }
    else
    {
      Color = m_Color;
      TextColor = m_Text.Color();
      ThumbColor = m_Thumb.Color();
    }

    if(!trap_Key_IsDown(K_MOUSE1) && m_MouseDown)
    {
      m_MouseDown = false;
      m_Parent->SetNoNewSelecting(false);
    }

    if(m_MouseDown)
    {
      m_CurrentValue = (uis.cursorx - m_Position.X()) / static_cast<float>(m_FocusSize.m_Width) * m_Bounds.Distance() + m_Bounds.Min();
      m_CurrentValue = Com_Clamp(m_Bounds.Min(), m_Bounds.Max(), m_CurrentValue);
      m_Parent->SetNoNewSelecting(true);
    }

    if(m_Picture)
    {
      SetColor(Color);
      DrawPicture(m_Picture, m_Picture);
      ResetColor();
    }

    if(m_Text)
      DrawProportionalString({ m_Picture.X() + m_Text.X(), m_Picture.Y() + m_Text.Y() }, m_Text, UI_SMALLFONT, TextColor);

    auto Text2 = m_Text.GetText2();
    if(Focus && !Text2.empty())
      DrawProportionalString({ m_Parent->GetDescriptionPosition().X(), m_Parent->GetDescriptionPosition().Y() }, Text2, UI_LEFT | UI_TINYFONT, EColors::Black);

    if(m_Bounds.Max() > m_Bounds.Min())
    {
      m_Range = (m_CurrentValue - m_Bounds.Min()) / static_cast<float>(m_Bounds.Distance());
      if(m_Range < 0)
        m_Range = 0;
      else if(m_Range > 1)
        m_Range = 1;
    }
    else
      m_Range = 0;

    const auto ThumbX = m_Position.X() - m_Thumb.VisibileGraphicWidth() / 2 + m_FocusSize.m_Width * m_Range;
    if(m_Range)
    {
      SetColor(BackGroundColor);
      DrawPicture({ m_Position.X(), m_Position.Y(), ThumbX - m_Position.X() + 4.f, 18.f }, uis.whiteShader);
    }

    if(m_Graph)
    {
      SetColor(Color);
      DrawPicture({ m_Position.X(), m_Position.Y(), m_Graph.Width(), m_Graph.Height() }, m_Graph);
    }

    if(m_Thumb)
    {
      SetColor(ThumbColor);
      DrawPicture({ ThumbX, m_Position.Y(), m_Thumb.Width(), m_Thumb.Height() }, m_Thumb);
    }
  }

  void Slider::Init()
  {
    if(m_Picture.X())
    {
      m_Rect.Left() = m_Picture.X();
      m_Rect.Right() = m_Position.X() + m_FocusSize.m_Width;
      m_Rect.Top() = m_Picture.Y();
      m_Rect.Bottom() = m_Position.Y() + m_FocusSize.m_Height;
    }
    else
    {
      m_Rect.Left() = m_Position.X();
      m_Rect.Right() = m_Position.X() + m_FocusSize.m_Width;
      m_Rect.Top() = m_Position.Y();
      m_Rect.Bottom() = m_Position.Y() + m_FocusSize.m_Height;
    }

    if(!m_Thumb.Name().empty())
      m_Thumb.Init();

    if(!m_Picture.Name().empty())
      m_Picture.Init();

    if(!m_Graph.Name().empty())
      m_Graph.Init();
  }

  sfxHandle_t Slider::Key(int32_t& Key)
  {
    sfxHandle_t Sound;

    switch (Key)
    {
    case K_MOUSE1:
      {
        const auto XVal = uis.cursorx - X();
        const auto OldValue = m_CurrentValue;
        m_CurrentValue = XVal / static_cast<float>(m_FocusSize.m_Width) * m_Bounds.Distance() + m_Bounds.Min();

        if(XVal < 0 && m_DefaultValue)
        {
          m_CurrentValue = m_DefaultValue;
          Sound = menu_move_sound;
          break;
        }

        m_MouseDown = true;

        if(m_CurrentValue < m_Bounds.Min())
          m_CurrentValue = m_Bounds.Min();
        else if(m_CurrentValue > m_Bounds.Max())
          m_CurrentValue = m_Bounds.Max();

        Sound = m_CurrentValue != OldValue ? menu_move_sound : 0;
        break;
      }
    case K_KP_LEFTARROW:
    case K_LEFTARROW:
      {
        if(m_CurrentValue > m_Bounds.Min())
        {
          m_CurrentValue--;
          Sound = menu_move_sound;
        }
        else
          Sound = menu_buzz_sound;
        break;
      }
    case K_KP_RIGHTARROW:
    case K_RIGHTARROW:
      {
        if(m_CurrentValue < m_Bounds.Max())
        {
          m_CurrentValue++;
          Sound = menu_move_sound;
        }
        else
          Sound = menu_buzz_sound;
        break;
      }
    default:
      {
        Sound = 0;
        break;
      }
    }

    if(Sound && OnCallback)
      OnCallback(this, QM_ACTIVATED);

    return Sound;
  }
}
