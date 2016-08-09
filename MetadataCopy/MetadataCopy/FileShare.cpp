///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����FileShare.cpp
// �����ߣ��ܺ���
// ����ʱ�䣺2001/06/28
// ����������������ļ�����
///////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "FileShare.h"
///////////////////////////////////////////////////////////////////////////////////////////
// ��������g_CreateDirectory
// ��д�ߣ��ܺ���
// �ο����ϣ�
// ��  �ܣ�����Ŀ¼
// ���������
//		LPCTSTR lpszPathName:Ҫ������Ŀ¼��
// ��������������ɹ���Ŀ¼�Ѵ��ڷ���TRUE�����򷵻�FALSE
// ��  ע��
///////////////////////////////////////////////////////////////////////////////////////////
BOOL g_CreateDirectory(LPCTSTR lpszDirectory)
{
	if(!::CreateDirectory(lpszDirectory, NULL))
	{
		DWORD dwErr = ::GetLastError();
		if(dwErr != ERROR_ALREADY_EXISTS)
		{
			CString strErrMsg;
			strErrMsg.Format(_T("����·��%s����!����ԭ��:\n"), lpszDirectory);
			strErrMsg += g_GetWin32ErrMsg();
			AfxMessageBox(strErrMsg);
			return FALSE;
		}
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////
// ��������g_EmptyDirectory
// ��д�ߣ��ܺ���
// �ο����ϣ�
// ��  �ܣ����Ŀ¼
// ���������
//		LPCTSTR lpszPathName:����յ�Ŀ¼��
// �����������ճɹ�����TRUE�����򷵻�FALSE
// ��  ע��
///////////////////////////////////////////////////////////////////////////////////////////
BOOL g_EmptyDirectory(LPCTSTR lpszDirectory)
{
	CString strFindFile = lpszDirectory;
	strFindFile += _T("\\*.*");
	WIN32_FIND_DATA finddata;
	HANDLE hFind = ::FindFirstFile(strFindFile, &finddata);
	BOOL bFind = (hFind != INVALID_HANDLE_VALUE);
	CString strFileName;
	BOOL bSuccess = TRUE;
	while(bFind)
	{
		if((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
		{
			strFileName.Format(_T("%s\\%s"), lpszDirectory, finddata.cFileName);
			if(!(bSuccess = ::DeleteFile(strFileName)))
			{
				CString strMsg;
				strMsg.Format(_T("ɾ���ļ�[%s]ʱ����!\n����ԭ��:%s"),
					strFileName, g_GetWin32ErrMsg());
				AfxMessageBox(strMsg);
				break;
			}
		}
		bFind = ::FindNextFile(hFind, &finddata);
	}
	::FindClose(hFind);
	return bSuccess;
}

///////////////////////////////////////////////////////////////////////////////////////////
// ��������g_RemovePathFromFileName
// ��д�ߣ��ܺ���
// �ο����ϣ�
// ��  �ܣ��õ�����·�����ļ���
// ���������
//		LPCTSTR lpszPathName:��·�����ļ���
// �������������·�����ļ���
// ��  ע��
///////////////////////////////////////////////////////////////////////////////////////////
CString g_RemovePathFromFileName(LPCTSTR lpszFileName)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	_tsplitpath(lpszFileName, drive, dir, fname, ext);
	CString strFileName;
	strFileName.Format(_T("%s%s"), fname, ext);
	return strFileName;
}

///////////////////////////////////////////////////////////////////////////////////////////
// ��������g_GetFilterByType
// ��д�ߣ��ܺ���
// �ο����ϣ�
// ��  �ܣ������ļ������͵õ��ļ��Ĺ����ַ���
// ���������
//		int nFileType	�ļ�����
// ����������ļ��Ĺ����ַ���
// ��  ע��
///////////////////////////////////////////////////////////////////////////////////////////
//CString g_GetFilterByType(int nFileType)
//{
//	CString strFilter;
//	if(IS_VIDEOFILE(nFileType))
//	{
//		if(nFileType & FILE_TYPE_AVI)
//			strFilter += FILE_TYPE_AVI_FILTER;
//		if(nFileType & FILE_TYPE_MPG)
//			strFilter += FILE_TYPE_MPG_FILTER;
//		if(nFileType & FILE_TYPE_DAT)
//			strFilter += FILE_TYPE_DAT_FILTER;
//	}
//	else if(IS_IMAGEFILE(nFileType))
//	{
//		if(nFileType & FILE_TYPE_DIB)
//			strFilter += FILE_TYPE_DIB_FILTER;
//		if(nFileType & FILE_TYPE_JPG)
//			strFilter += FILE_TYPE_JPG_FILTER;
//	}
//	else if(IS_AUDIOFILE(nFileType))
//	{
//		if(nFileType & FILE_TYPE_WAV)
//			strFilter += FILE_TYPE_WAV_FILTER;
//		if(nFileType & FILE_TYPE_MID)
//			strFilter += FILE_TYPE_MID_FILTER;
//	}
//	else if(IS_DOCUMENTFILE(nFileType))
//	{
//		if(nFileType & FILE_TYPE_TXT)
//			strFilter += FILE_TYPE_TXT_FILTER;
//		if(nFileType & FILE_TYPE_DOC)
//			strFilter += FILE_TYPE_DOC_FILTER;
//		if(nFileType & FILE_TYPE_PDF)
//			strFilter += FILE_TYPE_DOC_FILTER;
//	}
//	else
//	{
//		strFilter = FILE_TYPE_ALL_FILTER;
//	}
//	if(strFilter.IsEmpty())
//		strFilter = FILE_TYPE_ALL_FILTER;
//	strFilter += _T("|");
//	return strFilter;
//}

///////////////////////////////////////////////////////////////////////////////////////////
// ��������g_GetFileSize
// ��д�ߣ��ܺ���
// �ο����ϣ�
// ��  �ܣ��õ��ļ��ĳ���
// ���������
//		LPCTSTR lpszFileName �ļ���
// ���������
// ����ֵ��ULONGLONG �ļ�����
// ��  ע��
///////////////////////////////////////////////////////////////////////////////////////////
ULONGLONG g_GetFileSize(LPCTSTR lpszFileName)
{
	CFile file;
	HANDLE hFile = ::CreateFile( lpszFileName, GENERIC_READ, 0, NULL, 
		OPEN_EXISTING, 0, NULL );
	if(hFile == INVALID_HANDLE_VALUE)
	{
		hFile = ::CreateFile( lpszFileName, 0, 0, NULL, 
			OPEN_EXISTING, 0, NULL );
		if (INVALID_HANDLE_VALUE == hFile)
		{
			return 0;
		}
	}

	ULONGLONG ullFileSize;
	DWORD dwFileSizeLow, dwFileSizeHigh;
	dwFileSizeLow = ::GetFileSize(hFile, &dwFileSizeHigh);
	ullFileSize = dwFileSizeHigh;
	ullFileSize = ullFileSize << 32;
	ullFileSize += dwFileSizeLow;

	CloseHandle(hFile);
	return ullFileSize;
}

BOOL g_GetFileTime(LPCTSTR lpszFileName, COleDateTime &odtLastWrite)
{
	CFile file;
	HANDLE hFile = ::CreateFile( lpszFileName, GENERIC_READ, 0, NULL, 
		OPEN_EXISTING, 0, NULL );
	if(hFile == NULL)
		return FALSE;
	BY_HANDLE_FILE_INFORMATION bhfi;
	if(!GetFileInformationByHandle(hFile, &bhfi))
		return FALSE;

	odtLastWrite = bhfi.ftLastWriteTime;
	CloseHandle(hFile);
	return TRUE;
}

void g_ReportFileError(CFileException *fe)
{
	fe->ReportError();
}

CString g_GetWin32ErrMsg()
{
	CString strErrMsg;
	LPTSTR lpszErrMsg = strErrMsg.GetBuffer(1024);
	FormatMessage( 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		lpszErrMsg,
		1024,
		NULL 
	);
	strErrMsg.ReleaseBuffer();
	return strErrMsg;
}

BOOL g_IsFileExist(LPCTSTR lpszFileName)
{
	CFileFind find;
	return find.FindFile(lpszFileName);
}

BOOL g_SplitFile(CString &strFullFileName, CString &strPath, CString &strFileName)
{
	int nPos = strFullFileName.ReverseFind(_T('\\'));
	if( nPos <= 0 )
		return FALSE;
	if( nPos >= (strFullFileName.GetLength() - 1) )
		return FALSE;
	strPath = strFullFileName.Left( nPos );
	strFileName = strFullFileName.Mid( nPos  + 1 );
	return TRUE;
}

CString StripNulls(CString strOriginal)
{
	CString strStripNulls = strOriginal;
	if (strOriginal.Find(_T("\0") ) > 0 )
		strStripNulls = strOriginal.Left(strOriginal.Find(_T("\0") ) - 1);
	return strStripNulls;
}

///////////////////////////////////////////////////////////////////////////////////////////
// ��������g_GetDirectorySize
// ��д�ߣ�����
// �ο����ϣ�
// ��  �ܣ��õ�һ��Ŀ¼�������ļ����������ļ�������Ŀ¼����
// ���������
//		CString strPath ·������"F:\Dayang"
//		CString	strSearch Ѱ�ҵ��������磺"*","*.avi","dayang.rar"
//		int&	nFileCount ���ص��ļ�����
//		int&	nDirCount  ���ص�Ŀ¼����
// ���������
// ����ֵ��__int64 Ŀ¼������С
// ��  ע��
///////////////////////////////////////////////////////////////////////////////////////////
ULONGLONG g_GetDirectorySize(CString strPath, CString strSearch, int& nFileCount, int& nDirCount)
{
	CString strFileName;  
	CString strDirName;   
	CStringArray arystrDir; 
	int nDir = 0;//�ܵ�Ŀ¼��   
	HANDLE hSearch;
	WIN32_FIND_DATA WFD;
	BOOL bResult = TRUE;
	__int64	i64TotalSize = 0;
	
	if (strPath.Right(1) != _T("\\") )
		strPath = strPath + _T("\\");
	
	hSearch = FindFirstFile(strPath + _T("*"), &WFD);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		while (bResult)
		{
			strDirName = StripNulls((CString)WFD.cFileName);
			if (strDirName != _T(".") && strDirName != _T("..") )
				if ( GetFileAttributes(strPath + strDirName) == FILE_ATTRIBUTE_DIRECTORY)
				{	
					arystrDir.Add(strDirName);
					nDirCount++;
					nDir++;
				}
				bResult = FindNextFile(hSearch, &WFD);
		}
		bResult = FindClose(hSearch);
	}
	
	hSearch = FindFirstFile(strPath + strSearch, &WFD);
	bResult = TRUE;
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		while (bResult)
		{
			strFileName = StripNulls((CString)WFD.cFileName);
			if (strFileName != _T(".") && strFileName != _T("..") && 
				GetFileAttributes(strPath + strFileName) != FILE_ATTRIBUTE_DIRECTORY)
			{
				i64TotalSize = i64TotalSize + (WFD.nFileSizeHigh * MAXDWORD) + WFD.nFileSizeLow;
				nFileCount++;
			}
			bResult = FindNextFile(hSearch, &WFD);
		}
		bResult = FindClose(hSearch);
	}
	
	if (nDir > 0)
	{
		for (int i = 0; i < nDir; i++)
		{
			i64TotalSize = i64TotalSize + g_GetDirectorySize(strPath + arystrDir[i] + _T("\\"), strSearch, nFileCount, nDirCount);
		}
	}
	return i64TotalSize;
}

///////////////////////////////////////////////////////////////////////////////////////////
// ��������g_GetDiskFreeSpace
// ��д�ߣ�����
// �ο����ϣ�
// ��  �ܣ��õ�һ��Ŀ¼����������̵�ʣ������
// ���������
//		CString strDriverPath ·������"F:\Dayang"
// ���������
// ����ֵ��__int64 ʣ��������С
// ��  ע��
///////////////////////////////////////////////////////////////////////////////////////////
ULONGLONG g_GetDiskFreeSpace(CString strDriverPath)
{
	DWORD dwBytesPerSector = 512;
	DWORD dwSectorPerCluster;
	DWORD dwFreeClusters;
	DWORD dwTotalClusters;
	DWORD dwDiskFreeSpace;
	typedef BOOL (WINAPI *GetDiskFreeSpaceEx)(LPCTSTR lpDirectoryName,                 // directory name
		PULARGE_INTEGER lpFreeBytesAvailable,    // bytes available to caller
		PULARGE_INTEGER lpTotalNumberOfBytes,    // bytes on disk
		PULARGE_INTEGER lpTotalNumberOfFreeBytes // free bytes on disk
		);
	
	GetDiskFreeSpaceEx pGetDiskFreeSpaceEx = NULL;
	HMODULE hHandle = LoadLibrary(_T("kernel32.dll"));	
	pGetDiskFreeSpaceEx = (GetDiskFreeSpaceEx)GetProcAddress( hHandle,
		"GetDiskFreeSpaceExA");
	BOOL fResult;
	__int64 i64FreeBytesToCaller, i64FreeBytes, i64TotalBytes;
	if (pGetDiskFreeSpaceEx)
	{
		CString strPath = strDriverPath.Right(strDriverPath.GetLength()-2);
		fResult = pGetDiskFreeSpaceEx (strPath,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,
			(PULARGE_INTEGER)&i64FreeBytes);
		
		if (fResult)
			return i64FreeBytes;
	}
	else 
	{
		fResult = GetDiskFreeSpace(strDriverPath, 
			&dwSectorPerCluster, 
			&dwBytesPerSector,
			&dwFreeClusters, 
			&dwTotalClusters);	
	}
	if( !fResult)
	{
		dwBytesPerSector = 512; 
	}
	if( dwBytesPerSector == 0 )
	{
		dwBytesPerSector = 512; 
	}
	dwDiskFreeSpace = dwBytesPerSector*dwSectorPerCluster*dwFreeClusters;
	
	return (ULONGLONG)dwDiskFreeSpace;
}

BOOL DY_EXT_API g_FileCanbeDel(CString strFileName)
{
	if(strFileName.IsEmpty())
		return TRUE;

	if(!g_IsFileExist(strFileName))
		return TRUE;

	CFile file;
	BOOL bFlag = file.Open(strFileName, CFile::modeRead|CFile::shareDenyRead);
	if(bFlag)
		file.Close();
	return bFlag;
}