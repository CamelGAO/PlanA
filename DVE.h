#ifndef _DVE_H
#define _DVE_H

#include "stdafx.h"
#include "Common.h"
#include "Base.h"
#include "Rain.h"
#include "Snow.h"
#include "Dust.h"
#include "Fog.h"
#include "Time.h"
#include "AccSnow.h"
#include "ShaderDeployer.h"

class classDve:public classCommon,public classBase
{
public:
	classDve(const char* _name, vsChannel::Subscriber *_subscriber);
	~classDve();

	void updateData(void);    //����������ݣ�ÿ��ֻ��һ���ı���д�����runloop�б�����

	virtual void handleKey(vrWindow::Key _key, int _mod);
	virtual void handleRunLoop(void);

	void deleteDveObject(classEnvInfo::enumDve _dve);
	void enableDve(classEnvInfo::enumDve _dve);

	static float calculateAccSnowDensity(float _in);

protected:
private:
	classFog *pFog;
	classRain *pRain;
	classSnow *pSnow;
	classAccSnow *pAccSnow;
	classTime *pTime;
	classDust *pDust;

	vsChannel::Subscriber *pSubscriber;

	classPanel::PanelInformation* pPanelInfo;
	vpGLStudioComponent *pGLSComponent;

	static bool isMoveEnable;		//��¼�Ƿ��˶����������лᱻ����

	static int last_rainLevel;   //��¼�������һ��״̬������״̬�ȽϿ�֪�仯���������лᱻ����
	static int last_snowLevel;
	static int last_fogLevel;
	static int last_dustLevel;
	static int last_sandLevel;
	static int last_snowaccLevel;
	static int last_time;
};

#endif
