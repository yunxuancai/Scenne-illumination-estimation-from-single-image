#ifndef RENDERER_H
#define RENDERER_H

#include "SkyModel.h"

struct Renderer {

	~Renderer() {};

	unsigned Width, Height, Samples;
	float SunSize;
	Vector3f SunColor;
	Vector3f GroundAlbedo;
	float Turbidity;
	Vector3f SkyTint;
	Vector3f SunTint;
	bool bEnableSun;
	ColorSpace Colorspace;

	void RenderSkyDome(Vector3f sunDir, const char *filename, unsigned width, unsigned height, float sunSize, Vector3f sunColor, Vector3f groundAlbedo, float turbidity, Vector3f skyTint, Vector3f sunTint, bool bEnableSun, ColorSpace colorspace, float exposure = 1.0f);
};

#endif