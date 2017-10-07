#pragma once
#include <type_traits>

namespace Common
{
  template<typename V, typename T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_integral<V>::value>>
  bool IsSet(V Flags, T Value)
  {
    return (Flags & static_cast<V>(Value)) == static_cast<V>(Value);
  }

  template<typename V, typename T, typename = std::enable_if_t<std::is_enum<T>::value>, typename = void>
  bool IsSet(V Flags, T Value)
  {
    return IsSet(Flags, static_cast<typename std::underlying_type<T>::type>(Value));
  }

  template<typename V, typename T, typename = std::enable_if_t<std::is_integral<T>::value && std::is_integral<V>::value>>
  bool NotSet(V Flags, T Value)
  {
    return !IsSet(Flags, Value);
  }

  template<typename V, typename T, typename = std::enable_if_t<std::is_enum<T>::value>, typename = void>
  bool NotSet(V Flags, T Value)
  {
    return NotSet(Flags, static_cast<typename std::underlying_type<T>::type>(Value));
  }

}
