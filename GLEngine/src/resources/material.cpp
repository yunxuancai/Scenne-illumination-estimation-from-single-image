#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <texture.h>
#include <shader.h>

#include "material.h"


Material::Material()
{

}


Material::~Material()
{

}


void Material::addTexture(std::string uniformName, Texture texObj)
{
    this->texList.push_back(std::tuple<std::string, Texture>(uniformName, texObj));
}


void Material::setShader(Shader& shader)
{
    this->matShader = shader;
}


void Material::renderToShader()
{
    this->matShader.useShader();

    std::cout << "texList Size : " << this->texList.size() << std::endl;

    for(GLuint i = 0; i < this->texList.size(); i++)
    {
        std::string currentUniformName = std::get<0>(this->texList[i]);
        Texture currentTex = std::get<1>(this->texList[i]);

        std::cout << "i : " << i << std::endl;
        std::cout << "texWidth : " << currentTex.getTexWidth() << std::endl;
        std::cout << "texHeight : " << currentTex.getTexHeight() << std::endl;
        std::cout << "texUniformName : " << currentTex.getTexName() << std::endl;
        std::cout << "ActiveTexture sent : " << GL_TEXTURE0 + i << std::endl;

        glActiveTexture(GL_TEXTURE0 + i);
        currentTex.useTexture();
        glUniform1i(glGetUniformLocation(this->matShader.Program, currentUniformName.c_str()), i);

        std::cout << "------" << std::endl;
    }

    std::cout << "============" << std::endl;
}
