#include <doctest.h>
#include "../src/rtweekend.h"
#include "../src/aabb.h"

TEST_CASE("aabb operations and ray intersection") {
    aabb box(point3(-1, -1, -1), point3(1, 1, 1));

    SUBCASE("ray through center of AABB") {
        ray r(point3(0, 0, -5), vec3(0, 0, 1));
        CHECK(box.hit(r, interval(0.001, infinity)) == true);
    }

    SUBCASE("ray missing AABB") {
        ray r(point3(2, 2, -5), vec3(0, 0, 1));
        CHECK(box.hit(r, interval(0.001, infinity)) == false);
    }

    SUBCASE("ray inside AABB") {
        ray r(point3(0, 0, 0), vec3(0, 0, 1));
        CHECK(box.hit(r, interval(0.001, infinity)) == true);
    }

    SUBCASE("aabb union") {
        aabb box1(point3(0, 0, 0), point3(2, 2, 2));
        aabb box2(point3(-2, -2, -2), point3(0, 0, 0));
        aabb combined(box1, box2);

        CHECK(combined.x.min == doctest::Approx(-2.0));
        CHECK(combined.x.max == doctest::Approx(2.0));
        CHECK(combined.y.min == doctest::Approx(-2.0));
        CHECK(combined.y.max == doctest::Approx(2.0));
        CHECK(combined.z.min == doctest::Approx(-2.0));
        CHECK(combined.z.max == doctest::Approx(2.0));
    }
}
