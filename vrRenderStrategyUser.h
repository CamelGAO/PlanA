#ifndef _VRRENDERSTRATEGYUSER_H
#define _VRRENDERSTRATEGYUSER_H

#include "stdafx.h"

class classRenderStrategySetUniform : public vrRenderStrategy
{
public:
	classRenderStrategySetUniform(vsNode *_channel);
	virtual void draw(vrDrawContext *_context, const vrDrawFunc::Data &_data);
protected:
private:
	vsNode *pChannel;
	vuField<vuParameterTyped<vuVec3f>*, vuFieldTraitMemBase> mVertexMin_test;
	vuField<vuParameterTyped<vuVec3f>*, vuFieldTraitMemBase> mVertexMax_test;

	VUMEMBASE_HEADER_INCLUDES(classRenderStrategySetUniform)
};


#endif
