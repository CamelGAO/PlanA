#include "stdafx.h"
#include "Rain.h"

classRain::classRain(vpObserver *_observer, vsChannel::Subscriber *_subscriber):classBase(_observer,_subscriber,ENABLE_KEY|ENABLE_DRAWEVENT),RainLevel(classRain::GREEN)
{
	pParticleSystem = new ParticleSystem();

	init_flag = false;

	if(_subscriber != NULL)                             //添加事件只能在app->configure函数中进行，这里兼顾了通过classDve导入和单独导入的情况
		addEvent(vsChannel::EVENT_POST_DRAW);

	pParticleSystem->environmentData.total = 4000;
	pParticleSystem->environmentData.tex = 0;
	pParticleSystem->environmentData.areaRange = 200;
	pParticleSystem->environmentData.areaCenter = vuVec3d(0.0, 0.0, 0.0);
	pParticleSystem->environmentData.rotate = vuVec3d(0.0, 0.0, 0.0);

	pParticleSystem->particleData.life = 800;
	pParticleSystem->particleData.fade = 1;
	pParticleSystem->particleData.scale = vuVec3d(1.0, 1.0, 1.0);
	pParticleSystem->particleData.a = vuVec3d(0.0, 0.0, 0.0);
	pParticleSystem->particleData.dimension = vuVec3d(1.0, 1.0, 1.0);

	setRainLevel(RainLevel);
}

classRain::classRain(const char* _name, vsChannel::Subscriber *_subscriber):classBase(_name,_subscriber,ENABLE_KEY|ENABLE_DRAWEVENT),RainLevel(classRain::GREEN)
{
	pParticleSystem = new ParticleSystem();

	init_flag = false;

	if(_subscriber != NULL)								//添加事件只能在app->configure函数中进行，这里兼顾了通过classDve导入和单独导入的情况
		addEvent(vsChannel::EVENT_POST_DRAW);

	pParticleSystem->environmentData.total = 4000;
	pParticleSystem->environmentData.tex = 0;
	pParticleSystem->environmentData.areaRange = 200;
	pParticleSystem->environmentData.areaCenter = vuVec3d(0.0, 0.0, 0.0);
	pParticleSystem->environmentData.rotate = vuVec3d(0.0, 0.0, 0.0);

	pParticleSystem->particleData.life = 800;
	pParticleSystem->particleData.fade = 1;
	pParticleSystem->particleData.scale = vuVec3d(1.0, 1.0, 1.0);
	pParticleSystem->particleData.a = vuVec3d(0.0, 0.0, 0.0);
	pParticleSystem->particleData.dimension = vuVec3d(1.0, 1.0, 1.0);
	setRainLevel(RainLevel);
}

classRain::~classRain()
{
	delete pParticleSystem;

	if(pSubscriber != NULL)								
		removeEvent(vsChannel::EVENT_POST_DRAW);
}

void classRain::setRainLevel(weatherLevel _rainLevel)
{
	RainLevel = _rainLevel;
	switch(_rainLevel)
	{
	case GREEN:			
		pParticleSystem->particleData.scale = vuVec3d(0.1, 0.1, 0.3);
		pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, 0.0);
		break;

	case BLUE:
		{
			pParticleSystem->particleData.scale = vuVec3d(0.1, 0.1, 0.3);
			pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, -2.0);
			pParticleSystem->particleData.life = 100;
		}
		break;

	case YELLOW:
		{
			pParticleSystem->particleData.scale = vuVec3d(0.15, 0.15, 0.45);
			pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, -3.0);
			pParticleSystem->particleData.life = 66;
		}
		break;

	case ORANGE:
		{
			pParticleSystem->particleData.scale = vuVec3d(0.20, 0.20, 0.60);
			pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, -4.0);
			pParticleSystem->particleData.life = 50;
		}
		break;

	case RED:
		{	
			pParticleSystem->particleData.scale = vuVec3d(0.25, 0.25, 0.75);
			pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, -5.0);
			pParticleSystem->particleData.life = 40;
		}
		break;	
	}

}

void classRain::handleKey(vrWindow::Key _key, int _mod)
{
	switch(_key)
	{
	case vrWindow::KEY_F3:
		if(RainLevel == GREEN)
			RainLevel = BLUE;
		else if(RainLevel  == BLUE)
			RainLevel = YELLOW;
		else if(RainLevel  == YELLOW)
			RainLevel = ORANGE;
		else if(RainLevel == ORANGE)
			RainLevel = RED;
		else
			RainLevel = GREEN;

		setRainLevel(RainLevel);

		break;
	}
}

void classRain::handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context)
{
	if(RainLevel == GREEN || _channel != pChannel || _event != vsChannel::EVENT_POST_DRAW)
		return ;

	if(init_flag == false)
	{
		pParticleSystem->initData();
		init_flag = true;
	}

	vrTransform::ElementProjection projectionElement;   //投影矩阵
	projectionElement.m_matrix = _channel->getVrChannel()->getProjectionMatrix();
	vrTransform::ElementModelView modelViewElement;  //视图矩阵
	modelViewElement.m_matrix = _channel->getVrChannel()->getViewMatrixInverse();   
	_context->pushElements(true); //将其他当前的图形状态压入堆栈

	vuVec3d position,rotation;
	pObserver->getPosition(&position[0],&position[1],&position[2]);
	pObserver->getRotate(&rotation[0],&rotation[1],&rotation[2]);

	_context->setElement(vrTransform::ElementProjection::Id,&projectionElement);
	vuMatrixf rotMat1;
	rotMat1.makeRotate(0,-90,0);  //h、p、r
	modelViewElement.m_matrix.postMultiply(rotMat1);//坐标系转换
	_context->setElement(vrTransform::ElementModelView::Id,&modelViewElement);				

	position[0] = position[0]+180.0f*vuSin(vuDeg2Rad(-rotation[0]));
	position[1] = position[1]+180.0f*vuCos(vuDeg2Rad(-rotation[0]));
	position[2] = position[2]+60.0f;
	pParticleSystem->environmentData.areaCenter = position;
	pParticleSystem->environmentData.rotate = rotation;
	pParticleSystem->draw();

	_context->popElements(false);
}