#include "stdafx.h"
#include "Dust.h"

classDust::classDust(vpObserver *_observer, vsChannel::Subscriber *_subscriber):classBase(_observer,_subscriber,ENABLE_KEY|ENABLE_RUNLOOP)
{
	initDust();
}

classDust::classDust(const char* _name, vsChannel::Subscriber *_subscriber):classBase(_name,_subscriber,ENABLE_KEY|ENABLE_RUNLOOP)
{
	initDust();
}

classDust::~classDust(void)
{
	pFxParticleSystem->setEnable(false);
	pFxParticleSystem->unref();

	pIsectorHAT->setEnable(false);
	pIsectorHAT->setTarget(NULL);
	pIsectorHAT->unref();

	density = 0;
	isEnable = false;
}

void classDust::initDust(void)
{
	pIsectorHAT = NULL;
	pIsectorHAT = vpIsectorHAT::find("myIsector");
	if (pIsectorHAT == NULL)
	{std::cout << "Dust: pIsectorHAT(\"myIsector\") is NULL!" << std::endl;system("pause");}
	pIsectorHAT->setEnable(false);
	pIsectorHAT->setTarget(pObject);
	pIsectorHAT->ref();

	pFxParticleSystem = NULL;
	pFxParticleSystem = vpFxParticleSystem::find("myFx");
	if (pFxParticleSystem == NULL)
	{std::cout << "Dust: pFxParticleSystem(\"myFx\") is NULL!" << std::endl;system("pause");}
	pFxParticleSystem->setEnable(false);
	pFxParticleSystem->ref();

	density = 0;
	isEnable = false;
}

void classDust::dustGo(void)
{
	if(isEnable == true)
	{
		pFxParticleSystem->setRenderMask(1);
		if (!pIsectorHAT->getEnable())
			pIsectorHAT->setEnable(true);

		pObserver->getPosition(&position[0],&position[1],&position[2]);
		pIsectorHAT->setTranslate(position[0], position[1], position[2]);

		double height = 1000.0f;
		if(pIsectorHAT->getHit())
			height = pIsectorHAT->getHAT();

		if(height <= 50.0f)
		{
			float offset = 0 - height - height - (19 - density)*2;        //density影响粒子团的位置，利用地形的遮挡而影响场景中的例子数量
			pFxParticleSystem->setTranslate(position[0],position[1],position[2] + offset);
			if (!pFxParticleSystem->getEnable())
				pFxParticleSystem->setEnable(true);
		}
		else
			pFxParticleSystem->setEnable(false);
	}
	else
	{
		if (pFxParticleSystem->getEnable())
			pFxParticleSystem->setEnable(false);
		if (pIsectorHAT->getEnable())
			pIsectorHAT->setEnable(false);
	}
}

void classDust::handleKey(vrWindow::Key _key, int _mod)
{
	switch (_key)
	{
	case vrWindow::KEY_x:
		isEnable = !isEnable;
		if (density == 0) isEnable = false;
		std::cout << "Dust Enable: " << isEnable << std::endl;
		break;
	case vrWindow::KEY_a:
		density = density > 19 ? 19 : (density + 1);
		break;
	case vrWindow::KEY_z:
		density = density <= 0 ? 0 : (density - 1);
		break;
	}
}

void classDust::handleRunLoop(void)
{
	dustGo();
}

void classDust::dustSwitch(bool _enable)
{
	isEnable = _enable;
}

void classDust::setDensity(float _density)
{
	if (_density <= 0)
	{
		dustSwitch(false);
		_density = 0;
	}
	else 
	{
		dustSwitch(true);
		density = _density;
	}
}