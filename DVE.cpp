#include "stdafx.h"
#include "DVE.h"

bool classDve::isMoveEnable;

int classDve::last_rainLevel = INT_MAX;
int classDve::last_snowLevel = INT_MAX;
int classDve::last_fogLevel  = INT_MAX;
int classDve::last_dustLevel = INT_MAX;
int classDve::last_sandLevel = INT_MAX;
int classDve::last_snowaccLevel = INT_MAX;
int classDve::last_time = INT_MAX;

classDve::classDve(const char* _name, vsChannel::Subscriber *_subscriber):classBase(_name,_subscriber,ENABLE_KEY|ENABLE_RUNLOOP,true)
{
	pRain = new classRain(_name);
	pSnow = new classSnow(_name);
	pAccSnow = new classAccSnow(_name);
	pTime = new classTime(_name);
	pFog = new classFog(_name);
	pDust = new classDust(_name);

	isMoveEnable = pObserver->getStrategyEnable();

	pPanelInfo = pPanel->getPanel("DvePanel");   //获取dve面板
	if (pPanelInfo == NULL) 
	{
		std::cout << "DVE: pPanelInfo is NULL!" << std::endl;
		system("pause");
	}
	else 
	{
		pGLSComponent = pPanelInfo->_component;
		if(pGLSComponent == NULL) {std::cout << "DVE: pGLSComponent is NULL!" << std::endl;system("pause");}
		else pGLSComponent->ref();
	}


	addEvent(vsChannel::EVENT_POST_DRAW);

}

classDve::~classDve()
{
	if (pTime != NULL)
		delete pTime;
	if (pFog != NULL)
		delete pFog;
	if (pRain != NULL)
		delete pRain;
	if (pSnow != NULL)
		delete pSnow;
	if (pAccSnow != NULL)
		delete pAccSnow;
	if (pDust != NULL)
		delete pDust;
	if (pGLSComponent != NULL)
		pGLSComponent->unref();

	isMoveEnable = false;

	last_rainLevel = INT_MAX;
	last_snowLevel = INT_MAX;
	last_fogLevel  = INT_MAX;
	last_dustLevel = INT_MAX;
	last_sandLevel = INT_MAX;
	last_snowaccLevel = INT_MAX;
	last_time = INT_MAX;

	removeEvent(vsChannel::EVENT_POST_DRAW);
}

void classDve::updateData(void)
{


	char *cstr;
	int temp;

	//----------处理雨
	cstr = pGLSComponent->getAttrib("RainLevel");
	temp = atoi(cstr);
	pGLSComponent->getAttribFree(cstr);
	if(temp != last_rainLevel)
	{
		envInfo.rainLevel = last_rainLevel = temp;
		enableDve(classEnvInfo::Rain);
		printf("rain:%f\n", envInfo.rainLevel);
		return;
	}


	//----------处理雪
	cstr = pGLSComponent->getAttrib("SnowLevel");
	temp = atoi(cstr);
	pGLSComponent->getAttribFree(cstr);
	if(temp != last_snowLevel)
	{
		envInfo.snowLevel = last_snowLevel = temp;
		enableDve(classEnvInfo::Snow);
		printf("snow:%f\n", envInfo.snowLevel);
		return;
	}


	//----------处理雾
	cstr = pGLSComponent->getAttrib("FogLevel");
	temp = atoi(cstr);
	pGLSComponent->getAttribFree(cstr);
	if(temp != last_fogLevel)
	{
		envInfo.fogVisibility = last_fogLevel = temp;
		envInfo.fogColor = 1.0f;
		enableDve(classEnvInfo::Fog);
		printf("fog:%f %f\n", envInfo.fogVisibility, envInfo.fogColor);
		return;
	}

	//----------处理扬尘
	cstr = pGLSComponent->getAttrib("DustLevel");
	temp = atoi(cstr);
	pGLSComponent->getAttribFree(cstr);
	if(temp != last_dustLevel)
	{
		envInfo.dustDensity = last_dustLevel = temp;
		enableDve(classEnvInfo::Dust);
		printf("dust:%f\n", envInfo.dustDensity);
		return;
	}


	//----------处理沙尘暴
	cstr = pGLSComponent->getAttrib("SandLevel");
	temp = atoi(cstr);
	pGLSComponent->getAttribFree(cstr);
	if(temp != last_sandLevel)
	{
		envInfo.fogVisibility = last_sandLevel = temp;
		envInfo.fogColor = 0.0f;
		enableDve(classEnvInfo::Sandstorm);
		printf("Sandstorm:%f %f\n", envInfo.fogVisibility, envInfo.fogColor);
		return;
	}


	//----------处理积雪
	cstr = pGLSComponent->getAttrib("SnowaccLevel");
	temp = atoi(cstr);
	pGLSComponent->getAttribFree(cstr);
	if(temp != last_snowaccLevel)
	{
		envInfo.accDensity = last_snowaccLevel = temp;
		enableDve(classEnvInfo::AccSnow);
		printf("AccSnow:%f\n", envInfo.accDensity);
		return;
	}

	//----------处理天时
	cstr = pGLSComponent->getAttrib("time");
	temp = atoi(cstr);
	pGLSComponent->getAttribFree(cstr);
	if(temp != last_time)
	{
		envInfo.time = last_time = temp;
		enableDve(classEnvInfo::Time);
		printf("time:%f\n", envInfo.time);
		return;
	}

}

void classDve::deleteDveObject(classEnvInfo::enumDve _dve)
{
	switch (_dve)
	{
	case classEnvInfo::Nothing:
		break;

	case classEnvInfo::AccSnow:
		if(pAccSnow != NULL)
		{
			delete pAccSnow;
			pAccSnow = NULL;
		}
		else
			std::cout << "DVE.cpp: delete DveObject(pAccSnow) error!"<< std::endl;
		break;

	case classEnvInfo::Rain:
		if(pRain != NULL)
		{
			delete pRain;
			pRain = NULL;
		}
		else
			std::cout << "DVE.cpp: delete DveObject(pRain) error!"<< std::endl;
		break;

	case classEnvInfo::Snow:
		if(pSnow != NULL)
		{
			delete pSnow;
			pSnow = NULL;
		}
		else
			std::cout << "DVE.cpp: delete DveObject(pSnow) error!"<< std::endl;
		break;

	case classEnvInfo::Dust:
		if(pDust != NULL)
		{
			delete pDust;
			pDust = NULL;
		}
		else
			std::cout << "DVE.cpp: delete DveObject(pDust) error!"<< std::endl;
		break;

	default:
		break;

	}
}

void classDve::enableDve(classEnvInfo::enumDve _dve)
{
	switch (_dve)
	{
	case classEnvInfo::Nothing:
		break;

	case classEnvInfo::Fog:
		pFog->setVisibility(envInfo.fogVisibility == 0 ? 80000 : (19-envInfo.fogVisibility)*300);
		pFog->setColor(envInfo.fogColor);
		break;

	case classEnvInfo::Sandstorm:
		pFog->setVisibility(envInfo.fogVisibility == 0 ? (envInfo.fogColor = 1.0f, 80000) : (19-envInfo.fogVisibility)*300);
		pFog->setColor(envInfo.fogColor);
		break;

	case classEnvInfo::Time:
		pTime->setTime(envInfo.time);
		break;

	case classEnvInfo::Dust:
		pDust->setDensity(envInfo.dustDensity);
		break;

	case classEnvInfo::Rain:
		if(envInfo.rainLevel == 0 )
			pRain->setRainLevel(classRain::GREEN);
		else if(envInfo.rainLevel < 3)
			pRain->setRainLevel(classRain::BLUE);
		else if(envInfo.rainLevel < 8)
			pRain->setRainLevel(classRain::YELLOW);
		else if(envInfo.rainLevel < 15)
			pRain->setRainLevel(classRain::ORANGE);
		else 
			pRain->setRainLevel(classRain::RED);

		break;

	case classEnvInfo::Snow:
		if(envInfo.snowLevel == 0 )
			pSnow->setSnowLevel(classSnow::GREEN);
		else if(envInfo.snowLevel < 3)
			pSnow->setSnowLevel(classSnow::BLUE);
		else if(envInfo.snowLevel < 8)
			pSnow->setSnowLevel(classSnow::YELLOW);
		else if(envInfo.snowLevel < 15)
			pSnow->setSnowLevel(classSnow::ORANGE);
		else 
			pSnow->setSnowLevel(classSnow::RED);

		break;

	case classEnvInfo::AccSnow:
		pAccSnow->setDensity( calculateAccSnowDensity(envInfo.accDensity) );
		break;
	}
}

void classDve::handleKey(vrWindow::Key _key, int _mod)
{
	switch (_key)
	{
	case vrWindow::KEY_p:
		pObserver->setStrategyEnable(isMoveEnable = !isMoveEnable);
		break;
	}
}

void classDve::handleRunLoop(void)
{
	updateData();

	if (isStartFound)
	{
		double __pos[3], __hpr[3];
		getPositionHprFromStart(__pos, __hpr);
		pObserver->setTranslate(__pos[0], __pos[1], __pos[2]);
		pObserver->setRotate(__hpr[0], __hpr[1], 0);

		isStartFound = false;
	}
}

float classDve::calculateAccSnowDensity(float _in)
{
	return _in/4;
}
