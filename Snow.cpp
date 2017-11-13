#include "stdafx.h"
#include "Snow.h"

classSnow::classSnow(vpObserver *_observer, vsChannel::Subscriber *_subscriber):classBase(_observer,_subscriber,ENABLE_KEY|ENABLE_DRAWEVENT),SnowLevel(classSnow::GREEN)
{
	pParticleSystem = new ParticleSystem();

	init_flag = false;

	if(_subscriber != NULL)								//添加事件只能在app->configure函数中进行，这里兼顾了通过classDve导入和单独导入的情况
		addEvent(vsChannel::EVENT_POST_DRAW);

	pParticleSystem->environmentData.total = 200;
	pParticleSystem->environmentData.tex = 1;
	pParticleSystem->environmentData.areaRange = 200;
	pParticleSystem->environmentData.areaCenter = vuVec3d(0.0, 0.0, 0.0);
	pParticleSystem->environmentData.rotate = vuVec3d(0.0, 0.0, 0.0);

	pParticleSystem->particleData.life = 800;
	pParticleSystem->particleData.fade = 1;
	pParticleSystem->particleData.scale = vuVec3d(1.0, 1.0, 1.0);
	pParticleSystem->particleData.a = vuVec3d(0.0, 0.0, 0.0);
	pParticleSystem->particleData.dimension = vuVec3d(1.0, 1.0, 1.0);

	setSnowLevel(SnowLevel);
}

classSnow::classSnow(const char* _name, vsChannel::Subscriber *_subscriber):classBase(_name,_subscriber,ENABLE_KEY|ENABLE_DRAWEVENT),SnowLevel(classSnow::GREEN)
{
	pParticleSystem = new ParticleSystem();

	init_flag = false;
	if(_subscriber != NULL)								//添加事件只能在app->configure函数中进行，这里兼顾了通过classDve导入和单独导入的情况
		addEvent(vsChannel::EVENT_POST_DRAW);

	pParticleSystem->environmentData.total = 4000;
	pParticleSystem->environmentData.tex = 1;
	pParticleSystem->environmentData.areaRange = 200;
	pParticleSystem->environmentData.areaCenter = vuVec3d(0.0, 0.0, 0.0);
	pParticleSystem->environmentData.rotate = vuVec3d(0.0, 0.0, 0.0);

	pParticleSystem->particleData.life = 800;
	pParticleSystem->particleData.fade = 1;
	pParticleSystem->particleData.scale = vuVec3d(1.0, 1.0, 1.0);
	pParticleSystem->particleData.a = vuVec3d(0.0, 0.0, 0.0);
	pParticleSystem->particleData.dimension = vuVec3d(1.0, 1.0, 1.0);

	setSnowLevel(SnowLevel);
}

classSnow::~classSnow()
{
	delete pParticleSystem;

	if(pSubscriber != NULL)								
		removeEvent(vsChannel::EVENT_POST_DRAW);
}

void classSnow::setSnowLevel(weatherLevel _snowLevel)
{
	SnowLevel = _snowLevel;
	switch(_snowLevel)
	{
	case GREEN:			
		pParticleSystem->particleData.scale = vuVec3d(0.1, 0.1, 0.1);
		pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, 0.0);
		break;

	case BLUE:
		{
			pParticleSystem->particleData.scale = vuVec3d(0.1, 0.1, 0.1);
			pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, -0.5);
			pParticleSystem->particleData.life = 560;
		}
		break;

	case YELLOW:
		{
			pParticleSystem->particleData.scale = vuVec3d(0.15, 0.15, 0.15);
			pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, -0.7);
			pParticleSystem->particleData.life = 430;
		}
		break;

	case ORANGE:
		{	
			pParticleSystem->particleData.scale = vuVec3d(0.20, 0.20, 0.20);
			pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, -0.9);
			pParticleSystem->particleData.life = 300;
		}
		break;

	case RED:
		{	
			pParticleSystem->particleData.scale = vuVec3d(0.25, 0.25, 0.25);
			pParticleSystem->particleData.v = vuVec3d(0.0, 0.0, -1.2);
			pParticleSystem->particleData.life = 220;
		}
		break;	
	}
}

void classSnow::handleKey(vrWindow::Key _key, int _mod)
{
	switch(_key)
	{
	case vrWindow::KEY_F4:
		if(SnowLevel == GREEN)
			SnowLevel = BLUE;
		else if(SnowLevel  == BLUE)
			SnowLevel = YELLOW;
		else if(SnowLevel  == YELLOW)
			SnowLevel = ORANGE;
		else if(SnowLevel == ORANGE)
			SnowLevel = RED;
		else 
			SnowLevel = GREEN;

		setSnowLevel(SnowLevel);

		break;
	}
}

void classSnow::handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context)
{
	if(SnowLevel == GREEN || _channel != pChannel || _event != vsChannel::EVENT_POST_DRAW)
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