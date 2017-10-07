#pragma once
#include "GraphicsInfo.h"
#include "../../base_game/Color.h"

namespace UI::MenuItems
{
  class ThumbInfo : public GraphicsInfo
  {
  public:

    ThumbInfo(Common::Size Size, const BG::Color& Color, const BG::Color& ColorHighlight, int32_t VisibileGraphicWidth, Common::String Name)
      : GraphicsInfo(Name, Size)
      , m_Color(Color)
      , m_ColorHighlight(ColorHighlight)
      , m_VisibileGraphicWidth(VisibileGraphicWidth)
      , m_Name(Name)
    {
    }

    const BG::Color& Color() const { return m_Color; }
    const BG::Color& ColorHightlight() const { return m_ColorHighlight; }
    const int32_t& VisibileGraphicWidth() const { return m_VisibileGraphicWidth; }

  protected:
    BG::Color m_Color;
    BG::Color m_ColorHighlight;
    int32_t m_VisibileGraphicWidth = 0;
    Common::String m_Name;
  };
}
