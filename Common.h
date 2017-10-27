#ifndef _COMMON_H
#define _COMMON_H

#include "stdafx.h"

class classCommon
{
public:

	enum enumDve
	{
		Time,
		Fog,
		Sandstorm,
		AccSnow,
		Rain,
		Snow,
		Dust,
		Nothing
	};

	class classEnvInfo
	{
	public:
		classEnvInfo();
		~classEnvInfo();

		float time;
		float fogVisibility;
		float fogColor;
		float accDensity;
		float rainLevel;
		float snowLevel;
		float dustDensity;

	protected:
	private:
	};

	class classSensorInfo
	{
	public:
		classSensorInfo();
		~classSensorInfo();

	protected:
	private:
	};

	classCommon();
	~classCommon();

	static classEnvInfo envInfo;
	static classSensorInfo sensorInfo;
protected:
private:
};

#endif
