#include "stdafx.h"
#include "Fog.h"

classFog::classFog(vpObserver *_observer, vsChannel::Subscriber *_subscriber):classBase(_observer,_subscriber,ENABLE_KEY)
{
	shader_fogVisibility = vuParameter::obtain< vuParameterTyped<float> >("fog_visibility");
	shader_fogColor = vuParameter::obtain< vuParameterTyped<vuVec4f> >("fog_color");

	setVisibility(80000);
	setColor(1.0f);
}

classFog::classFog(const char* _name, vsChannel::Subscriber *_subscriber):classBase(_name,_subscriber,ENABLE_KEY)
{
	shader_fogVisibility = vuParameter::obtain< vuParameterTyped<float> >("fog_visibility");
	shader_fogColor = vuParameter::obtain< vuParameterTyped<vuVec4f> >("fog_color");

	setVisibility(80000);
	setColor(1.0f);
}

classFog::~classFog()
{
	pEnv->setVisibilityRange(80000.0);
	pEnv->setVisibilityColor(1.0,1.0,1.0,1.0);
}

void classFog::handleKey(vrWindow::Key _key, int _mod)
{
	float visibility;
	static bool color_flag = true;
	switch (_key)
	{
	case vrWindow::KEY_EQUAL_SIGN:
		visibility = getVisibility();
		pEnv->setVisibilityRange( visibility>100?(visibility*=0.75):(visibility) );
		break;
	case vrWindow::KEY_MINUS_SIGN:
		visibility = getVisibility();
		pEnv->setVisibilityRange( visibility<80000.0f?(visibility*=1.25f):(visibility=80000.0f) );
		break;
	case vrWindow::KEY_0:
		if(color_flag) setColor(229.0f/255,217.0f/255,194.0f/255,1.0f);
		else setColor(1.0,1.0,1.0,1.0);
		color_flag = !color_flag;
	}
}

void classFog::setVisibility(float	_visibility)   
{
	pEnv->setVisibilityRange(_visibility == 80000.0f ? -1 : _visibility);
	shader_fogVisibility->set(_visibility);
}

float classFog::getVisibility(void)
{
	return pEnv->getVisibilityRange();
}

void classFog::setColor(float _r, float _g, float _b, float _a)
{
	pEnv->setVisibilityColor(_r,_g,_b,_a);
	shader_fogColor->set(vuVec4f(_r,_g,_b,_a));
}

void classFog::setColor(float _flag)
{
	if (_flag > 0.5)
		setColor(1.0,1.0,1.0,1.0);
	else
		setColor(229.0f/255,217.0f/255,194.0f/255,1.0f);
}
