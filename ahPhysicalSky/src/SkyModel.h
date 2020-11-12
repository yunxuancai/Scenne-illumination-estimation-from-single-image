#ifndef SKYMODEL_H
#define SKYMODEL_H

#include "ShaderUtils.h"

// Hosek Sky Model forward declare
struct ArHosekSkyModelState;

struct SkyModel
{

	ArHosekSkyModelState *StateX = nullptr;
	ArHosekSkyModelState *StateY = nullptr;
	ArHosekSkyModelState *StateZ = nullptr;

	Vector3f SunDir;
	Vector3f SunRadiance;
	Vector3f SunIrradiance;
	Vector3f SunRenderColor;
	float SunSize = 0.0f;
	float Turbidity = 0.0f;
	Vector3f Albedo;
	float Elevation = 0.0f;
	ColorSpace Colorspace;

	void Shutdown();
	~SkyModel() {};
	void SetupSky(const Vector3f &_sunDir, float _sunSize, Vector3f _sunRenderColor, const Vector3f _groundAlbedo, float _turbidity, ColorSpace _colorspace);
	bool Initialized() const { return StateX != nullptr; }
	Vector3f Sample(Vector3f _sampleDir, bool _bEnableSun, Vector3f _skyTint, Vector3f _sunTint, float _exposure) const;
};

#endif
