#pragma once
#include "MenuText.h"

namespace UI::MenuItems
{
  class PText : public MenuText
  {
  public:
    PText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const Common::String& String, uint32_t TextStyle, const BG::Color& Clr, const BG::Color& ColorHighlight);
    PText(MenuItemID ID, MenuPosition Position, uint32_t Flags, const Common::String& Name, Menus::MenuBase* Parent, uint32_t TextStyle, const BG::Color& Clr, const BG::Color& ColorHighlight);
    PText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const ButtonTextInfo& ButtonTextInfo, uint32_t TextStyle, const BG::Color& Clr, const BG::Color& ColorHightlight);
    PText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, uint32_t TextStyle, const NormalTextInfo& NormalTextInfo, const BG::Color& Clr, const BG::Color& ColorHightlight);

    void Draw() override;
    void Init() override;
 };
}
