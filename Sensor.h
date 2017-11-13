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
	void getMatrixTexture(T * _data, size_t _offset_float, vuMatrixf _matrix);  //注意偏移量是偏移的float变量数，不是字节 //弃用

	void setShaderPara(const vsChannel *channel, vrDrawContext *context, vuVec4d * _position);
	float calHeightColor(const vuVec3f& _coord);     //在里面设置需要参考的所有节点

	virtual void handleKey(vrWindow::Key _key, int _mod);
	virtual void handleRunLoop(void);
	virtual void handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context) ;

	vsTraversal::Result travFunc_add_shader(vsNode *node, int);
	vsTraversal::Result travFunc_model_modify(vsNode *node, int);
	vsTraversal::Result travFunc_model_no_modify(vsNode *node, int);
	vsTraversal::Result travFunc_set_database_invisible(vsNode *node, int);
	vsTraversal::Result travFunc_set_height_color(vsNode *node, int);
	vsTraversal::Result travFunc_update_height_color_mt(vsNode *node, int);   //目前最多一个线程，因为调用了使用全局变量的函数
	vsTraversal::Result travFunc_set_radar_invisible(vsNode *node, int);
	vsTraversal::Result travFunc_find_local_z(vsNode *node, int);
	vsTraversal::Result travFunc_set_lidar_texcoord(vsNode *node, int);
	vsTraversal::Result travFunc_set_model_type(vsNode *node, int);

	static DWORD WINAPI updateModelModify(void* _p);

	vsTraversal::Result travFunc_deinit(vsNode *node, int);

	static bool isFirstInit;         //只执行一次遍历。如果需要重新加载地形，要将这里重置为true，如果它在本类的析构函数中被重置，这个变量其实是没有意义的
										//现在将它在析构中重置，这样每次加载本类的对象都会重新遍历
	static bool isMoveEnable;

protected:
private:
	double calTriangleArea(float *a, float *b, float *c, size_t d);

	float calDistance;  //计算local相对高度时，距离待处理的点不超过这个值的点会参与计算
	float planeThreshold;  //平面的起伏程度多大以内可以视为平面？设置这个值
	float planeThresholdTarget;    //按键将影响这个值，随后在线程启动前写入planeThreshold，避免planeThreshold在处理时被修改
	vuVec3f currentCoord;
	vector<float> z_data;
	float modelType;
   
	size_t idle;	//修改数据的操作会在当前不工作的buffer上进行，这个值记录了当前空闲的那个buffer
	size_t bufferIndex;  //每次遍历的顺序是一样的，这个值在遍历开始时为零，每遍历一个节点+1，遍历结束后清零
	vector<SwapData> swapBuffer; //用于在新线程中处理数据，实现指定不同的数据进行显示
	bool isMtDone; //处理完成标志

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

	vuVec3d lidar_pos;  //xyz,相对于视点
	vuVec3d lidar_head; //hpr
	vuVec3d radar_pos;  //xyz,相对于视点
	vuVec3d radar_head; //hpr
	vuVec3d ir_pos;  //xyz,相对于视点
	vuVec3d ir_head; //hpr
	vuMatrixf viewMatrix;
	vuMatrixd viewPositionMatrix;//确定手电相对位置的矩阵

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

