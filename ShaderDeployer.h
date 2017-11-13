#ifndef _SHADERDEPLOYER_H
#define _SHADERDEPLOYER_H

#include "stdafx.h"

class classShaderDeployer
{
public:
	enum ShaderType
	{
		VERTEX,
		FRAGMENT
	};

	static classShaderDeployer *instance();
	static void destory();
	void init(void);
	vrShader *loadShader(std::string _name, ShaderType _type, const char *_path);
	void loadAllShader(void);
	void removeAllShader(void);
	static vrShader *getShader(std::string _name);
	void removeShader(std::string _name);

	void addRepresentation(std::string _name);
	int getRepresentation(std::string _name);
	void removeRepresentation(std::string _name);

protected:
	~classShaderDeployer();
private:
	classShaderDeployer();
	static classShaderDeployer *pInstance;

	static std::map<std::string, vrShader *> shaderList;
	static std::map<std::string, int> representationList;
};



#endif
