#pragma once
#include "MenuItem.h"
#include "../../common_game/String.h"
#include "../Menus/MenuBase.h"
#include "../ui_local.h"
#include "../../base_game/bg_collision.h"


namespace UI::MenuItems
{
  class Field : public MenuItem
  {
  public:
    Field(MenuItemID ID, MenuPosition Position, size_t Flags, size_t MaxChars, size_t WidthInChars, int32_t Style, StringID TitleID, const BG::Color& TitleColor, const BG::Color& TextColor, const BG::Color& TextColor2, Menus::MenuBase* Parent = nullptr, const Common::String& InitialValue = {}, const Common::String& Name = {});

    virtual void Draw() override;
    virtual void Init() override;
    virtual sfxHandle_t Key(int32_t& Key) override;

    size_t GetCursor() const { return m_Cursor; }
    size_t GetMaxChars() const { return m_MaxChars; }
    void SetMaxChars(int32_t Max) { m_MaxChars = Max; }
    int32_t GetStyle() const { return m_Style; }
    void SetStyle(int32_t Style) { m_Style = Style; }
    StringID GetTitleID() const { return m_TitleID; }
    void SetTitleID(StringID ID) { m_TitleID = ID; }
    const BG::Color& GetTitleColor() const { return m_TitleColor; }
    void SetTitleColor(const BG::Color& Color) { m_TitleColor = Color; }
    const BG::Color& GetTextColor() const { return m_TextColor; }
    void SetTextColor(const BG::Color& Color) { m_TitleColor = Color; }
    const BG::Color& GetTextColor2() const { return m_TextColor2; }
    void SetTextColor2(const BG::Color& Color) { m_TextColor2 = Color; }
    const Common::String& GetText() const { return m_Buffer; }
    void SetText(const Common::String& Text) { m_Buffer = Text; m_Buffer.resize(m_MaxChars); }

  protected:
    size_t m_Cursor = 0;
    size_t m_Scroll = 0;
    size_t m_MaxChars;
    int32_t m_Style;
    size_t m_WidthInChars;
    StringID m_TitleID;
    BG::Color m_TitleColor;
    BG::Color m_TextColor;
    BG::Color m_TextColor2;
    Common::String m_Buffer;

    void Draw(MenuPosition::value_type X, MenuPosition::value_type Y, int32_t Style, const BG::Color& Color, int32_t Cursor);
    void Clear();
    void Paste();
    void CharEvent(int32_t Char);
    void KeyDownEvent(int32_t Key);

  };
}
