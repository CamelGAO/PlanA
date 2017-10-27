#include "stdafx.h"
#include "ShaderDeployer.h"

classShaderDeployer * classShaderDeployer::pShaderDeployer = new classShaderDeployer();
std::map<std::string, vrShader *> classShaderDeployer::shaderList;
std::map<std::string, int> classShaderDeployer::representationList;

void classShaderDeployer::loadAllShader(void)
{
	loadShader("v_accsnow", VERTEX, "E:\\newstart\\Data_CG\\SnowVertexShader1.cg");
	loadShader("f_accsnow", FRAGMENT, "E:\\newstart\\Data_CG\\SnowFragmentShader1.cg");

	loadShader("v_sensor_terrain", VERTEX, "E:\\newstart\\Data_CG\\SensorVertexShader1.cg");
	loadShader("f_sensor_terrain", FRAGMENT, "E:\\newstart\\Data_CG\\SensorFragmentShader1.cg");

	loadShader("v_sensor_model", VERTEX, "E:\\newstart\\Data_CG\\SensorVertexShader3.cg");
	loadShader("f_sensor_model", FRAGMENT, "E:\\newstart\\Data_CG\\SensorFragmentShader3.cg");
}

void classShaderDeployer::removeAllShader(void)
{
	std::map<std::string, vrShader *>::iterator it = shaderList.begin();
	for (;it != shaderList.end(); it++)
	{
		//cout << (*it).second->getRef() << endl;
		(*it).second->unref();
		shaderList.erase(it);
	}
}

classShaderDeployer::classShaderDeployer(void)
{

}

classShaderDeployer::~classShaderDeployer(void)
{

}

classShaderDeployer* classShaderDeployer::instance(void)
{
	return pShaderDeployer;
}
 
vrShader *classShaderDeployer::loadShader(std::string _name, ShaderType _type, const char *_path)
{
	vrShader *__shader = NULL;
	vrShaderFactory* __shaderFactory = new vrShaderFactory();
	switch(_type)
	{
	case VERTEX:
		__shader = __shaderFactory->read(_path, vrShader::TYPE_VERTEX);
		__shader->ref();		//从shaderfactory中出来引用数为0，需要ref
		__shader->setHardwareProfile(CG_PROFILE_VP40);
		break;
	case FRAGMENT:
		__shader = __shaderFactory->read(_path, vrShader::TYPE_FRAGMENT);
		__shader->ref();		//从shaderfactory中出来引用数为0，需要ref
		__shader->setHardwareProfile(CG_PROFILE_FP40);
		break;
	default:
		std::cout << "classShaderDeployer: addShader(): Unknown shader type!" << std::endl;
		break;
	}
	__shaderFactory->unref();

	//std::cout << __shader->getHardwareProfile() << std::endl;

	std::map<std::string, vrShader *>::iterator it;
	it = shaderList.find(_name);
	if (it != shaderList.end())
	{
		it->second->unref();
		shaderList.erase(it);
		std::cout << "ShaderDeployer:" << _name << " already exists and will be replaced! " << std::endl;
	}

	shaderList[_name] = __shader;

	return __shader;
}


void classShaderDeployer::removeShader(std::string _name)
{
	std::map<std::string, vrShader *>::iterator it;
	it = shaderList.find(_name);
	if (it != shaderList.end())
	{
		it->second->unref();
		shaderList.erase(it);
		return;
	}
	std::cout << "ShaderDeployer:" << _name << " doesn't exist! Fail to delete it!" << std::endl;
}

vrShader *classShaderDeployer::getShader(std::string _name)
{
	std::map<std::string, vrShader *>::iterator it;
	it = shaderList.find(_name);
	if (it != shaderList.end())
		return it->second;

	std::cout << "ShaderDeployer:" << _name << " doesn't exist! Fail to get it!" << std::endl;
	return NULL;
}

void classShaderDeployer::addRepresentation(std::string _name)
{
	std::map<std::string, int>::iterator it;

	it = representationList.find(_name);
	if (it != representationList.end())
	{
		std::cout << "ShaderDeployer:" << _name << " already exists! " << std::endl;
	}
	else
	{
		int __representation = 0;
		if(vsChannel::registerRepresentation(_name.c_str()) == vsgu::FAILURE)
			std::cout << "ShaderDeployer:" <<_name << ": registerRepresentation() failed! " << std::endl;
		__representation = vsChannel::getRepresentationIndex(_name.c_str());
		representationList[_name] = __representation;
	}
}

void classShaderDeployer::removeRepresentation(std::string _name)
{
	std::map<std::string, int>::iterator it;
	it = representationList.find(_name);
	if (it != representationList.end())
	{
		vsChannel::unregisterRepresentation(_name.c_str());
		representationList.erase(it);
		return;
	}
	std::cout << "ShaderDeployer:" << _name << " doesn't exist! Fail to delete it!" << std::endl;
}

int classShaderDeployer::getRepresentation(std::string _name)
{
	std::map<std::string, int>::iterator it;
	it = representationList.find(_name);
	if (it != representationList.end())
		return it->second;

	std::cout << "ShaderDeployer:" << _name << " doesn't exist! Fail to get it!" << std::endl;
	return 0;
}