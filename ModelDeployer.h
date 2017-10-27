#ifndef _MODELDEPLOYER_H
#define _MODELDEPLOYER_H

#include "stdafx.h"

int getPath(TCHAR * _buffer, size_t _buffer_length = MAX_PATH);
void getDirectoryAndFileNameFromFullPath(TCHAR * _directory_buffer, 
										TCHAR * _file_name_buffer, 
										const TCHAR * const _fullpath, 
										size_t _directory_buffer_length = MAX_PATH, 
										size_t _file_name_buffer_length = MAX_PATH
										);

vpObject* getModel(std::string _fullpath);
void changeModel(std::string _last_node_name, vsNode *_new_node);
void changeModel(vsNode * _old_node, vsNode *_new_node, vpScene * _scene);
void wcs2mbs(LPCOLESTR _wcstr, char* const _result, size_t _result_max_size = MAX_PATH);

#endif
