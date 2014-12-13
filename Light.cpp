#include "stdafx.h"
#include "Light.h"
#include "Util.h"


Light::Light() : direction(Vector3F(0, 1, 0)), angle(360), color(Vector3F(128, 128, 128)), range(float_max), intensity(1), type(LIGHT_DIRECTIONAL){}
Light::Light(LightType lt, Vector3F direction, Vector3F color) : direction(direction), color(color), intensity(1), type(lt){}
