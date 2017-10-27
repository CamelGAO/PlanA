#ifndef _TIME_H
#define _TIME_H

#include "stdafx.h"
#include "Base.h"

class classTime:public classBase
{
public:
	classTime(vpObserver *_observer, vsChannel::Subscriber *_subscriber = NULL);
	classTime(const char* _name, vsChannel::Subscriber *_subscriber = NULL);
	~classTime();

	virtual void handleKey(vrWindow::Key _key, int _mod);
	
	void setTime(float _time);
	float getTime(void);

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> shader_time;

	static float timeFactor[13];

protected:
private:
};

#endif
