#include "Bitmap.h"
#include "../ui_local.h"
#include "../../common_game/FlagSupport.h"
#include "../../base_game/q_shared.h"
#include "../Menus/MenuBase.h"

using namespace BG;

using namespace Common;

extern vec4_t pulse_color;

namespace UI::MenuItems
{
  Bitmap::Bitmap(MenuItemID ID, MenuPosition Position, uint32_t Flags, Menus::MenuBase* Parent, const String& Name, Size Size, const Color& Clr, const Color& ColorHightlight, String FocusShaderName, TextInfo Text): MenuItem{EMenuItemType::Bitmap, ID, Position, Flags, Parent, Name}
    , m_Shader{0}
    , m_ShaderFocus{0}
    , m_FocusSize{0,0}
    , m_FocusPosition{0,0}
    , m_Size{Size}
    , m_ColorFocus{nullptr}
    , m_Color{Clr}
    , m_ColorHightlight{ColorHightlight}
    , m_FocusShaderName{FocusShaderName}
    , m_Text{Text}
  {
  }

  void Bitmap::Draw()
  {
    auto TextStyle = m_Text.Flags();
    auto ValX = X();
    auto Highlight = false;
    vec4_t TempColor;
    float* Color;

    if(IsSet(m_Flags, QMF_RIGHT_JUSTIFY))
      ValX -= m_Size.m_Width;
    else if(IsSet(m_Flags, QMF_CENTER_JUSTIFY))
      ValX -= m_Size.m_Width / 2;

    if(!m_Name.empty() && !m_Shader)
      m_Shader = trap_R_RegisterShaderNoMip(m_Name.c_str());

    if(!m_FocusShaderName.empty() && !m_ShaderFocus)
      m_ShaderFocus = trap_R_RegisterShaderNoMip(m_FocusShaderName.c_str());

    if(IsSet(m_Flags, QMF_GRAYED))
    {
      if(m_Shader)
      {
        SetColor(colorMdGrey);
        DrawPicture({ ValX, Y(), m_Size.m_Width, m_Size.m_Height }, m_Shader);
        ResetColor();
      }
    } 
    else if(IsSet(m_Flags, QMF_HIGHLIGHT_IF_FOCUS) && m_Parent->ItemAtCursor() == this || IsSet(m_Flags, QMF_HIGHLIGHT))
    {
      if(m_ColorHightlight)
      {
        SetColor(m_ColorHightlight);
        Highlight = true;
      }

      if(m_Shader)
        DrawPicture({ ValX, Y(), m_Size.m_Width, m_Size.m_Height }, m_Shader);

      auto Text1_2 = m_Text.GetText2();
      if(!Text1_2.empty())
        DrawProportionalString(m_Parent->GetDescriptionPosition(), Text1_2, UI_LEFT | UI_TINYFONT, EColors::Black);

      ResetColor();
    }
    else
    {
      if(m_Shader)
      {
        if(m_Color)
        {
          SetColor(m_Color);
        }

        DrawPicture({ ValX, Y(), m_Size.m_Width, m_Size.m_Height }, m_Shader);
      }

      if(IsSet(m_Flags, QMF_PULSE) || IsSet(m_Flags, QMF_PULSEIFFOCUS) && m_Parent->ItemAtCursor() == this)
      {
        if(m_ColorFocus)
        {
          TempColor[0] = m_ColorFocus[0];
          TempColor[1] = m_ColorFocus[1];
          TempColor[2] = m_ColorFocus[2];
          Color = TempColor;
        }
        else
          Color = pulse_color;
        Color[3] = 0.5 + 0.5 * sin(uis.realtime / PULSE_DIVISOR);

        SetColor(Color);
        DrawPicture({ ValX, Y(), m_Size.m_Width, m_Size.m_Height }, m_ShaderFocus);
        ResetColor();
      }
      else if(IsSet(m_Flags, QMF_HIGHLIGHT) || IsSet(m_Flags, QMF_HIGHLIGHT_IF_FOCUS) && m_Parent->ItemAtCursor() == this)
      {
        if(m_ColorFocus)
        {
          SetColor(m_ColorFocus);
          DrawPicture({ ValX, Y(), m_Size.m_Width, m_Size.m_Height }, m_ShaderFocus);
          ResetColor();
        }
        else
          DrawPicture({ ValX, Y(), m_Size.m_Width, m_Size.m_Height }, m_ShaderFocus);
      }
    }

    if(m_Text || !m_TextStr.empty())
    {
      BG::Color ColorI;

      if(Highlight)
        ColorI = m_Text.ColorHightlight() ? m_Text.ColorHightlight() : EColors::White;
      else
        ColorI = m_Text.Color() ? m_Text.Color() : EColors::White;

      if(m_Text)
        DrawProportionalString({ ValX + m_Text.X(), Y() + m_Text.Y() }, m_Text, TextStyle, ColorI);
      else
        DrawProportionalString({ ValX + m_Text.X(), Y() + m_Text.Y() }, m_TextStr, TextStyle, ColorI);

      if(m_Text2)
      {
        int32_t IncY;
        if(IsSet(TextStyle, UI_SMALLFONT))
          IncY = PROP_HEIGHT * 1.15;
        else if(IsSet(TextStyle, UI_TINYFONT))
          IncY = PROP_TINY_HEIGHT * 1.15;
        else
          IncY = PROP_HEIGHT * 1.15;

        DrawProportionalString({ ValX + m_Text.X(), Y() + m_Text.Y() + IncY }, m_Text2, TextStyle, ColorI);
      }
    }
  }

  void Bitmap::Init()
  {
    auto ValX = X();
    auto H = m_Size.m_Height;
    auto W = m_Size.m_Width;

    if(W < 0)
      W = -W;
    if(H < 0)
      H = -H;

    if(IsSet(m_Flags, QMF_RIGHT_JUSTIFY))
      ValX -= W;
    else if(IsSet(m_Flags, QMF_CENTER_JUSTIFY))
      ValX -= W / 2;

    m_Rect.Left() = ValX;
    m_Rect.Right() = ValX + W;
    m_Rect.Top() = Y();
    m_Rect.Bottom() = Y() + H;
  }
}

