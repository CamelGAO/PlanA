#ifndef _SNOW_H
#define _SNOW_H

#include "stdafx.h"
#include "Base.h"
#include "ParticleSystem.h"

class classSnow:public classBase
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

	classSnow(vpObserver *_observer, vsChannel::Subscriber *_subscriber = NULL);
	classSnow(const char *_name, vsChannel::Subscriber *_subscriber = NULL);
	~classSnow();

	void setSnowLevel(weatherLevel);
	virtual void handleKey(vrWindow::Key _key, int _mod);
	virtual void handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context);
protected:
private:
	ParticleSystem *pParticleSystem;
	weatherLevel SnowLevel;
	bool init_flag;       //粒子系统初始化完成标志
};

#endif
