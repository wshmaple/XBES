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
	// 应该会有 多个 不同 IP的灯光界面
	CMsgLog m_msgLogLamp; // 灯光界面


	// 应该会有 多个 不同 IP的楼宇界面
	CMsgLog m_msgLogCross; // 楼宇界面数据
	
	CMsgFactoryControl m_msgFC;// 工厂消息管理类



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

	void DrawLoginDlg();// 绘制 登陆窗口

	void DrawLay(); // 绘制浮动窗口

	void ShowMark(int a,int b,int c,int d); // 显示图表

	bool m_bHasLogin;

	long m_dwBeginTm; // 系统开始时间的时间戳
	char m_cBeginTm[64]; // 系统登陆成功之后开始时间
};

