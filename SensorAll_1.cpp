// DVE_1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "DVE.h"
#include "ModelDeployer.h"
#include "Sensor.h"
#include "Geometry.h"

//vuAllocTracer tracer;

class myApp: public vpApp, public vsChannel::Subscriber
{
public:
	static bool restart;

	myApp()
	{
		pDve = NULL;
		pSensor = NULL;
		pGeometry = NULL;
	}

	~myApp()
	{
		if (pDve != NULL)
		{
			delete pDve;
			pDve = NULL;
		}
		if (pSensor != NULL)
		{
			delete pSensor;
			pSensor = NULL;
		}
		if (pGeometry != NULL)
		{
			delete pGeometry;
			pGeometry = NULL;
		}
	}

	virtual int configure(void)
	{
		int ret = vpApp::configure();

		isLoaded = loadModel();

		if (!isLoaded)
		{
			return ret;
		}
		else
		{
			classShaderDeployer::instance()->loadAllShader();

			pDve = new classDve("myObserver",this);
			pSensor = new classSensor("sensorObserver",this);
			pGeometry = new classGeometry("sensorObserver",this);
		}

		return ret;
	}

	virtual int unconfigure(void)
	{
		int ret = vpApp::unconfigure();
		removeModel();

		classShaderDeployer::instance()->removeAllShader();


		return ret;
	}


	virtual void onKeyInput(vrWindow::Key _key, int _mod)
	{
		switch(_key)
		{
		case vrWindow::KEY_ESCAPE:
			restart = false;
			breakFrameLoop();
			break;
		case vrWindow::KEY_s:
			vpApp::onKeyInput(_key,_mod);
			break;
		case vrWindow::KEY_F12:
			restart = true;
			breakFrameLoop();
			break;
		default:
			classBase::traversalKey(_key,_mod); 
		}
	}

	virtual void notify(vsChannel::Event, const vsChannel *,vsTraversalCull *) {}

	virtual void notify(vsChannel::Event _event, const vsChannel *_channel, vrDrawContext *_context) 
	{
		classBase::traversalDrawEvent(_event, _channel, _context);
	}

	bool loadModel(void);
	void removeModel(void);

	virtual void run(void) 
	{
		while( beginFrame() != 0) 
		{
			endFrame();
			classBase::traversalRunLoop();

			if (!isLoaded)
			{	
				breakFrameLoop();
			}
		}
		unconfigure();
	}

protected:
private:
	classDve *pDve;

	classSensor *pSensor;

	classGeometry* pGeometry;

	bool isLoaded;  //true ģ���Ѿ����� false ģ�ͼ��ر�ȡ��
};

bool myApp::restart = true;

int _tmain(int argc, _TCHAR* argv[])
{


	while(myApp::restart)
	{
		myApp::restart = false;

		// initialize Vega Prime
		vp::initialize(argc, argv);
		vrUserGeometry::initializeClass();   //�������Ҫִ�У�����s_classtypeΪNULL��ͬʱ���������׵�shutdownClass()��������

 		// create my app instance
 		myApp *app = new myApp;
	 
 		// use the code generated by LynX Prime
 		app->define("E:\\newstart\\Data_ACF\\SensorAll_1.acf");
	 
 		// configure my app instance
 		app->configure();
	 
 		// execute the main runtime loop
 		app->run();
	 
 		// delete my app instance
 		app->unref();

		vrUserGeometry::shutdownClass();//��shutdownClass()��������
		// shutdown Vega Prime
		vp::shutdown();

	}



	//system("pause");

	return 0;
}

bool myApp::loadModel(void)
{
	static char __full_name[MAX_PATH];
	static char __directory[MAX_PATH];
	static char __file_name[MAX_PATH];

	vpScene *__scene = vpScene::find("myScene");

	if (getPath(__full_name) > 0)
	{
		getDirectoryAndFileNameFromFullPath(__directory, __file_name, __full_name);
	}
	else
		return false;

	vpSearchPath *__search_path = vpSearchPath::find("mySearchPath");
	__search_path->append(__directory);

	vpObject * _object = vpObject::find("myObject");
	if (_object != NULL)
	{std::cout <<  "load new model : last model is still existing!" << std::endl;system("pause");}

	_object = new vpObject();
	_object->setName( "myObject" );
	_object->setCullMask( 0x0FFFFFFFF );
	_object->setRenderMask( 0x0FFFFFFFF );
	_object->setIsectMask( 0x0FFFFFFFF );
	_object->setStrategyEnable( false );
	_object->setTranslate( 0.000000 ,  0.000000 ,  0.000000 );
	_object->setRotate( 0.000000 ,  0.000000 ,  0.000000 );
	_object->setScale( 1.000000 ,  1.000000 ,  1.000000 );
	_object->setStaticEnable( false );
	_object->setFileName(__file_name);
	_object->setAutoPage( vpObject::AUTO_PAGE_SYNCHRONOUS );
	_object->setManualLODChild( -1 );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_COMBINE_LIGHT_POINTS ,  true );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_COMBINE_LODS ,  true );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_IGNORE_DOF_CONSTRAINTS ,  false );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_PRESERVE_EXTERNAL_REF_FLAGS ,  true );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_PRESERVE_GENERIC_NAMES ,  true );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_PRESERVE_GENERIC_NODES ,  true );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_PRESERVE_QUADS ,  false );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_ALL_GEOMETRIES_LIT ,  true );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_USE_MATERIAL_DIFFUSE_COLOR ,  false );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_MONOCHROME ,  false );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_CREATE_ANIMATIONS ,  true );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_SHARE_LIGHT_POINT_CONTROLS ,  true );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_SHARE_LIGHT_POINT_ANIMATIONS ,  true );
	_object->setLoaderOption( vsNodeLoader::Data::LOADER_OPTION_SHARE_LIGHT_POINT_APPEARANCES ,  true );
	_object->setLoaderDetailMultiTextureStage( -1 );
	_object->setLoaderBlendTolerance( 0.050000f );
	_object->setLoaderUnits( vsNodeLoader::Data::LOADER_UNITS_METERS );
	_object->setBuilderOption( vsNodeLoader::Data::BUILDER_OPTION_OPTIMIZE_GEOMETRIES ,  true );
	_object->setBuilderOption( vsNodeLoader::Data::BUILDER_OPTION_COLLAPSE_BINDINGS ,  true );
	_object->setBuilderOption( vsNodeLoader::Data::BUILDER_OPTION_COLLAPSE_TRIANGLE_STRIPS ,  true );
	_object->setBuilderNormalMode( vsNodeLoader::Data::BUILDER_NORMAL_MODE_PRESERVE );
	_object->setBuilderColorTolerance( 0.001000f );
	_object->setBuilderNormalTolerance( 0.860000f );
	_object->setBuilderVertexTolerance( 0.000100f );
	_object->setGeometryOption( vsNodeLoader::Data::GEOMETRY_OPTION_GENERATE_DISPLAY_LISTS ,  true );
	_object->setGeometryFormat( vrGeometryBase::FORMAT_VERTEX_ARRAY ,  0x0FFF );
	_object->setPostLoadOption( vpGeometryPageable::POST_LOAD_OPTION_FLATTEN ,  true );
	_object->setPostLoadOption( vpGeometryPageable::POST_LOAD_OPTION_CLEAN ,  true );
	_object->setPostLoadOption( vpGeometryPageable::POST_LOAD_OPTION_MERGE_GEOMETRIES ,  true );
	_object->setPostLoadOption( vpGeometryPageable::POST_LOAD_OPTION_COLLAPSE_BINDINGS ,  true );
	_object->setPostLoadOption( vpGeometryPageable::POST_LOAD_OPTION_COLLAPSE_TRIANGLE_STRIPS ,  true );
	_object->setPostLoadOption( vpGeometryPageable::POST_LOAD_OPTION_VALIDATE ,  true );
	_object->setTextureSubloadEnable( false );
	_object->setTextureSubloadRender( vpGeometry::TEXTURE_SUBLOAD_RENDER_DEFERRED );

	_object->ref();

	__scene->addChild(_object);

	return true;
}

void myApp::removeModel(void)
{
	vpObject * _object = vpObject::find("myObject");
	if (_object == NULL)
	{
		std::cout <<  "removeModel() : model is not existing!" << std::endl; 
		return;
	}

	vpScene *__scene = vpScene::find("myScene");

	__scene->removeChild(_object);

	//cout << _object->getRef() << endl;

	_object->unref();
}