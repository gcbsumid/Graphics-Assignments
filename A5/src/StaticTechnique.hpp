#ifndef STATIC_TECHNIQUE_HPP
#define STATIC_TECHNIQUE_HPP

#include "Technique.hpp"

class StaticTechnique : public Technique {
public:
    StaticTechnique();
    ~StaticTechnique();

    bool Render();

private:

    GLuint mBufferObject;
    GLuint mVertexArray;
};

#endif