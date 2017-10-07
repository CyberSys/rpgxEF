#include "MenuBase.h"
#include "../MenuItems/MenuItem.h"
#include "../../common_game/FlagSupport.h"
#include "../MenuItems/SpinControl.h"
#include "../../qcommon/stv_version.h"


extern int32_t propMapTiny[256][3];
extern int32_t propMapBig[256][3];
extern int32_t propMap[256][3];
extern int32_t specialTinyPropChars[256][2];
extern int32_t specialBigPropChars[256][2];


using namespace Common;
using namespace BG;

namespace UI::Menus
{
  bool MenuBase::s_Initialized = false;
  qhandle_t MenuBase::s_CornerUpper = -1;
  qhandle_t MenuBase::s_CornerUpper2 = -1;
  qhandle_t MenuBase::s_CornerLower = -1;


  void MenuBase::OnDraw()
  {
    for(auto Item : m_MenuItems)
    {
      if(IsSet(Item->GetFlags(), QMF_HIDDEN))
        continue;

      if(Item->OnOwnerDraw)
        Item->OnOwnerDraw(Item);
      else
        Item->Draw();

#ifndef NDEBUG
      if(uis.debug)
      {
        if(NotSet(Item->GetFlags(), QMF_INACTIVE))
        {
          const auto& Rect = Item->GetRect();

          if(IsSet(Item->GetFlags(), QMF_HASMOUSEFOCUS))
            DrawRect(Rect, colorYellow);
          else
            DrawRect(Rect, colorWhite);
        }
      }
#endif
    }

    const auto Item = ItemAtCursor();
    if(Item && Item->OnStatusBar)
      Item->OnStatusBar(Item);

    DrawSpinList();
  }

  MenuItems::MenuItem* MenuBase::ItemAtCursor()
  {
    if(m_Cursor < 0 || m_Cursor >= m_MenuItems.size())
    {
      return nullptr;
    }

    return m_MenuItems[m_Cursor];
  }

  MenuItems::MenuItem* MenuBase::DisplaySpinList()
  {
    return m_DisplaySpinList;
  }

  void MenuBase::SetDisplaySpinList(MenuItems::MenuItem* List)
  {
    m_DisplaySpinList = List;
  }

  void MenuBase::AdjustCursor(int32_t Dir, bool Wrapped)
  {
    while(m_Cursor >= 0 && m_Cursor < m_MenuItems.size())
    {
      const auto Item = m_MenuItems[m_Cursor];
      if(IsSet(Item->GetFlags(), QMF_GRAYED | QMF_MOUSEONLY | QMF_INACTIVE))
        m_Cursor += Dir;
      else
        break;
    }

    if(Dir == 1)
    {
      if(m_Cursor >= m_MenuItems.size())
      {
        if(m_WrapAround)
        {
          if(Wrapped)
          {
            m_Cursor = m_CursorPrev;
            return;
          }
          m_Cursor = 0;
          AdjustCursor(Dir, true);
          return;
        }
        m_Cursor = m_CursorPrev;
      }
    }
    else
    {
      if(m_Cursor < 0)
      {
        if(m_WrapAround)
        {
          if(Wrapped)
          {
            m_Cursor = m_CursorPrev;
            return;
          }
          m_Cursor = m_MenuItems.size() - 1;
          AdjustCursor(Dir, true);
          return;
        }
        m_Cursor = m_CursorPrev;
      }
    }
  }

  sfxHandle_t MenuBase::ActivateItem(MenuItems::MenuItem* Item) const
  {
    if(Item->OnCallback)
    {
      Item->OnCallback(Item, QM_ACTIVATED);
      if(NotSet(Item->GetFlags(), QMF_SILENT))
        return menu_move_sound;
    }

    return 0;
  }

  void MenuBase::SetCursor(int32_t Cursor)
  {
    if(Cursor >= m_MenuItems.size())
      return;

    if(IsSet(m_MenuItems[Cursor]->GetFlags(), QMF_GRAYED | QMF_INACTIVE))
      return;

    m_CursorPrev = m_Cursor;
    m_Cursor = Cursor;

    CursorMoved();
  }

  void MenuBase::SetCursorToItem(MenuItems::MenuItem* Item)
  {
    for(auto i = 0U; i < m_MenuItems.size(); i++)
      if(m_MenuItems[i] == Item)
      {
        SetCursor(i);
        return;
      }
  }

  sfxHandle_t MenuBase::DefaultKey(int32_t Key)
  {
    auto Sound = 0;
    int32_t CursorPrev;

    switch(Key)
    {
    case K_MOUSE2:
    case K_ESCAPE:
      {
        if(m_DisplaySpinList)
        {
          m_DisplaySpinList = nullptr;
          m_NoNewSelecting = false;
          return menu_out_sound;
        }

        PopMenu();
        return menu_out_sound;
      }
    default:
      break;
    }

    if(m_MenuItems.empty())
      return 0;

    auto Item = ItemAtCursor();
    if(Item && NotSet(Item->GetFlags(), QMF_GRAYED | QMF_INACTIVE))
    {
      Sound = Item->Key(Key);

      if(Sound)
        return Sound;
    }

    switch(Key)
    {
#ifndef NDEBUG
    case K_F11:
      uis.debug = static_cast<qboolean>(static_cast<int32_t>(uis.debug) ^ 1);
      break;

    case K_F12:
      trap_Cmd_ExecuteText(EXEC_APPEND, "screenshot\n");
      break;
#endif
    case K_KP_UPARROW:
    case K_UPARROW:
      {
        CursorPrev = m_Cursor;
        m_CursorPrev = m_Cursor;
        m_Cursor--;
        AdjustCursor(-1);
        if(CursorPrev != m_Cursor)
        {
          CursorMoved();
          Sound = menu_move_sound;
        }
        break;
      }
    case K_TAB:
    case K_KP_DOWNARROW:
    case K_DOWNARROW:
      {
        CursorPrev = m_Cursor;
        m_CursorPrev = m_Cursor;
        m_Cursor++;
        AdjustCursor(1);
        if(CursorPrev != m_Cursor)
        {
          CursorMoved();
          Sound = menu_move_sound;
        }
        break;
      }
    case K_MOUSE1:
    case K_MOUSE3:
    case K_MOUSE4:
    case K_MOUSE5:
      {
        if(Item)
          if(IsSet(Item->GetFlags(), QMF_HASMOUSEFOCUS) && !NotSet(Item->GetFlags(), QMF_GRAYED | QMF_INACTIVE))
            return ActivateItem(Item);
        break;
      }
    case K_JOY1:
    case K_JOY2:
    case K_JOY3:
    case K_JOY4:
    case K_AUX1:
    case K_AUX2:
    case K_AUX3:
    case K_AUX4:
    case K_AUX5:
    case K_AUX6:
    case K_AUX7:
    case K_AUX8:
    case K_AUX9:
    case K_AUX10:
    case K_AUX11:
    case K_AUX12:
    case K_AUX13:
    case K_AUX14:
    case K_AUX15:
    case K_AUX16:
    case K_KP_ENTER:
    case K_ENTER:
      {
        if(Item)
          if(NotSet(Item->GetFlags(), QMF_MOUSEONLY | QMF_GRAYED | QMF_GRAYED))
            return ActivateItem(Item);
        break;
      }
    default:
      break;
    }

    return Sound;
  }

  void MenuBase::RegisterMenuItem(MenuItems::MenuItem* Item)
  {
    if(m_MenuItems.size() >= MAX_MENUITEMS)
      trap_Error("Menu_AddItem: excessive items");

    m_MenuItems.push_back(Item);
    Item->SetParent(this);
    Item->SetMenuPosition(m_MenuItems.size());
    Item->SetFlags(Item->GetFlags() & ~QMF_HASMOUSEFOCUS);

    if(NotSet(Item->GetFlags(), QMF_NODEFAULTINIT))
    {
      Item->Init();
    }
  }

  void MenuBase::MenuFrameCache()
  {
    if(!s_Initialized)
    {
      s_CornerUpper = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_7.tga");
      s_CornerUpper2 = trap_R_RegisterShaderNoMip("menu/common/corner_ul_47_7.tga");
      s_CornerLower = trap_R_RegisterShaderNoMip("menu/common/corner_ll_47_18.tga");
    }
  }

  void MenuBase::MenuFrame()
  {
    auto Space = true;

    MenuFrameCache();
    m_Fullscreen = !ingameFlag;

    FrameTopGraphics();
    FrameBottomGraphics();

    if(m_FootNote)
    {
      DrawProportionalString({ MENU_TITLE_X, 440 }, menu_normal_text[m_FootNote], UI_RIGHT | UI_SMALLFONT,
        colorTable[CT_LTORANGE]);
      MenuBottomLineEndGraphics(menu_normal_text[m_FootNote], EColors::LightBrown1, Space);
    }

    if(Space)
      DrawProportionalString({ 371, 445 }, Q3_VERSION, UI_TINYFONT, colorTable[CT_BLACK]);
  }

  void MenuBase::FrameTopGraphics() const
  {
    SetColor(EColors::DarkPurple2);
    DrawPicture({ 30., 24., 47., 54. }, uis.whiteShader);	/* Top left hand column */

    SetColor(EColors::DarkPurple3);
    DrawPicture({ 30., 81., 47., 34. }, uis.whiteShader);	/* Middle left hand column */
    DrawPicture({ 30., 115., 128., 64. }, s_CornerUpper);	/* Corner */
    DrawPicture({ 100., 136., 49., 6.5 }, uis.whiteShader);	/* Start of line across bottom of top third section */
                                                          /*ABOVE HAS LINE BUG 111 - 38 - 7 */
    SetColor(EColors::LightBrown1);
    DrawPicture({ 152., 136., 135., 7. }, uis.whiteShader);	/* 2nd line across bottom of top third section */

    SetColor(EColors::LightPurple2);
    DrawPicture({ 290., 136., 12., 7. }, uis.whiteShader);	/* 3rd line across bottom of top third section */

    SetColor(EColors::LightBrown1); /* RPG-X ADDITION */
    DrawPicture({ 305., 139., 60., 4. }, uis.whiteShader);	/* 4th line across bottom of top third section */

                                                        /*RPG-X REMOVE trap_R_SetColor( colorTable[CT_LTBROWN1]);*/
    DrawPicture({ 368., 136., 111., 7. }, uis.whiteShader); /* 5th line across bottom of top third section */

    if(m_Title)
    {
      DrawProportionalString(m_Title.m_Position, menu_normal_text[m_Title], UI_RIGHT | UI_BIGFONT, colorTable[CT_LTORANGE]);
    }
  }

  void MenuBase::CursorMoved()
  {
    if(m_CursorPrev == m_Cursor)
      return;

    if(m_CursorPrev >= 0 && m_CursorPrev < m_MenuItems.size())
    {
      if(m_MenuItems[m_CursorPrev]->OnCallback)
        m_MenuItems[m_CursorPrev]->OnCallback(m_MenuItems[m_CursorPrev], QM_LOSTFOCUS);
    }

    if(m_Cursor >= 0 && m_Cursor < m_MenuItems.size())
    {
      if(m_MenuItems[m_Cursor]->OnCallback)
        m_MenuItems[m_Cursor]->OnCallback(m_MenuItems[m_Cursor], QM_GOTFOCUS);
    }
  }

  void MenuBase::FrameBottomGraphics()
  {
    SetColor(EColors::DarkBrown1);
    DrawPicture({ 30., 147., 128., 64. }, s_CornerUpper2); /* Top corner */
    DrawPicture({ 50., 147., 99., 7. }, uis.whiteShader);

    SetColor(EColors::LightBrown1); /* DKBROWN1 */
    DrawPicture({ 152., 147., 135., 7. }, uis.whiteShader);

    SetColor(EColors::DarkRed1); /* DKBROWN1 */
    DrawPicture({ 290., 147., 12., 7. }, uis.whiteShader);

    SetColor(EColors::LightBrown1);
    DrawPicture({ 305., 147., 60., 4. }, uis.whiteShader);

    SetColor(EColors::LightGold1); /* DKBROWN1 */
    DrawPicture({ 368., 147., 111., 7. }, uis.whiteShader);

    SetColor(EColors::DarkBrown1);
    DrawPicture({ 30., 173., 47., 270. }, uis.whiteShader);	/* Top left column (81453) */

                                                        /* Start of piece (431108) */
    DrawPicture({ 30., 392., 47., 33. }, uis.whiteShader);	/* Bottom left column */
    DrawPicture({ 30., 425., 128., 64. }, s_CornerLower); /* Bottom Left Corner */

    SetColor(EColors::LightBlue1); /* LTBROWN1]); */
    DrawPicture({ 96., 438., 268., 18. }, uis.whiteShader);	/* Bottom front Line */

    ResetColor();
  }

  void MenuBase::MenuBottomLineEndGraphics(const String& String, const Color& Color, bool& Space)
  {
    SetColor(Color);
    const auto HoldX = MENU_TITLE_X - UI_ProportionalStringWidth(String.c_str(), UI_SMALLFONT);
    const auto HoldLength = 367 + 6 - HoldX;
    DrawPicture({ 367, 438, HoldLength, 18 }, uis.whiteShader); /* Bottom end line */
    Space = (HoldLength < 0 ? -HoldLength : HoldLength) < UI_ProportionalStringWidth(Q3_VERSION, UI_TINYFONT) ? false : true;
  }

  void MenuBase::SetColor(const BG::Color& Color)
  {
    trap_R_SetColor(Color);
  }

  void MenuBase::SetColor(const BG::Color&& Color)
  {
    trap_R_SetColor(Color);
  }

  void MenuBase::ResetColor()
  {
    trap_R_SetColor(nullptr);
  }

  float MenuBase::ProportionalSizeScale(int32_t Style)
  {
    return UI_ProportionalSizeScale(Style);
  }

  int32_t MenuBase::ProportionalStringWidth(const String& Str, int32_t CharStyle)
  {
    return UI_ProportionalStringWidth(Str.c_str(), CharStyle);
  }

  void MenuBase::DrawPicture(const RectF& Rect, qhandle_t Shader)
  {
    UI_DrawHandlePic(Rect.Left(), Rect.Top(), Rect.Width(), Rect.Height(), Shader);
  }

  void MenuBase::DrawStretchPicture(const RectF& Rect, float S0, float T0, float S1, float T1, qhandle_t Shader)
  {
    UI_DrawHandleStretchPic(Rect.Left(), Rect.Top(), Rect.Width(), Rect.Height(), S0, T0, S1, T1, Shader);
  }

  void MenuBase::FillRect(const RectF& Rect, const Color& Color)
  {
    UI_FillRect(Rect.Left(), Rect.Top(), Rect.Width(), Rect.Height(), Color);
  }

  void MenuBase::PopMenu()
  {
    UI_PopMenu();
  }

  void MenuBase::SetupMenuButtons()
  {
    UI_Setup_MenuButtons();
  }

  void MenuBase::DrawRect(const RectF& Rect, const Color& Color)
  {
    UI_DrawRect(Rect.Left(), Rect.Top(), Rect.Width(), Rect.Height(), Color);
  }

  void MenuBase::Precache()
  {
    static  const char *smallNumbers[] =
    {
      "gfx/2d/numbers/s_zero.tga",
      "gfx/2d/numbers/s_one.tga",
      "gfx/2d/numbers/s_two.tga",
      "gfx/2d/numbers/s_three.tga",
      "gfx/2d/numbers/s_four.tga",
      "gfx/2d/numbers/s_five.tga",
      "gfx/2d/numbers/s_six.tga",
      "gfx/2d/numbers/s_seven.tga",
      "gfx/2d/numbers/s_eight.tga",
      "gfx/2d/numbers/s_nine.tga",
      "gfx/2d/numbers/s_nine.tga",
    };

    uis.charset = trap_R_RegisterShaderNoMip("gfx/2d/charsgrid_med");
    uis.charsetProp = trap_R_RegisterShaderNoMip("gfx/2d/chars_medium");
    uis.charsetPropTiny = trap_R_RegisterShaderNoMip("gfx/2d/chars_tiny");
    uis.charsetPropBig = trap_R_RegisterShaderNoMip("gfx/2d/chars_big");
    uis.charsetPropB = trap_R_RegisterShaderNoMip("gfx/2d/chars_medium");
    uis.cursor = trap_R_RegisterShaderNoMip("menu/common/cursor");
    uis.whiteShader = trap_R_RegisterShaderNoMip("white");
    uis.menuBackShader = trap_R_RegisterShaderNoMip("gfx/2d/conback.tga");

    // When hitting enter
    menu_in_sound = trap_S_RegisterSound("sound/interface/button2.wav");
    // When dragging mouse over buttons
    menu_move_sound = trap_S_RegisterSound("sound/interface/button1.wav");
    menu_out_sound = trap_S_RegisterSound("sound/interface/button2.wav");
    menu_buzz_sound = trap_S_RegisterSound("sound/interface/button2.wav");

    // need a nonzero sound, make an empty sound for this
    menu_null_sound = -1;

    // Common menu graphics
    uis.graphicCircle = trap_R_RegisterShaderNoMip("menu/common/circle.tga");
    uis.graphicButtonLeftEnd = trap_R_RegisterShaderNoMip("menu/common/barbuttonleft.tga");

    uis.graphicBracket1CornerLU = trap_R_RegisterShaderNoMip("menu/common/corner_lu.tga");
    uis.corner_12_18 = trap_R_RegisterShaderNoMip("menu/common/corner_ll_12_18.tga");
    uis.halfroundr_22 = trap_R_RegisterShaderNoMip("menu/common/halfroundr_22.tga");

    uis.graphicButtonLeft = trap_R_RegisterShaderNoMip("menu/new/bar2.tga");

    for(auto i = 0; i < 10; ++i)
    {
      uis.smallNumbers[i] = trap_R_RegisterShaderNoMip(smallNumbers[i]);
    }

    uis.menu_choice1_snd = trap_S_RegisterSound("sound/interface/button3.wav");
    uis.menu_datadisp1_snd = trap_S_RegisterSound("sound/interface/button4.wav");
    uis.menu_datadisp2_snd = trap_S_RegisterSound("sound/interface/button7.wav");
    uis.menu_datadisp3_snd = trap_S_RegisterSound("sound/interface/button5.wav");
  }

  void MenuBase::MouseHide()
  {
    uis.cursorDraw = qfalse;
  }

  void MenuBase::MouseShow()
  {
    uis.cursorDraw = qtrue;
  }

  float MenuBase::ClampVar(float Min, float Max, float Value)
  {
    if(Value < Min)
      return Min;
    if(Value > Max)
      return Max;
    return Value;
  }

  void MenuBase::DrawSpinList() const
  {
    if(m_DisplaySpinList == nullptr)
      return;

    auto List = dynamic_cast<MenuItems::SpinControl*>(m_DisplaySpinList);
    if(List == nullptr)
      return;

    const auto& DrawRect = List->m_DrawRect;

    SetColor(EColors::Black);
    if(List->ItemCount() < MAX_LIST)
      DrawPicture(DrawRect, uis.whiteShader);
    else
      DrawPicture({ DrawRect.Left(), DrawRect.Top(), DrawRect.Width() * 2, DrawRect.Height() }, uis.whiteShader);

    SetColor(List->m_Color);
    // left
    DrawPicture({ DrawRect.Left(), DrawRect.Top() + 1, 1, DrawRect.Height() - 2 }, uis.whiteShader);

    if(List->ItemCount() < MAX_LIST)
    {
      // top
      DrawPicture({ DrawRect.Left(), DrawRect.Top(), DrawRect.Width(), 1 }, uis.whiteShader);
      // right
      DrawPicture({ DrawRect.Right() - 1, DrawRect.Top() + 1, 1, DrawRect.Height() - 2 }, uis.whiteShader);
      // down
      DrawPicture({ DrawRect.Left(), DrawRect.Bottom() - 1, DrawRect.Width(), 1 }, uis.whiteShader);
    }
    else
    {
      // top
      DrawPicture({ DrawRect.Left(), DrawRect.Top(), DrawRect.Width() * 2, 1 }, uis.whiteShader);
      // right
      DrawPicture({ DrawRect.Right() + DrawRect.Width() - 1, DrawRect.Top() + 1, 1, DrawRect.Height() - 2 }, uis.whiteShader);
      // down
      DrawPicture({ DrawRect.Left(), DrawRect.Bottom() - 1, DrawRect.Width() * 2, 1 }, uis.whiteShader);
    }

    auto SelectedNum = (uis.cursory - DrawRect.Top() + 1) / SMALLCHAR_HEIGHT;

    if(List->ItemCount() > MAX_LIST && uis.cursorx > DrawRect.Right() && SelectedNum + MAX_LIST < List->ItemCount())
      SelectedNum += MAX_LIST;

    if(SelectedNum > 0)
      SelectedNum = 0;

    if(SelectedNum >= List->ItemCount())
      SelectedNum = List->ItemCount() - 1;

    if(SelectedNum >= MAX_LIST)
      DrawPicture({ DrawRect.Left() + DrawRect.Width() + 1, DrawRect.Top() + 1 + SMALLCHAR_HEIGHT * (SelectedNum - MAX_LIST), DrawRect.Width() - 2, SMALLCHAR_HEIGHT + 1 }, uis.whiteShader);
    else
      DrawPicture({ DrawRect.Left() + 1, DrawRect.Top() + 1 + SMALLCHAR_HEIGHT * SelectedNum, DrawRect.Width() - 2, SMALLCHAR_HEIGHT + 1 }, uis.whiteShader);

    for(auto i = 0; i < MAX_LIST; i++)
    {
      if(i >= List->ItemCount())
        break;

      if(!List->m_ListNames.empty())
      {
        if(IsSet(List->m_Text.Flags(), UI_CENTER))
        {
          DrawProportionalString({ DrawRect.Left() + 2, DrawRect.Top() + 2 + SMALLCHAR_HEIGHT * i }, menu_normal_text[List->m_ListNames[i]], UI_SMALLFONT | List->m_Text.Flags(), EColors::White);

          if(i + MAX_LIST < List->ItemCount())
            DrawProportionalString({ DrawRect.Left() + 2 + DrawRect.Width(), DrawRect.Top() + 2 + SMALLCHAR_HEIGHT * i }, menu_normal_text[List->m_ListNames[i + MAX_LIST]], UI_SMALLFONT | List->m_Text.Flags(), EColors::White);
        }
        else
        {
          DrawProportionalString({ List->X() + List->m_Text.X() - 2, DrawRect.Top() + 2 + SMALLCHAR_HEIGHT * i }, menu_normal_text[List->m_ListNames[i]], UI_SMALLFONT | List->m_Text.Flags(), EColors::White);

          if(i + MAX_LIST < List->ItemCount())
            DrawProportionalString({ DrawRect.Left() + List->m_Text.X() + 2 + DrawRect.Width(), DrawRect.Top() + 2 + SMALLCHAR_HEIGHT * i }, menu_normal_text[List->m_ListNames[i + MAX_LIST]], UI_SMALLFONT | List->m_Text.Flags(), EColors::White);
        }
      }
      else
      {
        if(IsSet(List->m_Text.Flags(), UI_CENTER))
        {
          DrawProportionalString({ DrawRect.Left() + 2, DrawRect.Top() + 2 + SMALLCHAR_HEIGHT * i }, List->ItemNames()[i], UI_SMALLFONT | List->m_Text.Flags(), EColors::White);

          if(i + MAX_LIST < List->ItemCount())
            DrawProportionalString({ DrawRect.Left() + 2 + DrawRect.Width(), DrawRect.Top() + 2 + SMALLCHAR_HEIGHT * i }, List->ItemNames()[i + MAX_LIST], UI_SMALLFONT | List->m_Text.Flags(), EColors::White);
        }
        else
        {
          DrawProportionalString({ List->X() + List->m_Text.X() + List->m_DrawOffsets.m_Width - 2, DrawRect.Top() + 2 + SMALLCHAR_HEIGHT * i }, List->ItemNames()[i], UI_SMALLFONT | List->m_Text.Flags(), EColors::White);

          if(i + MAX_LIST < List->ItemCount())
            DrawProportionalString({ DrawRect.Left() + List->m_Text.X() + 2 + DrawRect.Width(), DrawRect.Top() + 2 + SMALLCHAR_HEIGHT *  i }, List->ItemNames()[i + MAX_LIST], UI_SMALLFONT | List->m_Text.Flags(), EColors::White);
        }
      }
    }
  }


  bool MenuBase::IsWidescreen()
  {
    return ui_handleWidescreen.integer && uis.widescreen.ratio && uis.widescreen.state != WIDESCREEN_NONE;
  }

  void MenuBase::DrawProportionalString(const MenuPosition& Position, const String& Str, int32_t Style, const Color& Color)
  {
    UI_DrawProportionalString(Position.X(), Position.Y(), Str.c_str(), Style, Color);
  }

  void MenuBase::DrawString(const MenuPosition& Position, const String& Str, int32_t Style, const Color& Color, bool HighResolution)
  {
    UI_DrawString(Position.X(), Position.Y(), Str.c_str(), Style, Color, HighResolution ? qtrue : qfalse);
  }

  void MenuBase::DrawChar(const MenuPosition& Position, int32_t Ch, int32_t Style, const Color& Color)
  {
    UI_DrawChar(Position.X(), Position.Y(), Ch, Style, Color);
  }
}
