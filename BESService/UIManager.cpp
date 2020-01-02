#include "stdafx.h"
#include "UIManager.h"
#include "gui/src/imgui_memory_editor.h"
#include "resource.h"

UIManager::UIManager()
{
	m_dwBeginTm = 0;
	memset(m_cBeginTm, 64, 0);
}


UIManager::~UIManager()
{
}



#include "gui/src/imgui.h"
#include "gui/src/imgui_impl_dx9.h"
#include "gui/src/imgui_impl_win32.h"


#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <wtypes.h>
#include "gui/src/imgui_dock.h"


//#include "Log4Manager.h"

static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

inline std::string ToUTF8(const std::string str)
{
	int nw_len = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pw_buf = new wchar_t[nw_len + 1];
	memset(pw_buf, 0, nw_len * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pw_buf, nw_len);

	int len = WideCharToMultiByte(CP_UTF8, 0, pw_buf, -1, NULL, NULL, NULL, NULL);

	char* utf8_buf = (char*)malloc(len + 1);
	memset(utf8_buf, 0, len + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pw_buf, nw_len, utf8_buf, len, NULL, NULL);

	std::string outstr(utf8_buf);

	delete[] pw_buf;
	//delete[] utf8_buf;
	free(utf8_buf);
	utf8_buf = NULL;

	return outstr;
}
char* UnicodeToUtf8(const wchar_t* unicode)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = (char*)malloc(len + 1);
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
	return szUtf8;
}
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

int UIManager :: UIInit() {
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("BESService"), NULL };
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(_T("BESService"), _T("BESService"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	LPDIRECT3D9 pD3D;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		UnregisterClass(_T("BESService"), wc.hInstance);
		return 0;
	}
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // Present without vsync, maximum unthrottled framerate

	// Create the D3DDevice
	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
	{
		pD3D->Release();
		UnregisterClass(_T("BESService"), wc.hInstance);
		return 0;
	}

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	ImFont* font = io.Fonts->AddFontFromFileTTF("./fonts/simhei.TTF", 14.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

	//ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/msyh.ttc", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	IM_ASSERT(font != NULL);

	bool show_demo_window = false;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);
	while (msg.message != WM_QUIT)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static bool bshow1 = true;
		static bool bshow2 = true;

	//	ImGui::Text("I'm Wubuguidddd!");
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);


		//////
		//if (ImGui::Begin("Dock Demo"))
		//{
		//	// dock layout by hard-coded or .ini file
		//	ImGui::BeginDockspace();

		//	if (ImGui::BeginDock("Dock 1")) {
		//		ImGui::Text("I'm Wubugui!");
		//	}
		//	ImGui::EndDock();

		//	if (ImGui::BeginDock("Dock 2")) {
		//		ImGui::Text("I'm BentleyBlanks!");
		//	}
		//	ImGui::EndDock();

		//	if (ImGui::BeginDock("Dock 3")) {
		//		ImGui::Text("I'm LonelyWaiting!");
		//	}
		//	ImGui::EndDock();

		//	ImGui::EndDockspace();
		//}
		//ImGui::End();

	//	DrawMainUILamp();

	//	DrawMainUICross();

	//	DrawMsgUI(m_msgLogLamp,"Lamp",NULL);
		//DrawMsgUI(m_msgLogCross,"Cross",NULL);
	//	DrawMsgFC();
		
		if (!m_bHasLogin)
		{
			DrawLoginDlg();
		}
		else {
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("Main"))
				{
					//ShowExampleMenuFile();
					if (ImGui::MenuItem("MainShow", "", bshow1)) { bshow1 = !bshow1; }
					if (ImGui::MenuItem("SysShow", "", bshow2)) { bshow2 = !bshow2; }
				//	if (ImGui::MenuItem("DemoShow", "", show_demo_window)) { show_demo_window = !show_demo_window; }
					ImGui::EndMenu();
				}
				// if (ImGui::BeginMenu("Edit"))
				// {
				// 	if (ImGui::MenuItem("Undo", "CTRL+Z", show_demo_window)) {
				// 		show_demo_window = !show_demo_window;
				// 	}
				// 	if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				// 	ImGui::Separator();
				// 	if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				// 	if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				// 	if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				// 	ImGui::EndMenu();
				// }
				ImGui::EndMainMenuBar();
			}
			if (bshow1)
			{
				DrawMsgFC2();
			}
			if (bshow2)
			{
				DrawLay();
			}
			
			
		}
		
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	/*	{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			string strLL = "中文测试一下下吧哈哈哈 - 逻辑-理类";
			string strUt = ToUTF8(strLL);
			//	ImGui::Text(u8"显示中文");
			ImGui::Text(strUt.c_str());


			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}*/

		// 3. Show another simple window.
		//if (show_another_window)
		//{
		//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		//	ImGui::Text("Hello from another window!");
		//	if (ImGui::Button("Close Me"))
		//		show_another_window = false;
		//	ImGui::End();
		//}

		// Rendering



		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);


		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_pd3dDevice->Reset(&g_d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (pD3D) pD3D->Release();
	DestroyWindow(hwnd);
	UnregisterClass(_T("ImGui Example"), wc.hInstance);
	return 0;
}




void UIManager::SetBkImag()
{
	ImGuiIO& io = ImGui::GetIO();
	//设置窗口位置
	ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
	//设置窗口的大小
	ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
	//设置窗口为透明
	ImGui::SetNextWindowBgAlpha(0);
	//纹理ID
	static ImTextureID bg_tex_id = 0;
	if (!bg_tex_id)
	{
		//这里使用opencv加载图片当然你也可以使用其他方式加载图片
		//loadTexture是一个自定义的函数用于图片字符加载为纹理，不知道怎么加载纹理的可以自行百度
	//	bg_tex_id = (GLuint *)loadTexture(cvLoadImage("Template/bg2.jpg"));
	}
	//设置窗口的padding为0是图片控件充满窗口
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	//设置窗口为无边框
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	//创建窗口使其固定在一个位置
	ImGui::Begin("背景", NULL, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar);
	ImGui::Image(bg_tex_id, ImGui::GetContentRegionAvail());
	ImGui::End();
	ImGui::PopStyleVar(2);
}


void UIManager::DrawMsgFC2()
{
	
	ImGui::Begin("BesServices");
	//绘制工厂
	map<std::string, CMsgFactory>::iterator iter;
	map<string, CMsgLog>::iterator iter2;
	for (iter = m_msgFC.m_map.begin(); iter != m_msgFC.m_map.end(); iter++) {
		string strType; // 类型
		strType = iter->first;
		if (ImGui::CollapsingHeader(strType.c_str()))
		{
			for (iter2 = iter->second.m_map.begin(); iter2 != iter->second.m_map.end(); iter2++)
			{
				string strIP;
				strIP = iter2->first;
				string strCaption;
				strCaption = strType + "_" + strIP;
				ImGui::Checkbox(strCaption.c_str(), &iter2->second.m_bShow);
				ImGui::SameLine();
				ImGui::Text("[%d/%d][%d/%d]",iter2->second.m_nInCount,iter2->second.m_nOutCount, iter2->second.m_nOkCount, iter2->second.m_nErrorCount);
				ImGui::SameLine();
				ShowMark(iter2->second.m_nInCount, iter2->second.m_nOutCount, iter2->second.m_nOkCount, iter2->second.m_nErrorCount);
				if (iter2->second.m_bShow)
				{
					//DrawMsgUI(iter2->second, strType + "_" + strIP, &iter2->second.m_bShow);
					string strTitle = strType + "_" + strIP;
					ImGui::SetNextWindowSizeConstraints(ImVec2(800, 600), ImVec2(2800, 2600));
					// 绘制 表格
					if (ImGui::Begin(strTitle.c_str(), &(iter2->second.m_bShow))
						)
					{
						// dock layout by hard-coded or .ini file
						ImGui::SetNextDock(strTitle.c_str(), ImGuiDockSlot_Top);
						ImGui::BeginDockspace();
						string strMain1 = strTitle + "-Main";
						if (ImGui::BeginDock(strMain1.c_str())) {
							ImGui::Columns(6, "mycolumns"); // 4-ways, with border
							ImGui::Separator();
							ImGui::Text("ID"); ImGui::NextColumn();
							ImGui::Text("State"); ImGui::NextColumn();
							ImGui::Text("NO. of Use(ok/error)"); ImGui::NextColumn();
							ImGui::Text("Name"); ImGui::NextColumn();
							ImGui::Text("LastTimeWeb"); ImGui::NextColumn();
							ImGui::Text("LastTimeReturn"); ImGui::NextColumn();
							ImGui::Separator();
							static int selected = -1;
							ImVec4 colorgray = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
							ImVec4 colorGreen = ImVec4(0.05f, 0.95f, 0.02f, 1.00f);
							ImVec4 colorTmp;
							for (UINT i = 0; i < iter->second.m_mapNames.size(); i++) {
								string strLL = iter->second.m_mapNames[i];
								string strUt = ToUTF8(strLL);//ImVec4(0.46f, 0.54f, 0.80f, 1.00f);

								char label[32];
								sprintf_s(label, "%04d", i);
								if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
									selected = i;
								bool hovered = ImGui::IsItemHovered();
								ImGui::NextColumn();
								colorTmp = iter2->second.m_mapStates[i] == 1 ? colorGreen : colorgray;
								ImGui::Text("%d", iter2->second.m_mapStates[i]); ImGui::SameLine(); ImGui::ColorButton("State", *(ImVec4*)&colorTmp, ImGuiColorEditFlags_NoTooltip); ImGui::NextColumn();
								ImGui::Text("%d(%d/%d)", iter2->second.m_mapCounts[i], iter2->second.m_mapRtOkCount[i], iter2->second.m_mapRtErrorCount[i]); ImGui::NextColumn();
								ImGui::Text(strUt.c_str()); ImGui::NextColumn();
								ImGui::Text("%s", iter2->second.m_mapSecond[i].c_str()); ImGui::NextColumn();
								ImGui::Text("%s", iter2->second.m_mapSecond2[i].c_str()); ImGui::NextColumn();
							}
						}
						ImGui::EndDock();
						ImGui::EndDockspace();

						ImGui::SetNextDock(strTitle.c_str(), ImGuiDockSlot_Bottom);
						ImGui::BeginDockspace();
						string strMain2 = strTitle + "-web";
						if (ImGui::BeginDock(strMain2.c_str())) {
							string strLL = iter2->second.m_strLastMsgFormWebTime;
							string strUt = ToUTF8(strLL);
							ImGui::TextWrapped(strUt.c_str());

							string strLLMethod = iter2->second.m_strMethodName;
							string strUtMethod = ToUTF8(strLLMethod);
							ImGui::Text(strUtMethod.c_str());

							strUt = ToUTF8(iter2->second.m_strLastMsgFromWeb);
							ImGui::TextWrapped(strUt.c_str());

						}
						ImGui::EndDock();
						ImGui::EndDockspace();

						ImGui::SetNextDock(strTitle.c_str(), ImGuiDockSlot_Right);

						ImGui::BeginDockspace();
						string strMain3 = strTitle + "-mac";
						if (ImGui::BeginDock(strMain3.c_str())) {

							string strLL = iter2->second.m_strLastMsgFromMacTime;
							string strUt = ToUTF8(strLL);
							ImGui::Text(strUt.c_str());


							string strLLMethod = iter2->second.m_strMethodName;
							string strUtMethod = ToUTF8(strLLMethod);
							ImGui::Text(strUtMethod.c_str());

							strUt = ToUTF8(iter2->second.m_strLastMsgFromMac);
							ImGui::Text(strUt.c_str());
						}
						ImGui::EndDock();

						ImGui::EndDockspace();
						ImGui::End();
					}
					else {
						ImGui::End();
					}


				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
///	static MemoryEditor mem_edit;
///	char data[255] = "helloasdfjlaskdjoqiwerqwljalsdfj";
	
///	mem_edit.DrawWindow("Memory Editor", data, 200);

	ImGui::End();
}

void UIManager::DrawLoginDlg()
{
	ImGui::Begin("Login",0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
	
	float nx = ImGui::GetWindowSize().x;
	float ny = ImGui::GetWindowSize().y;
	float npx = (g_d3dpp.BackBufferWidth  - nx) / 2;
	float npy = (g_d3dpp.BackBufferHeight  - ny) / 2;
	UINT nW = g_d3dpp.BackBufferWidth / 3;
	UINT nH = g_d3dpp.BackBufferHeight / 2;


	float nppx = ImGui::GetWindowPos().x;
	float nppy = ImGui::GetWindowPos().y;

	ImGui::SetWindowPos("Login", ImVec2(npx, npy));
	ImGui::SetWindowSize("Login", ImVec2(400, 100));
	static char bufName[255] = "zc";
	static char bufpass[64] = "zc";
	
	ImGui::InputText("Name", bufName, 64, ImGuiInputTextFlags_CharsNoBlank);

	ImGui::InputText("PassWord", bufpass, 64, ImGuiInputTextFlags_Password | ImGuiInputTextFlags_CharsNoBlank);


	if (ImGui::Button("Login")) {	
		if (strcmp(bufName,"zc")==0 && strcmp(bufpass,"zc")==0)
		{
			m_bHasLogin = true;
			memset(m_cBeginTm, 0, 64);
			tm pTime;
			time_t now;
			time(&now);
			localtime_s(&pTime, &now);
			strftime(m_cBeginTm, sizeof(m_cBeginTm), "%Y/%m/%d %X %A %j %z", &pTime);
			m_dwBeginTm = GetTickCount();
		}
		else{
			m_bHasLogin = false;
		}
	}
	ImGui::End();
}

void UIManager::DrawLay()
{
	const float DISTANCE = 10.0f;
	static int corner = 3;
	static bool bLayShow = true;
	static bool p_open = true ;
	ImVec2 window_pos = ImVec2((corner & 1) ? ImGui::GetIO().DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? ImGui::GetIO().DisplaySize.y - DISTANCE : DISTANCE);
	ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
	if (corner != -1)
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
	if (ImGui::Begin("Sysinfo Overlay", &bLayShow, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
	
		ImGui::Text("Begin:" );
		ImGui::Text(m_cBeginTm);

		long dwNow = GetTickCount();

		ImGui::Separator();

		int deg = dwNow - m_dwBeginTm;
		int sec = deg / 1000; //秒
		int wsec = deg % 1000;

		int h, m, s;
		h = sec / 3600;
		m = (sec % 3600) / 60;
		s = (sec % 3600) % 60;

		//ImGui::Text(tmp);
		ImGui::Text("Run:");
		ImGui::Text("%d:%d:%d.%d",  h,m,s,wsec);
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
		else
			ImGui::Text("Mouse Position: <invalid>");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
			if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
			if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
		//	if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}
void UIManager::ShowMark(int a, int b, int c, int d)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();

		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);

		string strLL = "[请求/响应][正确/错误]";
		string strUt = ToUTF8(strLL);
		ImGui::TextWrapped(strUt.c_str());

		ImGui::Text("[%d/%d][%d/%d]", a, b, c, d);
			static float arr[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			if (a>0)
			{
				float ss =1.0f* b / a;
				arr[0] = 1.0f* a / a;
				arr[1] = 1.0f* b / a;
				arr[2] = 1.0f* c / a;
				arr[3] = 1.0f* d / a;

		//
		//		arr[0] = a;
		//		arr[1] = b;
		//		arr[2] = c;
		//		arr[3] = d;
			}
			
			ImGui::PlotHistogram("Histogram", arr, 4, 0, NULL, 0.0f, 1.0f, ImVec2(0, 80));
		//	ImGui::PlotHistogram("Histogram", arr, 4, 0, NULL, 0.0f, a, ImVec2(0, 80));

		ImGui::PopTextWrapPos();

		ImGui::EndTooltip();
	}
}
