#include "MenuText.h"
#include "RadioButton.h"
#include "../Menus/MenuBase.h"
#include "../../common_game/FlagSupport.h"

using namespace Common;
using namespace BG;

namespace UI::MenuItems
{
  MenuText::MenuText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const Common::String& String, uint32_t TextStyle, const Color& Clr, const Color& ColorHighlight)
    : MenuItem{ EMenuItemType::Text, ID, Position, Flags, Parent }
    , m_NormalTextID{ 0 }
    , m_ButtonTextID{ 0 }
    , m_NormalTextID2{ 0 }
    , m_ButtonTextID2{ 0 }
    , m_NormalTextID3{ 0 }
    , m_ButtonTextID3{ 0 }
    , m_TextStyle{ TextStyle }
    , m_Color{ Clr }
    , m_ColorHighlight{ ColorHighlight }
    , m_FocusPosition{ 0,0 }
    , m_String{ String }
  {}

  MenuText::MenuText(MenuItemID ID, MenuPosition Position, uint32_t Flags, const Common::String& Name, Menus::MenuBase* Parent, uint32_t TextStyle, const Color& Clr, const Color& ColorHighlight)
    : MenuItem{ EMenuItemType::Text, ID, Position, Flags, Parent, Name }
    , m_NormalTextID{ 0 }
    , m_ButtonTextID{ 0 }
    , m_NormalTextID2{ 0 }
    , m_ButtonTextID2{ 0 }
    , m_NormalTextID3{ 0 }
    , m_ButtonTextID3{ 0 }
    , m_TextStyle{ TextStyle }
    , m_Color{ Clr }
    , m_ColorHighlight{ ColorHighlight }
    , m_FocusPosition{ 0,0 }
  {}

  MenuText::MenuText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const ButtonTextInfo& ButtonTextInfo, uint32_t TextStyle, const Color& Clr, const Color& ColorHightlight)
    : MenuItem{ EMenuItemType::Text, ID, Position, Flags, Parent }
    , m_NormalTextID{ 0 }
    , m_ButtonTextID{ ButtonTextInfo.m_ButtonTextID }
    , m_NormalTextID2{ 0 }
    , m_ButtonTextID2{ ButtonTextInfo.m_ButtonTextID2 }
    , m_NormalTextID3{ 0 }
    , m_ButtonTextID3{ ButtonTextInfo.m_ButtonTextID3 }
    , m_TextStyle{ TextStyle }
    , m_Color{ Clr }
    , m_ColorHighlight{ ColorHightlight }
    , m_FocusPosition{ 0,0 }
  {}

  MenuText::MenuText(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, uint32_t TextStyle, const NormalTextInfo& NormalTextInfo, const Color& Clr, const Color& ColorHightlight)
    : MenuItem{ EMenuItemType::Text, ID, Position, Flags, Parent }
    , m_NormalTextID{ NormalTextInfo.m_NormalTextID }
    , m_ButtonTextID{ 0 }
    , m_NormalTextID2{ NormalTextInfo.m_NormalTextID2 }
    , m_ButtonTextID2{ 0 }
    , m_NormalTextID3{ NormalTextInfo.m_NormalTextID2 }
    , m_ButtonTextID3{ 0 }
    , m_TextStyle{ TextStyle }
    , m_Color{ Clr }
    , m_ColorHighlight{ ColorHightlight }
    , m_FocusPosition{ 0,0 }
  {}

  void MenuText::Draw()
  {
    String Buffer;
    String Buffer2;
    String Buffer3;
    int32_t IncY;
    float* Color;

    const auto ValX = X();
    auto ValY = Y();

    if(m_ButtonTextID)
    {
      Buffer = menu_button_text[m_ButtonTextID][0];

      if(m_ButtonTextID2)
        Buffer2 = menu_button_text[m_ButtonTextID2][0];

      if(m_ButtonTextID3)
        Buffer3 = menu_button_text[m_ButtonTextID3][0];
    }
    else if(m_NormalTextID)
    {
      Buffer = menu_normal_text[m_NormalTextID];

      if(m_NormalTextID2)
        Buffer2 = menu_normal_text[m_NormalTextID2];

      if(m_NormalTextID3)
        Buffer3 = menu_normal_text[m_ButtonTextID3];
    }
    else if(!m_Name.empty())
      Buffer = m_Name;
    else if(!m_String.empty())
      Buffer = m_String;

    if(IsSet(m_Flags, QMF_GRAYED))
      Color = colorTable[CT_DKGREY];
    else if(IsSet(m_Flags, QMF_HIGHLIGHT_IF_FOCUS) && m_Parent->ItemAtCursor() == this || IsSet(m_Flags, QMF_HIGHLIGHT))
    {
      if(m_ColorHighlight)
        Color = m_ColorHighlight;
      else
        Color = m_Color;

      if(m_ButtonTextID)
        if(menu_button_text[m_ButtonTextID][1])
          DrawProportionalString(m_Parent->GetDescriptionPosition(), menu_button_text[m_ButtonTextID][1], UI_LEFT | UI_TINYFONT, Color);
    }
    else
      Color = m_Color;

    DrawProportionalString({ ValX, ValY }, Buffer, m_TextStyle, Color);

    if(IsSet(m_TextStyle, UI_TINYFONT))
      IncY = PROP_TINY_HEIGHT * 1.15;
    else if(IsSet(m_TextStyle, UI_SMALLFONT))
      IncY = PROP_HEIGHT * 1.15;
    else
      IncY = PROP_BIG_HEIGHT * 1.15;

    if(!Buffer2.empty())
    {
      ValY += IncY;
      DrawProportionalString({ ValX, ValY }, Buffer2, m_TextStyle, Color);
    }

    if(!Buffer3.empty())
    {
      ValY += IncY;
      DrawProportionalString({ ValX, ValY }, Buffer3, m_TextStyle, Color);
    }
  }

  void MenuText::Init()
  {
    String Buffer;
    String Buffer2;
    String Buffer3;
    int32_t ValX;
    int32_t ValY;
    auto W = 0;
    int32_t H;

    if(m_FocusPosition.X())
    {
      ValX = m_FocusPosition.X();
      ValY = m_FocusPosition.Y();
      W = m_FocusSize.m_Width;
      H = m_FocusSize.m_Height;
    }
    else
    {
      auto W2 = 0;
      auto W3 = 0;

      if(m_ButtonTextID)
      {
        if(menu_button_text[m_ButtonTextID][0])
        {
          Buffer = menu_button_text[m_ButtonTextID][0];
          if(!Buffer.empty())
            W = ProportionalStringWidth(Buffer, m_TextStyle);
        }

        if(m_ButtonTextID2 && menu_button_text[m_ButtonTextID2][0])
        {
          Buffer2 = menu_button_text[m_ButtonTextID2][0];
          if(!Buffer2.empty())
            W2 = ProportionalStringWidth(Buffer2, m_TextStyle);
        }

        if(m_ButtonTextID3 && menu_button_text[m_ButtonTextID3][0])
        {
          Buffer3 = menu_button_text[m_ButtonTextID3][0];
          if(!Buffer3.empty())
            W3 = ProportionalStringWidth(Buffer3, m_TextStyle);
        }

        if(W > W2 && W > W3)
          ;
        else if(W2 > W && W2 > W3)
          W = W2;
        else if(W3 > W && W3 > W2)
          W = W3;
      }
      else if(m_NormalTextID)
      {
        W = W2 = W3 = 0;

        if(menu_normal_text[m_NormalTextID])
        {
          Buffer = menu_normal_text[m_NormalTextID];
          if(!Buffer.empty())
            W = ProportionalStringWidth(Buffer, m_TextStyle);
        }

        if(m_NormalTextID2 && menu_normal_text[m_NormalTextID])
        {
          Buffer2 = menu_normal_text[m_NormalTextID2];
          if(!Buffer2.empty())
            W2 = ProportionalStringWidth(Buffer2, m_TextStyle);
        }

        if(m_NormalTextID3 && menu_normal_text[m_NormalTextID3])
        {
          Buffer3 = menu_normal_text[m_NormalTextID3];
          if(!Buffer3.empty())
            W3 = ProportionalStringWidth(Buffer3, m_TextStyle);
        }

        if(W > W2 && W > W3)
          ;
        else if(W2 > W && W2 > W3)
          W = W2;
        else if(W3 > W && W3 > W2)
          W = W3;
      }
      else if(!m_Name.empty())
      {
        Buffer = m_Name;
        W = ProportionalStringWidth(Buffer, m_TextStyle);
      }

      ValX = X();
      ValY = Y();

      auto Lines = 0;
      if(!Buffer2.empty())
      {
        Lines++;
        if(!Buffer3.empty())
          Lines++;
      }

      if(IsSet(m_TextStyle, UI_TINYFONT))
        H = PROP_TINY_HEIGHT;
      else if(IsSet(m_TextStyle, UI_SMALLFONT))
        H = SMALLCHAR_HEIGHT;
      else if(IsSet(m_TextStyle, UI_BIGFONT))
        H = PROP_BIG_HEIGHT;
      else
        H = SMALLCHAR_HEIGHT;

      H += Lines * H * 1.25;
    }

    if(IsSet(m_Flags, QMF_RIGHT_JUSTIFY))
      ValX -= W;
    else if(IsSet(m_Flags, QMF_RIGHT_JUSTIFY))
      ValX -= W / 2;

    m_Rect.Left() = ValX;
    m_Rect.Right() = ValX + W;
    m_Rect.Top() = ValY;
    m_Rect.Bottom() = ValY + H;
  }
}
