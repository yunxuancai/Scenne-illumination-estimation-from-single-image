#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shape.h"


class Light
{
    public:
        static GLuint lightPointCount, lightDirectionalCount;
        static std::vector<Light> lightPointList;
        static std::vector<Light> lightDirectionalList;
        GLuint lightPointID, lightDirectionalID, lightVAO, lightVBO;
        std::string lightType;
        bool lightToMesh = false;
        float lightRadius;
        glm::vec3 lightPosition;
        glm::vec3 lightDirection;
        glm::vec4 lightColor;
        Shape lightMesh;

        Light();
        ~Light();
        void setLight(glm::vec3 position, glm::vec4 color, float radius, bool isMesh);
        void setLight(glm::vec3 direction, glm::vec4 color);
        void renderToShader(Shader& shader, Camera& camera);
        std::string getLightType();
        glm::vec3 getLightPosition();
        glm::vec3 getLightDirection();
        glm::vec4 getLightColor();
        float getLightRadius();
        GLuint getLightID();
        bool isMesh();
        void setLightPosition(glm::vec3 position);
        void setLightDirection(glm::vec3 direction);
        void setLightColor(glm::vec4 color);
        void setLightRadius(float radius);
};

#endif
