#ifndef SHAPE_H
#define SHAPE_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"


class Shape
{
    public:
        std::string shapeType;
        GLfloat shapeAngle;
        glm::vec3 shapePosition;
        glm::vec3 shapeRotationAxis;
        glm::vec3 shapeScale;
        GLuint shapeVAO, shapeVBO, shapeEBO, shapeDiffuseID, shapeSpecularID;

        Shape();
        ~Shape();
        void setShape(std::string type, glm::vec3 position);
        void drawShape(Shader& lightingShader, glm::mat4& view, glm::mat4& projection, Camera& camera);
        void drawShape();
        std::string getShapeType();
        glm::vec3 getShapePosition();
        GLfloat getShapeAngle();
        glm::vec3 getShapeRotationAxis();
        glm::vec3 getShapeScale();
        GLuint getShapeVAO();
        void setShapePosition(glm::vec3 position);
        void setShapeAngle(GLfloat angle);
        void setShapeRotationAxis(glm::vec3 rotationAxis);
        void setShapeScale(glm::vec3 scale);
};


#endif
