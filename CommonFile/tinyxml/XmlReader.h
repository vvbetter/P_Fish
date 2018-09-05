#pragma once
#include <list>
#include "tinyxml2.h"
using namespace tinyxml2;
class WHXml
{
	friend class WHXmlNode;
public:
	WHXml();
	virtual ~WHXml();
	virtual bool LoadXMLFilePath(const TCHAR * FilePath);//��ȡָ��·����XML�ļ� ����ʹ��TCHAR
	virtual WHXmlNode* GetChildNodeByName(const TCHAR* NodeName);//��ȡ���ڵ� ��������
private:
	void ActionXmlNode(WHXmlNode* pNode);
private:
	//����
	tinyxml2::XMLDocument*					m_XmlDoc;//XML �ĵ�����
	std::list<WHXmlNode*>					m_ActionNode;//�Ѿ������˵Ľڵ�
};
class WHXmlNode
{
	friend class WHXml;
public:
	WHXmlNode();
	virtual ~WHXmlNode();

	WHXmlNode* GetNextSignelNode();//��ȡ��һ����ͬ���ƵĽڵ�
	WHXmlNode* GetChildNodeByName(const TCHAR* NodeName);//��ȡ�ӽڵ� ��������

	//��ȡ���Եĺ���
	//int8 int16 int32 int64
	bool			GetAttribute(const TCHAR* AttributeName, __int64& nInt64Value);
	bool			GetAttribute(const TCHAR* AttributeName, __int32& nInt32Value);
	bool			GetAttribute(const TCHAR* AttributeName, __int16& nInt16Value);
	bool			GetAttribute(const TCHAR* AttributeName, __int8& nInt8Value);
	//uint8 uint16 uint32 uint64
	bool			GetAttribute(const TCHAR* AttributeName, unsigned __int64& nUInt64Value);
	bool			GetAttribute(const TCHAR* AttributeName, unsigned __int32& nUInt32Value);
	bool			GetAttribute(const TCHAR* AttributeName, unsigned __int16& nUInt16Value);
	bool			GetAttribute(const TCHAR* AttributeName, unsigned __int8& nUInt8Value);
	bool			GetAttribute(const TCHAR* AttributeName, DWORD& nDWORDValue);
	//float double
	bool			GetAttribute(const TCHAR* AttributeName, double& ndoubleValue);
	bool			GetAttribute(const TCHAR* AttributeName, float&  nfloatValue);
	//char          CHAR ����Int8
	//bool			GetAttribute(const TCHAR* AttributeName, char& nCharValue);
	//TCHAR[]
	bool			GetAttribute(const TCHAR* AttributeName, TCHAR* nStrValue, size_t Size);
private:
	void SetXmlNode(tinyxml2::XMLElement* nNode, WHXml* nXml);
private:
	tinyxml2::XMLElement*						m_XmlNode;//�ڵ�
	WHXml*										m_pXml;//XML�ĵ�
};