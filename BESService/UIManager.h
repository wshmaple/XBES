/**
 * @file UIManager.h.
 *
 * Declares the manager class.
 */

#pragma once

/**
 * A manager.
 *
 * @author WSH
 * @date 2018/9/3
 */
#include "CMsgFactoryControl.h"
class UIManager
{
public:

	/**
	 * Default constructor.
	 *
	 * @author WSH
	 * @date 2018/9/3
	 */

	UIManager();

	/**
	 * Destructor.
	 *
	 * @author WSH
	 * @date 2018/9/3
	 */

	~UIManager();

	/**
	 * Initializes this object.
	 *
	 * @author WSH
	 * @date 2018/9/3
	 *
	 * @return An int.
	 */

	int UIInit();

	void SetBkImag();
	// Ӧ�û��� ��� ��ͬ IP�ĵƹ����
	CMsgLog m_msgLogLamp; // �ƹ����


	// Ӧ�û��� ��� ��ͬ IP��¥�����
	CMsgLog m_msgLogCross; // ¥���������
	
	CMsgFactoryControl m_msgFC;// ������Ϣ������



	//************************************
	// Method:    DrawMsgUI
	// FullName:  UIManager::DrawMsgUI
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: CMsgLog & msglog
	// Parameter: string strTitle
	//************************************
//	void DrawMsgUI(CMsgLog &msglog,string strTitle, bool *bOpen);

//	void DrawMsgFC();
	void DrawMsgFC2();

	void DrawLoginDlg();// ���� ��½����

	void DrawLay(); // ���Ƹ�������

	void ShowMark(int a,int b,int c,int d); // ��ʾͼ��

	bool m_bHasLogin;

	long m_dwBeginTm; // ϵͳ��ʼʱ���ʱ���
	char m_cBeginTm[64]; // ϵͳ��½�ɹ�֮��ʼʱ��
};

