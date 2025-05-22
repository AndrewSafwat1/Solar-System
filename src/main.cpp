#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "texture.h"
#include <fstream>
#include "external/json.hpp"

using json = nlohmann::json;
#define M_PI 3.14159265358979323846


struct Orbit {
    double initial_x; 
    double initial_y;   
    double initial_z;
};

const double SUN_X = -1200, SUN_Y = 0, SUN_Z = 0;


Orbit mercury_orbit = { -400, 0, 0.0 };
Orbit venus_orbit   = { -220, 0, 0 };
Orbit earth_orbit   = { 30, 0, 0 };
Orbit mars_orbit    = { 320, 0, 0.0 };
Orbit jupiter_orbit = { 720, 0, 0.0 };
Orbit saturn_orbit  = { 1320, 0, 0.0 };
Orbit uranus_orbit  = { 1820, 0, 0 };
Orbit neptune_orbit = { 2270, 0, 0 };

point3 orbit_pos(const Orbit& o, double t) {
    // Calculate the radius and initial angle from the initial position (relative to sun)
    double dx = o.initial_x - SUN_X;
    double dz = o.initial_z - SUN_Z;
    double radius = sqrt(dx * dx + dz * dz);
    double initial_angle = atan2(dz, dx);

    // Define an orbital period (in arbitrary time units) based on radius
    double period = 500.0 + radius * 0.5;

    // Calculate current angle based on time and period
    double angle = initial_angle + 2 * M_PI * (t / period);

    // Calculate new position in the XZ plane, keep Y constant, and offset by sun's position
    double x = SUN_X + radius * cos(angle);
    double z = SUN_Z + radius * sin(angle);
    double y = o.initial_y + SUN_Y;

    return point3(x, y, z);
}

void solar_system(long current_time, point3 lookfrom, point3 lookat, vec3 vup) {
    hittable_list world;

    // Create a bright yellow-orange light for the sun
    auto sun_texture = make_shared<image_texture>("sunmap.png", 20.0);
    auto sun_surface = make_shared<diffuse_light>(sun_texture); // Bright yellow-orange light
    auto sun = make_shared<sphere>(point3(SUN_X, SUN_Y, SUN_Z), 550, sun_surface);
    world.add(sun);

    // Add a large sphere with an inward-facing stars texture as the background
    auto stars_texture = make_shared<image_texture>("stars_background.jpg");
    auto stars_material = make_shared<diffuse_light>(stars_texture);
    // Make the sphere large enough to encompass the scene, with normals pointing inward
    auto stars_sphere = make_shared<sphere>(point3(SUN_X, SUN_Y, SUN_Z), 5000, stars_material);
    world.add(stars_sphere);

    auto mercury_texture = make_shared<image_texture>("mercurymap.jpg");
    auto mercury_surface = make_shared<lambertian>(mercury_texture);
    auto mercury = make_shared<sphere>(orbit_pos(mercury_orbit, current_time), 40, mercury_surface);
    world.add(mercury);
    
    auto venus_texture = make_shared<image_texture>("venusmap.jpg");
    auto venus_surface = make_shared<lambertian>(venus_texture);
    auto venus = make_shared<sphere>(orbit_pos(venus_orbit, current_time), 95, venus_surface);
    world.add(venus);

    auto earth_texture = make_shared<image_texture>("earthmap1k.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto earth = make_shared<sphere>(orbit_pos(earth_orbit, current_time), 100, earth_surface);
    world.add(earth);

    auto mars_texture = make_shared<image_texture>("marsmap.jpg");
    auto mars_surface = make_shared<lambertian>(mars_texture);
    auto mars = make_shared<sphere>(orbit_pos(mars_orbit, current_time), 85, mars_surface);
    world.add(mars);

    auto jupiter_texture = make_shared<image_texture>("jupitermap.jpg");
    auto jupiter_surface = make_shared<lambertian>(jupiter_texture);
    auto jupiter = make_shared<sphere>(orbit_pos(jupiter_orbit, current_time), 240, jupiter_surface);
    world.add(jupiter);

    auto saturn_texture = make_shared<image_texture>("saturnmap.jpg");
    auto saturn_surface = make_shared<lambertian>(saturn_texture);
    auto saturn = make_shared<sphere>(orbit_pos(saturn_orbit, current_time), 210, saturn_surface);
    world.add(saturn);

    auto uranus_texture = make_shared<image_texture>("uranusmap.jpg");
    auto uranus_surface = make_shared<lambertian>(uranus_texture);
    auto uranus = make_shared<sphere>(orbit_pos(uranus_orbit, current_time), 150, uranus_surface);
    world.add(uranus);

    auto neptune_texture = make_shared<image_texture>("naptunemap.jpg");
    auto neptune_surface = make_shared<lambertian>(neptune_texture);
    auto neptune = make_shared<sphere>(orbit_pos(neptune_orbit, current_time), 130, neptune_surface);
    world.add(neptune);

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 500;
    cam.samples_per_pixel = 8000;
    cam.max_depth = 50;
    cam.background = color(0,0,0);

    cam.vfov = 45;
    cam.lookfrom = lookfrom;    // Pull back along Z axis, centered on system
    cam.lookat = lookat;         // Looking toward center of x-axis line
    cam.vup = vup;

    cam.defocus_angle = 0.0;
    cam.render(world);
}

int main() {
    std::ifstream config_file("../config.json");
    if (!config_file) {
        std::cerr << "Failed to open config.json" << std::endl;
        return 1;
    }

    json config;
    config_file >> config;

    long current_time = config["time"];

    auto pos = config["camera"]["position"];
    auto look = config["camera"]["look_at"];
    auto up = config["camera"]["up"];

    point3 lookfrom(pos[0], pos[1], pos[2]);
    point3 lookat(look[0], look[1], look[2]);
    vec3 vup(up[0], up[1], up[2]);

    solar_system(current_time, lookfrom, lookat, vup);
}