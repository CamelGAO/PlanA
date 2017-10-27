#ifndef _FOG_H
#define _FOG_H

#include "stdafx.h"
#include "Base.h"

class classFog:public classBase
{
public:
	classFog(vpObserver *_observer, vsChannel::Subscriber *_subscriber = NULL);				//����ģ�����ϵ�DVE�к���Բ�����_subscriber��_subscriberֻ�������ͨ���¼�
	classFog(const char* _name, vsChannel::Subscriber *_subscriber = NULL);
	~classFog();
	
	virtual void handleKey(vrWindow::Key _key, int _mod);
	void setVisibility(float _visibility);
	float getVisibility(void);
	void setColor(float _r, float _g, float _b, float _a);
	void setColor(float _flag);

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> shader_fogVisibility;
	vuField<vuParameterTyped<vuVec4f>*, vuFieldTraitMemBase> shader_fogColor;

protected:
private:
};

#endif
