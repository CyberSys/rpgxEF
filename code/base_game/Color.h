#pragma once
#include "q_shared.h"
#include <type_traits>
#include <array>

namespace BG
{
  namespace EColors
  {
    enum EColors
    {
      None,
      Black,
      Red,
      Green,
      Blue,
      Yellow,
      Magenta,
      Cyan,
      Teal,
      Gold,
      White,
      LightGrey,
      MediumGrey,
      DarkGrey,
      DarkGrey2,

      VeryLightOrange,
      LightOrange,
      DarkOrange,
      VeryDarkOrange,

      VeryLightBlue1,
      LightBlue1,
      DarkBlue1,
      VeryDarkBlue1,

      VeryLightBlue2,
      LightBlue2,
      DarkBlue2,
      VeryDarkBlue2,

      VeryLightBrown1,
      LightBrown1,
      DarkBrown1,
      VeryDarkBrown1,

      VeryLightGold1,
      LightGold1,
      DarkGold1,
      VeryDarkGold1,

      VeryLightPurple1,
      LightPurple1,
      DarkPurple1,
      VeryDarkPurple1,

      VeryLightPurple2,
      LightPurple2,
      DarkPurple2,
      VeryDarkPurple2,

      VeryLightPurple3,
      LightPurple3,
      DarkPurple3,
      VeryDarkPurple3,

      VeryLightRed1,
      LightRed1,
      DarkRed1,
      VeryDarkRed1,

      Max
    };
  }

  class Color
  {
  public:
    constexpr Color() : Color{ EColors::None } {}

    template<typename R, typename G, typename B, typename A,
      typename = std::enable_if_t<std::is_convertible<R, float>::value
      || std::is_convertible<G, float>::value
      || std::is_convertible<B, float>::value
      || std::is_convertible<A, float>::value>>
      constexpr Color(R Red, G Green, B Blue, A Alpha)
      : m_RGBA{ static_cast<float>(Red) , static_cast<float>(Green) , static_cast<float>(Blue) , static_cast<float>(Alpha) }
    {}

    constexpr Color(std::array<float, 4> RGBA)
      : m_RGBA{ RGBA[0], RGBA[1], RGBA[2], RGBA[3] }
    {}

    constexpr Color(float RGBA[4])
      : m_RGBA{ RGBA[0], RGBA[1], RGBA[2], RGBA[3] }
    {}

    constexpr Color(EColors::EColors Color);

    constexpr Color(const Color& Other)
      : m_RGBA{ Other.m_RGBA }
    {}

    constexpr Color(Color&& Other) noexcept
      : m_RGBA{ std::move(Other.m_RGBA) }
    {}

    Color& operator=(const Color& Other)
    {
      if(this == &Other)
        return *this;
      m_RGBA = Other.m_RGBA;
      return *this;
    }

    Color& operator=(Color&& Other) noexcept
    {
      if(this == &Other)
        return *this;
      m_RGBA = std::move(Other.m_RGBA);
      return *this;
    }

    float& Red() { return m_RGBA.m_Red; }
    float& Green() { return m_RGBA.m_Green; }
    float& Blue() { return m_RGBA.m_Blue; }
    float& Alpha() { return m_RGBA.m_Alpha; }

    constexpr const float& Red() const { return m_RGBA.m_Red; }
    constexpr const float& Green() const { return m_RGBA.m_Green; }
    constexpr const float& Blue() const { return m_RGBA.m_Blue; }
    constexpr const float& Alpha() const { return m_RGBA.m_Alpha; }

    constexpr operator float*() { return m_RGBA.m_Array.data(); }
    constexpr operator const float*() const { return m_RGBA.m_Array.data(); }
    constexpr operator bool() const { return m_RGBA.m_Red != 0.0 && m_RGBA.m_Green != 0.0 && m_RGBA.m_Blue != 0.0 && m_RGBA.m_Alpha != 0.0; }

  protected:
    union
    {
      struct
      {
        float m_Red;
        float m_Green;
        float m_Blue;
        float m_Alpha;
      };
      std::array<float, 4> m_Array;
    } m_RGBA;
  };

  namespace Colors
  {
    constexpr const Color ColorBlack = { 0, 0, 0, 1 };
    constexpr const Color ColorRed = { 1, 0, 0, 1 };
    constexpr const Color ColorGreen = { 0, 1, 0, 1 };
    constexpr const Color ColorBlue = { 0, 0, 1, 1 };
    constexpr const Color ColorYellow = { 1, 1, 0, 1 };
    constexpr const Color ColorMagenta = { 1, 0, 1, 1 };
    constexpr const Color ColorCyan = { 0, 1, 1, 1 };
    constexpr const Color ColorWhite = { 1, 1, 1, 1 };
    constexpr const Color ColorLtGrey = { 0.75, 0.75, 0.75, 1 };
    constexpr const Color ColorMdGrey = { 0.5, 0.5, 0.5, 1 };
    constexpr const Color ColorDkGrey = { 0.25, 0.25, 0.25, 1 };
  }

  namespace Detail
  {
    constexpr const Color ColorTable[] =
    {
      { 0, 0, 0, 0 },			 /* CT_NONE */
      { 0, 0, 0, 1 },			 /* CT_BLACK */
      { 1, 0, 0, 1 },			 /* CT_RED */
      { 0, 1, 0, 1 },			 /* CT_GREEN */
      { 0, 0, 1, 1 },			 /* CT_BLUE */
      { 1, 1, 0, 1 },			 /* CT_YELLOW */
      { 1, 0, 1, 1 },			 /* CT_MAGENTA */
      { 0, 1, 1, 1 },			 /* CT_CYAN */
      { 0.071f, 0.271f, 0.29f, 1 }, /* CT_TEAL */
      { 0.529f, 0.373f, 0.017f, 1 },/* CT_GOLD */
      { 1, 1, 1, 1 },			 /* CT_WHITE */
      { 0.75f, 0.75f, 0.75f, 1 },	 /* CT_LTGREY */
      { 0.50f, 0.50f, 0.50f, 1 },	 /* CT_MDGREY */
      { 0.25f, 0.25f, 0.25f, 1 },	 /* CT_DKGREY */
      { 0.15f, 0.15f, 0.15f, 1 },	 /* CT_DKGREY2 */

      { 0.688f, 0.797f, 1, 1 },	 /* CT_VLTORANGE -- needs values */
      { 0.688f, 0.797f, 1, 1 },	 /* CT_LTORANGE */
      { 0.620f, 0.710f, 0.894f, 1 },/* CT_DKORANGE */
      { 0.463f, 0.525f, 0.671f, 1 },/* CT_VDKORANGE */

      { 0.616f, 0.718f, 0.898f, 1 },/* CT_VLTBLUE1 */
      { 0.286f, 0.506f, 0.898f, 1 },/* CT_LTBLUE1 */
      { 0.082f, 0.388f, 0.898f, 1 },/* CT_DKBLUE1 */
      { 0.063f, 0.278f, 0.514f, 1 },/* CT_VDKBLUE1 */

      { 0.302f, 0.380f, 0.612f, 1 },/* CT_VLTBLUE2 -- needs values */
      { 0.196f, 0.314f, 0.612f, 1 },/* CT_LTBLUE2 */
      { 0.060f, 0.227f, 0.611f, 1 },/* CT_DKBLUE2 */
      { 0.043f, 0.161f, 0.459f, 1 },/* CT_VDKBLUE2 */

      { 0.082f, 0.388f, 0.898f, 1 },/* CT_VLTBROWN1 -- needs values */
      { 0.082f, 0.388f, 0.898f, 1 },/* CT_LTBROWN1 */
      { 0.078f, 0.320f, 0.813f, 1 },/* CT_DKBROWN1 */
      { 0.060f, 0.227f, 0.611f, 1 },/* CT_VDKBROWN1 */

      { 1, 0.784f, 0.365f, 1 },	 /* CT_VLTGOLD1 -- needs values */
      { 1, 0.706f, 0.153f, 1 },	 /* CT_LTGOLD1 */
      { 0.733f, 0.514f, 0.086f, 1 },/* CT_DKGOLD1 */
      { 0.549f, 0.384f, 0.063f, 1 },/* CT_VDKGOLD1 */

      { 0.688f, 0.797f, 1, 1 },	 /* CT_VLTPURPLE1 -- needs values */
      { 0.688f, 0.797f, 1, 1 },	 /* CT_LTPURPLE1 */
      { 0.313f, 0.578f, 1, 1 },	 /* CT_DKPURPLE1 */
      { 0.031f, 0.110f, 0.341f, 1 },/* CT_VDKPURPLE1 */

      { 0.688f, 0.797f, 1, 1 },	 /* CT_VLTPURPLE2 -- needs values */
      { 0.688f, 0.797f, 1, 1 },	 /* CT_LTPURPLE2 */
      { 0.688f, 0.797f, 1, 1 },	 /* CT_DKPURPLE2 */
      { 0.031f, 0.110f, 0.341f, 1 },/* CT_VDKPURPLE2 */

      { 0.686f, 0.808f, 0.1f, 1 },	 /* CT_VLTPURPLE3 */
      { 0.188f, 0.494f, 1, 1 },	 /* CT_LTPURPLE3 */
      { 0.094f, 0.471f, 1, 1 },	 /* CT_DKPURPLE3 */
      { 0.067f, 0.325f, 0.749f, 1 },/* CT_VDKPURPLE3 */

      { 1, 0.612f, 0.325f, 1 },	 /* CT_VLTRED1 */
      { 1, 0.478f, 0.098f, 1 },	 /* CT_LTRED1 */
      { 1, 0.438f, 0, 1 },		 /* CT_DKRED1 */
      { 0.784f, 0.329f, 0, 1 },	 /* CT_VDKRED1 */
    };
  }

  namespace Detail
  {
    constexpr Color ColorTableLookup(EColors::EColors Color)
    {
      return ColorTableLookup(Color);
    }
  }

  constexpr Color::Color(EColors::EColors Color)
    : Color{ Detail::ColorTableLookup(Color) }
  {}
}
