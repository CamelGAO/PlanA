#include "stdafx.h"
#include "Sensor.h"

bool classSensor::isFirstInit = true;
bool classSensor::isMoveEnable;
bool classSensor::isLidarTextureDone = false;
vrTexture * classSensor::texture[5];

classSensor::classSensor(vpObserver *_observer, vsChannel::Subscriber *_subscriber):classBase(_observer,_subscriber,ENABLE_KEY|ENABLE_RUNLOOP,false,TEXT("myObject"))
{
	init();
}

classSensor::classSensor(const char *_name, vsChannel::Subscriber *_subscriber):classBase(_name,_subscriber,ENABLE_KEY|ENABLE_RUNLOOP,false,TEXT("myObject"))
{
	init();
}

classSensor::~classSensor()
{
	removeEvent(vsChannel::EVENT_PRE_DRAW);

	pChannel->setRepresentationIndex(0);
	pChannel->removeRepresentation(representation);
	pShaderDeployer->removeRepresentation("r_sensor");

	vsTraversalUser<int, vsTraversalLookUpNodeId> trav_deinit;  
	trav_deinit.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_deinit);			
	if(NULL != pObject) trav_deinit.visit(pObject);

	isFirstInit = true;
	isMoveEnable = false;
}

void classSensor::init(void)
{
	addEvent(vsChannel::EVENT_PRE_DRAW);

	isMoveEnable = pObserver->getStrategyEnable();

	pSelectedVertexShader = NULL;
	pSelectedFragmentShader = NULL;

	if( (pVertexShader_terrain = pShaderDeployer->getShader("v_sensor_terrain")) == NULL )
		std::cout << " vShader \"v_sensor_terrain\" doesn't exist! " << std::endl;

	if( (pFragmentShader_terrain = pShaderDeployer->getShader("f_sensor_terrain")) == NULL )
		std::cout << " fShader \"f_sensor_terrain\" doesn't exist! " << std::endl;

	if( (pVertexShader_model = pShaderDeployer->getShader("v_sensor_model")) == NULL )
		std::cout << " vShader \"v_sensor_model\" doesn't exist! " << std::endl;

	if( (pFragmentShader_model = pShaderDeployer->getShader("f_sensor_model")) == NULL )
		std::cout << " fShader \"f_sensor_model\" doesn't exist! " << std::endl;

	pShaderDeployer->addRepresentation("r_sensor");
	representation = pShaderDeployer->getRepresentation("r_sensor");

	if(isLidarTextureDone == false) makeTexture(); //生成激光雷达纹理

	initUniform();

	if(isFirstInit == true)
	{
		//pChannel->setCullFunc(NULL);
		//pChannel->setDrawFunc(NULL);

		pSelectedVertexShader = pVertexShader_model;
		pSelectedFragmentShader = pFragmentShader_model;
		vsTraversalUser<int, vsTraversalLookUpNodeId> trav_add_shader;
		trav_add_shader.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_add_shader);
		if(NULL != pObject) trav_add_shader.visit(pObject);
		pSelectedVertexShader = NULL;
		pSelectedFragmentShader = NULL;

		vsTraversalUser<int, vsTraversalLookUpNodeId> trav_set_lidar_texcoord;
		trav_set_lidar_texcoord.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_set_lidar_texcoord);
		if(NULL != pObject) trav_set_lidar_texcoord.visit(pObject);

		vsTraversalUser<int, vsTraversalLookUpNodeId> trav_set_model_type;
		trav_set_model_type.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_set_model_type);
		modelType = 0.0f;
		if(NULL != pObject) trav_set_model_type.visit(pObject);
		modelType = 1.0f;
		if(NULL != pTerrain) trav_set_model_type.visit(pTerrain);

		vsTraversalUser<int, vsTraversalLookUpNodeId> trav_model_no_modify; 
		trav_model_no_modify.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_model_no_modify);			
		if(NULL != pObject) trav_model_no_modify.visit(pObject);

		vsTraversalUser<int, vsTraversalLookUpNodeId> trav_model_modify;  //会替换掉上面的不修改状态
		trav_model_modify.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_model_modify);			
		if(NULL != pBuilding) trav_model_modify.visit(pBuilding);
		if(NULL != pCar) trav_model_modify.visit(pCar);

		vsTraversalUser<int, vsTraversalLookUpNodeId> trav_set_database_invisible;
		trav_set_database_invisible.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_set_database_invisible);			
		if(NULL != pPowerLine) trav_set_database_invisible.visit(pPowerLine);
		if(NULL != pTree) trav_set_database_invisible.visit(pTree);
		if(NULL != pCar) trav_set_database_invisible.visit(pCar);

		vsTraversalUser<int, vsTraversalLookUpNodeId> trav_set_radar_invisible;
		trav_set_radar_invisible.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_set_radar_invisible);			
		if(NULL != pTree) trav_set_radar_invisible.visit(pTree);


		vsTraversalUser<int, vsTraversalLookUpNodeId> trav_set_height_color;
		trav_set_height_color.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_set_height_color);
		calDistance = 150;
		if(NULL != pTerrain) trav_set_height_color.visit(pTerrain);
		if(NULL != pBuilding) trav_set_height_color.visit(pBuilding);
		calDistance = 50;
		if(NULL != pTree) trav_set_height_color.visit(pTree);
		calDistance = 50;
		if(NULL != pCar) trav_set_height_color.visit(pCar);


		isFirstInit = false;
	}
}

void classSensor::initUniform(void)
{
	fShowMode = 0.0f;
	fBlendMode = 1.0f;

	fIsShadowMap = 0.0f;
	fLidarMaxRange = 1500.0f;
	fRadarMaxRange = 10000.0f;
	fIrMaxRange = 500000.0f;
	fSamplingRate = 0.2f;

	fLidarViewRangeR = 0.3f;
	fLidarViewRangeH = 0.2867f;
	fLidarViewRangeV = 0.2820f;
	fLidarViewShape = 2.0f;
	fLidarColorMode = 1.0f;

	fRadarViewRangeR = 0.3f;
	fRadarViewRangeH = 0.4f;
	fRadarViewRangeV = 0.4f;
	fRadarViewShape = 2.0f;
	fRadarColorMode = 1.0f;

	fIrViewRangeR = 0.3f;
	fIrViewRangeH = 0.46f;
	fIrViewRangeV = 0.57f;
	fIrViewShape = 2.0f;
	fIrColorMode = 1.0f;

	mLidarScale = vuParameter::obtain< vuParameterTyped<float> >("scale_test");
	mLidarScale->set(1.0f);


	//mVisibility_test->vuParameter::obtain< vuParameterTyped<float> >("visibility_test");
	//mVisibility_test->set(80000.0f);
	//mAccSnowDensity_test->vuParameter::obtain< vuParameterTyped<float> >("density_test");
	//mAccSnowDensity_test->set(0.0f);

	mLidarRange_test = vuParameter::obtain< vuParameterTyped<float> >("max_range_lidar_test");
	mLidarRange_test->set(fLidarMaxRange);
	mRadarRange_test = vuParameter::obtain< vuParameterTyped<float> >("max_range_radar_test");
	mRadarRange_test->set(fRadarMaxRange);
	mIrRange_test = vuParameter::obtain< vuParameterTyped<float> >("max_range_ir_test");
	mIrRange_test->set(fIrMaxRange);

	mLidarViewRangeR_test = vuParameter::obtain< vuParameterTyped<float> >("lidar_view_range_r_test");
	mLidarViewRangeR_test->set(fLidarViewRangeR);
	mLidarViewRangeH_test = vuParameter::obtain< vuParameterTyped<float> >("lidar_view_range_h_test");
	mLidarViewRangeH_test->set(fLidarViewRangeH);
	mLidarViewRangeV_test = vuParameter::obtain< vuParameterTyped<float> >("lidar_view_range_v_test");
	mLidarViewRangeV_test->set(fLidarViewRangeV);
	mLidarViewShape_test = vuParameter::obtain< vuParameterTyped<float> >("lidar_view_shape_test");
	mLidarViewShape_test->set(fLidarViewShape);
	mLidarColorMode = vuParameter::obtain< vuParameterTyped<float> >("lidar_color_mode_test");
	mLidarColorMode->set(fLidarColorMode);  //初始化为着色模式1

	mRadarViewRangeR_test = vuParameter::obtain< vuParameterTyped<float> >("radar_view_range_r_test");
	mRadarViewRangeR_test->set(fRadarViewRangeR);
	mRadarViewRangeH_test = vuParameter::obtain< vuParameterTyped<float> >("radar_view_range_h_test");
	mRadarViewRangeH_test->set(fRadarViewRangeH);
	mRadarViewRangeV_test = vuParameter::obtain< vuParameterTyped<float> >("radar_view_range_v_test");
	mRadarViewRangeV_test->set(fRadarViewRangeV);
	mRadarViewShape_test = vuParameter::obtain< vuParameterTyped<float> >("radar_view_shape_test");
	mRadarViewShape_test->set(fRadarViewShape);
	mRadarColorMode = vuParameter::obtain< vuParameterTyped<float> >("radar_color_mode_test");
	mRadarColorMode->set(fRadarColorMode);  //初始化为着色模式1

	mIrViewRangeR_test = vuParameter::obtain< vuParameterTyped<float> >("ir_view_range_r_test");
	mIrViewRangeR_test->set(fIrViewRangeR);
	mIrViewRangeH_test = vuParameter::obtain< vuParameterTyped<float> >("ir_view_range_h_test");
	mIrViewRangeH_test->set(fIrViewRangeH);
	mIrViewRangeV_test = vuParameter::obtain< vuParameterTyped<float> >("ir_view_range_v_test");
	mIrViewRangeV_test->set(fIrViewRangeV);
	mIrViewShape_test = vuParameter::obtain< vuParameterTyped<float> >("ir_view_shape_test");
	mIrViewShape_test->set(fIrViewShape);
	mIrColorMode = vuParameter::obtain< vuParameterTyped<float> >("ir_color_mode_test");
	mIrColorMode->set(fIrColorMode);  //初始化为着色模式1

	mLidarShowMode_test = vuParameter::obtain< vuParameterTyped<float> >("show_mode_test");
	mLidarShowMode_test->set(fShowMode);
	mBlendMode_test = vuParameter::obtain< vuParameterTyped<float> >("blendMode_test");
	mBlendMode_test->set(fBlendMode);
	mIsShadowMap = vuParameter::obtain< vuParameterTyped<float> >("is_shadow_map_test");
	mIsShadowMap->set(fIsShadowMap);

	mOffset_test = vuParameter::obtain< vuParameterTyped<float> >("offset_test");
	mOffset_test->set(0.1f);
	mSamplingRate_test = vuParameter::obtain< vuParameterTyped<float> >("sampling_rate_test");
	mSamplingRate_test->set(fSamplingRate);

	{
		//设置lidar的view偏转矩阵-start
		lidar_pos = vuVec3d(0,0,0);	//(正值手电在眼睛左侧，正值手电在眼睛下方，正值手电在眼睛前方)
		lidar_head = vuVec3d(0,0,0);	//(别动，正值向下照，正值向右找)
		view_lidar_test = vuParameter::obtain< vuParameterTyped<vuMatrixf > >("view_lidar_test");
		viewMatrix.makeIdentity();
		vuMatrixf _tempMatrix;
		_tempMatrix.makeTranslate(lidar_pos[0],lidar_pos[1],lidar_pos[2]); //(正值手电在眼睛左侧，正值手电在眼睛下方，正值手电在眼睛前方)
		viewMatrix.postMultiply(_tempMatrix);
		vuMatrixTruncate(&viewPositionMatrix, viewMatrix);
		_tempMatrix.makeRotate(lidar_head[0],lidar_head[1],lidar_head[2]); //(别动，正值向下照，正值向右找)
		viewMatrix.postMultiply(_tempMatrix);
		_tempMatrix.transpose(viewMatrix);
		view_lidar_test->set(_tempMatrix);
		//getMatrixTexture(rawMatrixTexture, 0, _tempMatrix);
		//设置lidar的view偏转矩阵-end
	}

	{
		//设置radar的view偏转矩阵-start
		radar_pos = vuVec3d(0,0,0);	//(正值手电在眼睛左侧，正值手电在眼睛下方，正值手电在眼睛前方)
		radar_head = vuVec3d(0,0,0);	//(别动，正值向下照，正值向右找)
		view_radar_test = vuParameter::obtain< vuParameterTyped<vuMatrixf > >("view_radar_test");
		viewMatrix.makeIdentity();
		vuMatrixf _tempMatrix;
		_tempMatrix.makeTranslate(radar_pos[0],radar_pos[1],radar_pos[2]); //(正值手电在眼睛左侧，正值手电在眼睛下方，正值手电在眼睛前方)
		viewMatrix.postMultiply(_tempMatrix);
		vuMatrixTruncate(&viewPositionMatrix, viewMatrix);
		_tempMatrix.makeRotate(radar_head[0],radar_head[1],radar_head[2]); //(别动，正值向下照，正值向右找)
		viewMatrix.postMultiply(_tempMatrix);
		_tempMatrix.transpose(viewMatrix);
		view_radar_test->set(_tempMatrix);
		//getMatrixTexture(rawMatrixTexture, 16, _tempMatrix);
		//设置radar的view偏转矩阵-end
	}

	{
		//设置ir的view偏转矩阵-start
		ir_pos = vuVec3d(0,0,0);	//(正值手电在眼睛左侧，正值手电在眼睛下方，正值手电在眼睛前方)
		ir_head = vuVec3d(0,0,0);	//(别动，正值向下照，正值向右找)
		view_ir_test = vuParameter::obtain< vuParameterTyped<vuMatrixf > >("view_ir_test");
		viewMatrix.makeIdentity();
		vuMatrixf _tempMatrix;
		_tempMatrix.makeTranslate(ir_pos[0],ir_pos[1],ir_pos[2]); //(正值手电在眼睛左侧，正值手电在眼睛下方，正值手电在眼睛前方)
		viewMatrix.postMultiply(_tempMatrix);
		vuMatrixTruncate(&viewPositionMatrix, viewMatrix);
		_tempMatrix.makeRotate(ir_head[0],ir_head[1],ir_head[2]); //(别动，正值向下照，正值向右找)
		viewMatrix.postMultiply(_tempMatrix);
		_tempMatrix.transpose(viewMatrix);
		view_ir_test->set(_tempMatrix);
		//getMatrixTexture(rawMatrixTexture, 32, _tempMatrix);
		//设置ir的view偏转矩阵-end
	}
	//使用纹理传递自建view偏转矩阵——end
}

unsigned char * classSensor::getRawLidarTexture(int _width, int _height, const char * _path)
{
	srand(unsigned(time(0)));
	int alpha;
	uchar *image, *ucp;
	image = ucp = vuAllocArray<uchar>::malloc(_width * _height * 4);
	for (size_t u=0;u<_width;u++) 
		for (size_t t=0;t<_height;t++) 
		{
			alpha = rand()%255;
			*ucp = (uchar) 255; ucp++;   //R
			*ucp = (uchar) 255; ucp++;   //G
			*ucp = (uchar) 255; ucp++;   //B
			*ucp = (uchar) alpha; ucp++;      //Alpha
		}

	return image;
}

template<typename T>
void classSensor::getMatrixTexture(T * data, size_t _offset_float, vuMatrixf _matrix)
{
	static UINT max = 1000;
	static UINT step = UINT_MAX/(2*max); //2147484
	T _d[4][4];

	if (data == NULL)
	{
		std::cout << "getMatrixTexture(): Null pointer!" << std::endl;
		system("pause");
		return;
	}

	for (size_t i = 0; i < 4; i++)
		for(size_t j = 0; j < 4; j++)
		{
			_d[i][j] = ((_matrix.m_mat)[i][j]+max)*step;
		}

		_d[0][3] = UINT_MAX*0.2f;
 
	data += _offset_float;
	memcpy(data, _d, 16 * sizeof(T));

	for (size_t i = 0; i < 4; i++)
	{
		std::cout << std::endl;
		for(size_t j = 0; j < 4; j++)
		{
			std::cout << data[4*i+j] << " ";
		}
	}
}

void classSensor::makeTexture()
{
	for(int i=0;i<4;i++)
	{
		texture[i] = new vrTexture();
		texture[i]->setFormat(vrTexture::FORMAT_RGBA, vrTexture::TYPE_UNSIGNED_BYTE);
		texture[i]->setMinFilter(vrTexture::FILTER_LINEAR, vrTexture::FILTER_LINEAR);
		texture[i]->setMagFilter(vrTexture::FILTER_LINEAR );
		texture[i]->setWrap(vrTexture::WRAP_REPEAT, vrTexture::WRAP_REPEAT);
		texture[i]->setBorderEnable(false);
		if(i == 0) 
		{
			texture[i]->setImageLevel(0, getRawLidarTexture(256,256));
			texture[i]->setDimensions(256,256);
		}
		else if(i == 1) 	
		{
			texture[i]->setImageLevel(0, getRawLidarTexture(128,128) );
			texture[i]->setDimensions(128,128);
		}
		else if(i == 2) 	
		{
			texture[i]->setImageLevel(0, getRawLidarTexture(64,64) );
			texture[i]->setDimensions(64,64);
		}
		else
		{
			texture[i]->setImageLevel(0, getRawLidarTexture(32,32) );
			texture[i]->setDimensions(32,32);
		}
		texture[i]->ref();
		vrTextureProcessorDefault *textureProcessor = new vrTextureProcessorDefault();
		textureProcessor->generateMipMaps(texture[i]);
		textureProcessor->unref();
	}

	texture[4] = new vrTexture();
	texture[4]->setFormat(vrTexture::FORMAT_RGBA, vrTexture::TYPE_UNSIGNED_BYTE);
	texture[4]->setDimensions(4096, 4096);
	texture[4]->setBorderEnable(false);
	texture[4]->setMagFilter(vrTexture::FILTER_NEAREST);
	texture[4]->setMinFilter(vrTexture::FILTER_NEAREST, vrTexture::FILTER_NONE);
	texture[4]->setWrap(vrTexture::WRAP_REPEAT, vrTexture::WRAP_REPEAT);
	texture[4]->setImageLevel(0, getRawLidarTexture(4096,4096));
	texture[4]->ref();
}

void classSensor::handleKey(vrWindow::Key _key, int _mod)
{
	switch (_key)
	{
	case vrWindow::KEY_F6:
		pChannel->setRepresentationIndex( pChannel->getRepresentationIndex() == representation ? 0 : representation );
		break;

	case vrWindow::KEY_p:
		pObserver->setStrategyEnable(isMoveEnable = !isMoveEnable);
		break;

	case vrWindow::KEY_1:
		fBlendMode += 1.0f;
		if(fBlendMode>8.1f)
			fBlendMode = 1.0f;
		mBlendMode_test->set(fBlendMode);
		break;

// 	case vrWindow::KEY_2:
// 		fViewShape += 1.0f;
// 		if(fViewShape>2.1f)
// 			fViewShape = 1.0f;
// 		mViewShape_test->set(fViewShape);
// 		break;

	case vrWindow::KEY_3:
		fLidarColorMode += 1.0f;
		if(fLidarColorMode>4.1f)
			fLidarColorMode = 1.0f;
		mLidarColorMode->set(fLidarColorMode);
		break;

	case vrWindow::KEY_4:
		fRadarColorMode += 1.0f;
		if(fRadarColorMode>2.1f)
			fRadarColorMode = 1.0f;
		mRadarColorMode->set(fRadarColorMode);
		break;

// 	case vrWindow::KEY_q:
// 		fViewRangeR -= 0.02f;
// 		if(fViewRangeR<0.02f)
// 			fViewRangeR = 0.02f;
// 		mViewRangeR_test->set(fViewRangeR);
// 		break;
// 	case vrWindow::KEY_w:
// 		fViewRangeR += 0.02f;
// 		if(fViewRangeR>0.4f)
// 			fViewRangeR = 0.4f;
// 		mViewRangeR_test->set(fViewRangeR);
// 		break;
// 
// 	case vrWindow::KEY_e:
// 		fViewRangeH -= 0.02f;
// 		if(fViewRangeH<0.02f)
// 			fViewRangeH = 0.02f;
// 		mViewRangeH_test->set(fViewRangeH);
// 		break;
// 	case vrWindow::KEY_r:
// 		fViewRangeH += 0.02f;
// 		if(fViewRangeH>0.4f)
// 			fViewRangeH = 0.4f;
// 		mViewRangeH_test->set(fViewRangeH);
// 		break;
// 
// 	case vrWindow::KEY_t:
// 		fViewRangeV -= 0.02f;
// 		if(fViewRangeV<0.02f)
// 			fViewRangeV = 0.02f;
// 		mViewRangeV_test->set(fViewRangeV);
// 		break;
// 	case vrWindow::KEY_y:
// 		fViewRangeV += 0.02f;
// 		if(fViewRangeV>0.4f)
// 			fViewRangeV = 0.4f;
// 		mViewRangeV_test->set(fViewRangeV);
// 		break;

	case vrWindow::KEY_u:
		fLidarMaxRange -= 100.0f;
		if(fLidarMaxRange<100.0f)
			fLidarMaxRange = 100.0f;
		mLidarRange_test->set(fLidarMaxRange);
		break;
	case vrWindow::KEY_i:
		fLidarMaxRange += 100.0f;
		if(fLidarMaxRange>3000.0f)
			fLidarMaxRange = 3000.0f;
		mLidarRange_test->set(fLidarMaxRange);
		break;

	case vrWindow::KEY_6:
		fSamplingRate -= 0.005f;
		if(fSamplingRate<0.0f)
			fSamplingRate = 0.0f;
		mSamplingRate_test->set(fSamplingRate);
		break;
	case vrWindow::KEY_7:
		fSamplingRate += 0.005f;
		if(fSamplingRate>1.0f)
			fSamplingRate = 1.0f;
		mSamplingRate_test->set(fSamplingRate);
		break;
	}
}

vsTraversal::Result classSensor::travFunc_add_shader(vsNode *node, int)
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();
	geometry_s->setGeometry(geometry_r, representation);
	vrState *state = new vrState();
	vsgu::copy(state, geometry_s->getState());


	vrTexture::Element *_texture = const_cast<vrTexture::Element *>(vrGetElement(state, vrTexture::Element::Id));
	_texture->m_enable[1] = true;
	_texture->m_texture[1] = texture[4];
	_texture->m_texture[1]->ref();                          //这个ref不能少
	state->setElement(vrTexture::Element::Id, _texture);

	vrAlphaBlend::Element *_alphablend = new vrAlphaBlend::Element();
	_alphablend->m_enable = true;
	_alphablend->m_src = vrAlphaBlend::MODE_SRC_ALPHA;
	_alphablend->m_dst  =vrAlphaBlend::MODE_INVERSE_SRC_ALPHA;
	state->setElement(vrAlphaBlend::Element::Id, _alphablend);

// 	vrDepthWrite::Element *_depthwrite = new vrDepthWrite::Element();
// 	_depthwrite->m_enable = true;
// 	state->setElement(vrDepthWrite::Element::Id, _depthwrite);
// 
// 	vrDepthTest::Element *_depthtest = new vrDepthTest::Element();
// 	_depthtest->m_enable = true;
// 	_depthtest->m_mode = vrDepthTest::MODE_LESS;
// 	state->setElement(vrDepthTest::Element::Id, _depthtest);

	if(pSelectedVertexShader != NULL)
	{
		vrShader::ElementVertex vertexShaderElement;
		vertexShaderElement.m_shader = pSelectedVertexShader;
		state->setElement(vrShader::ElementVertex::Id, &vertexShaderElement);
	}

	if(pSelectedFragmentShader != NULL)
	{
		vrShader::ElementFragment fragmentShaderElement;
		fragmentShaderElement.m_shader = pSelectedFragmentShader;
		state->setElement(vrShader::ElementFragment::Id, &fragmentShaderElement);	
	}

	geometry_s->setState(state, representation);
	return vsTraversal::RESULT_CONTINUE;
}

vsTraversal::Result classSensor::travFunc_model_modify(vsNode *node, int)   //修改纹理单元1为形变后的点的坐标，纹理单元2被用来记录一些参数
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();

	vuVec3f model_max = geometry_r->getBounds().m_max;
	vuVec3f model_min = geometry_r->getBounds().m_min;
	vuVec3f model_center = vuVec3f( (model_max[0]+model_min[0])*0.5, (model_max[1]+model_min[1])*0.5, (model_max[2]+model_min[2])*0.5 );
	float model_roof = model_max[2];
	float model_base = model_min[2];
	float model_height = model_roof - model_base + 0.01;  //+0.01防止出现0

	float roof_scale = 0.75;
	float base_scale = 1.1;

	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&geometry_r->getData());

// 	cout << model_roof << "  " << model_base << endl; 
// 	cout << "m_format:			" << data->m_format << endl;
// 	cout << "m_numColors:			" << data->m_numColors << endl;
// 	cout << "m_numIndices:			" << data->m_numIndices << endl;
// 	cout << "m_numNormals:			" << data->m_numNormals << endl;
// 	cout << "m_maxActiveStage:		" << data->m_maxActiveStage << endl;
// 	cout << "m_numTextureCoords0:		" << data->m_numTextureCoords[0] << endl;
// 	cout << "m_numTextureCoords1:		" << data->m_numTextureCoords[1] << endl;
// 	cout << "m_numVertices:			" << data->m_numVertices << endl;
// 	cout << "m_primitive:			" << data->m_primitive << endl;
// 	cout << "m_numPrimitives:		" << data->m_numPrimitives << endl;
// 	for(size_t i = 0; i < data->m_numPrimitives; i++)
// 		cout << "m_primitiveLengths" << i << ":		" << (data->m_primitiveLengths.get())[i] << endl; 


	data->m_maxActiveStage = data->m_maxActiveStage < 2 ? 2 : data->m_maxActiveStage;   //设定使用的纹理坐标层数

	data->m_numTextureCoords[1] = data->m_numVertices;
	data->m_dimensionTextureCoord[1] = vrDrawFunc::DIMENSION_3;
	vuVec3f * pos_tc1 = vuAllocArray<vuVec3f>::calloc(data->m_numVertices);
	for(size_t i = 0; i < data->m_numVertices; i++)
	{
		pos_tc1[i] = (data->m_vertex.get())[i];

		float scale = (pos_tc1[i][2] - model_base) / model_height ;
		scale = scale * (roof_scale - base_scale) + base_scale;

		pos_tc1[i][0] = (pos_tc1[i][0] - model_center[0]) * scale + model_center[0];// + ((float)rand()/RAND_MAX-0.5)*10;
		pos_tc1[i][1] = (pos_tc1[i][1] - model_center[1]) * scale + model_center[1];// + ((float)rand()/RAND_MAX-0.5)*10;
		pos_tc1[i][2] = pos_tc1[i][2];
	}
	(data->m_textureCoord[1]).replace(pos_tc1);
	vuAllocArray<vuVec3f>::ref(pos_tc1);

	vuVec3f * color_tc2;
	if(data->m_numTextureCoords[2] != data->m_numVertices)
	{
		data->m_numTextureCoords[2] = data->m_numVertices;
		data->m_dimensionTextureCoord[2] = vrDrawFunc::DIMENSION_3;
		color_tc2 = vuAllocArray<vuVec3f>::calloc(data->m_numVertices);
		(data->m_textureCoord[2]).replace(color_tc2);
		vuAllocArray<vuVec3f>::ref(color_tc2);
	}
	else
	{
		color_tc2 = (vuVec3f*)(data->m_textureCoord[2].get());
	}
	//文理坐标单元2中第1个元素用来控制被遍历的节点在哪个部分中不显示 1-database 2-radar 4-lidar 8-ir
	//纹理坐标单元2的地2个元素用来记录radar下顶点颜色的划分 0~1
	//纹理坐标单元2的地3个元素用来记录radar下是否改变顶点 1改 0 不改
	for(size_t i = 0; i < data->m_numVertices; i++)   
	{
		color_tc2[i][2] = 1.0f;
	}

//	cout << endl;


	return vsTraversal::RESULT_CONTINUE;
}

vsTraversal::Result classSensor::travFunc_model_no_modify(vsNode *node, int)   //将原始位置保存在纹理坐标中，用于电线
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();

	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&geometry_r->getData());

// 	cout << "m_format:			" << data->m_format << endl;
// 	cout << "m_numColors:			" << data->m_numColors << endl;
// 	cout << "m_numIndices:			" << data->m_numIndices << endl;
// 	cout << "m_numNormals:			" << data->m_numNormals << endl;
// 	cout << "m_maxActiveStage:		" << data->m_maxActiveStage << endl;
// 	cout << "m_numTextureCoords0:		" << data->m_numTextureCoords[0] << endl;
// 	cout << "m_numTextureCoords1:		" << data->m_numTextureCoords[1] << endl;
// 	cout << "m_numVertices:			" << data->m_numVertices << endl;
// 	cout << "m_primitive:			" << data->m_primitive << endl;
// 	cout << "m_numPrimitives:		" << data->m_numPrimitives << endl;
// 	for(size_t i = 0; i < data->m_numPrimitives; i++)
// 		cout << "m_primitiveLengths" << i << ":		" << (data->m_primitiveLengths.get())[i] << endl; 
// 	for(size_t i = 0; i < data->m_numVertices; i++)
// 	{
// 		cout << "Vertex" << i << ":	" << (data->m_vertex.get())[i][0] << "	" << (data->m_vertex.get())[i][1] << "	" << (data->m_vertex.get())[i][2] << endl; 
// 		cout << "tCoord" << i << ":	" << ((vuVec2f*)((data->m_textureCoord[0]).get()))[i][0] << "	" << ((vuVec2f*)((data->m_textureCoord[0]).get()))[i][1] << endl; 
// 	}
// 	cout << endl;

	data->m_maxActiveStage = data->m_maxActiveStage < 2 ? 2 : data->m_maxActiveStage;   //设定使用的纹理坐标层数

	vuVec3f * color_tc2;
	if(data->m_numTextureCoords[2] != data->m_numVertices)
	{
		data->m_numTextureCoords[2] = data->m_numVertices;
		data->m_dimensionTextureCoord[2] = vrDrawFunc::DIMENSION_3;
		color_tc2 = vuAllocArray<vuVec3f>::calloc(data->m_numVertices);
		(data->m_textureCoord[2]).replace(color_tc2);
	}
	else
	{
		color_tc2 = (vuVec3f*)(data->m_textureCoord[2].get());
	}
	//文理坐标单元2中第1个元素用来控制被遍历的节点在哪个部分中不显示 1-database 2-radar 4-lidar 8-ir
	//纹理坐标单元2的地2个元素用来记录radar下顶点颜色的划分 0~1
	//纹理坐标单元2的地3个元素用来记录radar下是否改变顶点 1改 0 不改
	for(size_t i = 0; i < data->m_numVertices; i++)   
	{
		color_tc2[i][0] = 0.0000f;
		color_tc2[i][1] = 0.5000f;
		color_tc2[i][2] = 0.0000f;
	}

	return vsTraversal::RESULT_CONTINUE;
}

vsTraversal::Result classSensor::travFunc_set_height_color(vsNode *node, int)   //修改纹理单元1为形变后的点的坐标，纹理单元2被用来记录一些参数
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();

	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&geometry_r->getData());

	data->m_maxActiveStage = data->m_maxActiveStage < 2 ? 2 : data->m_maxActiveStage;   //设定使用的纹理坐标层数

	vuVec3f * color_tc2;
	if(data->m_numTextureCoords[2] != data->m_numVertices)
	{
		data->m_numTextureCoords[2] = data->m_numVertices;
		data->m_dimensionTextureCoord[2] = vrDrawFunc::DIMENSION_3;
		color_tc2 = vuAllocArray<vuVec3f>::calloc(data->m_numVertices);
		(data->m_textureCoord[2]).replace(color_tc2);
		vuAllocArray<vuVec3f>::ref(color_tc2);
	}
	else
	{
		color_tc2 = (vuVec3f*)(data->m_textureCoord[2].get());
	}
	//文理坐标单元2中第1个元素用来控制被遍历的节点在哪个部分中不显示 1-database 2-radar 4-lidar 8-ir
	//纹理坐标单元2的地2个元素用来记录radar下顶点颜色的划分 0~1
	//纹理坐标单元2的地3个元素用来记录radar下是否改变顶点 1改 0 不改
	for(size_t i = 0; i < data->m_numVertices; i++)   
	{
		color_tc2[i][1] = calHeightColor((data->m_vertex.get())[i]);
	}

	return vsTraversal::RESULT_CONTINUE;
}

vsTraversal::Result classSensor::travFunc_set_radar_invisible(vsNode *node, int)   //修改纹理单元1为形变后的点的坐标，纹理单元2被用来记录一些参数
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();

	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&geometry_r->getData());

	data->m_maxActiveStage = data->m_maxActiveStage < 2 ? 2 : data->m_maxActiveStage;   //设定使用的纹理坐标层数

	vuVec3f * color_tc2;
	if(data->m_numTextureCoords[2] != data->m_numVertices)
	{
		data->m_numTextureCoords[2] = data->m_numVertices;
		data->m_dimensionTextureCoord[2] = vrDrawFunc::DIMENSION_3;
		color_tc2 = vuAllocArray<vuVec3f>::calloc(data->m_numVertices);
		(data->m_textureCoord[2]).replace(color_tc2);
		vuAllocArray<vuVec3f>::ref(color_tc2);
	}
	else
	{
		color_tc2 = (vuVec3f*)(data->m_textureCoord[2].get());
	}
	//文理坐标单元2中第1个元素用来控制被遍历的节点在哪个部分中不显示 1-database 2-radar 4-lidar 8-ir
	//纹理坐标单元2的地2个元素用来记录radar下顶点颜色的划分 0~1
	//纹理坐标单元2的地3个元素用来记录radar下是否改变顶点 1改 0 不改
	for(size_t i = 0; i < data->m_numVertices; i++)   
	{
		size_t temp = color_tc2[i][0];
		if (!(temp & 0x02))
		{
			color_tc2[i][0] += 2.0f;
		}
	}

	return vsTraversal::RESULT_CONTINUE;
}

vsTraversal::Result classSensor::travFunc_set_lidar_texcoord(vsNode *node, int)   //修改纹理单元1为形变后的点的坐标，纹理单元2被用来记录一些参数, 纹理单元3被用来记录lida_texcoord
{
	static const double base_rate = 10000.0;  //假设这个值为10000，则是被认为在大小为 100*100 = 10000 的平面上贴当前选定大小的纹理（在travFunc_add_shader遍历函数中进行选择）是合适的，也就是模型表面面积和纹理坐标面积（1*1=1）的比值是10000/1=10000，
												//所以如果n*n大小的平面上贴整张纹理是合适的，这个值就应该是n*n
	static const double scale  = 0.075f;	//最终的纹理坐标还是要乘一个值来做一下微调

	vsGeometry *geometry_s = static_cast<vsGeometry *>(node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();

	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&geometry_r->getData());

	if (data->m_numTextureCoords[0] != data->m_numVertices)
	{
		std::cout << "travFunc_set_lidar_texcoord() : m_numTextureCoords[0] != m_numVertices ." << std::endl;
		return vsTraversal::RESULT_CONTINUE;
	}

	data->m_maxActiveStage = data->m_maxActiveStage < 3 ? 3 : data->m_maxActiveStage;   //设定使用的纹理坐标层数

	vuVec3f* coordinate_tc3;

	vuVec3f* firstTriangleModelCoord = ((data->m_vertex).get());
	vuVec2f* firstTriangleTextureCoord = (vuVec2f*)((data->m_textureCoord[0]).get());

	double area_model;
	double area_texture;
	double rate;


	//文理坐标单元3中 前两个元素记录了lidar纹理坐标 第三个值记录了节点类型
	if(data->m_numTextureCoords[3] != data->m_numVertices)
	{
		data->m_numTextureCoords[3] = data->m_numVertices;
		data->m_dimensionTextureCoord[3] = vrDrawFunc::DIMENSION_3;
		coordinate_tc3 = vuAllocArray<vuVec3f>::calloc(data->m_numVertices);
		(data->m_textureCoord[3]).replace(coordinate_tc3);
		vuAllocArray<vuVec3f>::ref(coordinate_tc3);
	}
	else
	{
		coordinate_tc3 = (vuVec3f*)(data->m_textureCoord[3].get());
	}



	bool isContinuous = false;   //记录当前的三角面是连续的还是已经发生了跳跃（若当前三角面的面积为0则说明已经发生了跳跃，若上一个三角面的面积不为零且当前三角面的面积也不为零则说明当前是连续状态）,起始条件必然是false
	for(size_t i = 2; i < data->m_numVertices; i++)   
	{
		area_model = calTriangleArea((firstTriangleModelCoord[i-2]).m_vec, (firstTriangleModelCoord[i-1]).m_vec, (firstTriangleModelCoord[i]).m_vec, 3);
		if (area_model == 0.0) 
		{
			isContinuous = false;
			continue;
		}
		area_texture = calTriangleArea((firstTriangleTextureCoord[i-2]).m_vec, (firstTriangleTextureCoord[i-1]).m_vec, (firstTriangleTextureCoord[i]).m_vec, 2);
		if (area_texture == 0.0) 
		{
			isContinuous = false;
			continue;
		}


		if (isContinuous)  //triangle strip连续时，rate不变
		{
			coordinate_tc3[i][0] = firstTriangleTextureCoord[i][0] * rate * scale;
			coordinate_tc3[i][1] = firstTriangleTextureCoord[i][1] * rate * scale;
		}
		else
		{
			rate = sqrt(area_model/area_texture/base_rate);

			coordinate_tc3[i][0] = firstTriangleTextureCoord[i][0] * rate * scale;
			coordinate_tc3[i][1] = firstTriangleTextureCoord[i][1] * rate * scale;

			coordinate_tc3[i-1][0] = firstTriangleTextureCoord[i-1][0] * rate * scale;
			coordinate_tc3[i-1][1] = firstTriangleTextureCoord[i-1][1] * rate * scale;

			coordinate_tc3[i-2][0] = firstTriangleTextureCoord[i-2][0] * rate * scale;
			coordinate_tc3[i-2][1] = firstTriangleTextureCoord[i-2][1] * rate * scale;

			isContinuous = true;
		}

	}

	return vsTraversal::RESULT_CONTINUE;
}

vsTraversal::Result classSensor::travFunc_set_model_type(vsNode *node, int)   //修改纹理单元1为形变后的点的坐标，纹理单元2被用来记录一些参数, 纹理单元3被用来记录lida_texcoord
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();

	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&geometry_r->getData());

	data->m_maxActiveStage = data->m_maxActiveStage < 3 ? 3 : data->m_maxActiveStage;   //设定使用的纹理坐标层数

	vuVec3f* coordinate_tc3;

	if(data->m_numTextureCoords[3] != data->m_numVertices)
	{
		data->m_numTextureCoords[3] = data->m_numVertices;
		data->m_dimensionTextureCoord[3] = vrDrawFunc::DIMENSION_3;
		coordinate_tc3 = vuAllocArray<vuVec3f>::calloc(data->m_numVertices);
		(data->m_textureCoord[3]).replace(coordinate_tc3);
		vuAllocArray<vuVec3f>::ref(coordinate_tc3);
	}
	else
	{
		coordinate_tc3 = (vuVec3f*)(data->m_textureCoord[3].get());
	}

	//文理坐标单元3中 前两个元素记录了lidar纹理坐标 第三个元素记录了model类型 0-unknown  1-terrain


	for(size_t i = 0; i < data->m_numVertices; i++)   
	{
		coordinate_tc3[i][2] = modelType;
	}

	return vsTraversal::RESULT_CONTINUE;
}

vsTraversal::Result classSensor::travFunc_set_database_invisible(vsNode *node, int)   //修改纹理单元1为形变后的点的坐标，纹理单元2被用来记录一些参数
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();

	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&geometry_r->getData());

	data->m_maxActiveStage = data->m_maxActiveStage < 2 ? 2 : data->m_maxActiveStage;   //设定使用的纹理坐标层数

	vuVec3f * color_tc2;
	if(data->m_numTextureCoords[2] != data->m_numVertices)
	{
		data->m_numTextureCoords[2] = data->m_numVertices;
		data->m_dimensionTextureCoord[2] = vrDrawFunc::DIMENSION_3;
		color_tc2 = vuAllocArray<vuVec3f>::calloc(data->m_numVertices);
		(data->m_textureCoord[2]).replace(color_tc2);
		vuAllocArray<vuVec3f>::ref(color_tc2);
	}
	else
	{
		color_tc2 = (vuVec3f*)(data->m_textureCoord[2].get());
	}
	//纹理坐标单元2中第1个元素用来控制被遍历的节点在哪个部分中不显示 1-database 2-radar 4-lidar 8-ir
	//纹理坐标单元2的地2个元素用来记录radar下顶点颜色的划分 0~1
	//纹理坐标单元2的地3个元素用来记录radar下是否改变顶点 1改 0 不改
	for(size_t i = 0; i < data->m_numVertices; i++)   
	{
		size_t temp = color_tc2[i][0];                                      
		if (!(temp & 0x01))
		{
			color_tc2[i][0] += 1.0f;
		}
	}

	return vsTraversal::RESULT_CONTINUE;
}

vsTraversal::Result classSensor::travFunc_deinit(vsNode *node, int)   //修改纹理单元1为形变后的点的坐标，纹理单元2被用来记录一些参数
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();

	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&geometry_r->getData());

	size_t maxActiveStage = data->m_maxActiveStage;   //使用的纹理坐标层数
	data->m_maxActiveStage = 0;

	for(size_t stage = 1; stage <= maxActiveStage; stage++)
	{
		vuVec3f* p = (vuVec3f*)((data->m_textureCoord[stage]).get());

		if(p != NULL)
		{
			vuAllocArray<vuVec3f>::free(p);
		}

		(data->m_textureCoord[stage]).replace(NULL);

		data->m_dimensionTextureCoord[stage] = vrDrawFunc::DIMENSION_2;
	}


	return vsTraversal::RESULT_CONTINUE;
}

vsTraversal::Result classSensor::travFunc_find_z(vsNode *_node, int)
{
	vsGeometry *geometry_s = static_cast<vsGeometry *>(_node);   //注意vrGeometry和vsGeometry的区别
	vrGeometry *geometry_r = geometry_s->getGeometry();
	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&geometry_r->getData());

	size_t total = data->m_numVertices;
	float dis;
	for(size_t i = 0; i < total; i++) 
	{
		dis = ((data->m_vertex.get())[i]).distance(currentCoord);
		if (dis < calDistance)
		{
			z_data.push_back((data->m_vertex.get())[i][2]);
		}
	}
	return vsTraversal::RESULT_CONTINUE;
}

void classSensor::setShaderPara(const vsChannel *_channel, vrDrawContext *_context, vuVec4d * _position)
{
	
}

float classSensor::calHeightColor(const vuVec3f& _coord)   
{
	z_data.clear();
	currentCoord = _coord;
	vsTraversalUser<int, vsTraversalLookUpNodeId> trav;
	trav.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc_find_z);
	if(NULL != pBuilding) trav.visit(pBuilding);
	if(NULL != pTerrain) trav.visit(pTerrain);

	float z = _coord[2];
	z_data.push_back(z);
	float max = *std::max_element(z_data.begin(), z_data.end());  
	float min = *std::min_element(z_data.begin(), z_data.end());

	if ((max - min) < 0.5f)
		return 0.5f;

	return (z-min)/(max-min);
}

void classSensor::handleDrawEvent(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context)
{
	if(_channel != pChannel || _event != vsChannel::EVENT_PRE_DRAW)
		return;
}

void classSensor::handleRunLoop(void)
{
	static size_t __delay = 0;
	if (( ++__delay) > 2 )
	{
		mOffset_test->set( 0.001f * rand()); 
		__delay = 0;
	}

	//mOffset_test->set(__offset > 1.0f ? (__offset = 0.001f) : (__offset += 0.001f));
	//mOffset_test->set(__offset*rand()); 

	if (isStartFound)
	{
		double __pos[3], __hpr[3];
		getPositionHprFromStart(__pos, __hpr);
		pObserver->setTranslate(__pos[0], __pos[1], __pos[2]);
		pObserver->setRotate(__hpr[0], __hpr[1], 0);

		isStartFound = false;
	}
	
}

double classSensor::calTriangleArea(float *a, float *b, float *c, size_t d)
{
	double area = -1;  

	double side[3];//三条边的长度;  
	if(d == 3)
	{
		side[0] = sqrt(pow(a[0] - b[0], 2)+pow(a[1] - b[1], 2) + pow(a[2] - b[2], 2));   
		side[1] = sqrt(pow(a[0] - c[0], 2)+pow(a[1] - c[1], 2) + pow(a[2] - c[2], 2));  
		side[2] = sqrt(pow(c[0] - b[0], 2)+pow(c[1] - b[1], 2) + pow(c[2] - b[2], 2));   
	}
	if(d == 2)
	{
		side[0] = sqrt(pow(a[0] - b[0], 2)+pow(a[1] - b[1], 2));   
		side[1] = sqrt(pow(a[0] - c[0], 2)+pow(a[1] - c[1], 2));  
		side[2] = sqrt(pow(c[0] - b[0], 2)+pow(c[1] - b[1], 2));   
	}
	if(side[0] == 0.0 || side[1] == 0.0 || side[2] == 0.0)
		return 0.0;


	//不能构成三角形;  
// 	if(side[0]+side[1]<=side[2] || side[0]+side[2]<=side[1] || side[1]+side[2]<=side[0])
// 		return -2;   

	//利用海伦公式。s=sqrt(p*(p-a)(p-b)(p-c));   
	double p = (side[0]+side[1]+side[2])/2; //半周长;  
	area = sqrt(p*(p-side[0])*(p-side[1])*(p-side[2])); 
  
	return area;  
}


