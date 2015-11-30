#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Vector3F.h"

class Light
{
public:

	Vector3F oDirection;
	Vector3F direction;
	float angle;

	Vector3F oOrigin;
	Vector3F origin;
	Vector3F color;
	float range;
	float intensity;
	enum LightType{LIGHT_DIRECTIONAL, LIGHT_POINT, LIGHT_SPOTLIGHT, LIGHT_AMBIENT } type;

	Light();
	Light(LightType, Vector3F, Vector3F);
};

#endif
