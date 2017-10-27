#include "stdafx.h"
#include "Common.h"

classCommon::classEnvInfo classCommon::envInfo;
classCommon::classSensorInfo classCommon::sensorInfo;

classCommon::classEnvInfo::classEnvInfo()
{
	time = 12;
	accDensity = 0;
	dustDensity = 0;
	fogVisibility = 80000.0f;
	fogColor = 1.0f;
	rainLevel = 0;
	snowLevel = 0;
}

classCommon::classEnvInfo::~classEnvInfo()
{
}

classCommon::classSensorInfo::classSensorInfo()
{
}

classCommon::classSensorInfo::~classSensorInfo()
{
}



classCommon::classCommon()
{
}

classCommon::~classCommon()
{
}

