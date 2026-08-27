#include <doctest.h>
#include "../src/rtweekend.h"
#include "../src/material.h"

TEST_CASE("reflect and refract geometric math") {
    SUBCASE("vector reflection 45 degrees") {
        vec3 v(1, -1, 0);
        vec3 n(0, 1, 0);
        vec3 r = reflect(v, n);
        CHECK(r.x() == doctest::Approx(1.0));
        CHECK(r.y() == doctest::Approx(1.0));
        CHECK(r.z() == doctest::Approx(0.0));
    }

    SUBCASE("vector reflection normal incidence") {
        vec3 v(0, -1, 0);
        vec3 n(0, 1, 0);
        vec3 r = reflect(v, n);
        CHECK(r.x() == doctest::Approx(0.0));
        CHECK(r.y() == doctest::Approx(1.0));
        CHECK(r.z() == doctest::Approx(0.0));
    }

    SUBCASE("vector refraction perpendicular incidence") {
        vec3 uv(0, -1, 0);
        vec3 n(0, 1, 0);
        double eta = 1.0 / 1.5; // air to glass
        vec3 r = refract(uv, n, eta);
        CHECK(r.x() == doctest::Approx(0.0));
        CHECK(r.y() == doctest::Approx(-1.0));
        CHECK(r.z() == doctest::Approx(0.0));
    }

    SUBCASE("vector refraction Snell's law angle check") {
        // 30 degree incidence in air (sin theta1 = 0.5), entering medium with n=1.5
        // sin theta2 = 0.5 / 1.5 = 1/3 ~ 0.333333
        // ray incoming: (sin 30, -cos 30, 0) = (0.5, -sqrt(3)/2, 0)
        vec3 uv(0.5, -std::sqrt(3.0)/2.0, 0.0);
        vec3 n(0, 1, 0);
        double eta = 1.0 / 1.5;
        vec3 r = refract(uv, n, eta);
        CHECK(r.x() == doctest::Approx(0.5 / 1.5));
        double expected_y = -std::sqrt(1.0 - std::pow(0.5 / 1.5, 2));
        CHECK(r.y() == doctest::Approx(expected_y));
    }
}

TEST_CASE("materials scatter behavior") {
    hit_record rec;
    rec.p = point3(0, 0, 0);
    rec.normal = vec3(0, 1, 0);
    rec.front_face = true;

    SUBCASE("lambertian scattering") {
        lambertian mat(color(0.8, 0.6, 0.2));
        ray r_in(point3(0, 1, 0), vec3(0, -1, 0));
        ray scattered;
        color attenuation;

        bool did_scatter = mat.scatter(r_in, rec, attenuation, scattered);
        CHECK(did_scatter == true);
        CHECK(attenuation.x() == doctest::Approx(0.8));
        CHECK(attenuation.y() == doctest::Approx(0.6));
        CHECK(attenuation.z() == doctest::Approx(0.2));
        CHECK(scattered.origin().x() == doctest::Approx(0.0));
    }

    SUBCASE("metal 0 fuzz scattering") {
        metal mat(color(0.9, 0.9, 0.9), 0.0);
        ray r_in(point3(1, 1, 0), unit_vector(vec3(-1, -1, 0)));
        ray scattered;
        color attenuation;

        bool did_scatter = mat.scatter(r_in, rec, attenuation, scattered);
        CHECK(did_scatter == true);
        CHECK(attenuation.x() == doctest::Approx(0.9));
        CHECK(dot(scattered.direction(), rec.normal) > 0);
    }
}
