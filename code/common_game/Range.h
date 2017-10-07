#pragma once
#include <type_traits>
#include <cstdint>

namespace Common
{
  namespace Detail
  {
    template<typename T, typename = std::enable_if_t<std::is_integral<T>::value || std::is_floating_point<T>::value>>
    class RangeTempl
    {
    public:
      using value_type = T;

      template<typename A, typename B, typename = std::enable_if_t<std::is_convertible<A, T>::value && std::is_convertible<B, T>::value>>
      RangeTempl(A Min, B Max, bool Closed)
        : m_Min(T(Min))
        , m_Max(T(Max))
        , m_Closed(Closed)
      {
        if(m_Min > m_Max)
          std::swap(m_Min, m_Max);
      }

      template<typename A, typename = std::enable_if_t<std::is_convertible<A, T>::value>>
      RangeTempl(const RangeTempl<A>& Other)
        : m_Min{T(Other.m_Min)}
        , m_Max{T(Other.m_Max)}
        , m_Closed{Other.m_Closed}
      {
      }
      
      RangeTempl(RangeTempl&& Other) noexcept
        : m_Min{std::move(Other.m_Min)}
        , m_Max{std::move(Other.m_Max)}
        , m_Closed{Other.m_Closed}
      {
      }

      template<typename A, typename = std::enable_if_t<std::is_convertible<A, T>::value>>
      RangeTempl& operator=(const RangeTempl<A>& Other)
      {
        if (this == &Other)
          return *this;
        m_Min = T(Other.m_Min);
        m_Max = T(Other.m_Max);
        m_Closed = Other.m_Closed;
        return *this;
      }

      RangeTempl& operator=(RangeTempl&& Other) noexcept
      {
        if (this == &Other)
          return *this;
        m_Min = std::move(Other.m_Min);
        m_Max = std::move(Other.m_Max);
        m_Closed = Other.m_Closed;
        return *this;
      }

      bool Closed() const { return m_Closed; }
      T Max() const { return m_Max; }
      T Min() const { return m_Min; }
      T Distance() const { return m_Max - m_Min; }
      
      template<typename V, typename = std::enable_if_t<std::is_integral<V>::value || std::is_floating_point<V>::value>>
      bool Check(V Value)
      {
        return m_Closed ? Value >= m_Min && Value <= m_Max : Value > m_Min && Value < m_Max;
      }

    private:
      T m_Min;
      T m_Max;
      bool m_Closed;
    };
  }

  using Range = Detail::RangeTempl<int32_t>;
  using RangeF = Detail::RangeTempl<float>;
}
