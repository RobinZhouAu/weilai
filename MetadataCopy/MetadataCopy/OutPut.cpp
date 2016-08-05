// OutPut.cpp: implementation of the COutPut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "afxmt.h"
#include "OutPut.h"
#include "AFXPRIV.H"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//HANDLE COutPut::m_hLogFile = INVALID_HANDLE_VALUE;
//CCriticalSection COutPut::m_csFile = CCriticalSection();
COutPut::COutPut()
{
	//
	m_strFileName = _T("");	
	m_nMaxSize = 1;//MB
	m_hLogFile = INVALID_HANDLE_VALUE;
	TCHAR lpszModuleName[1025];
	GetModuleFileName(NULL,lpszModuleName,1024);
	CString strName = lpszModuleName;
	int nPos = strName.ReverseFind(TCHAR('\\'));
	m_strModuleName = strName.Right(strName.GetLength() - nPos - 1);
	nPos = m_strModuleName.ReverseFind(TCHAR('.'));
	if(nPos >= 0)
		m_strModuleName = m_strModuleName.Left(nPos);
	m_bNeedCopyFile = FALSE;
	m_bUnicodeChecked = FALSE;
	
}

COutPut::~COutPut()
{
	if(m_hLogFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hLogFile);
}


BOOL AFX_CDECL COutPut::OutPut(LPCTSTR lpszFormat, ...)
{
	ASSERT(AfxIsValidString(lpszFormat));
	va_list argList;
	va_start(argList, lpszFormat);
	int nBuf;
	TCHAR szBuffer[4096];
	nBuf = _vsntprintf(szBuffer, 4096, lpszFormat, argList);
	//afxDump << szBuffer;
	OutputDebugString(szBuffer);	
	va_end(argList);
	return TRUE;
}
BOOL  COutPut::SetLogHeader(LPCTSTR lpszLogHeader)
{
	m_strModuleName = lpszLogHeader;
	return TRUE;
}

BOOL  COutPut::SetLogFile(LPCTSTR lpszFile, BOOL bTruncate, int nMaxSize)
{
	if(m_hLogFile != INVALID_HANDLE_VALUE)
		::CloseHandle(m_hLogFile);
	m_hLogFile = INVALID_HANDLE_VALUE;
	CString strFullFileName = lpszFile;
	int nPos = strFullFileName.ReverseFind(_T('\\'));
	CString	strPath = strFullFileName.Left( nPos );
	if(!strPath.IsEmpty())//check dir exist
	{
		CFileFind fFind;
// 		TCHAR tchBuffer[MAX_PATH]; 
// 		LPTSTR lpszCurDir;
// 		lpszCurDir = tchBuffer; 
// 		GetCurrentDirectory(MAX_PATH , lpszCurDir);
// 		CString strDic = lpszCurDir;
		if(!fFind.FindFile(strPath))
			BOOL bOK = ::CreateDirectory(strPath, NULL);			
	}

	m_hLogFile = ::CreateFile(lpszFile,
			GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,    // FILE_SHARE_WRITE add by zjq
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if(m_hLogFile == INVALID_HANDLE_VALUE)
		return FALSE;
	if(!bTruncate)
	{
		CStdioFile file;
		BOOL bOK = file.Open(lpszFile,CFile::modeRead|CFile::shareDenyNone);
		BYTE byte[2];
		byte[0] = byte[1] = 0;
		if(bOK)
		{
			file.Read(byte,2);
			file.Close();
			if(byte[0] != 0xFF || byte[1] != 0xFE)//原文件非unicode备份
			{
				if(0 != byte[0] || 0 != byte[1])  //add by zjq, 防止第一次创建日志文件时，就生成备份文件。
					m_bNeedCopyFile = TRUE;
			}
		}
	}
	
	if(bTruncate)
	{
		::SetEndOfFile(m_hLogFile);
	}
	::SetFilePointer(m_hLogFile,0,NULL,FILE_END);
	
	m_strFileName = lpszFile;
	if(nMaxSize > 0 )// && nMaxSize < 10)
		m_nMaxSize = nMaxSize;
	return TRUE;
}

BOOL  COutPut::OutPut(BOOL bLogToFile,LPCTSTR lpszFormat, ...)
{

	va_list argList;
	va_start(argList, lpszFormat);
	int nBuf;
	TCHAR szBuffer[4096];
	nBuf = _vsntprintf(szBuffer, 4096, lpszFormat, argList);	
	CString strTemp;
	strTemp.Format(_T("[%s]\t[%ld] %s"), m_strModuleName, GetCurrentThreadId(), szBuffer);
	if(strTemp.Right(1) != _T("\n"))
		strTemp += _T("\r\n");
	m_csFile.Lock();
	::OutputDebugString((LPCTSTR)strTemp);
	m_csFile.Unlock();
	BOOL bOK = OutPut2(bLogToFile,(LPCTSTR)strTemp);
	va_end(argList);

	return bOK;
}
BOOL COutPut::OutPutEx(int nMask,
		LPCTSTR lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);
	int nBuf;
	TCHAR szBuffer[4096];
	nBuf = _vsntprintf(szBuffer, 4096, lpszFormat, argList);	
	CString strTemp;
	strTemp.Format(_T("[%s]\t[%ld] %s"), m_strModuleName, GetCurrentThreadId(), szBuffer);
	if(strTemp.Right(1) != _T("\n"))
		strTemp += _T("\r\n");
	m_csFile.Lock();	
	if(nMask&OUTPUT_DEBUG)
		::OutputDebugString((LPCTSTR)strTemp);
	if(nMask&OUTPUT_PRINT)
		_tprintf((LPCTSTR)strTemp);
	m_csFile.Unlock();	
	if(nMask&OUTPUT_FILE)
		OutPut2(1,(LPCTSTR)strTemp);
	va_end(argList);

	return TRUE;

}
BOOL  COutPut::OutPut2( int bLogToFile,LPCTSTR lpszMsg)
{	
	BOOL bOK = TRUE;
	if(bLogToFile && m_strFileName.IsEmpty())
	{
		TCHAR lpszModuleName[1025];
		GetModuleFileName(NULL,lpszModuleName,1024);
		CString strModuleName = lpszModuleName;
		int nPos = strModuleName.ReverseFind(TCHAR('\\'));
		CString strPath = strModuleName.Left(nPos);
		nPos = strPath.ReverseFind(TCHAR('\\'));
		if(nPos > 0 && nPos < strPath.GetLength())
			strPath = strPath.Left(nPos);
		strPath = strPath + _T("\\log");
	
		CString strFileName;
		strFileName.Format(_T("%s\\%s.log"),strPath,m_strModuleName);
		SetLogFile(strFileName);
	}	
	if(bLogToFile)
	{
		if(m_hLogFile == INVALID_HANDLE_VALUE)
			return FALSE;
		else
		{
			CFileStatus status;
			m_csFile.Lock();
			if(CFile::GetStatus(m_strFileName,status))
			{
				if(status.m_size > m_nMaxSize*1024*1024 || m_bNeedCopyFile)
				{
					//copy this file to another file
					CTime time = CTime::GetCurrentTime();;
					CString strTime = time.Format( _T("[%Y-%m-%d-%H-%M-%S]") );
					CString strNewFileName = m_strFileName + strTime + _T(".log");
					::CloseHandle(m_hLogFile);
					m_hLogFile = INVALID_HANDLE_VALUE;
					CopyFile(m_strFileName, strNewFileName,TRUE);
					//delete all logs of this file
					SetLogFile(m_strFileName,TRUE, m_nMaxSize);
					m_bNeedCopyFile = FALSE;
				}
			}
			bOK = FALSE;
			DWORD dwWrite;
			int n = _tcslen(lpszMsg);
			CTime time = CTime::GetCurrentTime();;
			CString strTime = time.Format( _T("[%Y-%m-%d %H:%M:%S]\t") );
			
			::SetFilePointer(m_hLogFile,0,NULL,FILE_END);
			
			bOK = WriteFile(m_hLogFile,
				(LPCTSTR)strTime,
				(DWORD)strTime.GetLength(),
				&dwWrite,
				NULL);
			bOK = WriteFile(m_hLogFile,
				(LPCVOID)lpszMsg,
				(DWORD)n,
				&dwWrite,
				NULL);
			CString strTemp = lpszMsg;	
			int nLength = 2;
			if(strTemp.Right(nLength) != _T("\r\n"))
			{
				bOK = WriteFile(m_hLogFile,
					(LPCVOID)_T("\r\n"),//lpszMsg,
					(DWORD)(nLength),
					&dwWrite,
					NULL);
			}
			m_csFile.Unlock();

		}
	}

	return bOK;
}

BOOL COutPut::WriteFile(
		HANDLE hFile,                    // handle to file
		LPCVOID lpBuffer,                // data buffer
		DWORD nNumberOfBytesToWrite,     // number of bytes to write	
		LPDWORD lpNumberOfBytesWritten,  // number of bytes written
		LPOVERLAPPED lpOverlapped        // overlapped buffer
		)
{
	BOOL bOK;

if(!m_bUnicodeChecked)
{
	DWORD dwSize = GetFileSize (hFile, NULL) ; 	
	if (dwSize == 0)
	{
		//write unicode bit"FFEE"
		BYTE byte[2];
		byte[0] = 0xFF;
		byte[1] = 0xFE;
		bOK = ::WriteFile(hFile,byte,2,lpNumberOfBytesWritten,lpOverlapped);	
	}
	m_bUnicodeChecked = TRUE;
}

#ifdef _UNICODE
	LPCTSTR lpszBuffer = (LPCTSTR)lpBuffer;
	DWORD dwLength = _tcsclen(lpszBuffer);
	bOK = ::WriteFile(hFile,lpBuffer,dwLength*2,lpNumberOfBytesWritten,lpOverlapped);		
#else
	USES_CONVERSION;
	LPWSTR lpszBuffer = A2W((char*)lpBuffer);
	DWORD dwLength = wcslen(lpszBuffer);
	bOK = ::WriteFile(hFile,lpszBuffer,dwLength*2,lpNumberOfBytesWritten,lpOverlapped);
#endif
	return bOK;

}