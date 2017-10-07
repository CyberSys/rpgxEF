#pragma once
#include <cstdint>

namespace Common
{
  namespace Detail
  {
    template<typename T>
    class SizeTempl
    {
    public:
      using value_type = T;

      T m_Width;
      T m_Height;
    };
  }

  using Size = Detail::SizeTempl<int32_t>;
}
