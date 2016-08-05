#ifndef _DATABASE_COMMON_FUNCTION_H
#define _DATABASE_COMMON_FUNCTION_H

enum _DB_STRING
{
	_DB_CUR_DATE = 0,
	_DB_NEW_GUID = 1,
	_DB_TOP_N = 2,
	_DB_DATETIME_TYPE = 3,//日期时间类型
		
};

inline CString GetDBString(_DB_STRING enumString,CD_DB_TYPE enumDB,int nTop = 1);
inline CString GetSQLSelectTop(LPCTSTR lpszSQL,CD_DB_TYPE enumDB,int nTop, int nMin = 0);
inline BOOL HasField(CDYRecordSetEx &rs,LPCTSTR lpszFieldName);

inline CString g_GetDateString(LPCTSTR lpszDate,//传入 2000-01-01 14:20:40 ，对于非Oracle数据库，返回的字符串两边增加了'，如'2000-01-01 14:20:40'
							   CD_DB_TYPE enumDB);

inline CString GetDBString(_DB_STRING enumString,CD_DB_TYPE enumDB,int nTop)
{
	CString strText;
	switch(enumString) {
	case _DB_CUR_DATE:
		{
			switch(enumDB)
			{
			case CDDT_SQLSERVER:
				strText = _T(" getdate() ");
				break;
			case CDDT_DB2:
				strText = _T(" CURRENT TIMESTAMP ");
				break;
			case CDDT_ORACLE:
				strText = _T(" sysdate ");
				break;
			default:
				break;
			}
		}
		break;
	case _DB_NEW_GUID:
		{
			switch(enumDB)
			{
			case CDDT_SQLSERVER:
				strText = _T(" newid() ");
				break;
			case CDDT_DB2:
				//use db2 MTK usder defined function,
				//defined in DB2MTK\mtkms7.udf,not real guid, just  current time value
				strText = _T(" MS7.newid() ");
				break;
			case CDDT_ORACLE:
				strText = _T(" SYS_GUID() ");
				break;
			default:
				break;
			}
		}
		break;
	case _DB_TOP_N:
		{
			ASSERT(nTop > 0);
			switch(enumDB)
			{
			case CDDT_SQLSERVER:
				strText.Format( _T(" TOP %d "),nTop);
				break;
			case CDDT_DB2:
				strText.Format(_T(" FETCH FIRST %d row ONLY  "),nTop);
				break;
			case CDDT_ORACLE:
				strText.Format(_T(" ROWNUM <= %d "),nTop);
				break;
			default:
				break;
			}
		}
		break;	
	case _DB_DATETIME_TYPE:
		{
			switch(enumDB)
			{
			case CDDT_SQLSERVER:
				strText = _T(" DATETIME ");
				break;
			case CDDT_DB2:
				strText = _T(" TIMESTAMP ");
				break;
			case CDDT_ORACLE:
				strText = _T(" DATE ");
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	return strText;
	
}

inline CString GetSQLSelectTop(LPCTSTR lpszSQL,CD_DB_TYPE enumDB,int nTop, int nMin )
{
	CString strSQL = lpszSQL;
	CString strTemp;
	switch(enumDB)
	{
	case CDDT_SQLSERVER:
		{
			CString strSQLUpper = lpszSQL;
			strSQLUpper.MakeUpper();
			CString strDistinct = _T("DISTINCT");
			CString strSelect = _T("SELECT");
			
			int nPos = strSQLUpper.Find(strDistinct);
			if(nPos > 0)
				nPos += strDistinct.GetLength();//size of 
			else 
			{
				nPos = strSQLUpper.Find(strSelect);
				if(nPos >= 0)
					nPos += strSelect.GetLength();
			}
			if(nPos < 0)
			{
				ASSERT(FALSE);
			}
			strTemp = strSQL.Left(nPos) 
				+ GetDBString(_DB_TOP_N,enumDB,nTop) 
				+ strSQL.Right(strSQL.GetLength() - nPos);
			strSQL = strTemp;
		}
		break;
	case CDDT_DB2:
		{
			strTemp = GetDBString(_DB_TOP_N,enumDB,nTop);
			strSQL += strTemp;
		}
		break;
	case CDDT_ORACLE:
		{
			if(nMin > 0)//not first page
			strTemp.Format(_T("select * from (select my_table.*,rownum as my_rownum from ( %s ) my_table where %s) \
				 where my_rownum > %d"),
				strSQL,
				GetDBString(_DB_TOP_N,enumDB,nTop),
				nMin
				);
			else//first page
			strTemp.Format(_T("select * from ( %s ) where %s"),
				strSQL,
				GetDBString(_DB_TOP_N,enumDB,nTop)
			);

			strSQL = strTemp;
		}
		break;
	default:
		break;
	}
	return strSQL;
}

inline BOOL HasField(CDYRecordSetEx &rs,LPCTSTR lpszFieldName)
{
	int nFields = rs.GetFieldCount();
	CString strFieldName;
	for(int i = 0; i < nFields; i++)
	{
		rs.GetFieldName(i,strFieldName);
		if(strFieldName.CompareNoCase(lpszFieldName) == 0)
			return TRUE;
	}
	return FALSE;
}

inline CString g_GetDateString(LPCTSTR lpszDate,CD_DB_TYPE enumDB)
{
	CString strDate;
	switch(enumDB)
	{
	case CDDT_ORACLE:
		strDate.Format(_T(" to_date('%s','YYYY-MM-DD HH24-MI-SS') "),lpszDate);
		break;
	default:
		strDate.Format(_T(" '%s' "),lpszDate);
	    break;
	}
	return strDate;
}
#endif //_DATABASE_COMMON_FUNCTION_H