///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����FileShare.h
// �����ߣ��ܺ���
// ����ʱ�䣺2001/06/28
// ����������������ļ��������궨��
///////////////////////////////////////////////////////////////////////////////////////////
#ifndef _FILESHARE_H_
#define _FILESHARE_H_

////----------------�ļ�����-----------------------------------------------//
////��Ŀϵͳ�ļ�����
//#define FILE_TYPE_ALL			0x00000000		//*.*
//#define FILE_TYPE_CIS			0x00000001		//*.CIS ��Ŀ���������ļ�
//#define FILE_TYPE_LCF			0x00000002		//*.LCF �ֲ������ļ�
//#define FILE_TYPE_OPT			0x00000004		//*.OPT ������ѡ���ļ�
////��Ƶ�ļ�����
//#define FILE_TYPE_AVI			0x00000010
//#define FILE_TYPE_MPG			0x00000020
//#define FILE_TYPE_DAT			0x00000040		
////ͼ���ļ�����
//#define FILE_TYPE_DIB			0x00001000
//#define FILE_TYPE_JPG			0x00002000
////��Ƶ�ļ�����
//#define FILE_TYPE_WAV			0x00100000
//#define FILE_TYPE_MID			0x00200000
////�����ļ�����
//#define FILE_TYPE_TXT			0x01000000
//#define FILE_TYPE_DOC			0x02000000
//#define FILE_TYPE_PDF			0x04000000
//
//#define FILE_TYPE_VIDEO		0x00000FF0		//������Ƶ����
//#define FILE_TYPE_IMAGE		0x000FF000		//����ͼ������
//#define FILE_TYPE_AUDIO		0x00F00000		//������Ƶ����
//#define FILE_TYPE_DOCUMENT	0xFF000000		//���������ĵ�����
//
//#define FILE_TYPE_ALL_FILTER	"�����ļ�(*.*)|*.*|"
//#define FILE_TYPE_CIS_FILTER	"��Ŀ���������ļ�(*.CIS)|*.CIS|"
//#define FILE_TYPE_LCF_FILTER	"��Ŀ�ֲ������ļ�(*.LCF)|*.LCF|"
//#define FILE_TYPE_OPT_FILTER	"������ѡ���ļ�(*.OPT)|*.OPT|"
//
//#define FILE_TYPE_AVI_FILTER	"AVI��Ƶ�ļ�(*.AVI)|*.AVI|"
//#define FILE_TYPE_MPG_FILTER	"MPG��Ƶ�ļ�(*.MPG)|*.MPG|"
//#define FILE_TYPE_DAT_FILTER	"DAT��Ƶ�ļ�(*.DAT)|*.DAT|"
//
//#define FILE_TYPE_DIB_FILTER	"��ѹ��ͼ���ļ�(*.BMP;*.TIF)|*.BMP;*.TIF|"
//#define FILE_TYPE_JPG_FILTER	"ѹ��ͼ���ļ�(*.GIF;*.JPG;*.TGA;*.PCX)|*.GIF;*.JPG;*.PCX;*.TGA|"
//
//#define FILE_TYPE_WAV_FILTER	"��ѹ����Ƶ�ļ�(*.WAV)|*.WAV|"
//#define FILE_TYPE_MID_FILTER	"ѹ����Ƶ�ļ�(*.MID;*.MP3)|*.MID;*.MP3|"
//
//#define FILE_TYPE_TXT_FILTER	"�ı��ļ�(*.TXT)|*.TXT|"
//#define FILE_TYPE_DOC_FILTER	"MS OFFICE �ĵ�(*.DOC;*.XLS;*.MDB)|*.DOC;*.XLS;*.MDB|"
//#define FILE_TYPE_PDF_FILTER	"PDF�ĵ�(*.PDF)|*.PDF|"
//
//
//#define FILEEXT_CIS			".CIS"
//#define FILEEXT_LCF			".LCF"
//
//#define FILEEXT_BMP			".BMP"
//#define FILEEXT_JPG			".JPG"
//
//#define IS_VIDEOFILE(type)		((type & FILE_TYPE_VIDEO) != 0x00000000)
//#define IS_IMAGEFILE(type)		((type & FILE_TYPE_IMAGE) != 0x00000000)
//#define IS_AUDIOFILE(type)		((type & FILE_TYPE_AUDIO) != 0x00000000)
//#define IS_DOCUMENTFILE(type)	((type & FILE_TYPE_DOCUMENT) != 0x00000000)
//
//#define FILETYPE_SYSTEM		0
//#define FILETYPE_VIDEO		1
//#define FILETYPE_IMAGE		2
//#define FILETYPE_AUDIO		3
//#define FILETYPE_DOCUMENT	4
////--------End of �ļ�����-----------------------------------------------//


BOOL DY_EXT_API g_CreateDirectory(LPCTSTR lpszDirectory);
BOOL DY_EXT_API g_EmptyDirectory(LPCTSTR lpszDirectory);
CString DY_EXT_API g_RemovePathFromFileName(LPCTSTR lpszFileName);
CString DY_EXT_API g_GetFilterByType(int nFileType);
ULONGLONG DY_EXT_API g_GetFileSize(LPCTSTR lpszFileName);
BOOL DY_EXT_API g_GetFileTime(LPCTSTR lpszFileName, COleDateTime &odtLastWrite);
void DY_EXT_API g_ReportFileError(CFileException *fe);
CString DY_EXT_API g_GetWin32ErrMsg();					//ȡ��Win32�������Ϣ�ı�
BOOL DY_EXT_API g_IsFileExist(LPCTSTR lpszFileName);
BOOL DY_EXT_API g_SplitFile(CString &strFullFileName, CString &strPath, CString &strFileName);
ULONGLONG DY_EXT_API g_GetDirectorySize(CString strPath, CString strSearch, int& nFileCount, int& nDirCount);
ULONGLONG DY_EXT_API g_GetDiskFreeSpace(CString strDriverPath);
// add by zjq
BOOL DY_EXT_API g_FileCanbeDel(CString strFileName);
#endif _FILESHARE_H_	