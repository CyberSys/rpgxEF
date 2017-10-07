#pragma once

#include "../ui_local.h"
#include "../../common_game/String.h"
#include "../../common_game/Rect.h"
#include <vector>
#include "../../base_game/Color.h"

namespace UI
{
  namespace MenuItems
  {
    class MenuItem;
  }
}

namespace UI::Menus
{
  class MenuBase
  {
  public:
    virtual ~MenuBase() = default;

    virtual void OnDraw();
    virtual sfxHandle_t OnKey(int32_t Key) = 0;

    MenuItems::MenuItem* ItemAtCursor();
    const MenuPosition& GetDescriptionPosition() const { return m_DescriptionPosition; }
    void SetNoNewSelecting(bool Value) { m_NoNewSelecting = Value; }
    const int32_t& Cursor() const { return m_Cursor; }
    MenuItems::MenuItem* DisplaySpinList();
    void SetDisplaySpinList(MenuItems::MenuItem* List);
    void AdjustCursor(int32_t Dir, bool Wrapped = false);
    sfxHandle_t ActivateItem(MenuItems::MenuItem* Item) const;
    void SetCursor(int32_t Cursor);
    void SetCursorToItem(MenuItems::MenuItem* Item);
    sfxHandle_t DefaultKey(int32_t Key);

  protected:
    int32_t	m_Cursor = 0;
    int32_t m_CursorPrev = 0;

    bool m_WrapAround = false;
    bool m_Fullscreen = false;
    bool m_NoBackground = false;
    bool m_Initialized = false;		// Have the structures for this menu been initialized?
    MenuPosition m_DescriptionPosition;
    MenuPosition m_ListPosition;
    MenuTitle m_Title;
    StringID m_FootNote = 0;		// Footnote text

    MenuItems::MenuItem* m_DisplaySpinList= nullptr;	//if not NULL, display the list from this one (must be typecast as menulist_s when used)
    bool m_NoNewSelecting = false;		//used when we want to stop other buttons getting selected. mainly for the spin list, and slider dragging

    std::vector<MenuItems::MenuItem*> m_MenuItems;

    void RegisterMenuItem(MenuItems::MenuItem* Item);

    static void MenuFrameCache();

    void MenuFrame();

    static void SetColor(const BG::Color& Color);
    static void SetColor(const BG::Color&& Color);
    static void ResetColor();

    static void DrawProportionalString(const MenuPosition& Position, const Common::String& Str, int32_t Style, const BG::Color& Color);
  
    static void DrawString(const MenuPosition& Position, const Common::String& Str, int32_t Style, const BG::Color& Color, bool HighResolution);
    static void DrawChar(const MenuPosition& Position, int32_t Ch, int32_t Style, const BG::Color& Color);

    static bool IsWidescreen();

    static float ProportionalSizeScale(int32_t Style);
    static int32_t ProportionalStringWidth(const Common::String& Str, int32_t CharStyle);

    static void DrawPicture(const Common::RectF& Rect, qhandle_t Shader);
    static void DrawStretchPicture(const Common::RectF& Rect, float S0, float T0, float S1, float T1, qhandle_t Shader);

    static void FillRect(const Common::RectF& Rect, const BG::Color& Color);

    static void PopMenu();

    static void SetupMenuButtons();

    static void DrawRect(const Common::RectF& Rect, const BG::Color& Color);

    static void Precache();

    static void MouseHide();
    static void MouseShow();

    static float ClampVar(float Min, float Max, float Value);

  private:
    void DrawSpinList() const;
    void FrameTopGraphics() const;
    void CursorMoved();

    static void FrameBottomGraphics();
    static void MenuBottomLineEndGraphics(const Common::String& String, const BG::Color& Color, bool& Space);

    static bool s_Initialized;
    static qhandle_t s_CornerUpper;
    static qhandle_t s_CornerUpper2;
    static qhandle_t s_CornerLower;
  };
}
