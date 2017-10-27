#include "stdafx.h"
#include "Drawer.h"

std::vector<size_t>  classId::_pool;

size_t classId::getId(void)
{
	static size_t num = 0;
	size_t temp;
	if(_pool.end() == _pool.begin())   //�ж��Ƿ������������id���ͷţ������²�����������֮
		temp = num++;
	else
	{
		temp = _pool.back();
		_pool.pop_back();
	}

	return temp;
}

void classId::returnId(size_t _id)  //id���ͷţ����뱻�ͷ�id��id�أ�
{
	_pool.push_back(_id);
}
