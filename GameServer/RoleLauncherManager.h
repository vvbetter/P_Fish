//�����̨���� Ҳ�漰�����ʵĿ���
//�����̨ ����Ʒ��� Ҳ��VIP��� Ҳ���¿����
#pragma once
class CRoleEx;
class CConfig;
class RoleLauncherManager
{
public:
	RoleLauncherManager();
	virtual ~RoleLauncherManager();
	bool			OnInit(CRoleEx* pRole);//����ʼ����ʱ�� Ҳ����ȫ����Ʒ�Ѿ���ȡ�ɹ��� ���ǽ���̨����������������
	bool			IsCanUserLauncherByID(BYTE LauncherID);//�Ƿ����ʹ��ָ������̨ ÿ�ο��ڶ���Ҫ�����жϵ�
private:
	CRoleEx*		m_pRole;
	CConfig*		m_pConfig;
	DWORD			m_LauncherStates;//��̨��״̬����
};