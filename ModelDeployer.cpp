#include "stdafx.h"
#include "ModelDeployer.h"
#include "DVE.h"

int getPath(TCHAR * _buffer, size_t _buffer_length)
{
	TCHAR __szFilter[] = TEXT("OpenFlight Files (*.flt)|*.flt||");
	TCHAR __inSeqence[MAX_PATH];

	AfxSetResourceHandle(GetModuleHandle(NULL));
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, __szFilter);
	if(dlg.DoModal() != IDOK)
	{std::cout << "getPath() : Cancel!" << std::endl; return -2;}

	wsprintf( __inSeqence, TEXT("%s"), dlg.GetPathName() );

	size_t __wLen = lstrlen(__inSeqence) + 1;

	if (__wLen > _buffer_length)
	{std::cout << "getPath() : File name is too long!" << std::endl; system("pause"); return -1;}

	_tcscpy(_buffer, __inSeqence);

	return	lstrlen(__inSeqence);
}

void getDirectoryAndFileNameFromFullPath(TCHAR * const _directory_buffer, TCHAR * const _file_name_buffer, const TCHAR * const _fullpath, size_t _directory_buffer_length, size_t _file_name_buffer_length)
{
	TCHAR __drive[MAX_PATH] = {0};
	TCHAR __dir[MAX_PATH] = {0};
	TCHAR __fileName[MAX_PATH] = {0};
	TCHAR __ext[MAX_PATH] = {0};

	_tsplitpath(_fullpath, __drive, __dir, __fileName, __ext);

	if((lstrlen(__drive) + lstrlen(__dir)) < MAX_PATH)
		_tcscat(__drive, __dir);
	else
	{std::cout << "getDirectoryAndFileNameFromFullPath() : Directory is too long!" << std::endl; system("pause");return;}

	if((lstrlen(__fileName) + lstrlen(__ext)) < MAX_PATH)
		_tcscat(__fileName, __ext);
	else
	{std::cout << "getDirectoryAndFileNameFromFullPath() : File name is too long!" << std::endl; system("pause");return;}

	_tcscpy(_directory_buffer, __drive);
	_tcscpy(_file_name_buffer, __fileName);

}

