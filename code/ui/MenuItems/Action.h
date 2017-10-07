#pragma once
#include "MenuItem.h"
#include "TextInfo.h"
#include "../../common_game/Size.h"
#include "../../base_game/Color.h"

namespace doctest {
  class String;
}

namespace UI::MenuItems
{
  class Action : public MenuItem
  {
  public:
    Action(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const Common::String& Name, const BG::Color& Clr, const BG::Color& ColorHighlight, const BG::Color& ColorGreyed, TextInfo Text, StringID Text2, Common::Size Size);

    void Draw() override;
    void Init() override;

  protected:
    BG::Color m_Color;
    BG::Color m_ColorHighlight;
    BG::Color m_ColorGreyed;
    TextInfo m_Text;
    TextInfo m_Text2;
    Common::Size m_Size;
    bool m_Updated;
  };
}
