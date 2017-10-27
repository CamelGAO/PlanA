#ifndef _BASE_H
#define _BASE_H

#include "stdafx.h"
#include "Drawer.h"
#include "ShaderDeployer.h"

#define ENABLE_KEY			0x00000001
#define ENABLE_CULLEVENT	0x00000002
#define ENABLE_DRAWEVENT	0x00000004
#define ENABLE_RUNLOOP		0x00000008
#define ENABLE_CONFIGURE	0x00000010

class classBase
{
public:

	struct EventRefCount
	{
		size_t PreCullEventRefCount;
		size_t PostCullEventRefCount;
		size_t PreDrawEventRefCount;
		size_t PostDrawEventRefCount;
	};

	classBase(const char* _name, vsChannel::Subscriber *_subscriber, size_t _mask = 0x00000000, bool _isShowFound = false, const char * _object_name = TEXT("myObject"));
	classBase(vpObserver *_observer, vsChannel::Subscriber *_subscriber, size_t _mask = 0x00000000, bool _isShowFound = false, const char * _object_name = TEXT("myObject"));


	void init(vpObserver *_observer, vsChannel::Subscriber *_subscriber, size_t _mask, bool _isShowFound, const char * _object_name);

	void addEvent( vsChannel::Event _event, vpChannel * _channel = NULL);
	void removeEvent( vsChannel::Event _event, vpChannel * _channel = NULL);

	void addTraversal(size_t _mask);
	void removeTraversal(size_t _mask);

	virtual void handleKey(vrWindow::Key _key, int _mod) ;

	virtual void handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context) ;

	virtual void handleCullEvent(vsChannel::Event _event, const vsChannel *_channel, vsTraversalCull *_traversal) ;

	virtual void handleRunLoop(void);

	virtual void handleConfigure(void);

	static void traversalKey(vrWindow::Key _key, int _mod);
	static void traversalDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context);
	static void traversalRunLoop(void);
	static void traversalConfigure(void);

	bool getPositionHprFromStart(double *_pos, double *_hpr, size_t _choose = 2);   //_choose用来置顶使用哪个start作为落脚点


protected:
	~classBase();

	vpSearchPath *				pSearchPath;
	vpObserver *				pObserver;
	vsChannel::Subscriber *		pSubscriber;
	vpChannel *					pChannel;
	vpScene *					pScene;
	vpEnv *						pEnv;


	vpObject *					pObject;      //保存场景中的节点
	vsNode *					pStart1;
	vsNode *					pStart2;
	vsNode *					pTerrain;
	vsNode *					pBuilding;
	vsNode *					pCar;
	vsNode *					pPowerLine;
	vsNode *					pTree;

	bool						isShowFound;
	bool						isStartFound;
	bool						isTerrainFound;     //节点发现标志
	bool						isBuildingFound;
	bool						isCarFound;
	bool						isPowerLineFound;
	bool						isTreeFound;


	classShaderDeployer *		pShaderDeployer;

	size_t						index;				//用于记录本对象在遍历列表中的索引
	size_t						index_key;
	size_t						index_cullEvent;
	size_t						index_drawEvent;
	size_t						index_runLoop;
	size_t						index_configure;

private:

	classBase();

	size_t						traversalMask;
	static classDrawer<classBase *>::drawer_data_type data_travKey;
	static classDrawer<classBase *>::drawer_data_type data_travCullEvent;
	static classDrawer<classBase *>::drawer_data_type data_travDrawEvent;
	static classDrawer<classBase *>::drawer_data_type data_travRunloop;
	static classDrawer<classBase *>::drawer_data_type data_travConfigure;

	static std::map<std::string, EventRefCount> data_eventRefCount;
};


#endif
