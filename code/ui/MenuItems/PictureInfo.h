#pragma once
#include "../ui_local.h"
#include "../../common_game/Rect.h"
#include "../../common_game/String.h"

namespace UI::MenuItems
{
  class PictureInfo
  {
  public:
    PictureInfo(const Common::String& Name, Common::RectF Rect)
      : m_Rect(Rect)
      , m_Name(Name)
    {
    }

    operator bool() const { return m_Shader != 0; }
    operator qhandle_t() const { return m_Shader; }
    operator const Common::String&() const { return m_Name; }
    operator const Common::RectF&() const { return m_Rect; }

    const Common::RectF::value_type& X() const { return m_Rect.Left(); }
    const Common::RectF::value_type& Y() const { return m_Rect.Top(); }
    const Common::RectF::value_type& Width() const { return m_Rect.Width(); }
    const Common::RectF::value_type& Heigth() const { return m_Rect.Height(); }

    const Common::String& Name() const { return m_Name; }
    void Init() { m_Shader = trap_R_RegisterShaderNoMip(m_Name.c_str()); }

  protected:
    qhandle_t m_Shader = 0;
    Common::RectF m_Rect = { 0,0,0,0 };
    Common::String m_Name;
  };
}
