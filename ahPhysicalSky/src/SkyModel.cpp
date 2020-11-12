#include "ShaderUtils.h"
#include "SkyModel.h"
#include "../HosekSky/ArHosekSkyModel.h"
#include <iostream>
#include <cstdlib>

// Actual physical size of the sun, expressed as an angular radius in radians
static const float PhysicalSunSize = Deg2Rad(0.27f);
static const float CosPhysicalSunSize = std::cos(PhysicalSunSize);

void SkyModel::SetupSky(const Vector3f &_sunDir, float _sunSize, Vector3f _sunRenderColor, const Vector3f _groundAlbedo, float _turbidity, ColorSpace _colorspace)
{
	Vector3f sunDir = _sunDir;
	Vector3f groundAlbedo = _groundAlbedo;
	sunDir.y = Clamp(sunDir.y, 0.0, 1.0);
	sunDir = sunDir.normalize();
	//_turbidity = _turbidity;// Clamp(_turbidity, 1.0f, 32.0f);
	//groundAlbedo = Vector3f(Clamp(groundAlbedo.x, 0.0, 1.0), Clamp(groundAlbedo.y, 0.0, 1.0), Clamp(groundAlbedo.z, 0.0, 1.0));
	//_sunSize = std::max(_sunSize, 0.01f);

	Colorspace = _colorspace;

	Shutdown();

	float thetaS = AngleBetween(sunDir, Vector3f(0, 1, 0));
	float elevation = M_PI_2 - thetaS;
	StateX = arhosek_xyz_skymodelstate_alloc_init(_turbidity, groundAlbedo.x, elevation);
	StateY = arhosek_xyz_skymodelstate_alloc_init(_turbidity, groundAlbedo.y, elevation);
	StateZ = arhosek_xyz_skymodelstate_alloc_init(_turbidity, groundAlbedo.z, elevation);

	Albedo = groundAlbedo;
	Elevation = elevation;
	SunDir = sunDir;
	Turbidity = _turbidity;
	SunSize = std::cos(Deg2Rad(_sunSize));
	SunRenderColor = _sunRenderColor;

}

void SkyModel::Shutdown()
{
	if (StateX != nullptr)
	{
		arhosekskymodelstate_free(StateX);
		StateX = nullptr;
	}

	if (StateY != nullptr)
	{
		arhosekskymodelstate_free(StateY);
		StateY = nullptr;
	}

	if (StateZ != nullptr)
	{
		arhosekskymodelstate_free(StateZ);
		StateZ = nullptr;
	}

}

Vector3f SkyModel::Sample(Vector3f _sampleDir, bool _bEnableSun, Vector3f _skyTint, Vector3f _sunTint, float _exposure) const
{
	assert(StateX != nullptr);

	float gamma = AngleBetween(_sampleDir, SunDir);
	float theta = AngleBetween(_sampleDir, Vector3f(0, 1, 0));

	Vector3f radiance;

	radiance.x = float(arhosek_tristim_skymodel_radiance(StateX, theta, gamma, 0));
	radiance.y = float(arhosek_tristim_skymodel_radiance(StateY, theta, gamma, 1));
	radiance.z = float(arhosek_tristim_skymodel_radiance(StateZ, theta, gamma, 2));

	// If raw XYZ values are required
	if (Colorspace == XYZ)
		return radiance;

	// Move to workable RGB color space
	radiance = XYZ_to_ACES2065_1(radiance);

	radiance.x *= _skyTint.x;
	radiance.y *= _skyTint.y;
	radiance.z *= _skyTint.z;


	if (_bEnableSun) {
		float cosGamma = _sampleDir.dotProduct(SunDir);
		if (cosGamma >= SunSize)
			radiance += SunRenderColor * _sunTint;
	}


	// Multiply by standard luminous effiacy of 683 lm/W to bring us in line with photometric units used during rendering
	//radiance *= 683.0f

	// Color space conversion. Default is ACES2065-1
	if (Colorspace == sRGB)
		radiance = ACES2065_1_to_sRGB(radiance);

	if (Colorspace == ACEScg)
		radiance = ACES2065_1_to_ACEScg(radiance);

	//radiance *= 50.0f;

	//radiance *= _exposure;

	return radiance*FP16Scale;
}

