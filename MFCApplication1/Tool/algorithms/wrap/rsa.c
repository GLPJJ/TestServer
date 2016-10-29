#include "rsa.h"
//!������˽Կd����
typedef struct _pKeyset
{
	unsigned int set[MAX_NUM];
	unsigned int size;
}pKeyset;

pKeyset pset;

//! ���湫��˽Կ��
typedef struct _pPairkey
{
	unsigned int d;
	unsigned int e;
	unsigned int n;
} pPairkey;
pPairkey pairkey;

// ���ƣ�isPrime
// ���ܣ��ж��������Ƿ���
//  ������m: ��a; n: ��b
// ���أ�m��n���ʷ���true; ���򷵻�false
//
int isPrime(unsigned int m, unsigned int n)
{
	unsigned int i = 0;
	int Flag = 1;

	if (m < 2 || n<2)
		return 0;

	unsigned int tem = (m > n) ? n : m;
	for (i = 2; i <= tem && Flag; i++)
	{
		int mFlag = 1;
		int nFlag = 1;
		if (m % i == 0)
			mFlag = 0;
		if (n % i == 0)
			nFlag = 0;
		if (!mFlag && !nFlag)
			Flag = 0;
	}
	if (Flag)
		return 1;
	else
		return 0;
}

// ���ƣ�MakePrivatedKeyd
// ���ܣ�������Q��Q����˽Կd
//  ������uiP: ����P; uiQ: ����Q
// ���أ�˽Կd

unsigned int MakePrivatedKeyd(unsigned int uiP, unsigned int uiQ)
{
	unsigned int i = 0;

	//! �õ�������z���ʵ���( ˽Կd�ļ��� )
	unsigned int z = (uiP - 1) * (uiQ - 1);
	pset.size = 0;
	for (i = 0; i < z; i++)
	{
		if (isPrime(i, z))
		{
			pset.set[pset.size++] = i;
		}
	}

	return pset.size;
}

// ���ƣ�MakePairKey
// ���ܣ�����RSA����˽Կ��
//  ������uiP: ����P; uiQ: ����Q; uiD: ˽Կd
// ���أ��������

unsigned int MakePairkey(unsigned int uiP, unsigned int uiQ, unsigned int uiD)
{
	int bFlag = 1;
	unsigned int i = 0, e;
	unsigned int z = (uiP - 1) * (uiQ - 1);
	unsigned int d = pset.set[uiD];
	//d=uiD;

	if (!isPrime(z, d))
		return ERROR_NOEACHPRIME;

	for (i = 2; i < z; i++)
	{
		if ((i*d) % z == 1)
		{
			e = i;
			bFlag = 0;
		}
	}
	if (bFlag)
		return ERROR_NOPUBLICKEY;

	if ((d*e) % z != 1)
		ERROR_GENERROR;

	pairkey.d = d;
	pairkey.e = e;
	pairkey.n = uiP * uiQ;
	return OK;
}

// ���ƣ�GetPairKey
// ���ܣ������ṩ�ӿڣ���ù���˽Կ��
//  ������uiP: ����P; uiQ: ����Q; uiD: ˽Կd
// ���أ�

unsigned int GetPairKey(unsigned int* d, unsigned int* e)
{
	*d = pairkey.d;
	*e = pairkey.e;
	return pairkey.n;
}

// ���ƣ�GetPrivateKeyd
// ���ܣ������ṩ�ӿڣ����û�ѡ��ID����˽Կd
//  ������iWhich: �û�ѡ��˽Կd��ID
// ���أ�˽Կdֵ

unsigned int GetPrivateKeyd(unsigned int iWhich)
{
	if (pset.size >= iWhich)
		return pset.set[iWhich];
	else
		return 0;
}

// ���ƣ�rsa_encrypt
// ���ܣ�RSA��������
//  ������n: ��Կn; e: ��Կe; mw: ��������; iLength: ���ĳ���; cw: �������
// ���أ���

void rsa_encrypt(int n, int e, char *mw, int mLength, int ** cw)
{
	int i = 0, j = 0;
	__int64 temInt = 0;

	for (i = 0; i < mLength; i++)
	{
		temInt = mw[i];
		if (e != 0)
		{
			for (j = 1; j < e; j++)
			{
				temInt = (temInt * mw[i]) % n;
			}
		}
		else
		{
			temInt = 1;
		}

		(*cw)[i] = (int)temInt;
	}
}

// ���ƣ�rsa_decrypt
// ���ܣ�RSA��������
//  ������n: ˽Կn; d: ˽Կd; cw: ����; cLength: ���ĳ���; mw: �������
// ���أ���

void rsa_decrypt(int n, int d, int **cw, int cLength, char *mw)
{
	int i = 0, j = -1;
	__int64 temInt = 0;

	for (i = 0; i < cLength / 4; ++i)
	{
		mw[i] = 0;
		temInt = (*cw)[i];

		if (d != 0)
		{
			for (j = 1; j < d; j++)
			{
				temInt = (__int64)(temInt * (*cw)[i]) % n;
			}
		}
		else
		{
			temInt = 1;
		}

		mw[i] = (char)temInt;
	}
}
void outputkey()
{
	printf("PublicKey(e,n): (%d,%d)\n", pairkey.e, pairkey.n);
	printf("PrivateKey(d,n): (%d,%d)\n", pairkey.d, pairkey.n);
}

/*
//main.c
// ���̣�RSA
// ���ܣ�RSA�ӡ������ļ�
//  ���ߣ�jlcss|ExpNIS


#include <stdio.h>
#include <afxwin.h>
#include <math.h>
#include "rsa.h"

#define DECRYPT_FILE "RSA��������.txt"
#define ENCRYPT_FILE "RSA��������.txt"
//! Լ���ļ����2M
#define MAX_FILE 1024*1024*2

// ���ƣ�usage
// ���ܣ�������Ϣ
//  ������Ӧ�ó�������
// ���أ���ʾ��Ϣ

void Usage(const char *appname)
{
	printf("\n\tusage:rsa -k ����P ����Q\n");
	printf("\tusage: rsa -e �����ļ� ��Կe ��Կn\n");
	printf("\tusage: rsa -d �����ļ� ˽Կd ˽Կn\n");
}

// ���ƣ�IsNumber
// ���ܣ��ж������ַ�����
//  ������strNumber:�ַ�����
// ���أ������������鷵��true�����򷵻�false;

bool IsNumber(const char *strNumber)
{
	unsigned int i;

	if (!strNumber)
		return false;

	for (i = 0; i < strlen(strNumber); i++)
	{
		if (strNumber[i] < '0' || strNumber[i] > '9')
			return false;
	}

	return true;
}

// ���ƣ�IsPrimeNumber
// ���ܣ��ж�����
//  ������num: ��������
// ���أ���������true�����򷵻�false;

bool IsPrimeNumber(unsigned int num)
{
	unsigned int i;
	if (num <= 1)
		return false;

	unsigned int sqr = (unsigned int)sqrt((double)num);
	for (i = 2; i <= sqr; i++)
	{
		if (num % i == 0)
			return false;
	}

	return true;
}

// ���ƣ�FileIn
// ���ܣ���ȡ�����ļ����ڴ�
//  ������strFile:�ļ����ƣ�inBuff:ָ���ļ����ݻ�����
// ���أ�ʵ�ʶ�ȡ���ݴ�С(�ֽ�)

int FileIn(const char *strFile, unsigned char *&inBuff)
{
	int iFileLen = 0, iBuffLen = 0;

	//! �������ļ�
	CFile file(strFile, CFile::modeRead);
	iFileLen = (int)file.GetLength();
	if (iFileLen > MAX_FILE)
	{
		printf("�ļ����Ȳ��ܴ��� %dM,!\n", MAX_FILE / (1024 * 1024));
		goto out;
	}
	iBuffLen = iFileLen;

	inBuff = new unsigned char[iBuffLen];
	if (!inBuff)
		goto out;

	ZeroMemory(inBuff, iBuffLen);

	file.Read(inBuff, iFileLen);
	file.Close();

out:
	return iBuffLen;
}

// ���ƣ�FileOut
// ���ܣ���/���ܽ���������ǰĿ¼�����ļ���
//  ������strOutָ������ַ��������������Сlen��strFileΪ����ļ�
// ���أ���

void FileOut(const void *strOut, int len, const char *strFile)
{
	//! ������ļ�
	CFile outfile(strFile, CFile::modeCreate | CFile::modeWrite);
	outfile.Write(strOut, len);
	outfile.Close();
}

// ���ƣ�CheckParse
// ���ܣ�У��Ӧ�ó�����ڲ���
//  ������argc����main������argc������argvָ��main������argv����
// ���أ��������Ϸ�����true�����򷵻�false
//  ��ע���򵥵���ڲ���У��

bool CheckParse(int argc, char** argv)
{
	bool bRes = false;

	if (argc != 4 && argc != 5)
		goto out;

	if (argc == 4 && argv[1][1] == 'k')
	{
		//! ���ɹ���˽Կ��
		if (!IsNumber(argv[2]) ||
			!IsNumber(argv[3]) ||
			atoi(argv[2]) > MAX_PRIME ||
			atoi(argv[3]) > MAX_PRIME)
			goto out;
	}
	else if ((argc == 5) && (argv[1][1] == 'e' || argv[1][1] == 'd'))
	{
		//! ���ܡ����ܲ���
		if (!IsNumber(argv[3]) ||
			!IsNumber(argv[4]) ||
			atoi(argv[3]) > MAX_NUM ||
			atoi(argv[4]) > MAX_NUM)
			goto out;
	}
	else
		Usage(*argv);
	bRes = true;

out:
	return bRes;
}

// ���ƣ�kOption1
// ���ܣ�����kѡ�������������P��Q����˽Կd����
//  ������uiP: ������ڲ���P; uiQ: ������ڲ���Q
// ���أ�ִ����ȷ��������˽Կ��Ŀ�����򷵻�0

unsigned int kOption1(unsigned int uiP, unsigned int uiQ)
{
	unsigned int uiRes = 0;

	if (!IsPrimeNumber(uiP))
	{
		printf("P�������P����Ϊ(0, %d]����", MAX_PRIME);
		return uiRes;
	}
	if (!IsPrimeNumber(uiQ))
	{
		printf("Q�������Q����Ϊ(0, %d]����", MAX_PRIME);
		return uiRes;
	}
	if (uiP == uiQ)
	{
		printf("����P������Q��ͬ�������׸��ݹ�Կn��ƽ���ó�����P��Q�����ּ��ܲ���ȫ�����������!\n");
		return uiRes;
	}
	printf("��������˽Կd����......\n");
	uiRes = MakePrivatedKeyd(uiP, uiQ);

	return uiRes;
}


����RSA

//! ����������
int main(int argc, char **argv)
{
	unsigned int p, q, d, n, e;//two prime p & q, public key(n, e) , private key(n , d)
	CheckParse(argc, argv);

	d = 4828; //uid
	if (argc == 4)
	{
		p = atoi(argv[2]);
		q = atoi(argv[3]);
		MakePrivatedKeyd(p, q);
		MakePairkey(p, q, d);
		outputkey();
	}
	else if (argc == 5)
	{
		char FileName[20];
		strcpy(FileName, argv[2]);
		int len;
		if (argv[1][1] == 'e')
		{
			unsigned char *inBuffer = (unsigned char *)malloc(MAX_FILE); //���뻺����
			int *cw = (int *)malloc(MAX_FILE);
			len = FileIn(FileName, inBuffer);
			e = atoi(argv[3]);
			n = atoi(argv[4]);
			rsa_encrypt(n, e, (char *)inBuffer, len, cw);
			FileOut(cw, 4 * len, DECRYPT_FILE);
		}
		else if (argv[1][1] == 'd')
		{
			char *Buffer = (char *)malloc(MAX_FILE); //���뻺����
			int *cw = (int *)malloc(MAX_FILE);
			len = FileIn(FileName, (unsigned char *&)cw);
			d = atoi(argv[3]);
			n = atoi(argv[4]);
			rsa_decrypt(n, d, cw, len, Buffer);
			FileOut(Buffer, len / 4, ENCRYPT_FILE);
		}
	}

	return 0;
}
*/
