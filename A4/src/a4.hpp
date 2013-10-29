#ifndef CS488_A4_HPP
#define CS488_A4_HPP

#include <string>
#include "algebra.hpp"
#include "scene.hpp"
#include "light.hpp"

void clamp(double& val, double min, double max);
Colour generate_primary_ray(Ray ray, SceneNode* root, 
                            const std::list<Light*>& lights, 
                            int depth, 
                            double reflectionFactor,
                            double x,
                            double y);

Colour generate_shadow_ray(Ray ray, 
                           SceneNode* root, 
                           const Colour& ambient, 
                           const std::list<Light*>& lights, 
                           IntersectObj* obj,
                           double refl);

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               );

#endif
