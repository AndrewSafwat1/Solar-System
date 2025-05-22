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


struct Position {
    double initial_x; 
    double initial_y;   
    double initial_z;
};

Position sun_pos     = { 0.0, 0.0, 0.0 };
Position mercury_pos = { 800, 0, 0.0 };
Position venus_pos   = { 1020, 0, 0 };
Position earth_pos   = { 1230, 0, 0 };
Position mars_pos    = { 1520, 0, 0.0 };
Position jupiter_pos = { 1920, 0, 0.0 };
Position saturn_pos  = { 2520, 0, 0.0 };
Position uranus_pos  = { 3020, 0, 0 };
Position neptune_pos = { 3470, 0, 0 };

point3 orbit_pos(const Position& o, double t) {
    // Calculate the radius and initial angle from the initial position (relative to sun)
    double dx = o.initial_x - sun_pos.initial_x;
    double dz = o.initial_z - sun_pos.initial_z;
    double radius = sqrt(dx * dx + dz * dz);
    double initial_angle = atan2(dz, dx);

    // Define an orbital period (in arbitrary time units) based on radius
    double period = 500.0 + radius * 0.5;

    // Calculate current angle based on time and period
    double angle = initial_angle + 2 * M_PI * (t / period);

    // Calculate new position in the XZ plane, keep Y constant, and offset by sun's position
    double x = sun_pos.initial_x + radius * cos(angle);
    double z = sun_pos.initial_z + radius * sin(angle);
    double y = o.initial_y + sun_pos.initial_y; 

    return point3(x, y, z);
}

point3 moon_pos(double t) {
    point3 earth_curr = orbit_pos(earth_pos, t);
    // Moon's orbital parameters
    double dx = 100;
    double dz = 100;
    double radius = sqrt(dx * dx + dz * dz);
    double initial_angle = atan2(dz, dx);

    // Moon's orbital period
    double period = 500.0 + radius * 0.5;

    // Current angle
    double angle = initial_angle + 2 * M_PI * (t / period);

    // Moon's orbital tilt (about 5 degrees)
    double tilt_rad = 5.0 * M_PI / 180.0;

    // Position in moon's orbital plane (before tilt)
    double x_orb = radius * cos(angle);
    double y_orb = 0;
    double z_orb = radius * sin(angle);

    // Apply tilt: rotate around X axis by tilt_rad
    double y_tilted = y_orb * cos(tilt_rad) - z_orb * sin(tilt_rad);
    double z_tilted = y_orb * sin(tilt_rad) + z_orb * cos(tilt_rad);

    double x = earth_curr.x() + x_orb;
    double y = earth_curr.y() + y_tilted;
    double z = earth_curr.z() + z_tilted;

    return point3(x, y, z);
}

void solar_system(long current_time, point3 lookfrom, point3 lookat, vec3 vup) {
    hittable_list world;

    // Create a bright yellow-orange light for the sun
    auto sun_texture = make_shared<image_texture>("sunmap.png", 20.0);
    auto sun_surface = make_shared<diffuse_light>(sun_texture); // Bright yellow-orange light
    auto sun = make_shared<sphere>(point3(sun_pos.initial_x, sun_pos.initial_y, sun_pos.initial_z), 550, sun_surface);
    world.add(sun);

    // Add a large sphere with an inward-facing stars texture as the background
    auto stars_texture = make_shared<image_texture>("stars_background.jpg");
    auto stars_material = make_shared<diffuse_light>(stars_texture);
    // Make the sphere large enough to encompass the scene, with normals pointing inward
    auto stars_sphere = make_shared<sphere>(point3(sun_pos.initial_x, sun_pos.initial_y, sun_pos.initial_z), 5000, stars_material);
    world.add(stars_sphere);

    auto mercury_texture = make_shared<image_texture>("mercurymap.jpg");
    auto mercury_surface = make_shared<lambertian>(mercury_texture);
    auto mercury = make_shared<sphere>(orbit_pos(mercury_pos, current_time), 40, mercury_surface);
    world.add(mercury);
    
    auto venus_texture = make_shared<image_texture>("venusmap.jpg");
    auto venus_surface = make_shared<lambertian>(venus_texture);
    auto venus = make_shared<sphere>(orbit_pos(venus_pos, current_time), 95, venus_surface);
    world.add(venus);

    auto earth_texture = make_shared<image_texture>("earthmap1k.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto earth = make_shared<sphere>(orbit_pos(earth_pos, current_time), 100, earth_surface);
    world.add(earth);

    auto moon_texture = make_shared<image_texture>("moonmap1k.jpg");
    auto moon_surface = make_shared<lambertian>(moon_texture);
    auto moon = make_shared<sphere>(moon_pos(current_time), 30, moon_surface);
    world.add(moon);

    auto mars_texture = make_shared<image_texture>("marsmap.jpg");
    auto mars_surface = make_shared<lambertian>(mars_texture);
    auto mars = make_shared<sphere>(orbit_pos(mars_pos, current_time), 85, mars_surface);
    world.add(mars);

    auto jupiter_texture = make_shared<image_texture>("jupitermap.jpg");
    auto jupiter_surface = make_shared<lambertian>(jupiter_texture);
    auto jupiter = make_shared<sphere>(orbit_pos(jupiter_pos, current_time), 240, jupiter_surface);
    world.add(jupiter);

    auto saturn_texture = make_shared<image_texture>("saturnmap.jpg");
    auto saturn_surface = make_shared<lambertian>(saturn_texture);
    auto saturn = make_shared<sphere>(orbit_pos(saturn_pos, current_time), 210, saturn_surface);
    world.add(saturn);

    auto uranus_texture = make_shared<image_texture>("uranusmap.jpg");
    auto uranus_surface = make_shared<lambertian>(uranus_texture);
    auto uranus = make_shared<sphere>(orbit_pos(uranus_pos, current_time), 150, uranus_surface);
    world.add(uranus);

    auto neptune_texture = make_shared<image_texture>("naptunemap.jpg");
    auto neptune_surface = make_shared<lambertian>(neptune_texture);
    auto neptune = make_shared<sphere>(orbit_pos(neptune_pos, current_time), 130, neptune_surface);
    world.add(neptune);

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 500;
    cam.samples_per_pixel = 10;
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