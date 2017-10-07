#include "BText.h"
#include "../../common_game/FlagSupport.h"

using namespace Common;
using namespace BG;

namespace UI::MenuItems
{
  BText::BText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const String& String, uint32_t TextStyle, const Color& Clr, const Color& ColorHighlight)
    : MenuText{ ID, Position, Flags, Parent, String, TextStyle, Clr, ColorHighlight }
  {
    m_Type = EMenuItemType::BText;
  }

  BText::BText(MenuItemID ID, MenuPosition Position, uint32_t Flags, const String& Name, Menus::MenuBase* Parent, uint32_t TextStyle, const Color& Clr, const Color& ColorHighlight)
    : MenuText{ ID, Position, Flags, Name, Parent, TextStyle, Clr, ColorHighlight }
  {
    m_Type = EMenuItemType::BText;
  }

  BText::BText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const ButtonTextInfo& ButtonTextInfo, uint32_t TextStyle, const Color& Clr, const Color& ColorHightlight)
    : MenuText{ ID, Position, Flags, Parent, ButtonTextInfo, TextStyle, Clr, ColorHightlight }
  {
    m_Type = EMenuItemType::BText;
  }

  BText::BText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, uint32_t TextStyle, const NormalTextInfo& NormalTextInfo, const Color& Clr, const Color& ColorHightlight)
    : MenuText{ ID, Position, Flags, Parent, TextStyle, NormalTextInfo, Clr, ColorHightlight }
  {
    m_Type = EMenuItemType::BText;
  }

  void BText::Draw()
  {
    const auto Color = IsSet(m_Flags, QMF_GRAYED) ? text_color_disabled : m_Color;
    DrawProportionalString(m_Position, m_String, m_TextStyle, Color);
  }

  void BText::Init()
  {
    m_Flags |= QMF_INACTIVE;
  }
}
