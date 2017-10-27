#ifndef _PARTICLE_SYSTEM_H
#define _PARTICLE_SYSTEM_H

#include "stdafx.h"
using namespace std;

class ParticleSystem
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

	struct particle
	{
		vuVec3d p;			//位置
		vuVec3d v;			//速度
		vuVec3d a;			//加速度
		vuVec3d dimension;	//尺寸
		vuVec3d scale;		//缩放
		vuVec3d rotate;		//旋转
		int life;			//剩余生命
		int fade;			//生命消逝速度

	};

	struct environment
	{
		int areaTop;			//定义粒子生成区域
		int areaButtom;
		int areaLeft;
		int areaRight;
		int areaFront;
		int areaBack;
		vuVec3d areaCenter;
		int areaRange;			
		vuVec3d rotate;			//视点的俯仰滚角
		int quantity;			//粒子数量
		int total;				//例子数量上限
		GLuint tex;				//纹理编号
	};

	particle particleData;
	environment environmentData;

	void initData();
	int loadTextures();
	AUX_RGBImageRec* loadBMP(const char *);
	void setParticle(vector<particle>::iterator &);
	void draw();
	void create(vector<particle>::iterator &);


protected:
private:
	vector<particle> particles;
	GLuint texture[2];	
	GLuint list;

	bool isInit;
};


#endif