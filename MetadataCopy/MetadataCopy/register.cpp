// Register.cpp: implementation of the CRegister class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "search.h"
#include "Register.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
only for string type;

//writer reg:
  	CRegister reg;
	reg.CreateKey(HKEY_CURRENT_USER,_T("Software\\Dayang\\CDSearch\\Login"));
	reg.SetString(_T("Name"), (LPCTSTR)m_strName);
	reg.SetString(_T("Server"), (LPCTSTR)m_strSvr);
	reg.CloseKey();
//read reg:
CRegister reg;
	reg.OpenKey(HKEY_CURRENT_USER,_T("Software\\Dayang\\CDSearch\\Login"));
	m_strSvr = reg.GetString(_T("Server"));
	m_strName = reg.GetString(_T("Name"));
	reg.CloseKey();


	*/
CRegister::CRegister()
{
	m_hKey = 0;
}

CRegister::~CRegister()
{

}

BOOL CRegister::CreateKey(HKEY hKey, LPCTSTR lpszSubKey)
{
	if(RegCreateKey(hKey, lpszSubKey, &m_hKey) != ERROR_SUCCESS)
	{
		m_hKey = 0;
		return FALSE;
	}
	return TRUE;		
}

BOOL CRegister::DeleteValue(LPCTSTR lpszSubKey)
{
	if(RegDeleteValue(m_hKey, lpszSubKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}
	return TRUE;		
}

BOOL CRegister::SetString(LPCTSTR lpszItem, LPCTSTR lpszValue)
{
	LPBYTE pBuf;
	int nBufSize;
	pBuf = (LPBYTE)lpszValue;
	nBufSize = _tcslen(lpszValue) * 2;
	if(RegSetValueEx(m_hKey, lpszItem,	0, REG_SZ, pBuf, nBufSize) != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

CString CRegister::GetString(LPCTSTR lpszItem, int nLength)
{
	DWORD dwBufLen = 40960;
	CString strValue;
	BYTE pBuf[40960];
	memset(pBuf,0,40960);
	DWORD dwType = REG_SZ;
	if(RegQueryValueEx(m_hKey, lpszItem, NULL, &dwType, pBuf, &dwBufLen) != ERROR_SUCCESS)
		return _T("");
	strValue = (TCHAR *)pBuf;
	return strValue;
}

BOOL CRegister::GetString(LPCTSTR lpszItem, CString &strValue, int nLength)
{
	DWORD dwBufLen = 40960;
	BYTE pBuf[40960];
	memset(pBuf,0,40960);
	DWORD dwType = REG_SZ;
	if(RegQueryValueEx(m_hKey, lpszItem, NULL, &dwType, pBuf, &dwBufLen) != ERROR_SUCCESS)
		return FALSE;
	strValue = (TCHAR *)pBuf;
	return TRUE;
}

BOOL CRegister::GetDWord(LPCTSTR lpszItem, DWORD &dwValue)
{
	DWORD dwType = REG_DWORD;
	DWORD dwSize = 4;
	if(RegQueryValueEx(m_hKey, lpszItem, NULL, &dwType, (BYTE *)&dwValue, &dwSize) != ERROR_SUCCESS)
		return FALSE;
	return TRUE;
}

BOOL CRegister::CloseKey()
{
	RegCloseKey(m_hKey);
	m_hKey = 0;
	return TRUE;
}

BOOL CRegister::OpenKey(HKEY hKey, LPCTSTR lpszSubKey)
{
	if(RegOpenKeyEx(hKey, lpszSubKey, 0, KEY_ALL_ACCESS, &m_hKey) != ERROR_SUCCESS)
	{
		m_hKey = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL CRegister::SetDWord(LPCTSTR lpszItem, DWORD dwValue)
{
	LONG lRet = RegSetValueEx(m_hKey, lpszItem,	0, REG_DWORD, (const BYTE *)&dwValue, 4);
	if(lRet != ERROR_SUCCESS)
		return FALSE;
	return TRUE;
}

BOOL CRegister::EnumValue(CArrayRegValue& arrRegValue)
{
	LONG  LRet;
	DWORD dwIndex = 0;
	DWORD dwNameSize = 40960;
	DWORD dwBufLen = 40960;
	DWORD dwType = REG_SZ;
	BYTE pBufValue[40960];
	//memset(pBufValue,0,1024);
	TCHAR pBufName[40960];
	//memset(pBufName,0,1024*sizeof(TCHAR));
	
	for (int i = 0; i < arrRegValue.GetSize(); i++)
		delete arrRegValue[i];
	arrRegValue.RemoveAll();
	while(LRet = RegEnumValue(m_hKey, dwIndex, pBufName, &dwNameSize, NULL, &dwType, pBufValue, &dwBufLen) != ERROR_NO_MORE_ITEMS)
	{
		PTR_ENUM_REG_VALUE	pRegValue = new ENUM_REG_VALUE;
		pRegValue->strName = (TCHAR *)pBufName;
		if ((dwType == REG_SZ || dwType == REG_DWORD) && pRegValue->strName != _T("") )//去掉注册表中的默认选项
			pRegValue->enumType = dwType;
		else
		{
			TRACE(_T("UnSupport RegType!") );
			delete	pRegValue;
			dwIndex++;
			dwNameSize = 40960;
			dwBufLen = 40960;			
			continue;
		}
		switch (pRegValue->enumType)
		{
		case REG_SZ:
			pRegValue->strValue = (TCHAR *)pBufValue;
			break;
		case REG_DWORD:
			pRegValue->dwValue = *(DWORD *)pBufValue;
			break;
		}
		arrRegValue.Add(pRegValue);
		dwIndex++;
		dwNameSize = 40960;
		dwBufLen = 40960;
	}
	if (LRet != ERROR_SUCCESS)
		return FALSE;
	return TRUE;
}