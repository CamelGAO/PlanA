#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "stdafx.h"
#include "Base.h"
#include "vrUserGeometry.h"

class classGeometry : public classBase
{
public:

	struct PathNode
	{
		float _pos[3];
		float _hpr[3];
	};

	struct AltimeterValuePointer
	{
		float* value0;
		float* value1;
	};

	classGeometry(vpObserver *_observer, vsChannel::Subscriber *_subscriber = NULL);
	classGeometry(const char *_name, vsChannel::Subscriber *_subscriber = NULL);
	~classGeometry();
	void init();
	void addBox(vsNode* _node);  //画一个可以将_node包起来的box
	void addAirWay(const vector<PathNode>& _path); //根据_path画一条路径
	void addLandingArea(const vuVec3f& _pos, const vuVec3f& _hpr);  // 画一个中心点在_pos处的着陆区,_hpr用来指示标志的方向

	inline void drawBox(vrDrawContext *_context, vsChannel* _channel, bool _isShowDistance = true);
	inline void drawAirWay(vrDrawContext *_context);
	inline void drawLandingArea(vrDrawContext *_context);

	void setAltimeter(float _height, size_t _index = 0); //_index用来选取LandingAreaData中对应的landingarea指针,这个函数会设置这个landingarea的高度计，在blueprint中两个高度计的指针分别为

	bool isInRange(vsNode* _node, float _range, vpObserver* _observer = NULL);

	vsTraversal::Result travFunc(vsNode *_node, int);

	virtual void handleKey(vrWindow::Key _key, int _mod);
	virtual void handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context);
protected:
private:
	void addAirWayElement(vrUserGeometry* _geometry, const vector<PathNode>& _data, int _index);
	int createXML(const char* _path);
	void recordPath(const char* _path, bool _isWorking);
	void praseAirWay(const TCHAR* _file);
	inline float getLength(const float* _a, const float* _b);
	inline void classGeometry::interpolation(const float* _a, const float* _b, float* const _out, float _cursor);

	void praseLandingAreaBlueprint(const TCHAR* _file);

	float range;

	vrFont2D* pFont; //用来绘制文字

	vector<vrUserGeometry *> AirWayData;
	vector<vrUserGeometry *> LandingAreaData;
	vector<vrBox *> BoxData;

	vector<PathNode> PathData;
	vector<AltimeterValuePointer> AltimeterValuePointerData;
	vector<vuVec3f> LandingAreaBlueprintData; //保存解析后的蓝图

	bool isDrawBoxEnable;
	bool isDrawAirWayEnable;
	bool isRecordWorking;

	static char* airWayXMLPath;
	static char* landingAreaBlueprintXMLPath;
};

#endif