#include "MenuItem.h"
#include "../../base_game/Color.h"

using namespace BG;

namespace UI::MenuItems
{
  MenuItem::MenuItem(EMenuItemType Type, MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const Common::String& Name) : m_Type(Type)
    , m_ID(ID)
    , m_Position(Position)
    , m_Parent(Parent)
    , m_Flags(Flags)
    , m_Name(Name)
  {}

  void MenuItem::DrawString(const MenuPosition& Position, const Common::String& Str, int32_t Style, const Color& Color, bool HighResolution)
  {
    UI_DrawString(Position.X(), Position.Y(), Str.c_str(), Style, Color, HighResolution ? qtrue : qfalse);
  }

  int32_t MenuItem::ProportionalStringWidth(const Common::String& Str, int32_t Style)
  {
    return UI_ProportionalStringWidth(Str.c_str(), Style);
  }

  float MenuItem::ProportionalSizeScale(int32_t Style)
  {
    return UI_ProportionalSizeScale(Style);
  }

  void MenuItem::DrawChar(const MenuPosition& Position, int32_t CursorChar, int32_t Style, const Color& Color)
  {
    UI_DrawChar(Position.X(), Position.Y(), CursorChar, Style, Color);
  }

  void MenuItem::SetColor(const Color& Color)
  {
    trap_R_SetColor(Color);
  }

  void MenuItem::SetColor(Color&& Color)
  {
    trap_R_SetColor(Color);
  }

  void MenuItem::ResetColor()
  {
    trap_R_SetColor(nullptr);
  }

  void MenuItem::DrawPicture(const Common::RectF& Rect, qhandle_t Shader)
  {
    UI_DrawHandlePic(Rect.Left(), Rect.Top(), Rect.Width(), Rect.Height(), Shader);
  }

  void MenuItem::DrawProportionalString(const MenuPosition& Position, const Common::String& Str, int32_t Style, const Color& Color)
  {
    UI_DrawProportionalString(Position.X(), Position.Y(), Str.c_str(), Style, Color);
  }

  void MenuItem::FillRect(const Common::RectF& Rect, const Color& Color)
  {
    UI_FillRect(Rect.Left(), Rect.Top(), Rect.Width(), Rect.Height(), Color);
  }

  bool MenuItem::CursorInRect(const Common::Rect& Rect)
  {
    return UI_CursorInRect(Rect.Left(), Rect.Top(), Rect.Width(), Rect.Height()) == qtrue;
  }
}
