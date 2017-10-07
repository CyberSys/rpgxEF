#pragma once
#include "../ui_local.h"
#include "../../base_game/Color.h"

namespace UI::MenuItems
{
  class TextInfo
  {
  public:
    TextInfo() = default;
    TextInfo(MenuPosition Position, StringID ID, const BG::Color& Clr, const BG::Color& ColorHightlight, const BG::Color& ColorGreyed = {})
      : m_Position(Position)
      , m_ID(ID)
      , m_Color(Clr)
      , m_ColorHightlight(ColorHightlight)
      , m_ColorGreyed{ ColorGreyed }
      , m_Flags{ 0 }
    {}

    operator bool() const { return m_ID != 0; }
    operator Common::String() const { return m_ID ? menu_button_text[m_ID][0] : ""; }
    operator const MenuPosition&() const { return m_Position; }

    Common::String GetText2() const { return m_ID && menu_button_text[m_ID][1] != nullptr ? menu_button_text[m_ID][1] : ""; }

    const MenuPosition::value_type& X() const { return m_Position.X(); }
    const MenuPosition::value_type& Y() const { return m_Position.Y(); }
    const BG::Color& Color() const { return m_Color; }
    const BG::Color& ColorHightlight() const { return m_ColorHightlight; }
    const BG::Color& ColorGreyed() const { return m_ColorGreyed; }
    int32_t Flags() const { return m_Flags; }

    void SetPosition(const MenuPosition& Position) { m_Position = Position; }
    void SetPositionX(MenuPosition::value_type X) { m_Position.X() = X; }
    void SetPositionY(MenuPosition::value_type Y) { m_Position.Y() = Y; }
    void SetStringID(StringID ID) { m_ID = ID; }
    void SetColor(const BG::Color& Color) { m_Color = Color; }
    void SetColorHighlight(const BG::Color& Color) { m_ColorHightlight = Color; }
    void SetFlags(uint32_t Flags) { m_Flags = Flags; }
    void SetColorGreyed(const BG::Color& Color) { m_ColorGreyed = Color; }

  protected:
    MenuPosition m_Position = { 0,0 };
    StringID m_ID = 0;
    BG::Color m_Color;
    BG::Color m_ColorHightlight;
    BG::Color m_ColorGreyed;
    uint32_t m_Flags = 0;
  };
}
