#include <stdexcept>
#include "StaticTechnique.hpp"

using namespace std;

StaticTechnique::StaticTechnique() : Technique() {
    if (!AddShader("static.vert", GL_VERTEX_SHADER)) {
        throw runtime_error("Can't load static vertex shader.");
    }

    if (!AddShader("static.frag", GL_FRAGMENT_SHADER)) {
        throw runtime_error("Can't load static fragment shader.");
    }

    if (!CompileShaders()) {
        throw runtime_error("Can't compile static shader program.");
    }

    Enable();

    glm::vec3 vertices[6];
    vertices[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
    vertices[1] = glm::vec3(-1.0f, 1.0f, 0.0f);
    vertices[2] = glm::vec3(1.0f, -1.0f, 0.0f);

    vertices[3] = glm::vec3(1.0f, -1.0f, 0.0f);
    vertices[4] = glm::vec3(-1.0f, 1.0f, 0.0f);
    vertices[5] = glm::vec3(1.0f, 1.0f, 0.0f);

    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    glGenBuffers(1, &mBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

StaticTechnique::~StaticTechnique() {
    glDeleteBuffers(1, &mBufferObject);
    glDeleteVertexArrays(1, &mVertexArray);
}

bool StaticTechnique::Render() {
    Enable();

    glBindVertexArray(mVertexArray);
    // glBindBuffer(GL_ARRAY_BUFFER, mBufferObject);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    return true;
}