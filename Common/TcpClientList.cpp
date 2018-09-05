#include "stdafx.h"
#include "TcpClientList.h"
TcpClientList::TcpClientList()
{
	m_SendIndex = 0;
}
TcpClientList::~TcpClientList()
{
	if (m_TcpClientMap.empty())
		return;
	delete[] m_GetCmdArray;
	vector<TcpClientOnce*>::iterator Iter = m_TcpClientMap.begin();
	for (; Iter != m_TcpClientMap.end(); ++Iter)
	{
		delete *Iter;
	}
	m_TcpClientMap.clear();
}
bool TcpClientList::Init(std::vector<TcpClientConnectOnce>& pList)
{
	if (pList.empty())
	{
		ASSERT(false);
		return false;
	}
	std::vector<TcpClientConnectOnce>::iterator Iter = pList.begin();
	for (; Iter != pList.end(); ++Iter)
	{
		//����һ���ڵ�
		TcpClientOnce* pOnce = new TcpClientOnce();
		pOnce->TcpID = Iter->TcpID;
		pOnce->m_TcpStates = false;
		if (!pOnce->m_Tcp.Init(Iter->IpStr, Iter->ClientData))
		{
			delete pOnce;
			return false;
		}
		pOnce->m_TcpStates = true;
		m_TcpClientMap.push_back(pOnce);//��������
		//OnTcpClientConnect(&pOnce->m_Tcp);//�����ӳɹ���ʱ��
	}
	if (!m_TcpClientMap.empty())
	{
		m_GetCmdArray = new NetCmd*[m_TcpClientMap.size()];
	}
		
	return true;
}
bool TcpClientList::Send(NetCmd *pCmd, bool bNoneUse)
{
	if (!IsConnected())
	{
		ASSERT(false);
		return false;
	}
	//���������ȥ ��ȫ���Ѿ������˵�����ѡ��һ�����з���
	//��ǰ�и���¼ ���ǰ���¼���з���
	if (m_SendIndex >= m_TcpClientMap.size())
		m_SendIndex = m_SendIndex%m_TcpClientMap.size();//������������
	if (!m_TcpClientMap[m_SendIndex]->m_TcpStates || !m_TcpClientMap[m_SendIndex]->m_Tcp.IsConnected())
	{
		DWORD NowIndex = m_SendIndex;
		//����ѭ����
		++m_SendIndex;
		if (m_SendIndex >= m_TcpClientMap.size())
			m_SendIndex = m_SendIndex%m_TcpClientMap.size();
		while (m_SendIndex != NowIndex)
		{
			if (!m_TcpClientMap[m_SendIndex]->m_TcpStates || !m_TcpClientMap[m_SendIndex]->m_Tcp.IsConnected())
			{
				++m_SendIndex;
				if (m_SendIndex >= m_TcpClientMap.size())
					m_SendIndex = m_SendIndex%m_TcpClientMap.size();
				continue;
			}
			//���ӵ� 
			m_TcpClientMap[m_SendIndex]->m_Tcp.Send(pCmd, bNoneUse);
			return true;
		}
		ASSERT(false);
		return false;
	}
	else
	{
		m_TcpClientMap[m_SendIndex]->m_Tcp.Send(pCmd, bNoneUse);//������ͳ�ȥ
		++m_SendIndex;
		return true;
	}
}
void TcpClientList::Shutdown()
{
	vector<TcpClientOnce*>::iterator Iter = m_TcpClientMap.begin();
	for (; Iter != m_TcpClientMap.end(); ++Iter)
	{
		(*Iter)->m_Tcp.Shutdown();
	}
}
bool TcpClientList::IsConnected()
{
	vector<TcpClientOnce*>::iterator Iter = m_TcpClientMap.begin();
	for (; Iter != m_TcpClientMap.end(); ++Iter)
	{
		if((*Iter)->m_TcpStates)
			return true;
	}
	return false;
}
NetCmd** TcpClientList::CheckAllTcpClient(bool CheckNetStates)//��ϵ�����״̬
{
	//vector<NetCmd*> pVec;
	ZeroMemory(m_GetCmdArray, sizeof(NetCmd*)*m_TcpClientMap.size());
	vector<TcpClientOnce*>::iterator Iter = m_TcpClientMap.begin();
	for (int i =0; Iter != m_TcpClientMap.end(); ++Iter)
	{
		if (CheckNetStates)
		{
			if ((*Iter)->m_Tcp.IsConnected())
			{
				if (!(*Iter)->m_TcpStates)
				{
					//�������ӳɹ���
					(*Iter)->m_TcpStates = true;
					std::cout << "TCPClientList һ���������ӳɹ���\n";
				}
				//��ȡ����
				NetCmd* pCmd = (*Iter)->m_Tcp.GetCmd();
				if (!pCmd)
					continue;
				//pVec.push_back(pCmd);
				m_GetCmdArray[i] = pCmd;
				++i;
			}
			else
			{
				if ((*Iter)->m_TcpStates)
				{
					//ʧȥ������
					(*Iter)->m_TcpStates = false;
					std::cout << "TCPClientList һ�����ӶϿ�������\n";
				}
			}
		}
		else
		{
			NetCmd* pCmd = (*Iter)->m_Tcp.GetCmd();
			if (!pCmd)
				continue;
			//pVec.push_back(pCmd);
			m_GetCmdArray[i] = pCmd;
			++i;
		}
	}
	return m_GetCmdArray;
}
