#include "stdafx.h"
#include "Common.h"

classEnvInfo classCommon::envInfo;
classSensorInfo classCommon::sensorInfo;

classEnvInfo::classEnvInfo()
{
	time = 12;
	accDensity = 0;
	dustDensity = 0;
	fogVisibility = 80000.0f;
	fogColor = 1.0f;
	rainLevel = 0;
	snowLevel = 0;
}
classEnvInfo::~classEnvInfo()
{
}


classSensorInfo::classSensorInfo()
{
}
classSensorInfo::~classSensorInfo()
{
}


classCommon::classCommon()
{
}

classCommon::~classCommon()
{
}

