#include "stdafx.h"
#include "Time.h"

float classTime::timeFactor[13] = {0.172549,0.121569,0.25,0.603922,0.811765,0.941176,0.992157,0.972549,0.878431,0.772549,0.6,0.184314,0.180392};


classTime::classTime(vpObserver *_observer, vsChannel::Subscriber *_subscriber):classBase(_observer,_subscriber,ENABLE_KEY)
{
	shader_time = vuParameter::obtain< vuParameterTyped<float> >("timefactor");
	setTime(12);
}

classTime::classTime(const char* _name, vsChannel::Subscriber *_subscriber):classBase(_name,_subscriber,ENABLE_KEY)
{
	shader_time = vuParameter::obtain< vuParameterTyped<float> >("time_factor");
	setTime(12);
}

classTime::~classTime()
{
	setTime(12);
}

void classTime::handleKey(vrWindow::Key _key, int _mod)
{
	switch (_key) 
	{
	case vrWindow::KEY_9:
		float time = getTime();
		time+=2.0f;
		setTime( time >= 24 ? (time = 2) : time);
		break;
	}
}
void classTime::setTime(float _time)
{
	pEnv->setTimeOfDay(_time);
	shader_time -> set(timeFactor[(size_t)(_time*0.5)]);
}

float classTime::getTime(void)
{
	return pEnv->getTimeOfDay();
}