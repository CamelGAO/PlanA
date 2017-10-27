#include "stdafx.h"
#include "Drawer.h"

std::vector<size_t>  classId::_pool;

size_t classId::getId(void)
{
	static size_t num = 0;
	size_t temp;
	if(_pool.end() == _pool.begin())   //判断是否有曾经分配的id被释放，无则新产生，有则用之
		temp = num++;
	else
	{
		temp = _pool.back();
		_pool.pop_back();
	}

	return temp;
}

void classId::returnId(size_t _id)  //id被释放，放入被释放id的id池；
{
	_pool.push_back(_id);
}
