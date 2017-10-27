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
		vuVec3d p;			//λ��
		vuVec3d v;			//�ٶ�
		vuVec3d a;			//���ٶ�
		vuVec3d dimension;	//�ߴ�
		vuVec3d scale;		//����
		vuVec3d rotate;		//��ת
		int life;			//ʣ������
		int fade;			//���������ٶ�

	};

	struct environment
	{
		int areaTop;			//����������������
		int areaButtom;
		int areaLeft;
		int areaRight;
		int areaFront;
		int areaBack;
		vuVec3d areaCenter;
		int areaRange;			
		vuVec3d rotate;			//�ӵ�ĸ�������
		int quantity;			//��������
		int total;				//������������
		GLuint tex;				//������
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