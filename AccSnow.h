#ifndef _ACCSNOW_H
#define _ACCSNOW_H

#include "stdafx.h"
#include "Base.h"
#include "ShaderDeployer.h"

class classAccSnow:public classBase
{
public:
	classAccSnow(vpObserver *_observer, vsChannel::Subscriber *_subscriber = NULL);
	classAccSnow(const char *_name, vsChannel::Subscriber *_subscriber = NULL);
	~classAccSnow();

	void init(void);

	virtual void handleKey(vrWindow::Key _key, int _mod);

	vsTraversal::Result travFunc(vsNode *node, int);

	void setDensity(float _density);

	static bool isFirstInit;         //ִֻ��һ�α����������Ҫ���¼��ص��Σ�Ҫ����������Ϊtrue��������ڱ�������������б����ã����������ʵ��û�������
										//���ڽ��������������ã�����ÿ�μ��ر���Ķ��󶼻����±���

protected:
private:
	float density;

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> acc_density;

	vrShader *pVertexShader;
	vrShader *pFragmentShader;

	int representation;
};

#endif
