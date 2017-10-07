#include "CdKeyMenu.h"

using namespace BG;

namespace UI::Menus
{
  void CdKeyMenu::OnDraw()
  {
    Graphics();
    // TODO: Menu_Draw in base class
  }

  int32_t CdKeyMenu::PreValidateKey(const Common::String& Key)
  {
    if(Key.length() > 22) return -1;

    auto Count = std::count_if(Key.begin(), Key.end(), [](Common::String::value_type V) { return std::isalnum(V); });

    if(Count < 16 || Count > 18) return -1;

    return 0;
  }

  void CdKeyMenu::Event(void* Ptr, int32_t Event)
  {
    if(Event == QM_ACTIVATED) return;

    switch(reinterpret_cast<menucommon_s*>(Ptr)->id)
    {
    case Accept:
      {
        m_KeyResult = PreValidateKey(m_CdKey.field.buffer);

        if(m_KeyResult == 0)
        {
          if(trap_SetCDKey(m_CdKey.field.buffer))
          {
            trap_Cvar_Set("ui_cdkeychecked", "1");

            if(m_FromMenu)
            {
              UI_PopMenu();
            }
          }
          else
          {
            m_KeyResult = -1;
            trap_S_StartLocalSound(menu_buzz_sound, CHAN_MENU1);
          }
        }
        else if(m_KeyResult < 0)
        {
          trap_S_StartLocalSound(menu_buzz_sound, CHAN_MENU1);
        }
        break;
      }
    case AcceptLater:
    default:
      {
        if(m_KeyResult != 0)
          trap_Cvar_Set("ui_cdkeychecked", "-1");
        PopMenu();
        break;
      }
    }
  }

  void CdKeyMenu::DrawKey()
  {
    auto Focus = m_Cursor == m_CdKey.generic.menuPosition;
    auto Style = UI_LEFT;
    auto Color = Focus ? colorTable[CT_LTGOLD1] : colorTable[CT_DKGOLD1];
    auto X = m_CdKey.generic.x;
    auto Y = m_CdKey.generic.y;

    FillRect({ X, Y, m_CdKey.field.widthInChars * SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT }, EColors::Black);
    FillRect({ X, Y, m_CdKey.field.widthInChars * SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT }, listbar_color);
    DrawString({ X, Y }, m_CdKey.field.buffer, Style, Color, true);

    if(Focus)
    {
      Common::char_t C = trap_Key_GetOverstrikeMode() ? 11 : 10;
      Style &= ~UI_PULSE;
      Style |= UI_BLINK;
      DrawChar({ X + m_CdKey.field.cursor * SMALLCHAR_WIDTH, Y }, C, Style, colorWhite);
    }
  }

  void CdKeyMenu::Graphics()
  {
    MenuFrame();

    SetColor(EColors::LightBrown1);
    DrawPicture({ 30, 203, 47, 186 }, uis.whiteShader);

    SetColor(EColors::DarkPurple1);
    DrawPicture({ 246, 238, 197, 24 }, uis.whiteShader);
    DrawPicture({ 227, 238, -16, 32 }, m_Halfround);	// Left round
    DrawPicture({ 446, 238, 16, 32 }, m_Halfround);	// Right round

    DrawProportionalString({ 74, 66 }, "557", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
    DrawProportionalString({ 74, 84 }, "2344", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
    DrawProportionalString({ 74, 188 }, "89-35", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
    DrawProportionalString({ 74, 206 }, "32906", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);
    DrawProportionalString({ 74, 395 }, "30-1789", UI_RIGHT | UI_TINYFONT, colorTable[CT_BLACK]);

    const MenuPosition Pos = { 344, 279 };

    if(m_KeyResult == 0)
      DrawProportionalString(Pos, menu_normal_text[MNT_VALID_CDKEY], UI_CENTER | UI_SMALLFONT, colorTable[CT_WHITE]);
    else if(m_KeyResult == 1)
      DrawProportionalString(Pos, menu_normal_text[MNT_ENTER_CDKEY], UI_CENTER | UI_SMALLFONT, colorTable[CT_LTGOLD1]);
    else
      DrawProportionalString(Pos, menu_normal_text[MNT_CDKEY_INVALID], UI_CENTER | UI_SMALLFONT, colorTable[CT_RED]);

    SetupMenuButtons();
  }
}
