#include "a4.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "material.hpp"
#include <cmath>

const double AMBIENT_COEFFICIENT = 0.2;
const int MAX_DEPTH = 4;
// const double M_PI = 3.14159265;
const int DEPTH = 4;

void clamp(double& val, double min, double max) {
  if (val < min) {
    val = min;
  } else if (val > max) {
    val = max;
  }
}


Colour generate_shadow_ray(Ray ray, SceneNode* root, const std::list<Light*>& lights, IntersectObj* obj,
  double refl) {
  Colour computed_colour(0.0);
  Material* mat = ((GeometryNode*)(obj->mNode))->get_material();
  for (auto& light : lights) {
    Vector3D lightToPoint = light->position - obj->mPoint;
    Ray lightRay(obj->mPoint, lightToPoint);
    lightToPoint.normalize();
    obj->mNormal.normalize();

    double diffuseCoefficient = obj->mNormal.dot(lightToPoint);
    clamp(diffuseCoefficient, 0.0, 1.0);
          
    if (diffuseCoefficient <= 0.0) {
      continue;
    }
    if (!root->isInShadow(lightRay, obj->mNode)) {
      Colour diffuse = diffuseCoefficient * (mat->get_diffuse() + light->colour) * 0.5 * refl;

      float reflCoef = 2.0 * (lightToPoint.dot(obj->mNormal));
      Vector3D reflection = lightToPoint - reflCoef * obj->mNormal;
      Vector3D rayDir = ray.mDirection;
      rayDir.normalize();
      reflection.normalize();

      Colour specular = mat->get_specular() * std::pow(rayDir.dot(reflection), mat->get_shininess());
      computed_colour = computed_colour + light->getAttenuation(obj->mPoint) * (diffuse + specular);
    }
  }

  return computed_colour;
}

Colour generate_primary_ray(Ray ray, SceneNode* root, const Colour& ambient, const std::list<Light*>& lights, int depth, double reflectionFactor, double x, double y) {
  Colour final_colour(0.0);
  if (depth >= MAX_DEPTH || reflectionFactor < 0.05) {
    return final_colour;
  }

  IntersectObj* obj = root->intersect(ray);

  if (obj == NULL && depth == 0) {
    final_colour = final_colour + Colour(y, y, 1.0);
    reflectionFactor = 0.0;
    return final_colour;
  } else if (obj == NULL) {
    final_colour = Colour(0.0, 0.0, 0.0);
    return final_colour;
  }

  Material* mat = ((GeometryNode*)(obj->mNode))->get_material();
  double mat_refl_coef = mat->get_reflectiveness();

  // Colour ambient = mat->get_diffuse() * AMBIENT_COEFFICIENT; // Note: light.ambientcoeffficient = 0.2
  final_colour = final_colour + ambient * (1 - mat_refl_coef)* AMBIENT_COEFFICIENT; // non reflective part
  final_colour = final_colour + generate_shadow_ray(ray, root, lights, obj, (1 - mat_refl_coef))  ;
  
  if (mat_refl_coef > 0.0) {
    float reflect = 2.0 * (ray.mDirection.dot(obj->mNormal));
    Ray reflection_ray(obj->mPoint, ray.mDirection - reflect * obj->mNormal);
    
    final_colour = final_colour + generate_primary_ray(reflection_ray, root, ambient, lights,
      depth + 1, reflectionFactor*mat_refl_coef, x, y) * mat_refl_coef;
  }

  delete obj;

  return final_colour;
}

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
               )
{
  

  Image img(width, height, 3);
  Vector3D normalizedView = view;
  normalizedView.normalize();

  double fov_radians = (fov * M_PI) / 180.0;
  double distance = width / std::tan(fov_radians / 2.0);
  Vector3D left_vec = normalizedView.cross(up);

  int max = height * width;
  int onePercent = max * 0.01;
  int counter = 0;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++, counter++) {
      if (counter % onePercent == 0) {
        std::cerr << (counter/(double)onePercent) << " \%" << std::endl;
      }

      Point3D screenPoint = eye + (distance * normalizedView) + (x - width/2)*left_vec + (y - height/2)*up;
      Vector3D eyeToScreen = screenPoint - eye;

      Ray ray(eye, eyeToScreen);

      Colour final_colour = generate_primary_ray(ray, root, ambient, lights, 0, 1.0, (double)x / width, (double)y / height);

      img(x, height - y - 1, 0) = final_colour.R();
      img(x, height - y - 1, 1) = final_colour.G();
      img(x, height - y - 1, 2) = final_colour.B();
    }
  }
  img.savePng(filename);
  
}
