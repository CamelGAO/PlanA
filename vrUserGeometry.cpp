#include "stdafx.h"
#include "vrUserGeometry.h"

VUBASE_SOURCE_INCLUDES_COMPOSITE(vrUserGeometry, vrGeometryBase,
                                 new vrUserGeometry, true, false)

///////////////////////////////////////////////////////////////////////////
//
//  constructor / public
//

vrUserGeometry::vrUserGeometry( int num )     //注意：在使用这个类的时候一定要vrUserGeometry::initializeClass()，并在程序最终结束时执行vrUserGeometry::shutdownClass()。这两个函数最好在main函数里执行，没有执行或者执行的位置（时机）不对将导致构造
												//函数或析构函数中的registerInstance或unregisterInstance方法崩溃，原因是在这种情形下，s_classType是无效的指针NULL。
{
    // our geometry class will use line primitives
    setPrimitive(vrGeometryBase::PRIMITIVE_LINE);

    // if the user gave us a valid number of lines, preallocate the array
    if( num > 0 )
    {
        vuVec3<float> *vertex = vuAllocArray<vuVec3<float> >::malloc(2 * num);
        setVertices(vertex);
        setNumPrimitives(num);
    }

    // create the color attribute array.  We'll use an overall color for all
    // of the lines and set the default color to white
    vuVec4<float> *color = vuAllocArray<vuVec4<float> >::malloc(1);
    color[0].set(1.0f, 1.0f, 1.0f, 1.0f);
    setColors(color, vrGeometryBase::BINDING_OVERALL);

    // register the instance w/ the system
    registerInstance(s_classType);
}

///////////////////////////////////////////////////////////////////////////
//
//  destructor / protected
//

vrUserGeometry::~vrUserGeometry()
{
    // unregister the instance w/ the system
    unregisterInstance(s_classType);
}

vrGeometryBase *vrUserGeometry::makeCopy(const vsgu::Options& options) const
{
    return vsgu::makeCopy(this,options);
}

///////////////////////////////////////////////////////////////////////////
//
//  setLineSegment / public
//

int vrUserGeometry::setLineSegment( int index, const vuVec3<float> &begin,
                                    const vuVec3<float> &end ) 
{
    // make sure we've got a valid index
    int numLines = getNumPrimitives();
    VUNOTIFY_CHECK_IF(index > numLines, (vuNotify::LEVEL_WARN, s_classType,
        "vrUserGeometry::setEndPoints - invalid index"), vsgu::FAILURE);

    // if necessary, allocate / reallocate the vertex array and increment the
    // number of primitives 
    vuVec3<float> *vertex = getVertices();

    if( index == numLines )
    {
        int numVertices = 2 * (numLines + 1);

        if( vertex == NULL )
            vertex = vuAllocArray<vuVec3<float> >::malloc(numVertices);
        else 
            vertex = vuAllocArray<vuVec3<float> >::realloc(vertex, numVertices);

        setNumPrimitives(numLines + 1);
        setVertices(vertex);
    }

    vertex[2*index]   = begin;
    vertex[2*index+1] = end;

    // Reset the vertex array on the geometry so it "knows" that it's been
    // changed
    setVertices( vertex );

    return vsgu::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
//  getLineSegment / public
//

int vrUserGeometry::getLineSegment( int index, vuVec3<float> *begin,
                                    vuVec3<float> *end ) const
{
    // Make sure we've got a valid index
    int numLines = getNumPrimitives();
    VUNOTIFY_CHECK_IF(index >= numLines, (vuNotify::LEVEL_WARN, s_classType,
        "vrUserGeometry::getEndPoints - invalid index"), vsgu::FAILURE);

    vuVec3<float> *vertex = getVertices();
    *begin = vertex[2*index];
    *end   = vertex[2*index+1];

    return vsgu::SUCCESS;
}

float* vrUserGeometry::getValue(int _index, int _pos, int _axis)
{
	int numLines = getNumPrimitives();
	if(_index >= numLines)
	{
		std::cout << "vrUserGeometry::getValue(): Invalid index:" << _index << std::endl;
		return NULL;
	}
	if (_pos != 0 && _pos != 1)
	{
		std::cout << "vrUserGeometry::getValue(): Invalid pos:" << _pos << std::endl;
		return NULL;
	}
	if (_axis != 0 && _axis != 1 && _axis != 2)
	{
		std::cout << "vrUserGeometry::getValue(): Invalid axis:" << _axis << std::endl;
		return NULL;
	}

	vuVec3f* vertex = getVertices();
	float* ret =&(((vertex[2*_index+_pos]).m_vec)[_axis]);
	return ret;
}

int vrUserGeometry::setNumPrimitives(int num)
{
	return vrGeometryBase::setNumPrimitives(num);
}

///////////////////////////////////////////////////////////////////////////
//
//  setColor / public
//

int vrUserGeometry::setColor(float r, float g, float b, float a) 
{
    // Make sure we've got a valid color
    VUNOTIFY_CHECK_COLOR(r, g, b, a, (vuNotify::LEVEL_WARN, s_classType,
        "vrUserGeometry::setColor - invalid color"), vsgu::FAILURE);

    // Update the entry in the color array and then reset the array on the
    // geometry so it "knows" that it's been changed
    vrGeometryBase::Binding binding;
    vuVec4<float> *color = getColors( &binding );
    color[0].set(r, g, b, a);
    setColors( color, vrGeometryBase::BINDING_OVERALL );

    return vsgu::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////
//
//  getColor / public
//

void vrUserGeometry::getColor(float *r, float *g, float *b, float *a) const
{
    vrGeometryBase::Binding binding;
    vuVec4<float> *color = getColors( &binding );
    color[0].get(r, g, b, a);
}
