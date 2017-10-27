#ifndef _RAIN_H
#define _RAIN_H

#include "stdafx.h"
#include "Base.h"
#include "ParticleSystem.h"

class classRain:public classBase
{
public:
	enum weatherLevel
	{
		GREEN,
		BLUE,
		YELLOW,
		ORANGE,
		RED
	};

	classRain(vpObserver *_observer, vsChannel::Subscriber *_subscriber = NULL);
	classRain(const char *_name, vsChannel::Subscriber *_subscriber = NULL);
	~classRain();

	void setRainLevel(weatherLevel);
	virtual void handleKey(vrWindow::Key _key, int _mod);
	virtual void handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context);
protected:
private:
	ParticleSystem *pParticleSystem;
	weatherLevel RainLevel;
	bool init_flag;				//粒子系统初始化完成标志
};

#endif
