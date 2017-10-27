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

	static bool isFirstInit;         //只执行一次遍历。如果需要重新加载地形，要将这里重置为true，如果它在本类的析构函数中被重置，这个变量其实是没有意义的
										//现在将它在析构中重置，这样每次加载本类的对象都会重新遍历

protected:
private:
	float density;

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> acc_density;

	vrShader *pVertexShader;
	vrShader *pFragmentShader;

	int representation;
};

#endif
