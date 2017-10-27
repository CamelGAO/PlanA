#ifndef _DUST_H
#define _DUST_H

#include "stdafx.h"
#include "Base.h"

class classDust:public classBase
{
public:
	classDust(vpObserver *_observer, vsChannel::Subscriber *_subscriber = NULL);
	classDust(const char* _name, vsChannel::Subscriber *_subscriber = NULL);
	~classDust();

	virtual void handleKey(vrWindow::Key _key, int _mod);
	virtual void handleRunLoop(void);
	void initDust(void);
	void removeDust(void);
	void dustGo(void);

	void dustSwitch(bool _enable);
	void setDensity(float _density);


protected:
private:
	vpIsectorHAT * pIsectorHAT;
	vpFxParticleSystem * pFxParticleSystem;

	int density;
	double position[3];
	double rotate[3];

	bool isEnable;
};

#endif
