#pragma once
#include "MenuItem.h"
#include "TextInfo.h"
#include "../../common_game/Size.h"

namespace UI::MenuItems
{
  class MenuText : public MenuItem
  {
  public:
    struct ButtonTextInfo
    {
      StringID m_ButtonTextID;
      StringID m_ButtonTextID2;
      StringID m_ButtonTextID3;
    };

    struct NormalTextInfo
    {
      StringID m_NormalTextID;
      StringID m_NormalTextID2;
      StringID m_NormalTextID3;
    };

    MenuText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const Common::String& String, uint32_t TextStyle, const BG::Color& Clr, const BG::Color& ColorHighlight = nullptr);
    MenuText(MenuItemID ID, MenuPosition Position, uint32_t Flags, const Common::String& Name, Menus::MenuBase* Parent, uint32_t TextStyle, const BG::Color& Clr, const BG::Color& ColorHighlight = nullptr);
    MenuText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const ButtonTextInfo& ButtonTextInfo, uint32_t TextStyle, const BG::Color& Clr, const BG::Color& ColorHightlight = nullptr);
    MenuText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, uint32_t TextStyle, const NormalTextInfo& NormalTextInfo, const BG::Color& Clr, const BG::Color& ColorHightlight = nullptr);
        
    virtual void Draw() override;
    virtual void Init() override;

    void SetFocusPosition(const MenuPosition& Position) { m_FocusPosition = Position; }
    void SetFocusSize(const Common::Size& Size) { m_FocusSize = Size; }

  protected:
    StringID m_NormalTextID;
    StringID m_ButtonTextID;
    StringID m_NormalTextID2;
    StringID m_ButtonTextID2;
    StringID m_NormalTextID3;
    StringID m_ButtonTextID3;
    uint32_t m_TextStyle;
    BG::Color m_Color;
    BG::Color m_ColorHighlight;
    MenuPosition m_FocusPosition;
    Common::Size m_FocusSize;
    Common::String m_String;
  };
}
