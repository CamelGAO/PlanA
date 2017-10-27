#ifndef _DRAWER_H
#define _DRAWER_H

#include "stdafx.h"

class classId    //产生全局唯一的id
{
public:
	classId();
	~classId();

	static std::vector<size_t> _pool;

	static size_t getId(void);

	static void returnId(size_t _id);

protected:
private:
};


template<typename T>
class classDrawer
{
public:

	struct _box
	{
		size_t _id;
		size_t _ref_count;
		typename T _data;
	};

	typedef typename std::vector<_box> drawer_data_type;
	typedef typename std::vector<_box>::iterator drawer_data_iterator; 

	classDrawer();
	~classDrawer();
	static drawer_data_type data;    //保存所有T类型的实例

	static size_t add(const T &_somehing, drawer_data_type &_drawer_data = data, bool _idSingle = false);
	static void erase(size_t _id, drawer_data_type &_drawer_data = data);

protected:
private:

};

template<typename T>
typename classDrawer<T>::drawer_data_type typename classDrawer<T>::data;



template<typename T>  
size_t typename classDrawer<T>::add(const T &_somehing, typename classDrawer<T>::drawer_data_type &_drawer_data, bool _isSingle)
{
	typename classDrawer<T>::_box _a_box;
	if (_isSingle == false)
	{

		_a_box._id = classId::getId();
		_a_box._ref_count = 1;
		_a_box._data = _somehing;
		_drawer_data.push_back(_a_box);

		return _a_box._id;
	}
	else
	{
		typename classDrawer<T>::drawer_data_iterator ib = _drawer_data.begin();
		typename classDrawer<T>::drawer_data_iterator ie = _drawer_data.end();
		while (ib != ie)
		{
			if ((*ib)._data == _somehing)
			{
				(*ib)._ref_count++;
				//std::cout << "_ref_count++" << std::endl;
				return (*ib)._id;
			}
			ib++;
		}

		typename classDrawer<T>::_box _a_box;
		_a_box._id = classId::getId();
		_a_box._ref_count = 1;
		_a_box._data = _somehing;
		_drawer_data.push_back(_a_box);

		return _a_box._id;
	}
}

template<typename T>
void typename classDrawer<T>::erase(size_t _id, typename classDrawer<T>::drawer_data_type &_drawer_data)
{
	typename classDrawer<T>::drawer_data_iterator ib = _drawer_data.begin();
	typename classDrawer<T>::drawer_data_iterator ie = _drawer_data.end();
	while (ib != ie)
	{
		if ((*ib)._id == _id)
		{
			if ((*ib)._ref_count != 0)
			{
				(*ib)._ref_count--;
				if((*ib)._ref_count == 0)
				{
					_drawer_data.erase(ib);
					classId::returnId(_id);
				}
				return;
			}
			else
			{
				std::cout << "classDrawer: erase(): _ref_count error!" <<std::endl;
				return;
			}

		}
		ib++;
	}

	std::cout << "Can't find ID: " << (unsigned int)_id <<std::endl;
}

#endif
