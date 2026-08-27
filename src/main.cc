#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "bvh.h"

#include <iostream>
#include <string>
#include <cstring>
#include <memory>

// Material specifically for visualizing surface normals
class normal_material : public material {
public:
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        (void)r_in;
        (void)scattered;
        // Map normal components from [-1, 1] to [0, 1]
        attenuation = 0.5 * color(rec.normal.x() + 1.0, rec.normal.y() + 1.0, rec.normal.z() + 1.0);
        return false; // Absorb ray, emitting surface normal color directly
    }
};

// Normal visualizer ray color fallback for camera
class normal_visualizer_camera : public camera {
public:
    // When using normal visualization directly without bounce:
};

hittable_list make_normals_scene() {
    hittable_list world;
    auto mat = make_shared<normal_material>();
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, mat));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, mat));
    return world;
}

hittable_list make_materials_scene() {
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.4, material_bubble));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    return world;
}

hittable_list make_final_scene() {
    set_random_seed(1337);
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

int main(int argc, char* argv[]) {
    std::string scene_name = "final";
    int image_width = 400;
    int samples_per_pixel = 100;
    int max_depth = 50;
    int num_threads = 0;
    bool use_bvh = true;
    std::string output_png = "";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--scene" && i + 1 < argc) {
            scene_name = argv[++i];
        } else if (arg == "--width" && i + 1 < argc) {
            image_width = std::stoi(argv[++i]);
        } else if (arg == "--samples" && i + 1 < argc) {
            samples_per_pixel = std::stoi(argv[++i]);
        } else if (arg == "--depth" && i + 1 < argc) {
            max_depth = std::stoi(argv[++i]);
        } else if (arg == "--threads" && i + 1 < argc) {
            num_threads = std::stoi(argv[++i]);
        } else if (arg == "--no-bvh") {
            use_bvh = false;
        } else if (arg == "--bvh") {
            use_bvh = true;
        } else if (arg == "--png" && i + 1 < argc) {
            output_png = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  --scene <sky|normals|materials|final>  Select scene to render (default: final)\n"
                      << "  --width <pixels>                       Render width (default: 400)\n"
                      << "  --samples <count>                      Samples per pixel (default: 100)\n"
                      << "  --depth <count>                        Max ray bounce depth (default: 50)\n"
                      << "  --threads <count>                      Worker threads (0 = auto)\n"
                      << "  --bvh / --no-bvh                       Enable/disable BVH acceleration\n"
                      << "  --png <filename.png>                   Output directly to PNG\n";
            return 0;
        }
    }

    camera cam;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.num_threads       = num_threads;

    hittable_list world;

    if (scene_name == "sky" || scene_name == "m1") {
        // Sky gradient background
        cam.aspect_ratio = 16.0 / 9.0;
        cam.vfov = 90;
        cam.lookfrom = point3(0,0,0);
        cam.lookat = point3(0,0,-1);
        cam.vup = vec3(0,1,0);
        cam.defocus_angle = 0;
    } else if (scene_name == "normals" || scene_name == "m2") {
        // Surface normal visualization
        cam.aspect_ratio = 16.0 / 9.0;
        cam.vfov = 90;
        cam.lookfrom = point3(0,0,0);
        cam.lookat = point3(0,0,-1);
        cam.vup = vec3(0,1,0);
        cam.defocus_angle = 0;
        world = make_normals_scene();
    } else if (scene_name == "materials" || scene_name == "m4") {
        // Three spheres (matte, dielectric, metal)
        cam.aspect_ratio = 16.0 / 9.0;
        cam.vfov = 20;
        cam.lookfrom = point3(-2,2,1);
        cam.lookat = point3(0,0,-1);
        cam.vup = vec3(0,1,0);
        cam.defocus_angle = 10.0;
        cam.focus_dist = 3.4;
        world = make_materials_scene();
    } else {
        // Final RTIOW Capstone Scene
        cam.aspect_ratio = 16.0 / 9.0;
        cam.vfov = 20;
        cam.lookfrom = point3(13,2,3);
        cam.lookat = point3(0,0,0);
        cam.vup = vec3(0,1,0);
        cam.defocus_angle = 0.6;
        cam.focus_dist = 10.0;
        world = make_final_scene();
    }

    if (use_bvh && !world.objects.empty()) {
        hittable_list bvh_world;
        bvh_world.add(make_shared<bvh_node>(world));
        if (!output_png.empty()) {
            cam.render_png(bvh_world, output_png);
        } else {
            cam.render(bvh_world);
        }
    } else {
        if (!output_png.empty()) {
            cam.render_png(world, output_png);
        } else {
            cam.render(world);
        }
    }

    return 0;
}
