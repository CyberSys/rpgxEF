#include "PText.h"
#include "../../common_game/FlagSupport.h"
#include "../Menus/MenuBase.h"

using namespace Common;
using namespace BG;

namespace UI::MenuItems
{
  PText::PText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const String& String, uint32_t TextStyle, const Color& Clr, const Color& ColorHighlight)
    : MenuText{ ID, Position, Flags, Parent, String, TextStyle, Clr, ColorHighlight }
  {
    m_Type = EMenuItemType::PText;
  }

  PText::PText(MenuItemID ID, MenuPosition Position, uint32_t Flags, const String& Name, Menus::MenuBase* Parent, uint32_t TextStyle, const Color& Clr, const Color& ColorHighlight)
    : MenuText{ ID, Position, Flags, Name, Parent, TextStyle, Clr, ColorHighlight }
  {
    m_Type = EMenuItemType::PText;
  }

  PText::PText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const ButtonTextInfo& ButtonTextInfo, uint32_t TextStyle, const Color& Clr, const Color& ColorHightlight)
    : MenuText{ ID, Position, Flags, Parent, ButtonTextInfo, TextStyle, Clr, ColorHightlight }
  {
    m_Type = EMenuItemType::PText;
  }

  PText::PText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, uint32_t TextStyle, const NormalTextInfo& NormalTextInfo, const Color& Clr, const Color& ColorHightlight)
    : MenuText{ ID, Position, Flags, Parent, TextStyle, NormalTextInfo, Clr, ColorHightlight }
  {
    m_Type = EMenuItemType::PText;
  }

  void PText::Draw()
  {
    auto Color = IsSet(m_Flags, QMF_GRAYED) ? text_color_disabled : m_Color;

    auto Style = m_TextStyle;
    if(IsSet(m_Flags, QMF_PULSEIFFOCUS))
    {
      if(m_Parent->ItemAtCursor() == this)
        Style |= UI_PULSE;
      else
        Style |= UI_INVERSE;
    }

    if(IsSet(m_Flags, QMF_HIGHLIGHT_IF_FOCUS) && m_Parent->ItemAtCursor() == this)
      Color = m_ColorHighlight;

    DrawProportionalString(m_Position, m_String, Style, Color);
  }

  void PText::Init()
  {
    const auto SizeScale = ProportionalSizeScale(m_TextStyle);
    auto ValX = X();
    const auto W = ProportionalStringWidth(m_String, UI_SMALLFONT) * SizeScale;
    const auto H = PROP_HEIGHT * SizeScale;

    if(IsSet(m_Flags, QMF_RIGHT_JUSTIFY))
      ValX -= W;
    else if(IsSet(m_Flags, QMF_CENTER_JUSTIFY))
      ValX -= W / 2;

    m_Rect.Left() = ValX - PROP_GAP_WIDTH * SizeScale;
    m_Rect.Right() = ValX + W + PROP_GAP_WIDTH * SizeScale;
    m_Rect.Top() = Y();
    m_Rect.Bottom() = Y() + H;
  }
}
