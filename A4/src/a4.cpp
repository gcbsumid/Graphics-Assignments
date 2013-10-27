#include "a4.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "material.hpp"
#include <cmath>

const double SCREEN_DISTANCE = 256.0;
// const double M_PI = 3.14159265;
const int DEPTH = 4;

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
  // Calculate projection?
  // Matrix4x4 projection;
  // double aspect = width / height;
  // double cotangent = 1.0 / std::tan( (M_PI * fov / 180.0) / 2.0 );
  // projection[0][0] = cotangent / aspect;
  // projection[1][1] = cotangent;
  // projection[2][2] = (NEAR_PLANE + FAR_PLANE) / (FAR_PLANE - NEAR_PLANE);
  // projection[2][3] = (-2.0 * FAR_PLANE * NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE);
  // projection[3][2] = 1.0;
  // projection[3][3] = 0.0;



  // Fill in raytracing code here.
  std::cerr << "Stub: a4_render(" << root << ",\n     "
            << filename << ", " << width << ", " << height << ",\n     "
            << eye << ", " << view << ", " << up << ", " << fov << ",\n     "
            << ambient << ",\n     {";

  for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I) {
    if (I != lights.begin()) std::cerr << ", ";
    std::cerr << **I;
  }
  std::cerr << "});" << std::endl;
  
  // For now, just make a sample image.

  Image img(width, height, 3);
  Vector3D normalizedView = view;
  normalizedView.normalize();
  std::cout << "normalized view: " << normalizedView << std::endl;

  double fov_radians = (fov * M_PI) / 180.0;
  double distance = width / std::tan(fov_radians / 2.0);
  // std::cout << "widthOfViewport " << widthOfViewport << std::endl;
  // std::cout << "heightOfViewport " << heightOfViewport << std::endl;
  std::cout << "fov_radians " << fov_radians << std::endl;
  Vector3D left_vec = normalizedView.cross(up);
  std::cout << "left " << left_vec << std::endl;
  std::cout << "up " << up << std::endl;


  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Point3D screenPoint = eye + SCREEN_DISTANCE*normalizedView + 
      //                       ((x / width * widthOfViewport) -(widthOfViewport/2)) * left_vec + 
      //                       ((y / height * heightOfViewport) -(heightOfViewport/2)) * up;
      Point3D screenPoint = eye + (distance * normalizedView) + (x - width/2)*left_vec + (y - height/2)*up;
      Vector3D eyeToScreen = screenPoint - eye;

      Ray ray(eye, eyeToScreen);
      // ray.mDirection.normalize();
      // std::cout << "eyeToScreen: " << ray.mDirection << std::endl;

      Colour final_colour(0.0);
      int currentDepth = 0;
      double reflectionFactor = 1.0;
      while (currentDepth < DEPTH && reflectionFactor > 0.05) {
        Colour computed_colour(0.0);

        // intersection(ray) returns the closest ray object which it intersects with
        IntersectObj* obj = root->intersect(ray);

        if (obj == NULL && currentDepth == 0) {
          // didn't intersect anything. 
          // final_colour = Colour(0.0, 0.0, 0.0);
          final_colour = Colour((double)y / height, (double)y / height, 1.0);
          break;
        } else if (obj== NULL) {
          break;;
        }

        // std::cout << "Sphere: " << obj->mNode->get_name() << std::endl;
        double ambientcoeffficient = 0.2;
        Material* mat = ((GeometryNode*)(obj->mNode))->get_material();
        Colour ambient = mat->get_diffuse() * ambientcoeffficient; // Note: light.ambientcoeffficient = 0.2
        computed_colour = computed_colour + ambient;

        for (auto& light : lights) {
          // TODO: secondary rays from obj->mPoint to light source
          Vector3D lightToPoint = obj->mPoint - light->position;
          Ray lightRay(obj->mPoint, lightToPoint);
          lightToPoint.normalize();

          double diffuseCoefficient = obj->mNormal.dot(lightToPoint);
          // check if they are pointing in the same direction.
          // if they are, light does not reach this point
          
          if (diffuseCoefficient <= 0.0) {
            continue;
          }

          if (obj->mNode->get_name() == "s4") {
            std::cout << "S4 is getting: " << final_colour << std::endl;
          }
          // Not normalized
          if (!root->isInShadow(lightRay, obj->mNode)) {
            /* 
             *      LIGHT
             *   so what we want is:
             *    kaIa = attenuation coef * light.colour -> Ambient
             *    kd(l . n)Id = mat.diffuse * diffuse coef -> Diffuse 
             *    ks(r . v)^pIs = mat.specular * (reflection of light on normal . ray.mDirection)^mshininess
             *
             */

            // std::cout << "\tAmbient: " << ambient << std::endl;
            // Colour lightAmbience = diffuseCoefficient * (light->colour);
            Colour diffuse = diffuseCoefficient * (mat->get_diffuse() + light->colour) * 0.5;
            // std::cout << "\tDiffuse: " << diffuse << std::endl;

            float reflCoef = 2.0 * (lightToPoint.dot(obj->mNormal));
            Vector3D reflection = lightToPoint - reflCoef * obj->mNormal;
            Vector3D rayDir = ray.mDirection;
            rayDir.normalize();
            reflection.normalize();
            // std::cout << "specular coef: " << std::pow(rayDir.dot(reflection), mat->get_shininess());

            // Colour specular = mat->get_specular() * std::pow(rayDir.dot(reflection), mat->get_shininess());
            Colour specular = (diffuseCoefficient > 0.0) ? mat->get_specular() * std::pow(rayDir.dot(reflection), mat->get_shininess()) : 0.0;

            // std::cout << "\tSpecular: " << specular << std::endl;
            // std::cout << "Attenuation: " << light->getAttenuation(obj->mPoint) << std::endl;
            // std::cout << "computed_colour: " << computed_colour << " and Attenuation(diffuse+specular): " << (light->getAttenuation(obj->mPoint) * (diffuse + specular)) << std::endl;
            computed_colour = computed_colour + light->getAttenuation(obj->mPoint) * (diffuse + specular);
            // std::cout << "computed_colour: " << computed_colour << std::endl;
          }
        }

        final_colour = final_colour + computed_colour;
        currentDepth++;

        // Temp
        reflectionFactor = 0.0;

        float reflect = 2.0 * (ray.mDirection.dot(obj->mNormal));
        ray.mOrigin = obj->mPoint;
        ray.mDirection = ray.mDirection - reflect * obj->mNormal;
        delete obj;

        // TODO: reflection
        // once you find the closest intersection if it exists,
        // for each light in the scene, cast secondary rays from the point
        //    to each light. 
        //    if it is not in the shaddow of a different object
        //        add the light contribution to the computed colour sent to img(x, y, 0-3)
        
        // final_colour = final_colour + computed_colour * previous reflectionFactor;
        // reflectionFactor = reflectionFactor * reflection factor of previous object;

        // Calculate new reflection ray
      }

      // std::cout << "Pixel: (" << x << ", " << y << "): " << final_colour << std::endl;
      img(x, height - y - 1, 0) = final_colour.R();
      img(x, height - y - 1, 1) = final_colour.G();
      img(x, height - y - 1, 2) = final_colour.B();
      // // Red: increasing from top to bottom
      // img(x, y, 0) = (double)y / height;
      // // Green: increasing from left to right
      // img(x, y, 1) = (double)x / width;
      // // Blue: in lower-left and upper-right corners
      // img(x, y, 2) = ((y < height/2 && x < height/2)
      //                 || (y >= height/2 && x >= height/2)) ? 1.0 : 0.0;
    }
  }
  img.savePng(filename);
  
}
