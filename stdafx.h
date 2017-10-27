// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#define WINVER 0x0501

#include <iostream>
#include <tchar.h>

// TODO: 在此处引用程序要求的附加头文件
#include <afx.h>  
#include <afxdlgs.h>  
#include <windows.h>  
#include <limits.h>


#include <cstring>
#include <vector>
#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <string>
#include <string.h>

#include "cg/cg.h"

#include "vpApp.h"

#include "vrWindow.h"
#include "vpChannel.h"
#include "vpEnv.h"
#include "vsChannel.h"
#include "vrMode.h"
#include "vrBox.h"
#include "vpModule.h"
#include "vpIsectorHAT.h"
#include "vrShader.h"
#include "vpObject.h"
#include "vrShaderFactory.h"
#include "vpFxParticleSystem.h"
#include "vsNode.h"
#include "vpSearchPath.h"
#include "vuVec3.h"
#include "vrGeometryBase.h"
#include "vuMath.h"
#include "vpGLStudioComponent.h"
#include "vrRenderStrategy.h"
#include "vsTraversalUser.h"
#include "vuFunctor.h"
#include "vrLight.h"
#include "vsLOD.h"
#include "vsAnimation.h"
#include "vsDOF.h"
#include "vsSwitch.h"
#include "vsDecal.h"
#include "vuAllocArray.h"
#include "vrFont.h"
#include "vuString.h"
#include "vuAllocTracer.h"
#include "vsgu.h"

#include <gl/GLAux.h>
#include <gl/glut.h>
#include <time.h>
#include <math.h>


#include "TinyXML2.h"

using namespace std;