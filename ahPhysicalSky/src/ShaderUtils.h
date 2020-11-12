#ifndef SHADERUTILS_H
#define SHADERUTILS_H

#include "Geometry.h"
#include <cassert>

enum ColorSpace { XYZ, sRGB, ACEScg, ACES2065_1 };

// Scale factor used for storing physical light units in fp16 floats (equal to 2^-10)
static const float FP16Scale = 0.0009765625f;
static const float FP16Max = 65000.0f;

// Useful shader functions
inline float Deg2Rad(float deg) { return deg * (1.0f / 180.0f) * M_PI; }
inline float Rad2Deg(float rad) { return rad * (1.0f / M_PI) * 180.0f; }
inline float Clamp(float val, float min, float max) { assert(max >= min); val = val < min ? min : val > max ? max : val; return val; }
inline static float AngleBetween(const Vector3f &dir0, const Vector3f &dir1) { return std::acos(std::max(dir0.dotProduct(dir1), 0.00001f)); }
inline float Mix(float x, float y, float s) { return x + (y - x) * s; }

inline Vector3f XYZ_to_ACES2065_1(Vector3f color)
{
	Vector3f out = Vector3f(0);
	out.x = color.x * 1.0498110175f + color.y * 0.0000000000f + color.z * -0.0000974845f;
	out.y = color.x * -0.4959030231f + color.y * 1.3733130458f + color.z * 0.0982400361f;
	out.z = color.x * 0.0000000000f + color.y * 0.0000000000f + color.z *  0.9912520182f;

	return out;
}

inline Vector3f ACES2065_1_to_ACEScg(Vector3f color)
{
	Vector3f out = Vector3f(0);
	out.x = color.x * 1.4514393161f + color.y * -0.2365107469f + color.z * -0.2149285693f;
	out.y = color.x * -0.0765537733f + color.y * 1.1762296998f + color.z * -0.0996759265f;
	out.z = color.x * 0.0083161484f + color.y * -0.0060324498f + color.z *  0.9977163014f;

	return out;
}

inline Vector3f ACES2065_1_to_sRGB(Vector3f color)
{
	Vector3f out = Vector3f(0);	
	out.x = color.x * 2.5216494298f + color.y * -1.1368885542f + color.z * -0.3849175932f;
	out.y = color.x * -0.2752135512f + color.y * 1.3697051510f + color.z * -0.0943924508f;
	out.z = color.x * -0.0159250101f + color.y * -0.1478063681f + color.z * 1.1638058159f;

	return out;

}

#endif
