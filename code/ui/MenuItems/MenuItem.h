#pragma once
#include <cstdint>
#include "../ui_local.h"
#include "../../common_game/Rect.h"
#include <functional>
#include "../Menus/EventID.h"
#include "../../common_game/String.h"

namespace BG {
  class Color;
}

namespace UI
{
  namespace Menus
  {
    class MenuBase;
  }
}

namespace UI::MenuItems
{
  class MenuItem
  {
  public:
    using MenuItemID = int32_t;

    MenuItem(EMenuItemType Type, MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent,
             const Common::String& Name = {});
    virtual ~MenuItem() = default;

    std::function<void(MenuItem* Menu, Menus::EventID)> OnCallback;
    std::function<void(MenuItem* Menu)> OnStatusBar;
    std::function<void(MenuItem* Menu)> OnOwnerDraw;

    virtual void Draw() {}
    virtual void Init() {}
    virtual sfxHandle_t Key(int32_t& Key) { UNUSED(Key); return 0; }

    EMenuItemType GetType() const { return m_Type; }
    MenuItemID GetID() const { return m_ID; }
    const MenuPosition& GetPosition() const { return m_Position; }
    void SetPosition(const MenuPosition& Position) { m_Position = Position; }
    void SetPositionX(MenuPosition::value_type X) { m_Position.X() = X; }
    void SetPositionY(MenuPosition::value_type Y) { m_Position.Y() = Y; }
    const Common::Rect& GetRect() const { return m_Rect; }
    uint32_t GetFlags() const { return m_Flags; }
    void SetFlags(uint32_t Flags) { m_Flags = Flags; }
    const Common::String& GetName() const { return m_Name; }
    const MenuPosition::value_type& X() const { return m_Position.X(); }
    const MenuPosition::value_type& Y() const { return m_Position.Y(); }
    void SetParent(Menus::MenuBase* Parent) { m_Parent = Parent; }
    void SetMenuPosition(int32_t Position) { m_MenuPosition = Position; }

    static void DrawString(const MenuPosition& Position, const Common::String& Str, int32_t Style, const BG::Color& Color, bool HighResolution);
    static void DrawProportionalString(const MenuPosition& Position, const Common::String& Str, int32_t Style, const BG::Color& Color);

    static int32_t ProportionalStringWidth(const Common::String& Str, int32_t Style);
    static float ProportionalSizeScale(int32_t Style);

    static void DrawChar(const MenuPosition& Position, int32_t CursorChar, int32_t Style, const BG::Color& Color);

    static void SetColor(const BG::Color& Color);
    static void SetColor(BG::Color&& Color);
    static void ResetColor();

    static void DrawPicture(const Common::RectF& Rect, qhandle_t Shader);

    static void FillRect(const Common::RectF& Rect, const BG::Color& Color);

    static bool CursorInRect(const Common::Rect& Rect);

  protected:
    EMenuItemType m_Type = EMenuItemType::Null;
    MenuItemID m_ID = 0;
    MenuPosition m_Position = { 0,0 };
    Common::Rect m_Rect = { 0,0,0,0 };
    Menus::MenuBase* m_Parent = nullptr;
    int32_t m_MenuPosition = 0;
    size_t m_Flags = 0;
    Common::String m_Name;
  };
}
