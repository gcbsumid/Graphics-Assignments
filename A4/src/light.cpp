#include "light.hpp"
#include <iostream>

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

double Light::getAttenuation(Point3D pt) {
  // light intensity is 1. Intensity is included in the attenuation'
  Vector3D pointToLight = pt - position;
  double dist = pointToLight.length();
  // std::cout << "dist:" << dist << std::endl;

  // std::cout << "Attenuation: " << 1/(falloff[2] + falloff[1]*dist + falloff[0]*dist*dist) << std::endl;
  return (1/(falloff[0] + falloff[1]*dist + falloff[2]*dist*dist));
}

std::ostream& operator<<(std::ostream& out, const Light& l)
{
  out << "L[" << l.colour << ", " << l.position << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}
