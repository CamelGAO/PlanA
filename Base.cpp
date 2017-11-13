#include "stdafx.h"
#include "Base.h"

classDrawer<classBase *>::drawer_data_type classBase::data_travKey;			//mask 1

classDrawer<classBase *>::drawer_data_type classBase::data_travCullEvent;	//mask 2

classDrawer<classBase *>::drawer_data_type classBase::data_travDrawEvent;	//mask 4

classDrawer<classBase *>::drawer_data_type classBase::data_travRunloop;		//mask 8

classDrawer<classBase *>::drawer_data_type classBase::data_travConfigure;	//mask 16

std::map<std::string, classBase::EventRefCount> classBase::data_eventRefCount;


classBase::classBase(const char* _name, vsChannel::Subscriber *_subscriber, size_t _mask, bool _isShowFound, const char * _object_name):traversalMask(_mask),isShowFound(_isShowFound)
{
	init( vpObserver::find(_name), _subscriber, _mask, _isShowFound, _object_name);
}

classBase::classBase(vpObserver *_observer, vsChannel::Subscriber *_subscriber, size_t _mask, bool _isShowFound, const char * _object_name):traversalMask(_mask),isShowFound(_isShowFound)
{
	init(_observer, _subscriber, _mask, _isShowFound, _object_name);
}

void classBase::init(vpObserver *_observer, vsChannel::Subscriber *_subscriber, size_t _mask, bool _isShowFound, const char * _object_name)            //场景中的信息在这个函数里提取    必须在pObserver初始化之后调用
{
	pSubscriber = _subscriber;
	addTraversal(_mask);			//这两句将本对象加入了遍历列表

	pStart1 = NULL;      //节点初始化为NULL，并将found标志置false
	pStart2 = NULL;
	pTerrain = NULL;												
	pBuilding = NULL;
	pCar = NULL;
	pPowerLine = NULL;
	pTree = NULL;
	pGrass = NULL;
	pStone = NULL;

	isStartFound = false;
	isTerrainFound = false;
	isBuildingFound = false;
	isCarFound = false;
	isPowerLineFound = false;
	isTreeFound = false;
	isGrassFound = false;
	isStoneFound = false;

	pShaderDeployer = classShaderDeployer::instance();
	pPanel = classPanel::instance();

	pSearchPath = vpSearchPath::find("mySearchPath");
	if (pSearchPath == NULL) {std::cout << "pSearchPath is NULL!" << std::endl;system("pause");}
	else pSearchPath->ref();

	if((pObserver = _observer) == NULL)       //如果pObserver为null，则表明继承了这个基类的派生类不需要相关信息，如classPanel，这个类只使用了遍历，不需要节点信息
	{
		std::cout << "pObserver is NULL! Automatically ignore node information!" << std::endl;
		//system("pause");
		return;
	}
	else
		pObserver->ref();

	pChannel = pObserver->getChannel(0);
	if (pChannel == NULL) {std::cout << "pChannel is NULL!" << std::endl;system("pause");}
	else pChannel->ref();

	pScene = pObserver->getScene();
	if (pScene == NULL) {std::cout << "pScene is NULL!" << std::endl;system("pause");}
	else pScene->ref();

	pEnv = static_cast<vpEnv *>(pObserver->getAttachment(0));
	if (pEnv == NULL) {std::cout << "pEnv is NULL!" << std::endl;system("pause");}
	else pEnv->ref();

	pObject = vpObject::find(_object_name);
	//pObject = dynamic_cast<vpObject *>(pObserver->getScene());
	if (pObject == NULL) {std::cout << "pObject is NULL!" << std::endl;system("pause");}
	else pObject->ref();

	pStart1 = pObject->find_named("start1");
	if (pStart1 != NULL)
	{
		pStart2 = pObject->find_named("start2");
		if (pStart2 != NULL)
		{
			pStart1->ref();
			pStart2->ref();
			isStartFound = true;
			if (isShowFound)
				std::cout << "Start is found!" << std::endl;
		}
	}

	pTerrain = pObject->find_named("terrain");
	if (pTerrain != NULL)
	{
		pTerrain->ref();
		isTerrainFound = true;
		if (isShowFound)
			std::cout << "Terrain is found!" << std::endl;
	}

	pBuilding = pObject->find_named("building");
	if (pBuilding != NULL)
	{
		pBuilding->ref();
		isBuildingFound = true;
		if (isShowFound)
			std::cout << "Building is found!" << std::endl;
	}

	pPowerLine = pObject->find_named("powerline");
	if (pPowerLine != NULL)
	{
		pPowerLine->ref();
		isPowerLineFound = true;
		if (isShowFound)
			std::cout << "Powerline is found!" << std::endl;
	}

	pCar = pObject->find_named("car");
	if (pCar != NULL)
	{
		pCar->ref();
		isCarFound = true;
		if (isShowFound)
			std::cout << "Car is found!" << std::endl;
	}

	pTree = pObject->find_named("tree");
	if (pTree != NULL)
	{
		pTree->ref();
		isTreeFound = true;
		if (isShowFound)
			std::cout << "Tree is found!" << std::endl;
	}

	pGrass = pObject->find_named("grass");
	if (pGrass != NULL)
	{
		pGrass->ref();
		isGrassFound = true;
		if (isShowFound)
			std::cout << "Grass is found!" << std::endl;
	}

	pStone = pObject->find_named("stone");
	if (pStone != NULL)
	{
		pStone->ref();
		isStoneFound = true;
		if (isShowFound)
			std::cout << "Stone is found!" << std::endl;
	}

}

classBase::~classBase() 
{
	removeTraversal(traversalMask);

	if(pSearchPath != NULL) pSearchPath->unref();
	if(pObserver != NULL) pObserver->unref();
	if(pChannel != NULL) pChannel->unref();
	if(pScene != NULL) pScene->unref();
	if(pEnv != NULL) pEnv->unref();
	if(pObject != NULL) pObject->unref();

	if(pTerrain!=NULL) pTerrain->unref();
	if(pBuilding!=NULL) pBuilding->unref();
	if(pCar!=NULL) pCar->unref();
	if(pTree!=NULL) pTree->unref();
	if(pGrass!=NULL) pGrass->unref();
	if(pPowerLine!=NULL) pPowerLine->unref();
	if(pStart1!=NULL) pStart1->unref();
	if(pStart2!=NULL) pStart2->unref();
}

void classBase::addEvent( vsChannel::Event _event, vpChannel * _channel)
{
	vsChannel *__channel = NULL;
	if (_channel == NULL)
		__channel = pChannel;
	else
		__channel = _channel;

	std::map<std::string, classBase::EventRefCount>::iterator it;
	it = data_eventRefCount.find(__channel->getName());
	if (it != data_eventRefCount.end())
	{
		if ( ((size_t *)&(it->second))[(size_t)_event]++ != 0)
			return;                                            //如果事件的Ref不为零，只需++不必再addSubscriber
	}
	else
	{
		classBase::EventRefCount __temp;
		__temp.PreCullEventRefCount = 0;
		__temp.PostCullEventRefCount = 0;
		__temp.PreDrawEventRefCount = 0;
		__temp.PostDrawEventRefCount = 0;

		((size_t *)&__temp)[(size_t)_event]++;

		data_eventRefCount[__channel->getName()] = __temp;
	}

	__channel->addSubscriber(_event, pSubscriber);
}

void classBase::removeEvent( vsChannel::Event _event, vpChannel * _channel)    //note：如果某个channel的Event ref变成全零，这个函数不会把它从map中剔除
{
	vsChannel *__channel = NULL;
	if (_channel == NULL)
		__channel = pChannel;
	else
		__channel = _channel;

	std::map<std::string, classBase::EventRefCount>::iterator it;
	it = data_eventRefCount.find(__channel->getName());
	if (it != data_eventRefCount.end())
	{
		size_t *__p = &(((size_t *)&(it->second))[(size_t)_event]);
		if((*__p) == 0) 
			{std::cout<< "classBase: removeEvent() error : "<< __channel->getName() <<"'s RefCount is already Zero!"<<std::endl;}
		else (*__p)--;
		if ((*__p) == 0)
			__channel->removeSubscriber(_event, pSubscriber, true);
	}
	else
		std::cout<< "classBase: removeEvent() error : Can't find information about " << __channel->getName() << "." <<std::endl;
}

void classBase::addTraversal(size_t _mask)
{
	if(_mask & 0x00000001)
		index_key = classDrawer<classBase *>::add(this,data_travKey);
	if(_mask & 0x00000002)
		index_cullEvent = classDrawer<classBase *>::add(this,data_travCullEvent);
	if(_mask & 0x00000004)
		index_drawEvent = classDrawer<classBase *>::add(this,data_travDrawEvent);
	if(_mask & 0x00000008)
		index_runLoop = classDrawer<classBase *>::add(this,data_travRunloop);
	if(_mask & 0x00000010)
		index_configure = classDrawer<classBase *>::add(this,data_travConfigure);
}

void classBase::removeTraversal(size_t _mask)
{
	if(_mask & 0x00000001)
		classDrawer<classBase *>::erase(index_key, data_travKey);
	if(_mask & 0x00000002)
		classDrawer<classBase *>::erase(index_cullEvent, data_travCullEvent);
	if(_mask & 0x00000004)
		classDrawer<classBase *>::erase(index_drawEvent, data_travDrawEvent);
	if(_mask & 0x00000008)
		classDrawer<classBase *>::erase(index_runLoop, data_travRunloop);
	if(_mask & 0x00000010)
		classDrawer<classBase *>::erase(index_configure, data_travConfigure);
}

void classBase::handleKey(vrWindow::Key _key, int _mod)
{std::cout	<< "This function has not yet been implemented in the subclass and nothing will be done." << std::endl;}

void classBase::handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context)
{std::cout	<< "This function has not yet been implemented in the subclass and nothing will be done." << std::endl;}

void classBase::handleCullEvent(vsChannel::Event _event, const vsChannel *_channel, vsTraversalCull *_traversal)
{std::cout	<< "This function has not yet been implemented in the subclass and nothing will be done." << std::endl;}

void classBase::handleRunLoop(void)
{std::cout	<< "This function has not yet been implemented in the subclass and nothing will be done." << std::endl;}

void classBase::handleConfigure(void)
{std::cout	<< "This function has not yet been implemented in the subclass and nothing will be done." << std::endl;}

void classBase::traversalKey(vrWindow::Key _key, int _mod)
{
	classDrawer<classBase *>::drawer_data_iterator ib = data_travKey.begin();
	classDrawer<classBase *>::drawer_data_iterator ie = data_travKey.end();
	while (ib != ie)
	{
		(*ib)._data->handleKey(_key, _mod);
		ib++;
	}
}

void classBase::traversalDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context)
{
	classDrawer<classBase *>::drawer_data_iterator ib = data_travDrawEvent.begin();
	classDrawer<classBase *>::drawer_data_iterator ie = data_travDrawEvent.end();
	while (ib != ie)
	{
		(*ib)._data->handleDrawEvent(_event, _channel, _context);
		ib++;
	}
}

void classBase::traversalRunLoop(void)
{
	classDrawer<classBase *>::drawer_data_iterator ib = data_travRunloop.begin();
	classDrawer<classBase *>::drawer_data_iterator ie = data_travRunloop.end();
	while (ib != ie)
	{
		(*ib)._data->handleRunLoop();
		ib++;
	}
}

void classBase::traversalConfigure(void)
{
	classDrawer<classBase *>::drawer_data_iterator ib = data_travConfigure.begin();
	classDrawer<classBase *>::drawer_data_iterator ie = data_travConfigure.end();
	while (ib != ie)
	{
		(*ib)._data->handleConfigure();
		ib++;
	}
}

bool classBase::getPositionHprFromStart(double *_pos, double *_hpr, size_t _choose)
{
	if(isStartFound != true)
	{
		printf("getPositionHprFromStart(): Warn: pStart1 and pStart2 are not found! Check the .flt!\n");
		system("pause");
		return false;
	}

	const double * __pos[2] = {0};
	__pos[0] = pStart1->getBounds().m_ctr.m_vec;
	__pos[1] = pStart2->getBounds().m_ctr.m_vec;

	if (_choose == 1 || _choose == 2)
	{
		memcpy(_pos, __pos[_choose-1], 3*sizeof(double));
	}
	else
	{
		printf("getPositionHprFromStart(): Wrong _choose!");
		system("pause");
		return false;
	}

	vuMatrixd __m;
	__m.makeRotate( vuVec3d(0,1,0), vuVec3d(__pos[1][0] - __pos[0][0], __pos[1][1] - __pos[0][1], __pos[1][2] - __pos[0][2]) );
	__m.getRotate(&_hpr[0], &_hpr[1], &_hpr[2]);
	//printf("%f, %f, %f\n", _hpr[0], _hpr[1], _hpr[2] );

	return true;

}