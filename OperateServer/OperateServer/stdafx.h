
#pragma once
// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "targetver.h"
#define CLIENT_CMD_SIZE 10240
#define MAX_SEND_CMD_COUNT 10240
#define USE_ACCEPT_RECV_DATA 1
#include "Common.h"


//��Ӫ�������Ĵ������ �Ϳͻ��� һһ��Ӧ

enum RoleOperateError //��Ӫ�������Ĵ���ö��
{
	//ʵ����֤
	ROE_RealNameVerification_Sucess = 1,//ʵ����֤�ɹ�
	ROE_RealNameVerification_IDENTITY_IsBeUse,//���֤��ʹ����
	ROE_RealNameVerification_IDENTITY_Error,//���֤��ʽ����
	ROE_RealNameVerification_IDENTITY_NotName,//���֤��ʵ�����Ʋ�����
	ROE_RealNameVerification_TimeOut,//ʵ����֤��ʱ
	ROE_RealNameVerification_IsExists,//ʵ����֤�ظ��ύ

	//�ֻ���
	ROE_PhoneVerificationNum_Sucess = 50,//��ȡ��֤��ɹ�
	ROE_PhoneVerificationNum_PhoneError,//�绰�������
	ROE_PhoneVerificationNum_PhoneBeUse,//�绰���뱻ʹ����
	ROE_PhoneVerificationNum_IsExists,//�ظ��ύ
	ROE_PhoneVerificationNum_WriteSend,//�ύ���ֻ��ɹ� �ȴ����ŷ���
	ROE_BindPhone_Sucess,//���ֻ��ɹ�
	ROE_BindPhone_BindNumError,//��֤�����
	ROE_BindPhone_BindTimeOut,//��֤�볬ʱ
	ROE_BindPhone_SendSmsInfoError,//������֤�����

	//�����
	ROE_BindMail_Sucess = 100,//������ɹ�
	ROE_BindMail_IsBeUse,//������ʹ���� �޷���
	ROE_BindPhone_MailError,//��������޷���

	//����ʵ����Ʒ
	ROE_BuyEntityItem_Sucess = 150,
	ROE_BuyEntityItem_Failed,

	//���ѳ�ֵ
	ROE_PhonePay_Sucess = 200,
	ROE_PhonePay_System,//�ڲ�����
	ROE_PhonePay_HttpError,//�ⲿ����
};