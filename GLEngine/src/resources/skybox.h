#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "shape.h"


class Skybox
{
    public:
        GLfloat cameraAperture, cameraShutterSpeed, cameraISO;
        Texture texSkybox;

        Skybox();
        ~Skybox();
        void setSkyboxTexture(const char* texPath);
        void renderToShader(Shader& shaderSkybox, glm::mat4& projection, glm::mat4& view);
        void setExposure(GLfloat aperture, GLfloat shutterSpeed, GLfloat iso);
};


#endif
