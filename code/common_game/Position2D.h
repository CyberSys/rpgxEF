#pragma once

#include <cstdint>
#include <algorithm>

namespace Common
{
  namespace Detail
  {
    template<typename T>
    class Position2DImpl
    {
    public:
      using value_type = T;

      Position2DImpl() : m_Coords{ 0,0 } {}

      template<typename A, typename B, typename = std::enable_if_t < std::is_convertible<A, T>::value && std::is_convertible<B, T>::value>>
      Position2DImpl(A X, B Y) : m_Coords{ T(X), T(Y) } {}

      template<typename A, typename = std::enable_if_t<std::is_convertible<A, T>::value>>
      Position2DImpl(A X) : m_Coords{ {T(X), 0} } {}

      template<typename A, typename = std::enable_if_t<std::is_convertible<A, T>::value>>
      Position2DImpl(const Position2DImpl<A>& Other)
      {
        m_Coords[0] = T(Other.m_Coords[0]);
        m_Coords[1] = T(Other.m_Coords[1]);
      }

      template<typename A, typename = std::enable_if_t<std::is_convertible<A, T>::value>>
      Position2DImpl(Position2DImpl<A>&& Other) noexcept
      {
        m_Coords[0] = T(Other.m_Coords[0]);
        m_Coords[1] = T(Other.m_Coords[1]);
      }

      template<typename A, typename = std::enable_if_t<std::is_convertible<A, T>::value>>
      Position2DImpl& operator=(const Position2DImpl<A>& Other)
      {
        if(this == &Other)
          return *this;

        m_Coords[0] = T(Other.m_Coords[0]);
        m_Coords[1] = T(Other.m_Coords[1]);

        return *this;
      }

      template<typename A, typename = std::enable_if_t<std::is_convertible<A, T>::value>>
      Position2DImpl& operator=(Position2DImpl<A>&& Other) noexcept
      {
        if(this == &Other)
          return *this;

        m_Coords[0] = T(Other.m_Coords[0]);
        m_Coords[1] = T(Other.m_Coords[1]);

        return *this;
      }

      T& X() { return m_Coords[0]; }
      T& Y() { return m_Coords[1]; }
      const T& X() const { return m_Coords[0]; }
      const T& Y() const { return m_Coords[1]; }
      T& operator[](size_t Index) { return Index >= 1 ? m_Coords[1] : m_Coords[0]; }
      const T& operator[](size_t Index) const { return Index >= 1 ? m_Coords[1] : m_Coords[0]; }

      T m_Coords[2] = { 0, 0 };
    };
  }

  using Position2D = Detail::Position2DImpl<int32_t>;
}
