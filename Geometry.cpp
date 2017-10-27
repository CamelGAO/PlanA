#include "stdafx.h"
#include "Geometry.h"

char* classGeometry::airWayXMLPath = "E:\\newstart\\Data_XML\\path1.xml";
char* classGeometry::landingAreaBlueprintXMLPath = "E:\\newstart\\Data_XML\\LandingAreaBlueprint.xml";

classGeometry::classGeometry(const char *_name, vsChannel::Subscriber *_subscriber)
: classBase(_name, _subscriber, ENABLE_KEY|ENABLE_DRAWEVENT, false, TEXT("myObject"))
{
	init();
}

classGeometry::classGeometry(vpObserver * _observer, vsChannel::Subscriber *_subscriber)
: classBase(_observer, _subscriber, ENABLE_KEY|ENABLE_DRAWEVENT, false, TEXT("myObject"))
{
	init();
}

classGeometry::~classGeometry()
{
	for(vector<vrBox *>::iterator it = BoxData.begin(); it != BoxData.end(); it++)
	{
		(*it)->unref();
	}
	BoxData.clear();

	for(vector<vrUserGeometry *>::iterator it = AirWayData.begin(); it != AirWayData.end(); it++)
	{
		(*it)->unref(); 
	}
 	AirWayData.clear();

	for(vector<vrUserGeometry *>::iterator it = LandingAreaData.begin(); it != LandingAreaData.end(); it++)
	{
		(*it)->unref(); 
	}
	LandingAreaData.clear();

	removeEvent(vsChannel::EVENT_POST_DRAW);
}

void classGeometry::init()
{
	addEvent(vsChannel::EVENT_POST_DRAW);

	range = 999999;

	isRecordWorking = false;

	vrFontFactory *fontFactory = new vrFontFactory();
	pFont = (vrFont2D*)fontFactory->read( "system" ); 
	fontFactory->unref();

	vsTraversalUser<int, vsTraversalLookUpNodeId> trav;
	trav.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc);			
	trav.visit(pBuilding);

	//creatXML(landingAreaBlueprintXMLPath);

	praseAirWay(airWayXMLPath);
	addAirWay(PathData);

	praseLandingAreaBlueprint(landingAreaBlueprintXMLPath);
	addLandingArea(vuVec3f(0,0,200), vuVec3f(0,0,0));
}

int classGeometry::creatXML(const char* _path)
{
	return 0;
}

void classGeometry::recordPath(const char* _path, bool _isWorking)
{
	static int count = 0;
	static int state = 0;
	static tinyxml2::XMLDocument* doc = NULL;
	static tinyxml2::XMLElement* root = NULL;
	static char buffer[1024] = {0};
	const char* d ="xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"";

	switch(state)
	{
	case 0:
		if(_isWorking)
		{
			doc = new tinyxml2::XMLDocument();
			tinyxml2::XMLDeclaration* declaration=doc->NewDeclaration(d);
			doc->InsertFirstChild(declaration);
			root=doc->NewElement("root");

			state = 1;
		}
		break;
	case 1:
		if(_isWorking)
		{
			double pos[3];
			double hpr[3];
			pObserver->getTranslate(&pos[0], &pos[1], &pos[2]);
			pObserver->getRotate(&hpr[0], &hpr[1], &hpr[2]);

			tinyxml2::XMLElement* node = doc->NewElement("node");
			node->SetAttribute("index",count);
			root->InsertEndChild(node);

			tinyxml2::XMLElement* x = doc->NewElement("x");
			tinyxml2::XMLUtil::ToStr(pos[0], buffer, 1024);
			x->InsertEndChild(doc->NewText(buffer));
			node->InsertEndChild(x);

			tinyxml2::XMLElement* y = doc->NewElement("y");
			tinyxml2::XMLUtil::ToStr(pos[1], buffer, 1024);
			y->InsertEndChild(doc->NewText(buffer));
			node->InsertEndChild(y);

			tinyxml2::XMLElement* z = doc->NewElement("z");
			tinyxml2::XMLUtil::ToStr(pos[2], buffer, 1024);
			z->InsertEndChild(doc->NewText(buffer));
			node->InsertEndChild(z);

			tinyxml2::XMLElement* h = doc->NewElement("h");
			tinyxml2::XMLUtil::ToStr(hpr[0], buffer, 1024);
			h->InsertEndChild(doc->NewText(buffer));
			node->InsertEndChild(h);

			tinyxml2::XMLElement* p = doc->NewElement("p");
			tinyxml2::XMLUtil::ToStr(hpr[1], buffer, 1024);
			p->InsertEndChild(doc->NewText(buffer));
			node->InsertEndChild(p);

			tinyxml2::XMLElement* r = doc->NewElement("r");
			tinyxml2::XMLUtil::ToStr(hpr[2], buffer, 1024);
			r->InsertEndChild(doc->NewText(buffer));
			node->InsertEndChild(r);

			count++;
		}
		else
		{
			if (count > 0)
			{
				int ret = 0;
				doc->InsertEndChild(root);
				ret = doc->SaveFile(_path);
				if (ret != 0)
					std::cout << "recordPath(): Fail to write file! Code: " << ret << std::endl;
			}
			if (doc != NULL)
			{
				doc->Clear();
				delete doc;
			}

			doc = NULL;
			root = NULL;
			count = 0;

			state = 0;
		}
		break;
	}
}

void classGeometry::addBox(vsNode* _node)
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(_node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();

	vuVec3f box_max = geometry_r->getBounds().m_max;
	vuVec3f box_min = geometry_r->getBounds().m_min;
	vuVec3f box_ctr = (box_min + box_max) * 0.5;
	box_ctr.negate(box_ctr);

	box_max = (box_max + box_ctr) * 0.01f + box_max;  //让box膨胀一些，防止model表面和box表面重叠时出现闪烁
	box_min = (box_min + box_ctr) * 0.01f + box_min;

	vrBox* box = new vrBox(box_min, box_max);
	box->setColor(0.5, 0.2, 0.2, 0.6);
	box->ref();

	BoxData.push_back(box);
}

inline float classGeometry::getLength(const float* _a, const float* _b)
{
	return sqrt(pow(_a[0] - _b[0], 2) + pow(_a[1] - _b[1], 2) + pow(_a[2] - _b[2], 2));
}

inline void classGeometry::interpolation(const float* _a, const float* _b, float* const _out, float _cursor)
{
	if (_cursor < 0) 
	{
		std::cout << "interpolation() : _cursor = " << _cursor << std::endl;
	    _cursor = 0;
	}
	if (_cursor > 1) 
	{
		std::cout << "interpolation() : _cursor = " << _cursor << std::endl;
		_cursor = 1;
	}

	_out[0] = _a[0] * (1 - _cursor) + _b[0] * _cursor;
	_out[1] = _a[1] * (1 - _cursor) + _b[1] * _cursor;
	_out[2] = _a[2] * (1 - _cursor) + _b[2] * _cursor;
}

void classGeometry::addAirWay(const vector<PathNode>& _path)
{
	static float step = 60.0f;  //生成空中通道时，方框的间隔

	size_t nodeNum = _path.size();  //path中的节点数量
	if (nodeNum <= 1) 
	{
		std::cout << "addAirWay(): No node in path!" << std::endl;
		return;
	}
	float pathLength = 0;   //从起点开始的路径长度
	int numElement = 0;	//最终需要绘制的element数量
	vector<PathNode> ElementData; //记录每个element的pathnode信息，而不是每个采样点的信息，这是对原始采样点数据的重新采样,element间距（element长度）由step指定

	for(size_t i = 1; i < nodeNum; i++)
	{
		float last = pathLength; //记录上一次的length
		float increment = getLength(_path[i-1]._pos, _path[i]._pos); //本次增加的长度
		pathLength += increment;
		if ( pathLength >= step*(numElement+1) )
		{
			PathNode temp;
			float cursor = (step*(numElement+1) - last)/increment;    //插值
			interpolation(_path[i-1]._pos, _path[i]._pos, temp._pos, cursor);
			interpolation(_path[i-1]._hpr, _path[i]._hpr, temp._hpr, cursor);
			numElement++;
			ElementData.push_back(temp);
		}
	}

	if (numElement == 0) 
	{
		std::cout << "addAirWay(): No element added!" << std::endl;
		return;
	}

	ElementData.push_back(_path[nodeNum - 1]);   //最后一个元素的作用是记录结束点的位置

 	vrUserGeometry* geometry = new vrUserGeometry(6*numElement);  //单element的线段数量*element数量
	geometry->ref();

	for (int i = 0; i < numElement; i++)
	{
		addAirWayElement(geometry, ElementData, i);
	}

	AirWayData.push_back(geometry);
}

void classGeometry::addAirWayElement(vrUserGeometry* _geometry, const vector<PathNode>& _data, int _index)
{
	static float w = 15; //方格的宽的一半
	static float h = 12; //方格的高的一半

	vuVec4f lt1(-w, 0.0f, h, 1.0f);
	vuVec4f rt1( w, 0.0f, h, 1.0f);
	vuVec4f lb1(-w, 0.0f,-h, 1.0f);
	vuVec4f rb1( w, 0.0f,-h, 1.0f);

	vuVec4f lt2(-w, 0.0f, h, 1.0f);
	vuVec4f rt2( w, 0.0f, h, 1.0f);
	vuVec4f lb2(-w, 0.0f,-h, 1.0f);
	vuVec4f rb2( w, 0.0f,-h, 1.0f);

	const float* pos1 = (_data[_index]._pos);
	const float* hpr1 = (_data[_index]._hpr);

	const float* pos2 = (_data[_index+1]._pos);
	const float* hpr2 = (_data[_index+1]._hpr);

	vuMatrixf m;
	m.makeIdentity();
	m.makeRotate(hpr1[0], hpr1[1], hpr1[2]);
	m.transform(&lt1);
	m.transform(&rt1);
	m.transform(&lb1);
	m.transform(&rb1);

	m.makeIdentity();
	m.makeRotate(hpr2[0], hpr2[1], hpr2[2]);
// 	m.transform(&lt2);
// 	m.transform(&rt2);
	m.transform(&lb2);
	m.transform(&rb2);

	m.makeIdentity();
	m.makeTranslate(pos1[0], pos1[1], pos1[2]);
	m.transform(&lt1);
	m.transform(&rt1);
	m.transform(&lb1);
	m.transform(&rb1);

	m.makeIdentity();
	m.makeTranslate(pos2[0], pos2[1], pos2[2]);
// 	m.transform(&lt2);
// 	m.transform(&rt2);
	m.transform(&lb2);
	m.transform(&rb2);

	_geometry->setLineSegment(_index*6+0, vuVec3f(lt1[0], lt1[1], lt1[2]), vuVec3f(rt1[0], rt1[1], rt1[2]));
	_geometry->setLineSegment(_index*6+1, vuVec3f(rt1[0], rt1[1], rt1[2]), vuVec3f(rb1[0], rb1[1], rb1[2]));
	_geometry->setLineSegment(_index*6+2, vuVec3f(rb1[0], rb1[1], rb1[2]), vuVec3f(lb1[0], lb1[1], lb1[2]));
	_geometry->setLineSegment(_index*6+3, vuVec3f(lb1[0], lb1[1], lb1[2]), vuVec3f(lt1[0], lt1[1], lt1[2]));
	_geometry->setLineSegment(_index*6+4, vuVec3f(lb1[0], lb1[1], lb1[2]), vuVec3f(lb2[0], lb2[1], lb2[2]));
	_geometry->setLineSegment(_index*6+5, vuVec3f(rb1[0], rb1[1], rb1[2]), vuVec3f(rb2[0], rb2[1], rb2[2]));
}

void classGeometry::addLandingArea(const vuVec3f& _pos, const vuVec3f& _hpr)
{
	vrUserGeometry* geometry = new vrUserGeometry(104);  //一个Landing area标志一共104条线
	geometry->ref();

	for (int i = 0; i < 104; i++)
	{
		vuVec4f begin(LandingAreaBlueprintData[2*i][0], LandingAreaBlueprintData[2*i][1], LandingAreaBlueprintData[2*i][2], 1.0f);
		vuVec4f end(LandingAreaBlueprintData[2*i+1][0], LandingAreaBlueprintData[2*i+1][1], LandingAreaBlueprintData[2*i+1][2], 1.0f);

		vuMatrixf m;
		m.makeIdentity();
		m.makeRotate(_hpr[0], _hpr[1], _hpr[2]);
		m.transform(&begin);
		m.transform(&end);

		m.makeIdentity();
		m.makeTranslate(_pos[0], _pos[1], _pos[2]);
		m.transform(&begin);
		m.transform(&end);

		geometry->setLineSegment(i, vuVec3f(begin[0], begin[1], begin[2]), vuVec3f(end[0], end[1], end[2]));
	}

	geometry->setColor(0,1,0,1); 

	LandingAreaData.push_back(geometry);

	AltimeterValuePointer altimeter;
	altimeter.value0 = geometry->getValue(0, 1, 2);
	altimeter.value1 = geometry->getValue(1, 1, 2);
	AltimeterValuePointerData.push_back(altimeter);
}

void classGeometry::setAltimeter(float _height, size_t _index)
{
	*(AltimeterValuePointerData[_index].value0) = _height;
	*(AltimeterValuePointerData[_index].value1) = _height;
}

void classGeometry::praseAirWay(const TCHAR* _file)
{
	PathData.clear();

	tinyxml2::XMLDocument doc;
	int ret;
	ret = doc.LoadFile(_file);
	if(ret != 0) 
		std::cout << "praseAirWay(): Fail to load file! Code : " << ret << std::endl;
	tinyxml2::XMLElement* root = doc.RootElement();
	tinyxml2::XMLElement* node = root->FirstChildElement("node");
	while( node)
	{
 		PathNode temp;

		tinyxml2::XMLElement* data;

		data = node->FirstChildElement("x");
		tinyxml2::XMLUtil::ToFloat(data->GetText(), &temp._pos[0]);
		data = node->FirstChildElement("y");
		tinyxml2::XMLUtil::ToFloat(data->GetText(), &temp._pos[1]);
		data = node->FirstChildElement("z");
		tinyxml2::XMLUtil::ToFloat(data->GetText(), &temp._pos[2]);

		data = node->FirstChildElement("h");
		tinyxml2::XMLUtil::ToFloat(data->GetText(), &temp._hpr[0]);
		data = node->FirstChildElement("p");
		tinyxml2::XMLUtil::ToFloat(data->GetText(), &temp._hpr[1]);
		data = node->FirstChildElement("r");
		tinyxml2::XMLUtil::ToFloat(data->GetText(), &temp._hpr[2]);

		PathData.push_back(temp);

		node = node->NextSiblingElement("node");
	}
}

void classGeometry::praseLandingAreaBlueprint(const TCHAR* _file)
{
	LandingAreaBlueprintData.clear();

	float temp[3];
	tinyxml2::XMLDocument doc;  
	int ret;
	ret = doc.LoadFile(_file);
	if(ret != 0) 
		std::cout << "praseAirWay(): Fail to load file! Code : " << ret << std::endl;
	tinyxml2::XMLElement* root = doc.RootElement();
	tinyxml2::XMLElement* node = root->FirstChildElement("line");
	while( node)
	{
		const char* data;

		data = node->Attribute("x1");
		tinyxml2::XMLUtil::ToFloat(data, &temp[0]);
		data = node->Attribute("y1");
		tinyxml2::XMLUtil::ToFloat(data, &temp[1]);
		data = node->Attribute("z1");
		tinyxml2::XMLUtil::ToFloat(data, &temp[2]);
		LandingAreaBlueprintData.push_back(vuVec3f(temp[0], temp[1], temp[2]));

		data = node->Attribute("x2");
		tinyxml2::XMLUtil::ToFloat(data, &temp[0]);
		data = node->Attribute("y2");
		tinyxml2::XMLUtil::ToFloat(data, &temp[1]);
		data = node->Attribute("z2");
		tinyxml2::XMLUtil::ToFloat(data, &temp[2]);
		LandingAreaBlueprintData.push_back(vuVec3f(temp[0], temp[1], temp[2]));

		node = node->NextSiblingElement("line");
	}
}

inline void classGeometry::drawBox(vrDrawContext *_context, vsChannel* _channel,  bool _isShowDistance /* = true */)
{
	//首先保存一份通道的modelview和projection矩阵
	vrTransform::ElementModelView *modelview = const_cast<vrTransform::ElementModelView *>(vrGetElement(_context, vrTransform::ElementModelView::Id));
	vrTransform::ElementProjection *projection = const_cast<vrTransform::ElementProjection *>(vrGetElement(_context, vrTransform::ElementProjection::Id));

	_context->pushElements(true);

	//恢复通道的modelview和projection矩阵
	_context->setElement(vrTransform::ElementModelView::Id, modelview);
	_context->setElement(vrTransform::ElementProjection::Id, projection);

	vrAlphaBlend::Element alphaBlendElement;
	alphaBlendElement.m_enable = true;
	alphaBlendElement.m_src = vrAlphaBlend::MODE_SRC_ALPHA;
	alphaBlendElement.m_dst = vrAlphaBlend::MODE_INVERSE_SRC_ALPHA;
	_context->setElement(vrAlphaBlend::Element::Id, &alphaBlendElement);

	for(vector<vrBox *>::iterator it = BoxData.begin(); it != BoxData.end(); it++)
	{
		vrBox* box = *it;
		box->draw(_context);  //绘制box

		if(_isShowDistance)
		{    //绘制距离
			vuMatrixf world2screen;
			vuString pString;
			vuVec3d pos;

			pObserver->getPosition(&pos[0], &pos[1], &pos[2]);
			world2screen = _channel->getVrChannel()->getWorldToScreenMatrix();

			vuVec3f pos1 = *( box->getVertices() );
			vuVec4f point;
			point.set(pos1[0],pos1[1],pos1[2],1.0f);
			vuVec3f pos2;
			pos2[0] = pos[0]; pos2[1] = pos[1]; pos2[2] = pos[2];
			float dis = pos2.distance(pos1);
			if(dis > range) continue;
			pString.sprintf("%.0f",dis);

			world2screen.transform(&point);
			// divide by w
			float fx = point[0]/point[3]; // x between [-1,1]
			float fy = point[1]/point[3]; // y between [-1,1]
			float fz = point[2]/point[3]; // z between [-1,1]

			_context->pushElements(true);
			vrDepthTest::Element depthTestElement;
			depthTestElement.m_enable = false;
			_context->setElement(vrDepthTest::Element::Id, &depthTestElement);

			vrTransform::ElementProjection projectionElement;
			projectionElement.makeOrthographic(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
			_context->setElement(vrTransform::ElementProjection::Id, &projectionElement);

			vuVec4<float> color(1.0f, 0.0f, 0.0f, 1.0f);
			if( (fz > -1.0f) && (fz < 1.0f) && (fx > -1.0f) && (fx < 1.0f) && (fy > -1.0f) && (fy < 1.0f))  //裁剪，判断是否在视口内,通过裁剪才绘制
			{
				pFont->displayStringAt(_context, pString.c_str(), color, fx, fy);
			}
			_context->popElements(false);

		}

	}

	_context->popElements();
}

void classGeometry::drawAirWay(vrDrawContext *_context)
{
	//首先保存一份通道的modelview和projection矩阵
	vrTransform::ElementModelView *modelview = const_cast<vrTransform::ElementModelView *>(vrGetElement(_context, vrTransform::ElementModelView::Id));
	vrTransform::ElementProjection *projection = const_cast<vrTransform::ElementProjection *>(vrGetElement(_context, vrTransform::ElementProjection::Id));

	_context->pushElements(true);

	//恢复通道的modelview和projection矩阵
	_context->setElement(vrTransform::ElementModelView::Id, modelview);
	_context->setElement(vrTransform::ElementProjection::Id, projection);

	for(vector<vrUserGeometry *>::iterator it = AirWayData.begin(); it != AirWayData.end(); it++)
	{
		vrUserGeometry* geometry = *it;
		geometry->draw(_context);  //绘制box
	}

	_context->popElements();
}

void classGeometry::drawLandingArea(vrDrawContext *_context)
{
	//首先保存一份通道的modelview和projection矩阵
	vrTransform::ElementModelView *modelview = const_cast<vrTransform::ElementModelView *>(vrGetElement(_context, vrTransform::ElementModelView::Id));
	vrTransform::ElementProjection *projection = const_cast<vrTransform::ElementProjection *>(vrGetElement(_context, vrTransform::ElementProjection::Id));

	_context->pushElements(true);

	//恢复通道的modelview和projection矩阵
	_context->setElement(vrTransform::ElementModelView::Id, modelview);
	_context->setElement(vrTransform::ElementProjection::Id, projection);



	for(vector<vrUserGeometry *>::iterator it = LandingAreaData.begin(); it != LandingAreaData.end(); it++)
	{
		vrUserGeometry* geometry = *it;

		double pos[3];
		pObserver->getTranslate(&pos[0], &pos[1], &pos[2]);
		setAltimeter(pos[2], 0); //这里目前只适用于有一个landingarea的情况

		geometry->draw(_context);  //绘制landingarea

	}

	_context->popElements();
}

void classGeometry::handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context)
{
	if ((_event != vsChannel::EVENT_POST_DRAW) || (_channel != pChannel))
		return;

	drawBox(_context, pChannel);
	drawAirWay(_context);
	drawLandingArea(_context);
	recordPath(airWayXMLPath, isRecordWorking);
}

void classGeometry::handleKey(vrWindow::Key _key, int _mod)
{
	switch(_key)
	{
	case vrWindow::KEY_SLASH: // '/'键
		isRecordWorking = !isRecordWorking;
		if(isRecordWorking)
			std::cout << "Start recording..." << std::endl;
		else
			std::cout << "Recording over!" << std::endl;
		break;
	}
}

vsTraversal::Result classGeometry::travFunc(vsNode *_node, int)
{
	addBox(_node);
	return vsTraversal::RESULT_CONTINUE;
}

bool classGeometry::isInRange(vsNode* _node, float _range, vpObserver* _observer)
{
	bool flag = false;
	if(_observer == NULL) _observer = pObserver;
	if (_node == NULL) 
	{
		std::cout << "classGeometry::isInRange() : Node is NULL" << std::endl; 
		return flag;
	}

	vuSphere<double>* sphere = const_cast<vuSphere<double>*>(&(_node->getBounds()));
	vuVec3d* ctr = &(sphere->m_ctr);
	vuVec3d pos;

	_observer->getPosition(&pos[0], &pos[1], &pos[2]);

	float dis = pos.distance(*ctr);
	if (dis < _range)
		flag = true;

	return flag;
}

