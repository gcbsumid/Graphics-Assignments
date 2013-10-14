#include "primitive.hpp"
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

Primitive::~Primitive()
{
}

Sphere::Sphere() : tempAngle(0.0) {
    std::cout <<"Sphere created!" << std::endl;
}

Sphere::~Sphere() 
{
}

void Sphere::walk_gl(bool picking) //const
{
    // You must apply all transformations before the primitive gets drawn
    float theta = 0.0f, phi = 0.0f, deltaTheta = M_PI / 20.0, deltaPhi = M_PI / 20.0;
    Point3D p1, p2, p3, p4;
    float r = 1.0;

    // tempAngle += 1.0;
    // if (tempAngle >= 360.0) {
    //     tempAngle = tempAngle - 360.0;
    // }

    // glLoadIdentity();
    // glPushMatrix();
    // glTranslated(0.0, 0.0, -4.0);
    // glRotated(tempAngle, 0.0, 1.0, 0.0);

    // Colour black = Colour(0.0, 0.0, 0.0);
    // Colour white = Colour(1.0, 1.0, 1.0);
    // bool isBlack = true;
    for (theta = 0; theta <= 2 * M_PI; theta += deltaTheta) {
        glBegin(GL_QUAD_STRIP);
        for (phi = 0; phi <= 2 * M_PI; phi += deltaPhi) {
            p1 = Point3D(
                r * cosf(theta) * sinf(phi),
                r * sinf(theta) * sinf(phi),
                r * cosf(phi));

            p2 = Point3D(
                r * cosf(theta + deltaTheta) * sinf(phi),
                r * sinf(theta + deltaTheta) * sinf(phi),
                r * cosf(phi));

              // p1 = Point3D(
              //   r * sin(phi + deltaPhi) * sin(theta + deltaTheta),
              //   r * cos(phi + deltaPhi),
              //   r * sin(phi + deltaPhi) * cos(theta + deltaTheta));

              // p2 = Point3D(
              //   r * sin(phi) * sin(theta + deltaTheta),
              //   r * cos(phi),
              //   r * sin(phi) * cos(theta + deltaTheta));

              // p3 = Point3D(
              //   r * sin(phi) * sin(theta),
              //   r * cos(phi),
              //   r * sin(phi) * cos(theta));

              // p4 = Point3D(
              //   r * sin(phi + deltaPhi) * sin(theta),
              //   r * cos(phi + deltaPhi),
              //   r * sin(phi + deltaPhi) * cos(theta));

            // or whatever colour it is
            // glColor3f(1.0, 1.0, 1.0);
            // glNormal somehow
            // Colour temp(white);
            // if (isBlack) {
            //     temp = black;
            //     isBlack = false;
            // } else {
            //     temp = white;
            //     isBlack = true;
            // }


            // glColor3f(temp.R(), temp.G(), temp.B());
            glNormal3f(p1[0], p1[1], p1[2]);
            glVertex3f(p1[0], p1[1], p1[2]);
            glNormal3f(p2[0], p2[1], p2[2]);
            glVertex3f(p2[0], p2[1], p2[2]);
            // glVertex3f(p1[0], p1[1], p1[2]);
            // glVertex3f(p2[0], p2[1], p3[2]);
            // glVertex3f(p3[0], p3[1], p3[2]);
            // glVertex3f(p4[0], p4[1], p4[2]);
        }
        glEnd();
    }
    // glPopMatrix();
}