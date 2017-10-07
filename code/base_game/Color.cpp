#include "Color.h"
#include "../doctest/parts/doctest_fwd.h"


namespace BG
{
  TEST_CASE("Color Construct")
  {
    float TestColor[4]{ 1.0, 0.0, 2.0, 42.0 };

    Color TestColor2(TestColor);
    REQUIRE(TestColor2.Red() == doctest::Approx(1.0));
    REQUIRE(TestColor2.Green() == doctest::Approx(0.0));
    REQUIRE(TestColor2.Blue() == doctest::Approx(2.0));
    REQUIRE(TestColor2.Alpha() == doctest::Approx(42.0));

    Color TestColor3(1.5, 2.3, -2.6, -0.1);
    REQUIRE(TestColor3.Red() == doctest::Approx(1.5));
    REQUIRE(TestColor3.Green() == doctest::Approx(2.3));
    REQUIRE(TestColor3.Blue() == doctest::Approx(-2.6));
    REQUIRE(TestColor3.Alpha() == doctest::Approx(-0.1));

    Color TestColor4{ 23, 312, 4U, 1LU };
    REQUIRE(TestColor4.Red() == doctest::Approx(23.0));
    REQUIRE(TestColor4.Green() == doctest::Approx(312.0));
    REQUIRE(TestColor4.Blue() == doctest::Approx(4.0));
    REQUIRE(TestColor4.Alpha() == doctest::Approx(1.0));
  }
}
