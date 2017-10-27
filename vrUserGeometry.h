#ifndef _VRUSERGEOMETRY_H
#define _VRUSERGEOMETRY_H

#include "stdafx.h"

/**
 * The vrUserGeometry class is a specialization of vrGeometryBase which is
 * used to render a line.
 */
class vrUserGeometry : public vrGeometryBase {

public:

    /**
     * constructor
     */
    explicit vrUserGeometry(int num = 0);

    /**
     * set / get end points 
     */
    int setLineSegment(int index, const vuVec3<float> &begin,
                       const vuVec3<float> &end);

    int getLineSegment(int index, vuVec3<float> *begin,
                       vuVec3<float> *end) const;

    /**
     * set / get the color
     */
    int setColor(float r, float g, float b, float a);

    void getColor(float *r, float *g, float *b, float *a) const;

	float* getValue(int _index, int _pos, int _axis); //_index是线段的索引，_pos表示begin（0）还是end（1）,_axis表示选择哪个轴0-x，1-y， 2-z； 返回值就是被选定的值的指针
	int setNumPrimitives(int num);

    /**
     * returns an instance which is a copy of this instance.  This allows
     * vsGeometryBase to operate on all vrGeometryBase derived instances
     * generically.  Without this, all vrGeometryBase derived classes would be
     * forced to create a corresponding vsGeometryBase derived node in order
     * to add the geometry to the scene graph.
     */
    virtual vrGeometryBase *makeCopy(const vsgu::Options& options) const;

protected:

    VUBASE_HEADER_INCLUDES_COMPOSITE(vrUserGeometry)

};
namespace vsgu {

    template<>
    inline int copy<vrUserGeometry>(vrUserGeometry* dst, 
                        const vrUserGeometry* src, const vsgu::Options& options)
    {
        int ret = vsgu::SUCCESS;

        if(dst == src) return vsgu::SUCCESS;
        if(dst == NULL || src == NULL) return vsgu::FAILURE;
        
        if (!(options[vuBase::getOptionIndex()] & 
              vuBase::OPTION_IGNORE_BASE_CLASS))
        {
            ret = vsgu::copy<vrGeometryBase>(dst,src, options);
            if(ret != vsgu::SUCCESS) return ret;
        }

        return (vsgu::SUCCESS);
    }

    template<>
    inline bool compare<vrUserGeometry>(const vrUserGeometry* s1, 
                            const vrUserGeometry* s2, const vsgu::Options& options)
    {
        if(s1 == s2) return true;
        if(s1 == NULL || s2 == NULL) return false;
 
        bool bRes = true;
        if (!(options[vuBase::getOptionIndex()] & 
              vuBase::OPTION_IGNORE_BASE_CLASS))
        {
            bRes = vsgu::compare<vrGeometryBase>(s1,s2, options);
            if(!bRes) return bRes;
        }

        return true;
    }
}//namespace vsgu

#endif
