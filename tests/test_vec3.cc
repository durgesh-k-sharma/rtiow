#include <doctest.h>
#include "../src/vec3.h"
#include "../src/rtweekend.h"

TEST_CASE("vec3 basic operations") {
    vec3 u(1.0, 2.0, 3.0);
    vec3 v(4.0, 5.0, 6.0);

    SUBCASE("component access") {
        CHECK(u.x() == doctest::Approx(1.0));
        CHECK(u.y() == doctest::Approx(2.0));
        CHECK(u.z() == doctest::Approx(3.0));
        CHECK(u[0] == doctest::Approx(1.0));
        CHECK(u[1] == doctest::Approx(2.0));
        CHECK(u[2] == doctest::Approx(3.0));
    }

    SUBCASE("unary minus") {
        vec3 neg = -u;
        CHECK(neg.x() == doctest::Approx(-1.0));
        CHECK(neg.y() == doctest::Approx(-2.0));
        CHECK(neg.z() == doctest::Approx(-3.0));
    }

    SUBCASE("addition and subtraction") {
        vec3 sum = u + v;
        CHECK(sum.x() == doctest::Approx(5.0));
        CHECK(sum.y() == doctest::Approx(7.0));
        CHECK(sum.z() == doctest::Approx(9.0));

        vec3 diff = v - u;
        CHECK(diff.x() == doctest::Approx(3.0));
        CHECK(diff.y() == doctest::Approx(3.0));
        CHECK(diff.z() == doctest::Approx(3.0));
    }

    SUBCASE("multiplication and division") {
        vec3 scaled = 2.0 * u;
        CHECK(scaled.x() == doctest::Approx(2.0));
        CHECK(scaled.y() == doctest::Approx(4.0));
        CHECK(scaled.z() == doctest::Approx(6.0));

        vec3 div = scaled / 2.0;
        CHECK(div.x() == doctest::Approx(1.0));
        CHECK(div.y() == doctest::Approx(2.0));
        CHECK(div.z() == doctest::Approx(3.0));

        vec3 prod = u * v;
        CHECK(prod.x() == doctest::Approx(4.0));
        CHECK(prod.y() == doctest::Approx(10.0));
        CHECK(prod.z() == doctest::Approx(18.0));
    }

    SUBCASE("length and length_squared") {
        vec3 a(3.0, 4.0, 0.0);
        CHECK(a.length_squared() == doctest::Approx(25.0));
        CHECK(a.length() == doctest::Approx(5.0));

        vec3 unit = unit_vector(a);
        CHECK(unit.length() == doctest::Approx(1.0));
        CHECK(unit.x() == doctest::Approx(0.6));
        CHECK(unit.y() == doctest::Approx(0.8));
        CHECK(unit.z() == doctest::Approx(0.0));
    }

    SUBCASE("dot product") {
        CHECK(dot(u, v) == doctest::Approx(1*4 + 2*5 + 3*6)); // 4 + 10 + 18 = 32
        CHECK(dot(vec3(1, 0, 0), vec3(0, 1, 0)) == doctest::Approx(0.0));
    }

    SUBCASE("cross product") {
        vec3 cx = cross(vec3(1, 0, 0), vec3(0, 1, 0));
        CHECK(cx.x() == doctest::Approx(0.0));
        CHECK(cx.y() == doctest::Approx(0.0));
        CHECK(cx.z() == doctest::Approx(1.0));

        vec3 cy = cross(vec3(0, 1, 0), vec3(0, 0, 1));
        CHECK(cy.x() == doctest::Approx(1.0));
        CHECK(cy.y() == doctest::Approx(0.0));
        CHECK(cy.z() == doctest::Approx(0.0));
    }
}
