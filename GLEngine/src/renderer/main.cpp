#include "shader.h"
#include "camera.h"
#include "model.h"
#include "shape.h"
#include "texture.h"
#include "light.h"
//#include "skybox.h"
#include "material.h"
#include <filesystem.h>
#include<iostream>  

#include <locale.h>
#include <Windows.h>
#include <atlstr.h>
#include <Python.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <tuple>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
using namespace std;

//---------------
// GLFW Callbacks
//---------------
unsigned int hdrTexture0;
static void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void cameraMove();
void imGuiSetup();
void gBufferSetup();
void saoSetup();
void postprocessSetup();
void iblSetup();

GLuint WIDTH = 1280;
GLuint HEIGHT = 960;
unsigned int planeVAO;
GLuint screenQuadVAO, screenQuadVBO;
GLuint gBuffer, zBuffer, gPosition, gNormal, gAlbedo, gEffects;
GLuint saoFBO, saoBlurFBO, saoBuffer, saoBlurBuffer;
GLuint postprocessFBO, postprocessBuffer;
GLuint envToCubeFBO, irradianceFBO, prefilterFBO, brdfLUTFBO, envToCubeRBO, irradianceRBO, prefilterRBO, brdfLUTRBO;

GLint gBufferView = 1;
GLint tonemappingMode = 1;
GLint lightDebugMode = 3;
GLint attenuationMode = 2;
GLint saoSamples = 12;
GLint saoTurns = 7;
GLint saoBlurSize = 4;
GLint motionBlurMaxSamples = 32;

GLfloat lastX = WIDTH / 2;
GLfloat lastY = HEIGHT / 2;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat deltaGeometryTime = 0.0f;
GLfloat deltaLightingTime = 0.0f;
GLfloat deltaSAOTime = 0.0f;
GLfloat deltaPostprocessTime = 0.0f;
GLfloat deltaForwardTime = 0.0f;
GLfloat deltaGUITime = 0.0f;
GLfloat materialRoughness = 0.01f;
GLfloat materialMetallicity = 0.02f;
GLfloat ambientIntensity = 0.005f;
GLfloat saoRadius = 0.3f;
GLfloat saoBias = 0.001f;
GLfloat saoScale = 0.7f;
GLfloat saoContrast = 0.8f;
GLfloat lightPointRadius1 = 3.0f;
GLfloat lightPointRadius2 = 3.0f;
GLfloat lightPointRadius3 = 3.0f;
GLfloat cameraAperture = 16.0f;
GLfloat cameraShutterSpeed = 0.5f;
GLfloat cameraISO = 1000.0f;
GLfloat modelRotationSpeed = 0.0f;

bool cameraMode;
bool pointMode = false;
bool directionalMode = false;
bool iblMode = true;
bool saoMode = false;
bool fxaaMode = false;
bool motionBlurMode = false;
bool screenMode = true;
bool firstMouse = true;
bool guiIsOpen = true;
bool keys[1024];

glm::vec2 sunPosition = glm::vec2(glm::degrees(-2.847), glm::degrees(0.785));//-174.375, 48.0865);//////-129.375, 49.88);//
glm::vec3 lightPos;
string tur = "4.255";
string elv = "0.036";
string fov = "51.494";
glm::vec3 albedoColor = glm::vec3(1.0f);
glm::vec3 materialF0 = glm::vec3(0.04f);  // UE4 dielectric
glm::vec3 lightPointPosition1 = glm::vec3(1.5f, 0.75f, 1.0f);
glm::vec3 lightPointPosition2 = glm::vec3(-1.5f, 1.0f, 1.0f);
glm::vec3 lightPointPosition3 = glm::vec3(0.0f, 0.75f, -1.2f);
glm::vec3 lightPointColor1 = glm::vec3(1.0f);
glm::vec3 lightPointColor2 = glm::vec3(1.0f);
glm::vec3 lightPointColor3 = glm::vec3(1.0f);
glm::vec3 lightDirectionalDirection1 = glm::vec3(-0.2f, -1.0f, -0.3f);
glm::vec3 lightDirectionalColor1 = glm::vec3(1.0f);
glm::vec3 modelPosition = glm::vec3(0.0f);
glm::vec3 modelRotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 modelScale = glm::vec3(0.1f);

glm::mat4 projViewModel;
glm::mat4 prevProjViewModel = projViewModel;
glm::mat4 envMapProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 envMapView[] =
{
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
Shader gBufferShader;
Shader latlongToCubeShader;
Shader simpleShader;
Shader lightingBRDFShader;
Shader irradianceIBLShader;
Shader prefilterIBLShader;
Shader integrateIBLShader;
Shader firstpassPPShader;
Shader saoShader;
Shader saoBlurShader;

Texture objectAlbedo;
Texture objectNormal;
Texture objectRoughness;
Texture objectMetalness;
Texture objectAO;
Texture envMapHDR;
Texture envMapCube;
Texture envMapIrradiance;
Texture envMapPrefilter;
Texture envMapLUT;

Material pbrMat;

Model objectModel;

Light lightPoint1;
Light lightPoint2;
Light lightPoint3;
Light lightDirectional1;

Shape quadRender;
Shape envCubeRender;

ImGuiIO& io = ImGui::GetIO();

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void renderCube();
void renderQuad();

int main(int argc, char* argv[])
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWmonitor* glfwMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* glfwMode = glfwGetVideoMode(glfwMonitor);

    glfwWindowHint(GLFW_RED_BITS, glfwMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, glfwMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, glfwMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, glfwMode->refreshRate);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, u8"光照还原系统", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);

    gladLoadGL();

    glViewport(0, 0, WIDTH, HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    ImGui_ImplGlfwGL3_Init(window, true);

	
	io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesChinese());

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);


    //----------
    // Shader(s)
    //----------
    gBufferShader.setShader("resources/shaders/gBuffer.vert", "resources/shaders/gBuffer.frag");
    latlongToCubeShader.setShader("resources/shaders/latlongToCube.vert", "resources/shaders/latlongToCube.frag");

    simpleShader.setShader("resources/shaders/lighting/simple.vert", "resources/shaders/lighting/simple.frag");
    lightingBRDFShader.setShader("resources/shaders/lighting/lightingBRDF.vert", "resources/shaders/lighting/lightingBRDF.frag");
    irradianceIBLShader.setShader("resources/shaders/lighting/irradianceIBL.vert", "resources/shaders/lighting/irradianceIBL.frag");
    prefilterIBLShader.setShader("resources/shaders/lighting/prefilterIBL.vert", "resources/shaders/lighting/prefilterIBL.frag");
    integrateIBLShader.setShader("resources/shaders/lighting/integrateIBL.vert", "resources/shaders/lighting/integrateIBL.frag");

    firstpassPPShader.setShader("resources/shaders/postprocess/postprocess.vert", "resources/shaders/postprocess/firstpass.frag");
    saoShader.setShader("resources/shaders/postprocess/sao.vert", "resources/shaders/postprocess/sao.frag");
    saoBlurShader.setShader("resources/shaders/postprocess/sao.vert", "resources/shaders/postprocess/saoBlur.frag");

	Shader pbrShader; pbrShader.setShader("resources/shaders/2.2.2.pbr.vs", "resources/shaders/2.2.2.pbr.fs");
	Shader equirectangularToCubemapShader; equirectangularToCubemapShader.setShader("resources/shaders/2.2.2.cubemap.vs", "resources/shaders/2.2.2.equirectangular_to_cubemap.fs");
	Shader backgroundShader; backgroundShader.setShader("resources/shaders/2.2.2.background.vs", "resources/shaders/2.2.2.background.fs");
	Shader shader; shader.setShader("resources/shaders/3.1.3.shadow_mapping.vs", "resources/shaders/3.1.3.shadow_mapping.fs");
	Shader simpleDepthShader; simpleDepthShader.setShader("resources/shaders/3.1.3.shadow_mapping_depth.vs", "resources/shaders/3.1.3.shadow_mapping_depth.fs");
	Shader debugDepthQuad; debugDepthQuad.setShader("resources/shaders/3.1.3.debug_quad.vs", "resources/shaders/3.1.3.debug_quad_depth.fs");


    //-----------
    // Textures(s)
    //-----------
    objectAlbedo.setTexture("resources/textures/pbr/rustediron/rustediron_albedo.png", "ironAlbedo", true);
    objectNormal.setTexture("resources/textures/pbr/rustediron/rustediron_normal.png", "ironNormal", true);
    objectRoughness.setTexture("resources/textures/pbr/rustediron/rustediron_roughness.png", "ironRoughness", true);
    objectMetalness.setTexture("resources/textures/pbr/rustediron/rustediron_metalness.png", "ironMetalness", true);
    objectAO.setTexture("resources/textures/pbr/rustediron/rustediron_ao.png", "ironAO", true);

    envMapHDR.setTextureHDR("resources/textures/hdr/environment_EMY.hdr", "appartHDR", true);

    envMapCube.setTextureCube(512, GL_RGB, GL_RGB16F, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR);
    envMapIrradiance.setTextureCube(32, GL_RGB, GL_RGB16F, GL_FLOAT, GL_LINEAR);
    envMapPrefilter.setTextureCube(128, GL_RGB, GL_RGB16F, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR);
    envMapPrefilter.computeTexMipmap();
    envMapLUT.setTextureHDR(512, 512, GL_RG, GL_RG16F, GL_FLOAT, GL_LINEAR);

    //---------
    // Model(s)
    //---------
    objectModel.loadModel("resources/models/shaderball/shaderball.obj");


    //---------------
    // Shape(s)
    //---------------
    envCubeRender.setShape("cube", glm::vec3(0.0f));
    quadRender.setShape("quad", glm::vec3(0.0f));


    //----------------
    // Light source(s)
    //----------------
    lightPoint1.setLight(lightPointPosition1, glm::vec4(lightPointColor1, 1.0f), lightPointRadius1, true);
    lightPoint2.setLight(lightPointPosition2, glm::vec4(lightPointColor2, 1.0f), lightPointRadius2, true);
    lightPoint3.setLight(lightPointPosition3, glm::vec4(lightPointColor3, 1.0f), lightPointRadius3, true);

    lightDirectional1.setLight(lightDirectionalDirection1, glm::vec4(lightDirectionalColor1, 1.0f));


    lightingBRDFShader.useShader();
	lightingBRDFShader.setInt("gPosition", 0);
	lightingBRDFShader.setInt("gAlbedo", 1);
	lightingBRDFShader.setInt("gNormal", 2);
	lightingBRDFShader.setInt("gEffects", 3);
	lightingBRDFShader.setInt("sao", 4);
	lightingBRDFShader.setInt("envMap", 5);
	lightingBRDFShader.setInt("envMapIrradiance", 6);
	lightingBRDFShader.setInt("envMapPrefilter", 7);
	lightingBRDFShader.setInt("envMapLUT", 8);

    saoShader.useShader();
	saoShader.setInt("gPosition", 0);
	saoShader.setInt("gNormal", 1);

    firstpassPPShader.useShader();
	firstpassPPShader.setInt("sao", 1);
	firstpassPPShader.setInt("gEffects", 2);
	firstpassPPShader.setInt("gPosition", 3);

    latlongToCubeShader.useShader();
	latlongToCubeShader.setInt("envMap", 0);

    irradianceIBLShader.useShader();
	irradianceIBLShader.setInt("envMap", 0);

    prefilterIBLShader.useShader();
	prefilterIBLShader.setInt("envMap", 0);

    gBufferSetup();

    saoSetup();

    postprocessSetup();

    iblSetup();



    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	camera.cameraFOV = glm::radians(stod(fov));

	// initialize static shader uniforms before rendering
	// --------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(camera.cameraFOV), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	pbrShader.use();
	pbrShader.setMat4("projection", projection);
	backgroundShader.use();
	backgroundShader.setMat4("projection", projection);

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	float phi = glm::radians(sunPosition.x-90);
	float theta = glm::radians(90-sunPosition.y);
	glm::vec3 dir(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
	lightPos = dir;

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 25.0f, -0.8f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.8f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.8f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		 25.0f, -0.8f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.8f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 25.0f, -0.8f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
	};
	// plane VAO
	unsigned int planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	unsigned int woodTexture = loadTexture(FileSystem::getPath("resources/textures/wood.png").c_str());

	// configure depth map FBO
	// -----------------------
	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);
	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);

	// pbr: setup framebuffer
	// ----------------------
	unsigned int captureFBO0;
	unsigned int captureRBO0;
	glGenFramebuffers(1, &captureFBO0);
	glGenRenderbuffers(1, &captureRBO0);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO0);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO0);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO0);
	int width, height, nrComponents;

	unsigned char *data0 = stbi_load(FileSystem::getPath("resources/textures/hdr/eMYQx-XepZ9t-sDFVjfvpQ_B.png").c_str(), &width, &height, &nrComponents, 0);

	
	if (data0)
	{
		glGenTextures(1, &hdrTexture0);
		glBindTexture(GL_TEXTURE_2D, hdrTexture0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data0); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data0);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}

	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	unsigned int envCubemap0;
	glGenTextures(1, &envCubemap0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap0);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection0 = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews0[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	equirectangularToCubemapShader.use();
	equirectangularToCubemapShader.setInt("equirectangularMap", 0);
	equirectangularToCubemapShader.setMat4("projection", captureProjection0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture0);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO0);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.setMat4("view", captureViews0[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);

	

    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        cameraMove();

        imGuiSetup();

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 0.0f, far_plane = 3.0f;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// floor
		glm::mat4 model0 = glm::mat4(1.0f);
		simpleDepthShader.setMat4("model", model0);
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		model0 = glm::scale(model0, modelScale);
		GLfloat rotationAngle0 = glfwGetTime() / 5.0f * modelRotationSpeed;
		model0 = glm::mat4();
		model0 = glm::translate(model0, modelPosition);
		model0 = glm::rotate(model0, rotationAngle0, modelRotationAxis);
		model0 = glm::scale(model0, modelScale);
		simpleDepthShader.setMat4("model", model0);
		objectModel.Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Camera setting
        glm::mat4 projection = glm::perspective(camera.cameraFOV, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model;

		pbrShader.use();
		pbrShader.setMat4("projection", projection);
		backgroundShader.use();
		backgroundShader.setMat4("projection", projection);

        // Model(s) rendering
        gBufferShader.useShader();

		gBufferShader.setMat4("projection", projection);
		gBufferShader.setMat4("view", view);

        GLfloat rotationAngle = glfwGetTime() / 5.0f * modelRotationSpeed;
        model = glm::mat4();
        model = glm::translate(model, modelPosition);
        model = glm::rotate(model, rotationAngle, modelRotationAxis);
        model = glm::scale(model, modelScale);

        projViewModel = projection * view * model;

		gBufferShader.setMat4("projViewModel", projViewModel);
		gBufferShader.setMat4("prevProjViewModel", prevProjViewModel);
		gBufferShader.setMat4("model", model);
		gBufferShader.setVec3("albedoColor", albedoColor.r, albedoColor.g, albedoColor.b);

        glActiveTexture(GL_TEXTURE0);
        objectAlbedo.useTexture();
		gBufferShader.setInt("texAlbedo", 0);
        glActiveTexture(GL_TEXTURE1);
        objectNormal.useTexture();
		gBufferShader.setInt("texNormal", 1);
        glActiveTexture(GL_TEXTURE2);
        objectRoughness.useTexture();
		gBufferShader.setInt("texRoughness", 2);
        glActiveTexture(GL_TEXTURE3);
        objectMetalness.useTexture();
		gBufferShader.setInt("texMetalness", 3);
        glActiveTexture(GL_TEXTURE4);
        objectAO.useTexture();
		gBufferShader.setInt("texAO", 4);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		gBufferShader.setInt("shadowMap", 5);

        objectModel.Draw();

		gBufferShader.setInt("shadowMode", 1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0, 0, WIDTH, HEIGHT);

		gBufferShader.setVec3("lightPos", lightPos);
		gBufferShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		
		model = glm::mat4(1.0f);
		gBufferShader.setMat4("model", model);
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);

		gBufferShader.setInt("shadowMode", 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        prevProjViewModel = projViewModel;

        //---------------
        // sao rendering
        //---------------
        glBindFramebuffer(GL_FRAMEBUFFER, saoFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        if (saoMode)
        {
            // SAO noisy texture
            saoShader.useShader();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);

			saoShader.setInt("saoSamples", saoSamples);
			saoShader.setFloat("saoRadius", saoRadius);
			saoShader.setInt("saoTurns", saoTurns);
			saoShader.setFloat("saoBias", saoBias);
			saoShader.setFloat("saoScale", saoScale);
			saoShader.setFloat("saoContrast", saoContrast);
			saoShader.setInt("viewportWidth", WIDTH);
			saoShader.setInt("viewportHeight", HEIGHT);

            quadRender.drawShape();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // SAO blur pass
            glBindFramebuffer(GL_FRAMEBUFFER, saoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);

            saoBlurShader.useShader();
			saoBlurShader.setInt("saoBlurSize", saoBlurSize);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, saoBuffer);

            quadRender.drawShape();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        //------------------------
        // Lighting Pass rendering
        //------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, postprocessFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingBRDFShader.useShader();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gEffects);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, saoBlurBuffer);
        glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, hdrTexture0);
        //envMapHDR.useTexture();
        glActiveTexture(GL_TEXTURE6);
        envMapIrradiance.useTexture();
        glActiveTexture(GL_TEXTURE7);
        envMapPrefilter.useTexture();
        glActiveTexture(GL_TEXTURE8);
        envMapLUT.useTexture();

        lightPoint1.setLightPosition(lightPointPosition1);
        lightPoint2.setLightPosition(lightPointPosition2);
        lightPoint3.setLightPosition(lightPointPosition3);
        lightPoint1.setLightColor(glm::vec4(lightPointColor1, 1.0f));
        lightPoint2.setLightColor(glm::vec4(lightPointColor2, 1.0f));
        lightPoint3.setLightColor(glm::vec4(lightPointColor3, 1.0f));
        lightPoint1.setLightRadius(lightPointRadius1);
        lightPoint2.setLightRadius(lightPointRadius2);
        lightPoint3.setLightRadius(lightPointRadius3);

        for (int i = 0; i < Light::lightPointList.size(); i++)
        {
            Light::lightPointList[i].renderToShader(lightingBRDFShader, camera);
        }

        lightDirectional1.setLightDirection(lightDirectionalDirection1);
        lightDirectional1.setLightColor(glm::vec4(lightDirectionalColor1, 1.0f));

        for (int i = 0; i < Light::lightDirectionalList.size(); i++)
        {
            Light::lightDirectionalList[i].renderToShader(lightingBRDFShader, camera);
        }

		lightingBRDFShader.setMat4("inverseView", view);
		lightingBRDFShader.setMat4("inverseProj", projection);

		glm::vec3 axis(0, 1, 0);
		view = glm::rotate(view, glm::radians(0.0f), axis);

		lightingBRDFShader.setMat4("view", view);
		lightingBRDFShader.setFloat("materialRoughness", materialRoughness);
		lightingBRDFShader.setFloat("materialMetallicity", materialMetallicity);
		lightingBRDFShader.setVec3("materialF0", materialF0.r, materialF0.g, materialF0.b);
		lightingBRDFShader.setFloat("ambientIntensity", ambientIntensity);
		lightingBRDFShader.setInt("gBufferView", gBufferView);
		lightingBRDFShader.setInt("pointMode", pointMode);
		lightingBRDFShader.setInt("directionalMode", directionalMode);
		lightingBRDFShader.setInt("iblMode", iblMode);
		lightingBRDFShader.setInt("attenuationMode", attenuationMode);

        quadRender.drawShape();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        //-------------------------------
        // Post-processing Pass rendering
        //-------------------------------
        glClear(GL_COLOR_BUFFER_BIT);

        firstpassPPShader.useShader();
		firstpassPPShader.setInt("gBufferView", gBufferView);
		firstpassPPShader.setVec2("screenTextureSize", 1.0f / WIDTH, 1.0f / HEIGHT);
		firstpassPPShader.setFloat("cameraAperture", cameraAperture);
		firstpassPPShader.setFloat("cameraShutterSpeed", cameraShutterSpeed);
		firstpassPPShader.setFloat("cameraISO", cameraISO);
		firstpassPPShader.setInt("saoMode", saoMode);
		firstpassPPShader.setInt("fxaaMode", fxaaMode);
		firstpassPPShader.setInt("motionBlurMode", motionBlurMode);
		firstpassPPShader.setFloat("motionBlurScale", int(ImGui::GetIO().Framerate) / 60.0f);
		firstpassPPShader.setInt("motionBlurMaxSamples", motionBlurMaxSamples);
		firstpassPPShader.setInt("tonemappingMode", tonemappingMode);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postprocessBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, saoBlurBuffer);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gEffects);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gPosition);

        quadRender.drawShape();



        //-----------------------
        // Forward Pass rendering
        //-----------------------
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        // Copy the depth informations from the Geometry Pass into the default framebuffer
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Shape(s) rendering
        if (pointMode)
        {
            simpleShader.useShader();
			simpleShader.setMat4("projection", projection);
			simpleShader.setMat4("view", view);

            for (int i = 0; i < Light::lightPointList.size(); i++)
            {
				simpleShader.setVec4("lightColor", Light::lightPointList[i].getLightColor().r, Light::lightPointList[i].getLightColor().g, Light::lightPointList[i].getLightColor().b, Light::lightPointList[i].getLightColor().a);

                if (Light::lightPointList[i].isMesh())
                    Light::lightPointList[i].lightMesh.drawShape(simpleShader, view, projection, camera);
            }
        }

		debugDepthQuad.use();
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		//renderQuad();

        //----------------
        // ImGui rendering
        //----------------
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    //---------
    // Cleaning
    //---------
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    return 0;
}



void cameraMove()
{

    if (io.KeysDown[GLFW_KEY_W])
        camera.keyboardCall(FORWARD, deltaTime);
    if (io.KeysDown[GLFW_KEY_S])
        camera.keyboardCall(BACKWARD, deltaTime);
    if (io.KeysDown[GLFW_KEY_A])
        camera.keyboardCall(LEFT, deltaTime);
    if (io.KeysDown[GLFW_KEY_D])
        camera.keyboardCall(RIGHT, deltaTime);
}


void imGuiSetup()
{

    ImGui_ImplGlfwGL3_NewFrame();

    ImGui::Begin(u8"属性配置", &guiIsOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
    ImGui::SetWindowSize(ImVec2(350, HEIGHT));

    if (ImGui::CollapsingHeader(u8"渲染", 0, true, true))
    {
        if (ImGui::TreeNode(u8"光照"))
        {
            if (ImGui::TreeNode(u8"模式"))
            {
                ImGui::Checkbox(u8"点", &pointMode);
                ImGui::Checkbox(u8"方向", &directionalMode);
                ImGui::Checkbox(u8"图像照明", &iblMode);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(u8"点光源"))
            {
                if (ImGui::TreeNode(u8"位置"))
                {
                    ImGui::SliderFloat3(u8"点 1", (float*)&lightPointPosition1, -5.0f, 5.0f);
                    ImGui::SliderFloat3(u8"点 2", (float*)&lightPointPosition2, -5.0f, 5.0f);
                    ImGui::SliderFloat3(u8"点 3", (float*)&lightPointPosition3, -5.0f, 5.0f);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode(u8"颜色"))
                {
                    ImGui::ColorEdit3(u8"点 1", (float*)&lightPointColor1);
                    ImGui::ColorEdit3(u8"点 2", (float*)&lightPointColor2);
                    ImGui::ColorEdit3(u8"点 3", (float*)&lightPointColor3);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode(u8"半径"))
                {
                    ImGui::SliderFloat(u8"点 1", &lightPointRadius1, 0.0f, 10.0f);
                    ImGui::SliderFloat(u8"点 2", &lightPointRadius2, 0.0f, 10.0f);
                    ImGui::SliderFloat(u8"点 3", &lightPointRadius3, 0.0f, 10.0f);

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(u8"方向光"))
            {
                if (ImGui::TreeNode(u8"方向"))
                {
                    ImGui::SliderFloat3(u8"方向 1", (float*)&lightDirectionalDirection1, -5.0f, 5.0f);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode(u8"颜色"))
                {
                    ImGui::ColorEdit3(u8"方向 1", (float*)&lightDirectionalColor1);

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(u8"环境贴图"))
            {
				static char str0[128] = u8"resources/textures/hdr/environment.hdr";
				ImGui::InputText(u8"地址", str0, 128);
                if (ImGui::Button(u8"确认"))
                {
                    envMapHDR.setTextureHDR(str0, "appartHDR", true);
                    iblSetup();
                }
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode(u8"后期处理"))
        {
            if (ImGui::TreeNode("SAO"))
            {
                ImGui::Checkbox(u8"启用", &saoMode);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("FXAA"))
            {
                ImGui::Checkbox(u8"启用", &fxaaMode);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(u8"色调映射"))
            {
                ImGui::RadioButton("Reinhard", &tonemappingMode, 1);
                ImGui::RadioButton("Filmic", &tonemappingMode, 2);
                ImGui::RadioButton("Uncharted", &tonemappingMode, 3);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode(u8"虚拟物体"))
        {
            ImGui::SliderFloat3(u8"位置", (float*)&modelPosition, -5.0f, 5.0f);
            ImGui::SliderFloat(u8"转动速度", &modelRotationSpeed, 0.0f, 50.0f);
            ImGui::SliderFloat3(u8"转轴", (float*)&modelRotationAxis, 0.0f, 1.0f);

            if (ImGui::TreeNode(u8"模型"))
            {
                if (ImGui::Button(u8"球"))
                {
                    objectModel.~Model();
                    objectModel.loadModel("resources/models/sphere/sphere.obj");
                    modelScale = glm::vec3(0.6f);
                }

                if (ImGui::Button("Teapot"))
                {
                    objectModel.~Model();
                    objectModel.loadModel("resources/models/teapot/teapot.obj");
                    modelScale = glm::vec3(0.6f);
                }

                if (ImGui::Button("Shader ball"))
                {
                    objectModel.~Model();
                    objectModel.loadModel("resources/models/shaderball/shaderball.obj");
                    modelScale = glm::vec3(0.1f);
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode(u8"材质"))
            {
                if (ImGui::Button(u8"材质1"))
                {
                    objectAlbedo.setTexture("resources/textures/pbr/rustediron/rustediron_albedo.png", "ironAlbedo", true);
                    objectNormal.setTexture("resources/textures/pbr/rustediron/rustediron_normal.png", "ironNormal", true);
                    objectRoughness.setTexture("resources/textures/pbr/rustediron/rustediron_roughness.png", "ironRoughness", true);
                    objectMetalness.setTexture("resources/textures/pbr/rustediron/rustediron_metalness.png", "ironMetalness", true);
                    objectAO.setTexture("resources/textures/pbr/rustediron/rustediron_ao.png", "ironAO", true);

                    materialF0 = glm::vec3(0.04f);
                }

                if (ImGui::Button(u8"材质2"))
                {
                    objectAlbedo.setTexture("resources/textures/pbr/gold/gold_albedo.png", "goldAlbedo", true);
                    objectNormal.setTexture("resources/textures/pbr/gold/gold_normal.png", "goldNormal", true);
                    objectRoughness.setTexture("resources/textures/pbr/gold/gold_roughness.png", "goldRoughness", true);
                    objectMetalness.setTexture("resources/textures/pbr/gold/gold_metalness.png", "goldMetalness", true);
                    objectAO.setTexture("resources/textures/pbr/gold/gold_ao.png", "goldAO", true);

                    materialF0 = glm::vec3(1.0f, 0.72f, 0.29f);
                }

                if (ImGui::Button(u8"材质3"))
                {
                    objectAlbedo.setTexture("resources/textures/pbr/woodfloor/woodfloor_albedo.png", "woodfloorAlbedo", true);
                    objectNormal.setTexture("resources/textures/pbr/woodfloor/woodfloor_normal.png", "woodfloorNormal", true);
                    objectRoughness.setTexture("resources/textures/pbr/woodfloor/woodfloor_roughness.png", "woodfloorRoughness", true);
                    objectMetalness.setTexture("resources/textures/pbr/woodfloor/woodfloor_metalness.png", "woodfloorMetalness", true);
                    objectAO.setTexture("resources/textures/pbr/woodfloor/woodfloor_ao.png", "woodfloorAO", true);

                    materialF0 = glm::vec3(0.04f);
                }

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }
    if (ImGui::CollapsingHeader(u8"性能表现", 0, true, true))
    {
        char* glInfos = (char*)glGetString(GL_VERSION);
        char* hardwareInfos = (char*)glGetString(GL_RENDERER);

        ImGui::Text(u8"OpenGL版本:");
        ImGui::Text(glInfos);
        ImGui::Text(u8"硬件信息");
        ImGui::Text(hardwareInfos);
        ImGui::Text(u8"\n帧率 %.2f FPS / 帧生成时间 %.4f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    }

	if (ImGui::CollapsingHeader(u8"参数", 0, true, true))
	{
		char* glInfos = (char*)glGetString(GL_VERSION);
		char* hardwareInfos = (char*)glGetString(GL_RENDERER);

		ImGui::Text(u8"太阳位置:");
		ImGui::Text(u8"theta %.2f phi %.2f", sunPosition.x, sunPosition.y);
		ImGui::Text(u8"大气浑浊度");
		ImGui::Text(tur.c_str());
		ImGui::Text(u8"仰角");
		ImGui::Text(elv.c_str());
		ImGui::Text(u8"FOV");
		ImGui::Text(fov.c_str());
		
	}
    ImGui::End();
	ImGui::Begin(u8"光照还原", &guiIsOpen, ImVec2(50, 50), 0.5f,  ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(350, 350));
	ImGui::SetWindowPos(ImVec2(WIDTH - 350, 0));
	if (ImGui::CollapsingHeader(u8"原始图像输入", 0, true, true)){
		//ImGui::Text(u8"如：C:\\outputa.png");
		static char str0[128] = u8"output.png";
		ImGui::InputText(u8"地址", str0, 128);
		if (ImGui::Button(u8"生成场景"))
		{
			CString paramstr;
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;

			//要执行的完整CMD命令，一般是一个字符串
			paramstr.Format("python C:\\Users\\caiyx\\Desktop\\dashcam-illumination-estimation-master\\inference.py --img_path  %s --pre_trained  C:\\Users\\caiyx\\Desktop\\dashcam-illumination-estimation-master\\weights.pth", str0);
			//创建线程
			BOOL fRet1 = CreateProcess(NULL, paramstr.GetBuffer(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
			if (fRet1 == TRUE)
			{
				WaitForSingleObject(pi.hThread, INFINITE);
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			Sleep(100);

			//终止子进程  
			TerminateProcess(pi.hProcess, 300);

			//要执行的完整CMD命令，一般是一个字符串
			paramstr.Format("python C:\\Users\\caiyx\\Desktop\\dashcam-illumination-estimation-master\\exr2hdr.py");
			//创建线程
			BOOL fRet2 = CreateProcess(NULL, paramstr.GetBuffer(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
			if (fRet2 == TRUE)
			{
				WaitForSingleObject(pi.hThread, INFINITE);
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
			Sleep(100);

			//终止子进程  
			TerminateProcess(pi.hProcess, 300);

			//终止本进程，状态码  
			//ExitProcess(1001);
			static char str[128];
			for (int i = 0, j = 0; i < 128; i++,j++) {
				if (str0[i] == '\\') {
					str[j] = '/';
					i++;
				}
				else {
					str[j] = str0[i];
				}
				
			}

			int width, height, nrComponents;

			unsigned char *data0 = stbi_load(str, &width, &height, &nrComponents, 0);

			if (data0)
			{
				glGenTextures(1, &hdrTexture0);
				glBindTexture(GL_TEXTURE_2D, hdrTexture0);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data0); // note how we specify the texture's data value to be float

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data0);
			}
			else
			{
				std::cout << "Failed to load Background image." << str << std::endl;
			}

			vector<string> vec;
			ifstream myfile("para.txt");
			string temp;
			if (!myfile.is_open())
			{
				cout << "未成功打开文件" << endl;
			}
			while (getline(myfile, temp))
			{
				vec.push_back(temp);
			}
			for (auto it = vec.begin(); it != vec.end(); it++)
			{
				cout << *it << endl;
			}

			sunPosition = glm::vec2(stod(vec[0]), stod(vec[1]));
			float phi = glm::radians(sunPosition.x - 90);
			float theta = glm::radians(90 - sunPosition.y);
			glm::vec3 dir(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
			lightPos = dir;
			lightDirectionalDirection1 = -dir;
			tur = vec[2];
			elv = vec[3];
			fov = vec[4];

			camera.cameraFOV = glm::radians(stod(fov));

			envMapHDR.setTextureHDR("environment.hdr", "appartHDR", true);
			iblSetup();
		}
	}
	ImGui::End();
}

void gBufferSetup()
{
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // Position
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // Albedo + Roughness
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gAlbedo, 0);

    // Normals + Metalness
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

    // Effects (AO + Velocity)
    glGenTextures(1, &gEffects);
    glBindTexture(GL_TEXTURE_2D, gEffects);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEffects, 0);

    // Define the COLOR_ATTACHMENTS for the G-Buffer
    GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);

    // Z-Buffer
    glGenRenderbuffers(1, &zBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, zBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, zBuffer);

    // Check if the framebuffer is complete before continuing
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete !" << std::endl;
}

void saoSetup()
{
    // SAO Buffer
    glGenFramebuffers(1, &saoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, saoFBO);
    glGenTextures(1, &saoBuffer);
    glBindTexture(GL_TEXTURE_2D, saoBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, saoBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SAO Framebuffer not complete !" << std::endl;

    // SAO Blur Buffer
    glGenFramebuffers(1, &saoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, saoBlurFBO);
    glGenTextures(1, &saoBlurBuffer);
    glBindTexture(GL_TEXTURE_2D, saoBlurBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, saoBlurBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SAO Blur Framebuffer not complete !" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void postprocessSetup()
{
    // Post-processing Buffer
    glGenFramebuffers(1, &postprocessFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessFBO);

    glGenTextures(1, &postprocessBuffer);
    glBindTexture(GL_TEXTURE_2D, postprocessBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postprocessBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Postprocess Framebuffer not complete !" << std::endl;
}

void iblSetup()
{
    // Latlong to Cubemap conversion
    glGenFramebuffers(1, &envToCubeFBO);
    glGenRenderbuffers(1, &envToCubeRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, envToCubeFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, envToCubeRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapCube.getTexWidth(), envMapCube.getTexHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, envToCubeRBO);

    latlongToCubeShader.useShader();

	latlongToCubeShader.setMat4("projection", envMapProjection);
    glActiveTexture(GL_TEXTURE0);
    envMapHDR.useTexture();

    glViewport(0, 0, envMapCube.getTexWidth(), envMapCube.getTexHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, envToCubeFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
		latlongToCubeShader.setMat4("view", envMapView[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMapCube.getTexID(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        envCubeRender.drawShape();
    }

    envMapCube.computeTexMipmap();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Diffuse irradiance capture
    glGenFramebuffers(1, &irradianceFBO);
    glGenRenderbuffers(1, &irradianceRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, irradianceRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapIrradiance.getTexWidth(), envMapIrradiance.getTexHeight());

    irradianceIBLShader.useShader();
	irradianceIBLShader.setMat4("projection", envMapProjection);
    glActiveTexture(GL_TEXTURE0);
    envMapCube.useTexture();

    glViewport(0, 0, envMapIrradiance.getTexWidth(), envMapIrradiance.getTexHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, irradianceFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
		irradianceIBLShader.setMat4("view", envMapView[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMapIrradiance.getTexID(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        envCubeRender.drawShape();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Prefilter cubemap
    prefilterIBLShader.useShader();
	prefilterIBLShader.setMat4("projection", envMapProjection);
    envMapCube.useTexture();

    glGenFramebuffers(1, &prefilterFBO);
    glGenRenderbuffers(1, &prefilterRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, prefilterFBO);

    unsigned int maxMipLevels = 5;

    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth = envMapPrefilter.getTexWidth() * std::pow(0.5, mip);
        unsigned int mipHeight = envMapPrefilter.getTexHeight() * std::pow(0.5, mip);

        glBindRenderbuffer(GL_RENDERBUFFER, prefilterRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);

        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);

		prefilterIBLShader.setFloat("roughness", roughness);
		prefilterIBLShader.setFloat("cubeResolutionWidth", envMapPrefilter.getTexWidth());
		prefilterIBLShader.setFloat("cubeResolutionHeight", envMapPrefilter.getTexHeight());

        for (unsigned int i = 0; i < 6; ++i)
        {
			prefilterIBLShader.setMat4("view", envMapView[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMapPrefilter.getTexID(), mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            envCubeRender.drawShape();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // BRDF LUT
    glGenFramebuffers(1, &brdfLUTFBO);
    glGenRenderbuffers(1, &brdfLUTRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, brdfLUTFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, brdfLUTRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envMapLUT.getTexWidth(), envMapLUT.getTexHeight());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, envMapLUT.getTexID(), 0);

    glViewport(0, 0, envMapLUT.getTexWidth(), envMapLUT.getTexHeight());
    integrateIBLShader.useShader();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    quadRender.drawShape();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, WIDTH, HEIGHT);
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
    {
        screenMode = !screenMode;


    }

    if (keys[GLFW_KEY_1])
        gBufferView = 1;

    if (keys[GLFW_KEY_2])
        gBufferView = 2;

    if (keys[GLFW_KEY_3])
        gBufferView = 3;

    if (keys[GLFW_KEY_4])
        gBufferView = 4;

    if (keys[GLFW_KEY_5])
        gBufferView = 5;

    if (keys[GLFW_KEY_6])
        gBufferView = 6;

    if (keys[GLFW_KEY_7])
        gBufferView = 7;

    if (keys[GLFW_KEY_8])
        gBufferView = 8;

    if (keys[GLFW_KEY_9])
        gBufferView = 9;

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (cameraMode)
        camera.mouseCall(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        cameraMode = true;
    else
        cameraMode = false;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (cameraMode)
        camera.scrollCall(yoffset);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}