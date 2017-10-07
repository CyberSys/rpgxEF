#include "Action.h"
#include "../../common_game/FlagSupport.h"
#include "../Menus/MenuBase.h"

using namespace Common;
using namespace BG;

namespace UI::MenuItems
{
  Action::Action(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const String& Name, const Color& Clr, const Color& ColorHighlight, const Color& ColorGreyed, TextInfo Text, StringID Text2, Size Size) : MenuItem{ EMenuItemType::Action, ID, Position, Flags, Parent, Name }
    , m_Color{ Clr }
    , m_ColorHighlight{ ColorHighlight }
    , m_ColorGreyed{ ColorGreyed }
    , m_Text{ Text }
    , m_Text2{ Text }
    , m_Size{ Size }
    , m_Updated{ false }
  {
    m_Text2.SetStringID(Text2);
  }

  void Action::Draw()
  {
    auto Style = 0;
    Color TextColor;
    Color ButtonColor;

    if(IsSet(m_Flags, QMF_GRAYED))
    {
      ButtonColor = m_ColorGreyed;
      TextColor = m_Text.ColorGreyed();
    }
    else if(IsSet(m_Flags, QMF_PULSEIFFOCUS) && m_Parent->Cursor() == m_MenuPosition)
    {
      ButtonColor = m_ColorHighlight;
      TextColor = m_Text.ColorHightlight();
      Style = UI_PULSE;
    }
    else if(IsSet(m_Flags, QMF_HIGHLIGHT_IF_FOCUS) && m_Parent->Cursor() == m_MenuPosition)
    {
      ButtonColor = m_ColorHighlight;
      TextColor = m_Text.ColorHightlight();
    }
    else if(IsSet(m_Flags, QMF_BLINK))
    {
      if(IsSet(uis.realtime / BLINK_DIVISOR, 1))
      {
        ButtonColor = m_Color;
        TextColor = m_Text.Color();
      }
      else
      {
        ButtonColor = m_ColorHighlight;
        TextColor = m_Text.ColorHightlight();
      }
      Style = UI_BLINK;
    }
    else
    {
      ButtonColor = m_ColorHighlight;
      TextColor = m_Text.ColorHightlight();
    }

    SetColor(ButtonColor);
    DrawPicture({ X(), Y(), m_Size.m_Width, m_Size.m_Height }, uis.whiteShader);

    if(m_Text)
      DrawProportionalString({ X() + m_Text.X(), Y() + m_Text.Y() }, m_Text, Style | UI_SMALLFONT, TextColor);

    if(m_Text2)
    {
      const auto IncY = PROP_HEIGHT * 1.15f;
      DrawProportionalString({ X() + m_Text.X(), Y() + m_Text.Y() + IncY }, m_Text2, Style, TextColor);
    }
  }

  void Action::Init()
  {
    auto ValX = X();
    const auto ValY = Y();
    const auto W = m_Size.m_Width;
    const auto H = m_Size.m_Height;

    if(IsSet(m_Flags, QMF_RIGHT_JUSTIFY))
      ValX = ValX - W;
    else if(IsSet(m_Flags, QMF_CENTER_JUSTIFY))
      ValX = ValX - W / 2;

    m_Rect.Left() = ValX;
    m_Rect.Right() = ValX + W;
    m_Rect.Top() = ValY;
    m_Rect.Bottom() = ValY + H;
  }
}
