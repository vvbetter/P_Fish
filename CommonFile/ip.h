#define STR_BUFF_SIZE 128

//IP��ѯ����ṹ��
typedef struct _IpLocation
{
	unsigned int IpStart;
	unsigned int IpEnd;
	char  Country[40];
	char  City[40];
	_IpLocation()
	{
		memset(this,0,sizeof(IpLocation));
	}
} IpLocation;

//IP�ַ��� -> IP��ֵ
extern unsigned int IpStringToInt(const char * ipStr);

//IP��ֵ -> IP�ַ���
extern char * IntToIpString(int ipv);

//��ѯIP�κ����ڵ�
extern IpLocation GetIpLocation(unsigned int ipv);

//���ݵ�ַ����IP��
extern void IpSearch(const char * fnData, const char * fnOut);

//��ѹ����
extern void DataCompress(const char * fnData, const char * fnOut);

//2. IpLocator.cpp

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")//Ӧ�ü��ϸþ�Ϳ���



//#include "IpLocator.h"

//IP�ַ��� -> IP��ֵ
unsigned int IpStringToInt(const char * ipStr)
{
	int t[4];
	unsigned int ipv;
	unsigned char * p=(unsigned char *)&ipv;
	sscanf_s(ipStr,"%d.%d.%d.%d",&t[0],&t[1],&t[2],&t[3]);
	p[0] = ConvertIntToBYTE(t[3]); p[1] = ConvertIntToBYTE(t[2]); p[2] = ConvertIntToBYTE(t[1]); p[3] = ConvertIntToBYTE(t[0]);
	return ipv;
}
//IP��ֵ -> IP�ַ���
char * IntToIpString(int ipv)
{
	unsigned char * p=(unsigned char *)&ipv;
	static char ipStr[sizeof("xxx.xxx.xxx.xxx")];
	sprintf_s(ipStr,"%d.%d.%d.%d",p[3],p[2],p[1],p[0]);
	return ipStr;
}
// ���á����ַ�������������, ��λIP������¼λ��
int getIndexOffset(FILE * fp, int fo, int lo, unsigned int ipv)
{
	int mo;    //�м�ƫ����
	unsigned int mv;    //�м�ֵ
	unsigned int fv,lv; //�߽�ֵ
	unsigned int llv;   //�߽�ĩĩֵ
	fseek(fp,fo,SEEK_SET);
	fread(&fv,4,1,fp);
	fseek(fp,lo,SEEK_SET);
	fread(&lv,4,1,fp);
	//��ʱ������,ĩ��¼��ƫ����
	fread(&mo,3,1,fp); mo&=0xffffff;
	fseek(fp,mo,SEEK_SET);
	fread(&llv,sizeof(int),1,fp);
	//printf("%d %d %d %d %d %d %d \n",fo,lo,mo,ipv,fv,lv,llv);
	//�߽��⴦��
	if(ipv<fv)
		return -1;
	else if(ipv>llv)
		return -1;
	//ʹ��"���ַ�"ȷ����¼ƫ����
	do
	{
		mo=fo+(lo-fo)/7/2*7;
		fseek(fp,mo,SEEK_SET);
		fread(&mv,sizeof(int),1,fp);
		if(ipv>=mv)
			fo=mo;
		else
			lo=mo;
		if(lo-fo==7)
			mo=lo=fo;
	} while(fo!=lo);
	return mo;
}
// ��ȡIP���ڵ��ַ���
char * getString(char * strBuf, FILE * fp)
{
	//byte Tag;
	//int Offset;
	//Tag=fp.ReadByte();
	char tag;
	int so;
	fread(&tag,sizeof(char),1,fp);

	if(tag==0x01)   // �ض���ģʽ1: ������Ϣ�������Ϣ����
	{
		fread(&so,3,1,fp); so&=0xffffff;
		fseek(fp,so,SEEK_SET);
		return getString(strBuf,fp);
	}
	else if(tag==0x02)   // �ض���ģʽ2: ������Ϣû���������Ϣ����
	{
		fread(&so,3,1,fp); so&=0xffffff;
		//�����ļ���ǰ��λ��
		int tmo=ftell(fp);
		fseek(fp,so,SEEK_SET);
		getString(strBuf,fp);
		fseek(fp,tmo,SEEK_SET);
		return strBuf;
	}
	else   // ���ض���: ���ģʽ
	{
		fseek(fp,-1,SEEK_CUR);
		//��ȡ�ַ���
		fread(strBuf,sizeof(char),STR_BUFF_SIZE,fp);
		//�����ļ�ָ��
		fseek(fp,strlen(strBuf)+1-STR_BUFF_SIZE,SEEK_CUR);
		return strBuf;
	}
}

//��ѯIP�κ����ڵ�
IpLocation GetIpLocation(unsigned int ipv)
{
	char szPath[MAX_PATH]={0};
	GetModuleFileNameA(NULL, szPath, sizeof(szPath));
	PathRemoveFileSpecA(szPath);
	PathAppendA(szPath,("qqwry.dat"));
	FILE * fp = null;
	errno_t Error= fopen_s(&fp, szPath, "rb");
	if (Error!= 0  || !fp)
	{
		throw "�������ļ�ʧ��";
	}
	int fo,lo;   //��ĩ����ƫ����
	fread(&fo,sizeof(int),1,fp);
	fread(&lo,sizeof(int),1,fp);
	//��ȡ������¼ƫ����
	int rcOffset=getIndexOffset(fp,fo,lo,ipv);
	fseek(fp,rcOffset,SEEK_SET);
	IpLocation ipl;
	if(rcOffset>=0)
	{
		fseek(fp,rcOffset,SEEK_SET);
		//��ȡ��ͷIPֵ
		fread(&ipl.IpStart,sizeof(int),1,fp);
		//ת����¼��
		int ro;   //��¼��ƫ����
		fread(&ro,3,1,fp); ro&=0xffffff;
		fseek(fp,ro,SEEK_SET);
		//��ȡ��βIPֵ
		fread(&ipl.IpEnd,sizeof(int),1,fp);
		char strBuf[STR_BUFF_SIZE];
		getString(strBuf,fp);
		//ipl.Country=new char[strlen(strBuf+1)];
		if(strlen(ipl.Country)+strlen(strBuf)<40)
		{
			strcpy_s(ipl.Country, CountArray(ipl.Country),strBuf);
		}
		getString(strBuf,fp);
		//ipl.City=new char[strlen(strBuf+1)];
		if(strlen(ipl.City)+strlen(strBuf)<40)
		{
			strcpy_s(ipl.City, CountArray(ipl.City), strBuf);
		}
	}
	else
	{
		//û�ҵ�
		ipl.IpStart=0;
		ipl.IpEnd=0;
		strcpy_s(ipl.Country, CountArray(ipl.Country), "δ֪����");
		strcpy_s(ipl.City, CountArray(ipl.City), "δ֪��ַ");

		//ipl.Country="δ֪����";
		//ipl.City="δ֪��ַ";
	}
	fclose(fp);
	return ipl;
}

//��ѹ����
void DataCompress(const char * fnData, const char * fnOut)
{
	FILE * fp = null;
	errno_t Error = fopen_s(&fp, fnData, "rb");
	if (Error != 0 || !fp)
	{
		throw "�������ļ�ʧ��";
	}
	FILE * fpo = null;
	Error = fopen_s(&fp, fnOut, "w");
	//���û�д��ļ����������Ļ
	if (Error != 0 || !fpo)
		fpo=stdout;
	int fo,lo,mo,ro;
	unsigned int ipStart,ipEnd;
	char country[STR_BUFF_SIZE];
	char city[STR_BUFF_SIZE];
	char sStart[sizeof("xxx.xxx.xxx.xxx")];
	char sEnd[sizeof("xxx.xxx.xxx.xxx")];
	fread(&fo,sizeof(int),1,fp);
	fread(&lo,sizeof(int),1,fp);
	int rCount=0;   //��¼����
	for(mo=fo;mo<=lo;mo+=7)
	{
		fseek(fp,mo,SEEK_SET);
		fread(&ipStart,sizeof(int),1,fp);
		fread(&ro,3,1,fp); ro&=0xffffff;
		fseek(fp,ro,SEEK_SET);
		fread(&ipEnd,sizeof(int),1,fp);
		getString(country,fp);
		getString(city,fp);
		//��IPֵת��Ϊ�ַ���
		strcpy_s(sStart, CountArray(sStart),IntToIpString(ipStart));
		strcpy_s(sEnd, CountArray(sEnd), IntToIpString(ipEnd));
		fprintf(fpo,"%s - %s %s %s\n",sStart,sEnd,country,city);
		rCount++;
	}
	fprintf(fpo,"\n���ݿ��¼����: %d\n",rCount);
	fclose(fp);
	if(fpo!=stdout)
		fclose(fpo);
}

void IpLocating(const char * ipStr,char *result)
{
	unsigned int ipv=IpStringToInt(ipStr);
	IpLocation ipl;
	ipl=GetIpLocation(ipv);
	char ipStart[sizeof("xxx.xxx.xxx.xxx")];
	strcpy_s(ipStart, CountArray(ipStart),IntToIpString(ipl.IpStart));
	char ipEnd[sizeof("xxx.xxx.xxx.xxx")];
	strcpy_s(ipEnd, CountArray(ipEnd), IntToIpString(ipl.IpEnd));

	if(strlen(result)+strlen(ipl.Country)<40)
	{
		strcpy_s(result, CountArray(result), ipl.Country);
	}
	if(strlen(result)+strlen(ipl.City)<40)
	{
		strcpy_s(result, CountArray(result), ipl.City);
	}
	//printf("IP��ַ��: %s - %s\n",ipStart,ipEnd);
	//printf("IP���ڵ�: %s %s\n",ipl.Country,ipl.City);
}
void IpLocating(DWORD ipv, char *result,DWORD ArrayLength)
{
	IpLocation ipl;
	ipl = GetIpLocation(ipv);
	char ipStart[sizeof("xxx.xxx.xxx.xxx")];
	strcpy_s(ipStart, CountArray(ipStart), IntToIpString(ipl.IpStart));
	char ipEnd[sizeof("xxx.xxx.xxx.xxx")];
	strcpy_s(ipEnd, CountArray(ipEnd), IntToIpString(ipl.IpEnd));

	if (strlen(result) + strlen(ipl.Country)<MAX_ADDRESS_LENGTH_IP)
	{
		strncpy_s(result, ArrayLength, ipl.Country, CountArray(ipl.Country));
	}
	if (strlen(result) + strlen(ipl.City)<MAX_ADDRESS_LENGTH_IP)
	{
		strncat_s(result, ArrayLength, ipl.City, CountArray(ipl.City));
	}
}

