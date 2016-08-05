// OutPut.h: interface for the COutPut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUT_H__0F35F392_0FBE_4F53_9B18_B2ED9CD3E2C4__INCLUDED_)
#define AFX_OUTPUT_H__0F35F392_0FBE_4F53_9B18_B2ED9CD3E2C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "afxmt.h"

#define OUTPUT_FILE			1 //output to file
#define OUTPUT_PRINT		2 //output as printf
#define OUTPUT_DEBUG		4 //output to debugView
#define OUTPUT_ALL			7 //output to all


class CCriticalSection;
class DY_EXT_CLASS COutPut  
{
public:
	COutPut();
	virtual ~COutPut();

	//if not set log header, use the module name as header
	BOOL  SetLogHeader(LPCTSTR lpszLogHeader);
	//if not set Log file, use c:\modulename.log as file name
	BOOL  SetLogFile(LPCTSTR lpszFile,
		BOOL bTruncate = FALSE,
		int nMaxSize = 1//MB
		);

	//int vsprintf( char *buffer, const char *format, va_list argptr );
	//_vstprintf

	// AFX_CDECL is used for rare functions taking variable arguments
	//only OutputDebugString
	static BOOL AFX_CDECL OutPut(LPCTSTR lpszFormat, ...);
	//OutputDebugString and/or Log to File
	BOOL AFX_CDECL OutPut(BOOL bLogToFile,
		LPCTSTR lpszFormat, ...);
	//OutputDebugString and/or log to file and/or  printf 
	BOOL AFX_CDECL OutPutEx(int nMask,//OUTPUT_FILE | OUTPUT_PRINT | OUTPUT_DEBUG
		LPCTSTR lpszFormat, ...);
	
private:
	BOOL  OutPut2(int bLogtoFile, LPCTSTR lpszMsg );
	BOOL WriteFile(
		HANDLE hFile,                    // handle to file
		LPCVOID lpBuffer,                // data buffer
		DWORD nNumberOfBytesToWrite,     // number of bytes to write
		LPDWORD lpNumberOfBytesWritten,  // number of bytes written
		LPOVERLAPPED lpOverlapped        // overlapped buffer
		);

	HANDLE m_hLogFile;
	CCriticalSection m_csFile;
	CString m_strFileName;//full name
	int m_nMaxSize;//MB
	CString m_strModuleName;
	
	BOOL m_bNeedCopyFile;
	BOOL m_bUnicodeChecked;

};

#endif // !defined(AFX_OUTPUT_H__0F35F392_0FBE_4F53_9B18_B2ED9CD3E2C4__INCLUDED_)
