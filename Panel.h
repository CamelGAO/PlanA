#ifndef _PANEL_H
#define _PANEL_H

#include "stdafx.h"
#include "Common.h"

class classPanel : public classCommon
{
public:
	struct Attribute
	{
		string _name;
		float _lower;
		float _upper;

		Attribute& operator=(const Attribute& _attribute)
		{
			if(this != &_attribute)
			{
				this->_lower = _attribute._lower;
				this->_upper = _attribute._upper;
				this->_name = _attribute._name;
			}
			return *this;
		}
	};

	struct PanelInformation
	{
		vpGLStudioComponent* _component;
		vpChannel* _channel;
		float _position[2];
		float _scale[2];
		std::string _name;
		std::string _dll;
		std::string _class;
		vector<Attribute> _input;
		vector<Attribute> _output;
	};

	static classPanel* instance();
	static void destory();
	void init(const char *_name = TEXT("myWindow"));

	vpGLStudioComponent* loadPanel(PanelInformation& _panel, vpChannel* _channel = NULL, bool _isNeedPush = false);  //_panel�г�component��Ա֮���������Ա��Ӧ���Ѿ�����ȷ��д
	void removePanel(vpGLStudioComponent* _component);
	void loadAllPanel();
	void removeAllPanel();
	PanelInformation* getPanel(std::string _name);

	void enableTraversalProxy();

protected:
private:
	classPanel();  //_name��vp::windows������
	~classPanel();

	void parse(const char* _xmlFilePath);
	void loadPanel(const char* _xmlFilePath);
	void removePanel(PanelInformation& _panel);

	vpWindow* pWindow;

	vector<PanelInformation> PanelData;

	static classPanel* pInstance;
};

#endif 
