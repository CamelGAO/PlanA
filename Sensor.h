#ifndef _SENSOR_H
#define _SENSOR_H

#include "stdafx.h"
#include "Base.h"
#include "ShaderDeployer.h"
#include "vrRenderStrategyUser.h"

class classSensor:public classBase
{
public:
	struct SwapData
	{
		vuField<void*, vrDrawFunc::AttributeTrait>* field;
		vuVec3f* buffer[2];
	};

	classSensor(vpObserver *_observer, vsChannel::Subscriber *_subscriber = NULL);
	classSensor(const char *_name, vsChannel::Subscriber *_subscriber = NULL);
	~classSensor();

	void init(void);
	void initUniform(void);

	unsigned char *getRawLidarTexture(int _width, int _height, const char * _path = NULL);
	void makeTexture();

	template<typename T>
	void getMatrixTexture(T * _data, size_t _offset_float, vuMatrixf _matrix);  //ע��ƫ������ƫ�Ƶ�float�������������ֽ� //����

	void setShaderPara(const vsChannel *channel, vrDrawContext *context, vuVec4d * _position);
	float calHeightColor(const vuVec3f& _coord);     //������������Ҫ�ο������нڵ�

	virtual void handleKey(vrWindow::Key _key, int _mod);
	virtual void handleRunLoop(void);
	virtual void handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context) ;

	vsTraversal::Result travFunc_add_shader(vsNode *node, int);
	vsTraversal::Result travFunc_model_modify(vsNode *node, int);
	vsTraversal::Result travFunc_model_no_modify(vsNode *node, int);
	vsTraversal::Result travFunc_set_database_invisible(vsNode *node, int);
	vsTraversal::Result travFunc_set_height_color(vsNode *node, int);
	vsTraversal::Result travFunc_update_height_color_mt(vsNode *node, int);   //Ŀǰ���һ���̣߳���Ϊ������ʹ��ȫ�ֱ����ĺ���
	vsTraversal::Result travFunc_set_radar_invisible(vsNode *node, int);
	vsTraversal::Result travFunc_find_local_z(vsNode *node, int);
	vsTraversal::Result travFunc_set_lidar_texcoord(vsNode *node, int);
	vsTraversal::Result travFunc_set_model_type(vsNode *node, int);

	static DWORD WINAPI updateModelModify(void* _p);

	vsTraversal::Result travFunc_deinit(vsNode *node, int);

	static bool isFirstInit;         //ִֻ��һ�α����������Ҫ���¼��ص��Σ�Ҫ����������Ϊtrue��������ڱ�������������б����ã����������ʵ��û�������
										//���ڽ��������������ã�����ÿ�μ��ر���Ķ��󶼻����±���
	static bool isMoveEnable;

protected:
private:
	double calTriangleArea(float *a, float *b, float *c, size_t d);

	float calDistance;  //����local��Ը߶�ʱ�����������ĵ㲻�������ֵ�ĵ��������
	float planeThreshold;  //ƽ�������̶ȶ�����ڿ�����Ϊƽ�棿�������ֵ
	float planeThresholdTarget;    //������Ӱ�����ֵ��������߳�����ǰд��planeThreshold������planeThreshold�ڴ���ʱ���޸�
	vuVec3f currentCoord;
	vector<float> z_data;
	float modelType;
   
	size_t idle;	//�޸����ݵĲ������ڵ�ǰ��������buffer�Ͻ��У����ֵ��¼�˵�ǰ���е��Ǹ�buffer
	size_t bufferIndex;  //ÿ�α�����˳����һ���ģ����ֵ�ڱ�����ʼʱΪ�㣬ÿ����һ���ڵ�+1����������������
	vector<SwapData> swapBuffer; //���������߳��д������ݣ�ʵ��ָ����ͬ�����ݽ�����ʾ
	bool isMtDone; //������ɱ�־

	float distance;
	float range_x;
	float range_y;

	static vrTexture *texture[5];
	vrTexture *mtex;
	static bool isLidarTextureDone;

	vector<vuVec3f *> vecModifyData;

	vrShader *pSelectedVertexShader;
	vrShader *pSelectedFragmentShader;;

	vrShader *pVertexShader_terrain;
	vrShader *pFragmentShader_terrain;
	vrShader *pVertexShader_model;
	vrShader *pFragmentShader_model;

	vrRenderStrategyBounds * pStrategyBounds;
	vrRenderStrategyHighlight * pStrategyHighlight;
	vrRenderStrategyNormals * pStrategyNormal;
	classRenderStrategySetUniform * pStrategySetUniform;

	int representation;


	float fShowMode;

	float fLidarViewRangeR;
	float fLidarViewRangeH;
	float fLidarViewRangeV;
	float fLidarViewShape;
	float fLidarColorMode;

	float fRadarViewRangeR;
	float fRadarViewRangeH;
	float fRadarViewRangeV;
	float fRadarViewShape;
	float fRadarColorMode;

	float fIrViewRangeR;
	float fIrViewRangeH;
	float fIrViewRangeV;
	float fIrViewShape;
	float fIrColorMode;

	float fBlendMode;
	float fIsShadowMap;
	float fSamplingRate;

	float fLidarMaxRange;
	float fRadarMaxRange;
	float fIrMaxRange;

	vuVec3d lidar_pos;  //xyz,������ӵ�
	vuVec3d lidar_head; //hpr
	vuVec3d radar_pos;  //xyz,������ӵ�
	vuVec3d radar_head; //hpr
	vuVec3d ir_pos;  //xyz,������ӵ�
	vuVec3d ir_head; //hpr
	vuMatrixf viewMatrix;
	vuMatrixd viewPositionMatrix;//ȷ���ֵ����λ�õľ���

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mLidarScale;  

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mLidarRange_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mRadarRange_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mIrRange_test;

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mLidarViewRangeR_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mLidarViewRangeH_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mLidarViewRangeV_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mLidarViewShape_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mLidarColorMode;

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mRadarViewRangeR_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mRadarViewRangeH_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mRadarViewRangeV_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mRadarViewShape_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mRadarColorMode;

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mIrViewRangeR_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mIrViewRangeH_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mIrViewRangeV_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mIrViewShape_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mIrColorMode;

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mLidarShowMode_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mBlendMode_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mIsShadowMap;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mVisibility_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mAccSnowDensity_test;

	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mOffset_test;
	vuField<vuParameterTyped<float>*, vuFieldTraitMemBase> mSamplingRate_test;

	vuField<vuParameterTyped<vuMatrixf>*, vuFieldTraitMemBase> view_lidar_test;
	vuField<vuParameterTyped<vuMatrixf>*, vuFieldTraitMemBase> view_radar_test;
	vuField<vuParameterTyped<vuMatrixf>*, vuFieldTraitMemBase> view_ir_test;

};

#endif

