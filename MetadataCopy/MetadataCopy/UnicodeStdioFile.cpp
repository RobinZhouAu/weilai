// UnicodeStdioFile.cpp: implementation of the CUnicodeStdioFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UnicodeStdioFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//static DY_VIDEO_STANDARD g_enumThisVideoStandard = DYVIDEOSTANDARD_UNKNOW;
//DY_EXT_API void SetThisVideoStandard(DY_VIDEO_STANDARD enumVideoStandard)
//{
//	g_enumThisVideoStandard = enumVideoStandard;
//}
//
//DY_EXT_API DY_VIDEO_STANDARD GetThisVideoStandard()
//{
//	return g_enumThisVideoStandard;
//}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 Read write Ansi file in unicode application (when _UNICODE defined), 
 cannot read unicode file
 */


CUnicodeStdioFile::CUnicodeStdioFile()
{

}

CUnicodeStdioFile::~CUnicodeStdioFile()
{

}

void CUnicodeStdioFile::WriteString(LPCTSTR lpsz)
{
#ifdef _UNICODE
	int nBytes = WideCharToMultiByte(CP_ACP, 0, lpsz, -1, NULL, 0, NULL, NULL);
	LPSTR lpszWrite = new char[nBytes];
	WideCharToMultiByte(CP_ACP, 0, lpsz, -1, lpszWrite, nBytes, NULL, NULL);
	CStdioFile::Write(lpszWrite, nBytes - 1);
	delete lpszWrite;
#else
	CStdioFile::WriteString(lpsz);
#endif
}

LPTSTR CUnicodeStdioFile::ReadString(LPTSTR lpsz, UINT nMax)
{
	BYTE cRead[3];
	CString strRead;
	int nIndex = 0;
	BOOL bRead = FALSE;
	while(nIndex < (int)nMax - 1)
	{
		if(Read(cRead, 1) == 0)
			break;
		bRead = TRUE;
		if(cRead[0] == '\r' || cRead[0] == '\n')
		{
			if (cRead[0] == '\r')
				if(Read(cRead, 1) == 0)
					break;
			break;
		}
		else
		{
			if(::IsDBCSLeadByte(cRead[0]))
			{
				if(Read(&cRead[1], 1) == 0)
					break;
				cRead[2] = '\0';
			}
			else
			{
				cRead[1] = '\0';
			}
			strRead = cRead;
			lpsz[nIndex ++] = strRead.GetAt(0);
		}
	}
	lpsz[nIndex] = _T('\0');
	if(!bRead)
		return NULL;
	return lpsz;
}

BOOL CUnicodeStdioFile::ReadString(CString& rString)
{
	rString.Empty();
	BYTE cRead[3];
	CString strRead;
	BOOL bRead = FALSE;
	while(Read(cRead, 1) > 0)
	{
		if(cRead[0] == '\r' || cRead[0] == '\n')
		{
			if (cRead[0] == '\r')
				Read(cRead, 1);
			return TRUE;
		}
		else
		{
			if(::IsDBCSLeadByte(cRead[0]))
			{
				Read(&cRead[1], 1);
				cRead[2] = '\0';
			}
			else
			{
				cRead[1] = '\0';
			}
			strRead = cRead;
			rString += strRead;
		}
		bRead = TRUE;
	}
	return bRead;
}
