#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "light.h"
#include "shape.h"



Light::Light()
{

}


Light::~Light()
{

}


void Light::setLight(glm::vec3 position, glm::vec4 color, float radius, bool isMesh)
{
    this->lightType = "point";
    this->lightPosition = position;
    this->lightColor = color;
    this->lightRadius = radius;
    this->lightPointID = lightPointCount;
    this->lightToMesh = isMesh;

    if(this->lightToMesh)
    {
        this->lightMesh.setShape("cube", glm::vec3(0.0f, 0.0f, 0.0f));
        this->lightMesh.setShapePosition(this->lightPosition);
        this->lightMesh.setShapeScale(glm::vec3(0.15f, 0.15f, 0.15f));
    }

    lightPointCount = ++lightPointCount;
    lightPointList.push_back(*this);
}


void Light::setLight(glm::vec3 direction, glm::vec4 color)
{
    this->lightType = "directional";
    this->lightDirection = direction;
    this->lightColor = color;
    this->lightDirectionalID = lightDirectionalCount;

    lightDirectionalCount = ++lightDirectionalCount;
    lightDirectionalList.push_back(*this);
}


void Light::renderToShader(Shader& shader, Camera& camera)
{
    shader.useShader();

    if(this->lightType == "point")
    {
        glm::vec3 lightPositionViewSpace = glm::vec3(camera.GetViewMatrix() * glm::vec4(this->lightPosition, 1.0f));

        glUniform3f(glGetUniformLocation(shader.Program, ("lightPointArray["+ std::to_string(this->lightPointID) +"].position").c_str()), lightPositionViewSpace.x, lightPositionViewSpace.y, lightPositionViewSpace.z);
        glUniform4f(glGetUniformLocation(shader.Program, ("lightPointArray["+ std::to_string(this->lightPointID) +"].color").c_str()), this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
        glUniform1f(glGetUniformLocation(shader.Program, ("lightPointArray["+ std::to_string(this->lightPointID) +"].radius").c_str()), this->lightRadius);
    }

    else if(this->lightType == "directional")
    {
        glm::vec3 lightDirectionViewSpace = glm::vec3(camera.GetViewMatrix() * glm::vec4(this->lightDirection, 0.0f));

        glUniform3f(glGetUniformLocation(shader.Program, ("lightDirectionalArray["+ std::to_string(this->lightDirectionalID) +"].direction").c_str()), lightDirectionViewSpace.x, lightDirectionViewSpace.y, lightDirectionViewSpace.z);
        glUniform4f(glGetUniformLocation(shader.Program, ("lightDirectionalArray["+ std::to_string(this->lightDirectionalID) +"].color").c_str()), this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
    }
}


std::string Light::getLightType()
{
    if(this->lightType == "point")
        return lightPointList[this->lightPointID].lightType;
    if(this->lightType == "directional")
        return lightDirectionalList[this->lightPointID].lightType;

}


glm::vec3 Light::getLightPosition()
{
    return lightPointList[this->lightPointID].lightPosition;
}


glm::vec3 Light::getLightDirection()
{
    return lightDirectionalList[this->lightPointID].lightDirection;
}


glm::vec4 Light::getLightColor()
{
    if(this->lightType == "point")
        return lightPointList[this->lightPointID].lightColor;
    if(this->lightType == "directional")
        return lightDirectionalList[this->lightPointID].lightColor;
}


float Light::getLightRadius()
{
    return lightPointList[this->lightPointID].lightRadius;
}


GLuint Light::getLightID()
{
    if(this->lightType == "point")
        return lightPointID;
    if(this->lightType == "directional")
        return lightDirectionalID;
}


bool Light::isMesh()
{
    return lightToMesh;
}


void Light::setLightPosition(glm::vec3 position)
{
    lightPointList[this->lightPointID].lightPosition = position;
    lightPointList[this->lightPointID].lightMesh.setShapePosition(position);
}


void Light::setLightDirection(glm::vec3 direction)
{
    lightDirectionalList[this->lightDirectionalID].lightDirection = direction;
}


void Light::setLightColor(glm::vec4 color)
{
    if(this->lightType == "point")
        lightPointList[this->lightPointID].lightColor = color;
    if(this->lightType == "directional")
        lightDirectionalList[this->lightDirectionalID].lightColor = color;
}

void Light::setLightRadius(float radius)
{
    lightPointList[this->lightPointID].lightRadius = radius;
}


GLuint Light::lightPointCount = 0;
GLuint Light::lightDirectionalCount = 0;

std::vector<Light> Light::lightPointList;
std::vector<Light> Light::lightDirectionalList;
