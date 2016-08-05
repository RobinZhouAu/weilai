// Base641.cpp: implementation of the CBase64 class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
//#include "TestXmlBin.h"
#include "Base64.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#ifdef _USE_HTTP

const char CBase64::cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
/*
** Translation Table to decode (created by author)
*/
const char CBase64::cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";
CBase64::CBase64()
{

}

CBase64::~CBase64()
{

}

BOOL CBase64::Encode(LPBYTE pbufIn, int nLength, LPBYTE &pBufOut, int &nLengthOut)
{
	int nBlocks = (nLength%3 == 0)? (nLength/3) : (nLength/3 + 1);	
	nLengthOut = nBlocks*4;
	pBufOut = new BYTE[nLengthOut + 1];
	pBufOut[nLengthOut] = 0;
	LPBYTE pBuffer = pBufOut;
	unsigned char in[3], out[4];
    int i, len, blocksout = 0;
	
	int nPos = 0;
    while( nPos < nLength )
	{
        len = 0;
        for( i = 0; i < 3; i++ )
		{
            if(nPos < nLength)
			{
				in[i] = pbufIn[nPos]; 
				nPos ++;
                len++;
            }
            else
			{
                in[i] = 0;
            }
        }
        if( len ) 
		{
            encodeblock( in, out, len );
            for( i = 0; i < 4; i++ )
			{
                *pBufOut = out[i]; pBufOut ++;
            }
            //blocksout++;
        }		
	}
	nPos = pBufOut - pBuffer;	
	pBufOut = pBuffer;
	return TRUE;
}


BOOL CBase64::encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
	out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
	
	return TRUE;
}

BOOL CBase64::Decode(LPBYTE pbufIn, int nLength, LPBYTE &pBufOut, int &nLengthOut)
{
	int nBlocks = (nLength%4 == 0)? (nLength/4 ): (nLength/4 + 1);	
	nLengthOut = nBlocks*3;
	pBufOut = new BYTE[nLengthOut+1];
	BYTE *pBuffer = pBufOut;
	
	unsigned char in[4], out[3], v;
    int i, len;
	int nPos = 0;
	BOOL bLast = FALSE;
    while( nPos < nLength  ) 
	{
        for( len = 0, i = 0; i < 4 && nPos < nLength ; i++ ) 
		{
            v = 0;
            while( nPos < nLength && v == 0 ) 
			{
                v = pbufIn[nPos]; nPos ++;
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
                if( v )
				{
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
				if(nPos == nLength)
					bLast = TRUE;
            }    
			if( nPos < nLength || bLast )
			{
                len++; bLast = FALSE;
                if( v ) 
                    in[ i ] = (unsigned char) (v - 1);
            }
            else 
                in[i] = 0;
        }
        if( len )
		{
            decodeblock( in, out );
            for( i = 0; i < len - 1; i++ )
			{
                *pBufOut = out[i]; pBufOut ++;
            }
        }
    }
	nPos = pBufOut - pBuffer;
	if(nPos < nLengthOut)
	{
		nLengthOut = nPos;
	}
	pBufOut = pBuffer;
	pBufOut[nLengthOut] = 0;
	return TRUE;
}

BOOL CBase64::decodeblock( unsigned char in[4], unsigned char out[3] )
{
	out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
	return TRUE;		
}

//#endif