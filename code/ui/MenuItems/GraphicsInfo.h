#pragma once
#include "../../base_game/q_shared.h"
#include "../../common_game/Size.h"

namespace UI::MenuItems
{
  class GraphicsInfo
  {
  public:
    GraphicsInfo(const Common::String& Name, Common::Size Size)
      : m_Size(Size)
      , m_Name(Name)
    {
    }

    operator bool() const { return m_Shader != 0; }
    operator qhandle_t() const { return m_Shader; }
    operator const Common::Size&() const { return m_Size; }
    operator const Common::String&() const { return m_Name; }

    const Common::Size::value_type& Width() const { return m_Size.m_Width; }
    const Common::Size::value_type& Height() const { return m_Size.m_Height; }
    const Common::String& Name() const { return m_Name; }

    void Init() { m_Shader = trap_R_RegisterShaderNoMip(m_Name.c_str()); }

  protected:
    qhandle_t m_Shader = 0;
    Common::Size m_Size = { 0,0 };
    Common::String m_Name;
  };
}
