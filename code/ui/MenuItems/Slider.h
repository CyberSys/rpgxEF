#pragma once
#include "MenuItem.h"
#include "../../common_game/Size.h"
#include "../../common_game/Range.h"
#include "GraphicsInfo.h"
#include "ThumbInfo.h"
#include "TextInfo.h"
#include "PictureInfo.h"

namespace UI::MenuItems
{
  class Slider : public MenuItem
  {
  public:

    void Draw() override;
    void Init() override;
    virtual sfxHandle_t Key(int32_t& Key) override;

  protected:
    bool m_MouseDown;
    float m_CurrentValue;
    float m_DefaultValue;
    float m_Range;
    BG::Color m_Color;
    BG::Color m_ColorHighlight;
    Common::RangeF m_Bounds;
    Common::Size m_FocusSize;
    GraphicsInfo m_Graph;
    ThumbInfo m_Thumb;
    TextInfo m_Text;
    PictureInfo m_Picture;
  };
}
