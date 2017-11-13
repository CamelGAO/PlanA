#include "stdafx.h"
#include "Panel.h"

classPanel* classPanel::pInstance = new classPanel();

classPanel::classPanel()
{
}
classPanel::~classPanel()
{
}

classPanel* classPanel::instance()
{
	return pInstance;
}

void classPanel::destory()
{
	delete pInstance;
}

void classPanel::init(const char* _name)  //_name是vpWindow的名字
{
	if(!_name)
	{
		std::cout << "classPanel::init(): _name is Null!" << std::endl;
		system("pause");
		return;
	}
	loadAllPanel();
}

void classPanel::parse(const char* _xmlFilePath)
{
	PanelData.clear();

	tinyxml2::XMLDocument doc;
	int ret;
	ret = doc.LoadFile(_xmlFilePath);
	if(ret != 0) 
		std::cout << "classPanel::parse(): Fail to load file! Code : " << ret << std::endl;
	tinyxml2::XMLElement* root = doc.RootElement();
	tinyxml2::XMLElement* node0 = root->FirstChildElement("panel");
	const char* data; //暂存attribute的结果指针
	bool flag; //暂存attribute的bool结果
	while(node0)
	{
		data = node0->Attribute("enable");
		tinyxml2::XMLUtil::ToBool(data, &flag);
		if(flag) 
		{
			PanelInformation temp;
			data = node0->Attribute("channel");
			temp._channel = vpChannel::find(data);
			if (!temp._channel) 
				std::cout << "classPanel::parse(): Invalid channel name: " << data << std::endl;
			data = node0->Attribute("position_x");
			tinyxml2::XMLUtil::ToFloat(data, &temp._position[0]);
			data = node0->Attribute("position_y");
			tinyxml2::XMLUtil::ToFloat(data, &temp._position[1]);
			data = node0->Attribute("scale_x");
			tinyxml2::XMLUtil::ToFloat(data, &temp._scale[0]);
			data = node0->Attribute("scale_y");
			tinyxml2::XMLUtil::ToFloat(data, &temp._scale[1]);
			data = node0->Attribute("name");
			temp._name = data;
			data = node0->Attribute("dll");
			temp._dll = data;
			data = node0->Attribute("class");
			temp._class = data;
			tinyxml2::XMLElement* node1 = node0->FirstChildElement("attribute");
			while (node1)
			{
				Attribute attr;
				data = node1->Attribute("out");
				tinyxml2::XMLUtil::ToBool(data, &flag);
				data = node1->Attribute("lower");
				tinyxml2::XMLUtil::ToFloat(data, &attr._lower);
				data = node1->Attribute("upper");
				tinyxml2::XMLUtil::ToFloat(data, &attr._upper);
				attr._name = std::string(node1->GetText());
				if (flag)
					temp._output.push_back(attr);
				else
					temp._input.push_back(attr);
				
				node1 = node1->NextSiblingElement("attribute");
			}
			temp._component = NULL;
			PanelData.push_back(temp);
		}
		node0 = node0->NextSiblingElement("panel");
	}
}

void classPanel::loadPanel(const char* _xmlFilePath)
{
	if (!_xmlFilePath)
	{
		std::cout << "classPanel::loadPanel(): _xmlFilePath is Null!" << std::endl;
		system("pause");
		return;
	}
	parse(_xmlFilePath);

	for(vector<PanelInformation>::iterator i = PanelData.begin(); i != PanelData.end(); i++)
	{
		loadPanel(*i, NULL, false);
	}
}

vpGLStudioComponent* classPanel::loadPanel(PanelInformation& _panel, vpChannel* _channel, bool isNeedPush)
{
	vpGLStudioComponent* component;
	component = new vpGLStudioComponent();
	_panel._component = component;
	if (_channel)
		_panel._channel = _channel;
	component->setName( _panel._name.c_str() );
	component->setCullMask( 0x0FFFFFFFF );
	component->setRenderMask( 0x0FFFFFFFF );
	component->setIsectMask( 0x0FFFFFFFF );
	component->setStrategyEnable( true );
	component->setTranslate( _panel._position[0] ,  _panel._position[1] ,  0.000000 );
	component->setRotate( 0.000000 ,  0.000000 ,  0.000000 );
	component->setScale( _panel._scale[0] ,  _panel._scale[1] ,  1.000000 );
	component->setStaticEnable( false );
	component->setDLLName( _panel._dll.c_str() );
	component->setClassName( _panel._class.c_str() );
	component->dumpClassResourceNames( false );
	component->setGeoRef( 0.000000, 0.000000, 0.000000);
	component->addChannel(_panel._channel);
	component->ref();

	if (isNeedPush)
	{
		PanelData.push_back(_panel);
	}
	return component;
}

void classPanel::loadAllPanel()
{
	loadPanel("E:\\newstart\\Data_XML\\Panel.xml");
}

void classPanel::removePanel(vpGLStudioComponent* _component)
{
	if (_component != NULL)
	{
		int num =  _component->getNumChannels();
		for(int i = 0; i < num; i++)
		{
			vpChannel* _channel = _component->getChannel(i);
			if(_channel != NULL)
			{
				_component->removeChannel(_channel);
				//cout << "ss" << endl;
			}
			else
			{
				std::cout << "classPanel::removePanel() : Get a null channel pointer!" << std::endl;	
			}
			_component->unref();
		}
	}
	else
		std::cout << "classPanel::removePanel() : NULL pointer!" << std::endl;
}

void classPanel::removePanel(PanelInformation& _panel)
{
	removePanel(_panel._component);
	_panel._component = NULL;
	_panel._channel = NULL;
	_panel._name.clear();
	_panel._dll.clear();
	_panel._class.clear();
	_panel._input.clear();
	_panel._output.clear();
}

void classPanel::removeAllPanel(void)
{
	for(vector<PanelInformation>::iterator i = PanelData.begin(); i != PanelData.end(); i++)
	{
		removePanel(*i);
		//std::cout << "remove" << std::endl;
	}
	PanelData.clear();
}

classPanel::PanelInformation* classPanel::getPanel(std::string _name)
{
	for(vector<PanelInformation>::iterator i = PanelData.begin(); i != PanelData.end(); i++)
	{
		if((*i)._name == _name)
			return &(*i);
	}
	return NULL;
}


