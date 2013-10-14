#include "a2.hpp"

// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.
Matrix4x4 rotation(double angle, char axis)
{
    Matrix4x4 r;

    switch (axis) {
        case 'x':
            r[1][1] = std::cos(angle);
            r[1][2] = -std::sin(angle);
            r[2][1] = std::sin(angle);
            r[2][2] = std::cos(angle);
            break;
        case 'y':
            r[0][0] = std::cos(angle);
            r[2][0] = -std::sin(angle);
            r[0][2] = std::sin(angle);
            r[2][2] = std::cos(angle);
            break;
        case 'z':
            r[0][0] = std::cos(angle);
            r[0][1] = -std::sin(angle);
            r[1][0] = std::sin(angle);
            r[1][1] = std::cos(angle);
            break;
        default:
            std::cerr << "Error: Invalid Rotation Axis." << std::endl;
            break;
    }

    return r;
}

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& displacement)
{
    Matrix4x4 t;

    /*
    * [ 1, 0, 0, x ] 
    * [ 0, 1, 0, y ]
    * [ 0, 0, 1, z ]
    * [ 0, 0, 0, 1 ]
    */

    t[0][3] = displacement[0];
    t[1][3] = displacement[1];
    t[2][3] = displacement[2];

    return t;
}

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& scale)
{
    Matrix4x4 s;

    /*
    * [ x, 0, 0, 0 ] 
    * [ 0, y, 0, 0 ]
    * [ 0, 0, z, 0 ]
    * [ 0, 0, 0, 1 ]
    */

    s[0][0] = 1.0+scale[0];
    s[1][1] = 1.0+scale[1];
    s[2][2] = 1.0+scale[2];

    return s;
}
