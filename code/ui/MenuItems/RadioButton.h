#pragma once
#include "MenuItem.h"

namespace UI::MenuItems
{
  class RadioButton : public MenuItem
  {
  public:
    RadioButton(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const Common::String& Name, bool CurrentValue)
      : MenuItem(EMenuItemType::RadioButton, ID, Position, Flags, Parent, Name)
      , m_CurrentValue(CurrentValue)
    {
    }

    virtual void Draw() override;
    virtual void Init() override;
    virtual sfxHandle_t Key(int32_t& Key) override;

    bool CurrentValue() const { return m_CurrentValue; }

  protected:
    bool m_CurrentValue;
  };
}
