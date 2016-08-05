#if! defined (DB_COMMON_STRUCT_H_0DD6BA00_ED0B_4960_B28A_4464D190B277__)
#define DB_COMMON_STRUCT_H_0DD6BA00_ED0B_4960_B28A_4464D190B277__

//#include "..\..\..\DYRSoftWareSDK\DayangResearch\include\DYR_DatabaseAccessAPI.h"

enum CD_DB_TYPE
{
	CDDT_SQLSERVER = 0,
	CDDT_DB2,
	CDDT_ORACLE,
	CDDT_ODBCDRIVER,
	CDDT_ACCESS,
	CDDT_MYSQL,
	CDDT_EXCEL,
	CDDT_WINDOWSDOMAIN,
	CDDT_ORACLEHTTP,
	CDDT_XINCHENUSER,//新辰的用户库
	CDDT_OTHER = 201,
	CDDT_OTHER2 = 202,
	CDDT_OTHER3 = 203,
	CDDT_UNKNOW = -1,
};

//enum CD_DB_TYPE
//{
//	CDDT_UNKNOW = 0x00000000,
//	CDDT_SQLSERVER = 0x00000001,
//	CDDT_ORACLE = 0x00000002,
//	CDDT_ODBCDRIVER = 0x00000004,
//	CDDT_ACCESS = 0x00000008,
//	CDDT_MYSQL = 0x00000010,
//	CDDT_DB2 = 0x00000020,
//	CDDT_EXCEL,
//	CDDT_WINDOWSDOMAIN,
//	CDDT_ORACLEHTTP,
//	CDDT_XINCHENUSER,//新辰的用户库
//	CDDT_OTHER = 201,
//	CDDT_OTHER2 = 202,
//	CDDT_OTHER3 = 203,
//};

typedef struct _CD_DB_CONNECT
{
	CD_DB_TYPE enumType;
	BOOL	bUseDSN;
	CString	strDataSource;
	CString	strUID;
	CString	strPWD;
	CString	strDatabase;
	_CD_DB_CONNECT()
	{
		enumType = CDDT_SQLSERVER;
		bUseDSN = FALSE;
	}
	CString GetString()
	{
		CString strText;
		switch(enumType)
		{
		case CDDT_SQLSERVER:
			strText = _T("SQLServer");
			break;
		case CDDT_DB2:
			strText = _T("DB2");
			break;
		case CDDT_ORACLE:
			strText = _T("Oracle");
			break;
		}
		strText += _T("数据源");
		strText += strDataSource;
		return strText;
	}

	//SDYDataBaseInfo ToDYDatabaseInfo()
	//{
	//	SDYDataBaseInfo pDBInfo;
	//	switch(enumType)
	//	{
	//	case CDDT_SQLSERVER:
	//		pDBInfo.enumDBType = DYDATABASETYPE_SQLSERVER;
	//		break;
	//	case CDDT_DB2:
	//		pDBInfo.enumDBType = DYDATABASETYPE_DB2;
	//		break;
	//	case CDDT_ORACLE:
	//		pDBInfo.enumDBType = DYDATABASETYPE_ORACLE;
	//		break;
	//	case CDDT_ODBCDRIVER:
	//		pDBInfo.enumDBType = DYDATABASETYPE_ODBCDRIVER;
	//		break;
	//	case CDDT_ACCESS:
	//		pDBInfo.enumDBType = DYDATABASETYPE_ACCESS;
	//		break;
	//	case CDDT_MYSQL:
	//		pDBInfo.enumDBType = DYDATABASETYPE_MYSQL;
	//		break;
	//	case CDDT_ORACLEHTTP:
	//		pDBInfo.enumDBType = DYDATABASETYPE_ORACLEHTTP;
	//		break;

	//	/*case CDDT_EXCEL:
	//	case CDDT_WINDOWSDOMAIN:
	//	case CDDT_XINCHENUSER:
	//	case CDDT_OTHER:
	//	case CDDT_OTHER2:
	//	case CDDT_OTHER3:
	//	case CDDT_UNKNOW:
	//		pDBInfo.enumDBType = DYDATABASETYPE_UNKNOW;
	//		break;*/
	//	default:
	//		pDBInfo.enumDBType = DYDATABASETYPE_UNKNOW;
	//		break;
	//	}
	//	pDBInfo.strDataSource = strDatabase;
	//	pDBInfo.strPWD = strPWD;
	//	pDBInfo.strServerId = strDataSource;
	//	pDBInfo.strUserId = strUID;
	//	return pDBInfo;
	//}

	//void FromDYDatabaseInfo(SDYDataBaseInfo& pDBInfo)
	//{
	//	switch(pDBInfo.enumDBType) {
	//	case DYDATABASETYPE_SQLSERVER:
	//		enumType = CDDT_SQLSERVER;
	//		break;
	//	case DYDATABASETYPE_ORACLE:
	//		enumType = CDDT_ORACLE;
	//		break;
	//	case DYDATABASETYPE_ODBCDRIVER:
	//		enumType = CDDT_ODBCDRIVER;
	//		break;
	//	case DYDATABASETYPE_ACCESS:
	//		enumType = CDDT_ACCESS;
	//		break;
	//	case DYDATABASETYPE_MYSQL:
	//		enumType = CDDT_MYSQL;
	//		break;
	//	case DYDATABASETYPE_DB2:
	//		enumType = CDDT_DB2;
	//		break;
	//	/*case DYDATABASETYPE_ORACLEHTTP:
	//		enumType = CDDT_ORACLEHTTP;*/
	//		break;
	//	default:
	//		enumType = CDDT_UNKNOW;
	//	}
	//	strDatabase = pDBInfo.strDataSource;
	//	strPWD = pDBInfo.strPWD;
	//	strDataSource = pDBInfo.strServerId;
	//	strUID = pDBInfo.strUserId;		
	//}
	//
	//SDYRDataBaseInfo ToDYRDatabaseInfo()//modified by cml 20130726 升级到研究院数据库所需
	//{
	//	SDYRDataBaseInfo pDBInfo;
	//	switch(enumType)
	//	{
	//	case CDDT_SQLSERVER:
	//		pDBInfo.enumDBType = DYDATABASETYPE_SQLSERVER;
	//		break;
	//	case CDDT_DB2:
	//		pDBInfo.enumDBType = DYDATABASETYPE_DB2;
	//		break;
	//	case CDDT_ORACLE:
	//		pDBInfo.enumDBType = DYDATABASETYPE_ORACLE;
	//		break;
	//	case CDDT_ODBCDRIVER:
	//		pDBInfo.enumDBType = DYDATABASETYPE_ODBCDRIVER;
	//		break;
	//	case CDDT_ACCESS:
	//		pDBInfo.enumDBType = DYDATABASETYPE_ACCESS;
	//		break;
	//	case CDDT_MYSQL:
	//		pDBInfo.enumDBType = DYDATABASETYPE_MYSQL;
	//		break;
	//	case CDDT_ORACLEHTTP:
	//		pDBInfo.enumDBType = DYDATABASETYPE_ORACLEHTTP;
	//		break;

	//	/*case CDDT_EXCEL:
	//	case CDDT_WINDOWSDOMAIN:
	//	case CDDT_XINCHENUSER:
	//	case CDDT_OTHER:
	//	case CDDT_OTHER2:
	//	case CDDT_OTHER3:
	//	case CDDT_UNKNOW:
	//		pDBInfo.enumDBType = DYDATABASETYPE_UNKNOW;
	//		break;*/
	//	default:
	//		pDBInfo.enumDBType = DYDATABASETYPE_UNKNOW;
	//		break;
	//	}
	//	pDBInfo.strDataSource = strDatabase;
	//	pDBInfo.strPWD = strPWD;
	//	pDBInfo.strServerId = strDataSource;
	//	pDBInfo.strUserId = strUID;
	//	return pDBInfo;
	//}

	//void FromDYRDatabaseInfo(SDYRDataBaseInfo& pDBInfo)
	//{
	//	switch(pDBInfo.enumDBType) {
	//	case DYDATABASETYPE_SQLSERVER:
	//		enumType = CDDT_SQLSERVER;
	//		break;
	//	case DYDATABASETYPE_ORACLE:
	//		enumType = CDDT_ORACLE;
	//		break;
	//	case DYDATABASETYPE_ODBCDRIVER:
	//		enumType = CDDT_ODBCDRIVER;
	//		break;
	//	case DYDATABASETYPE_ACCESS:
	//		enumType = CDDT_ACCESS;
	//		break;
	//	case DYDATABASETYPE_MYSQL:
	//		enumType = CDDT_MYSQL;
	//		break;
	//	case DYDATABASETYPE_DB2:
	//		enumType = CDDT_DB2;
	//		break;
	//	case DYDATABASETYPE_ORACLEHTTP:
	//		enumType = CDDT_ORACLEHTTP;
	//		break;
	//	default:
	//		enumType = CDDT_UNKNOW;
	//	}
	//	strDatabase = pDBInfo.strDataSource;
	//	strPWD = pDBInfo.strPWD;
	//	strDataSource = pDBInfo.strServerId;
	//	strUID = pDBInfo.strUserId;		
	//}
}CDDBCONNECT;

#define OUTPUT_TRACE_NO_USED 0xFFFFFFFF




#endif//DB_COMMON_STRUCT_H_0DD6BA00_ED0B_4960_B28A_4464D190B277__