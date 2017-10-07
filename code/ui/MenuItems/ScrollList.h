#pragma once
#include "SpinControl.h"

namespace UI::MenuItems
{
  class ScrollList : public SpinControl
  {
  public:
    ScrollList(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const Common::String& Name, Common::Size Size, const BG::Color& Clr, const BG::Color& ColorHighlight, TextInfo Text)
      : SpinControl{ID, Position, Flags, Parent, Name, Size, Clr, ColorHighlight, Text}
    {
    }

    void Draw() override;
    void Init() override;
    sfxHandle_t Key(int32_t& Key) override;
  };
}

