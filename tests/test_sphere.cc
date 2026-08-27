#include <doctest.h>
#include "../src/rtweekend.h"
#include "../src/sphere.h"
#include "../src/material.h"

TEST_CASE("sphere ray intersections") {
    auto dummy_mat = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    sphere s(point3(0, 0, -5), 1.0, dummy_mat);
    hit_record rec;

    SUBCASE("ray through center") {
        ray r(point3(0, 0, 0), vec3(0, 0, -1));
        CHECK(s.hit(r, interval(0.001, infinity), rec) == true);
        CHECK(rec.t == doctest::Approx(4.0));
        CHECK(rec.p.x() == doctest::Approx(0.0));
        CHECK(rec.p.y() == doctest::Approx(0.0));
        CHECK(rec.p.z() == doctest::Approx(-4.0));
        CHECK(rec.normal.x() == doctest::Approx(0.0));
        CHECK(rec.normal.y() == doctest::Approx(0.0));
        CHECK(rec.normal.z() == doctest::Approx(1.0));
        CHECK(rec.front_face == true);
    }

    SUBCASE("tangent ray hit") {
        ray r(point3(1.0, 0, 0), vec3(0, 0, -1));
        CHECK(s.hit(r, interval(0.001, infinity), rec) == true);
        CHECK(rec.t == doctest::Approx(5.0));
        CHECK(rec.p.x() == doctest::Approx(1.0));
        CHECK(rec.p.y() == doctest::Approx(0.0));
        CHECK(rec.p.z() == doctest::Approx(-5.0));
    }

    SUBCASE("miss ray") {
        ray r(point3(2.0, 0, 0), vec3(0, 0, -1));
        CHECK(s.hit(r, interval(0.001, infinity), rec) == false);
    }

    SUBCASE("ray originating inside sphere") {
        ray r(point3(0, 0, -5), vec3(0, 0, 1));
        CHECK(s.hit(r, interval(0.001, infinity), rec) == true);
        CHECK(rec.t == doctest::Approx(1.0));
        CHECK(rec.p.z() == doctest::Approx(-4.0));
        CHECK(rec.front_face == false);
        // Normal should oppose ray direction (inward pointing)
        CHECK(rec.normal.z() == doctest::Approx(-1.0));
    }

    SUBCASE("ray behind sphere") {
        ray r(point3(0, 0, 0), vec3(0, 0, 1));
        CHECK(s.hit(r, interval(0.001, infinity), rec) == false);
    }
}
