#include "a4.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "material.hpp"
#include <cmath>

const double SCREEN_DISTANCE = 1.0;
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

  double fov_radians = (fov * M_PI) / 180.0;
  double widthOfViewport = std::tan(fov_radians / 2.0) * SCREEN_DISTANCE;
  double heightOfViewport = widthOfViewport / (width * height);
  Vector3D left_vec = view.cross(up);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < height; x++) {
      Point3D screenPoint = eye + SCREEN_DISTANCE*view + 
                            (x -(width/2)) * left_vec + 
                            (y -(height/2)) * up;

      Ray ray(screenPoint, screenPoint - eye);
      ray.mDirection.normalize();

      Colour final_colour(0.0);
      int currentDepth = 0;
      double reflectionFactor = 1.0;
      while (currentDepth < DEPTH && reflectionFactor > 0.05) {
        Colour computed_colour(0.0);

        // intersection(ray) returns the closest ray object which it intersects with
        IntersectObj* obj = root->intersect(ray);

        if (obj == NULL && currentDepth == 0) {
          // didn't intersect anything. 
          final_colour = Colour((double)y / height, 0.0, 0.0);
          continue;
        } else if (obj== NULL) {
          continue;
        }

        for (auto& light : lights) {
          // TODO: secondary rays from obj->mPoint to light source
          Vector3D lightToPoint = obj->mPoint - light->position;
          Ray lightRay(obj->mPoint, lightToPoint);
          lightToPoint.normalize();

          // check if they are pointing in the same direction.
          // if they are, light does not reach this point
          double diffuseCoefficient = obj->mNormal.dot(lightToPoint);
          if (diffuseCoefficient <= 0.0) {
            continue;
          }

          // Not normalized
          if (root->isInShadow(lightRay)) {

            computed_colour = computed_colour + (light->colour * light->getAttenuation(obj->mPoint));
            Material* mat = ((GeometryNode*)(obj->mNode))->get_material();
            computed_colour = computed_colour + mat->getColour();
          }
        }

        final_colour = final_colour + computed_colour;
        currentDepth++;

        // Temp
        reflectionFactor = 0.0;

        float reflect = 2.0 * (ray.mDirection.dot(obj->mNormal));
        ray.mOrigin = obj->mPoint;
        ray.mDirection = ray.mDirection - reflect * obj->mNormal;

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

      std::cout << "Pixel: (" << x << ", " << y << "): " << final_colour << std::endl;
      img(x, y, 0) = final_colour.R();
      img(x, y, 1) = final_colour.G();
      img(x, y, 2) = final_colour.B();
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
