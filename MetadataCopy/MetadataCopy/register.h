// Register.h: interface for the CRegister class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTER_H__BBA6B631_A4DC_4EA6_B103_A8704EE26603__INCLUDED_)
#define AFX_REGISTER_H__BBA6B631_A4DC_4EA6_B103_A8704EE26603__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _ENUM_REG_VALUE_{
	CString			strName;		//注册表键值名称
	DWORD			enumType;		//字符串类型
	CString			strValue;		//字符串值
	DWORD			dwValue;		//DWORD值		
	_ENUM_REG_VALUE_()
	{
		strName = _T("");
		strValue = _T("");
		enumType = REG_SZ;
		dwValue = 0;
	}
	
}ENUM_REG_VALUE, *PTR_ENUM_REG_VALUE;

typedef CArray<PTR_ENUM_REG_VALUE, PTR_ENUM_REG_VALUE&> CArrayRegValue;

class DY_EXT_CLASS CRegister  
{
public:
	BOOL CloseKey();
	BOOL OpenKey(HKEY hKey, LPCTSTR lpszSubKey);
	BOOL CreateKey(HKEY hKey, LPCTSTR lpszSubKey);
	BOOL DeleteValue(LPCTSTR lpszSubKey);

	BOOL EnumValue(CArrayRegValue& arrRegValue);	
	BOOL GetString(LPCTSTR lpszItem, CString &strValue, int nLength = 500);
	BOOL GetDWord(LPCTSTR lpszItem, DWORD &dwValue);
	CString GetString(LPCTSTR lpszItem, int nLength = 500);
	BOOL SetString(LPCTSTR lpszItem, LPCTSTR lpszValue);
	BOOL SetDWord(LPCTSTR lpszItem, DWORD dwValue);
	
	CRegister();
	virtual ~CRegister();
private:
	HKEY m_hKey;
	HKEY m_hRootKey;
	LPCTSTR m_lpszSubKey;
};

#endif // !defined(AFX_REGISTER_H__BBA6B631_A4DC_4EA6_B103_A8704EE26603__INCLUDED_)
