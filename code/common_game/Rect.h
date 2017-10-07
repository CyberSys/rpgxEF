#pragma once
#include "Position2D.h"
#include <utility>

namespace Common
{
  namespace Detail
  {
    template<typename T>
    class RectImpl
    {
    public:
      using value_type = T;

      RectImpl() : RectImpl{ 0,0,0,0 } {}

      template<typename A, typename B, typename C, typename D,
        typename = std::enable_if_t<std::is_convertible<A, T>::value && std::is_convertible<B, T>::value && std::is_convertible<C, T>::value && std::is_convertible<D, T>::value>>
        RectImpl(A Top, B Left, C Width, D Height)
        : m_TopLeft(T(Top), T(Left))
        , m_BottomRight(T(Top) + T(Height), T(Left) + T(Width))
      {}

      RectImpl(const RectImpl& Other)
        : m_TopLeft{ Other.TopLeft() }
        , m_BottomRight{ Other.BottomRight() }
      {}

      template<typename A, typename = std::enable_if_t<std::is_convertible<A, T>::value>>
      RectImpl(const RectImpl<A>& Other)
        : m_TopLeft{ Other.TopLeft() }
        , m_BottomRight{ Other.BottomRight() }
      {}

      RectImpl(RectImpl&& Other) noexcept
        : m_TopLeft{ std::move(Other.m_TopLeft) }
        , m_BottomRight{ std::move(Other.m_BottomRight) }
      {}

      template<typename A, typename = std::enable_if_t<std::is_convertible<A, T>::value>>
      RectImpl& operator=(const RectImpl<A>& Other)
      {
        if(this == &Other)
          return *this;
        m_TopLeft = Other.m_TopLeft;
        m_BottomRight = Other.m_BottomRight;
        return *this;
      }

      RectImpl& operator=(RectImpl&& Other) noexcept
      {
        if(this == &Other)
          return *this;
        m_TopLeft = std::move(Other.m_TopLeft);
        m_BottomRight = std::move(Other.m_BottomRight);
        return *this;
      }

      T& Left() { return m_TopLeft.X(); }
      T& Top() { return m_TopLeft.Y(); }
      T& Right() { return m_BottomRight.X(); }
      T& Bottom() { return m_BottomRight.Y(); }

      const T& Left() const { return m_TopLeft.X(); }
      const T& Top() const { return m_TopLeft.Y(); }
      const T& Right() const { return m_BottomRight.X(); }
      const T& Bottom() const { return m_BottomRight.Y(); }

      Position2DImpl<T>& TopLeft() { return m_TopLeft; }
      Position2DImpl<T>& BottomRight() { return m_BottomRight; }

      const Position2DImpl<T>& TopLeft() const { return m_TopLeft; }
      const Position2DImpl<T>& BottomRight() const { return m_BottomRight; }

      T Width() const { return m_BottomRight.X() - m_TopLeft.X(); }
      T Height() const { return m_BottomRight.Y() - m_TopLeft.Y(); }

    protected:
      Position2DImpl<T> m_TopLeft;
      Position2DImpl<T> m_BottomRight;
    };
  }

  using Rect = Detail::RectImpl<int32_t>;
  using RectF = Detail::RectImpl<float>;
}
