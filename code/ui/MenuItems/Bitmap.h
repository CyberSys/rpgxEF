#pragma once
#include "../MenuItems/MenuItem.h"
#include "../../common_game/Size.h"
#include "TextInfo.h"
#include "../../base_game/Color.h"

namespace UI::MenuItems
{
  class Bitmap : public MenuItem
  {
  public:
    Bitmap(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const Common::String& Name, Common::Size Size, const BG::Color& Color, const BG::Color& ColorHightlight, Common::String FocusShaderName, TextInfo Text);

    void Draw() override;
    void Init() override;

    void SetFocusSize(const Common::Size& Size) { m_FocusSize = Size; }
    void SetFocusPosition(const MenuPosition& Position) { m_FocusPosition = Position; }
    void SetFocusColor(const BG::Color& Color) { m_ColorFocus = Color; }
    void SetText(const Common::String& Text) { m_TextStr = Text; }
    void SetText2(StringID ID) { m_Text2 = m_Text; m_Text.SetStringID(ID); }

  protected:
    qhandle_t m_Shader;
    qhandle_t m_ShaderFocus;
    Common::Size m_FocusSize;
    MenuPosition m_FocusPosition;
    Common::Size m_Size;
    BG::Color m_ColorFocus;
    BG::Color m_Color;
    BG::Color m_ColorHightlight;
    Common::String m_FocusShaderName;
    Common::String m_ErrorShaderName;
    Common::String m_TextStr;
    TextInfo m_Text;
    TextInfo m_Text2;
  };
}
