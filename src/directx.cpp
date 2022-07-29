#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3d12.h>

#include <stdio.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "User32.lib")

// Handle to the window
HWND hwnd = NULL;

// name of the window (not the title)
LPCTSTR WindowName = L"DirectX";

// title of the window
LPCTSTR WindowTitle = L"DirectX Version";


// width and height of the window
int Width = 800;
int Height = 600;

// is window full screen?
bool FullScreen = false;

// create a window
bool InitializeWindow(HINSTANCE hInstance,
					  int ShowWnd,
					  int width, int height,
					  bool fullscreen);

// main application loop
void mainloop();

// callback function for windows messages
LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT msg,
						 WPARAM wParam,
						 LPARAM lParam);





bool InitializeWindow(HINSTANCE hInstance,
					  int ShowWnd,
					  int width, int height,
					  bool fullscreen)
{
	
	
    if (fullscreen)
    {
        HMONITOR hmon = MonitorFromWindow(hwnd,
										  MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(hmon, &mi);
		
        width = mi.rcMonitor.right - mi.rcMonitor.left;
        height = mi.rcMonitor.bottom - mi.rcMonitor.top;
    }
	
	WNDCLASSEX wc;
    
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WindowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering class",
				   L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	
	
	hwnd = CreateWindowEx(NULL,
						  WindowName,
						  WindowTitle,
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, CW_USEDEFAULT,
						  width, height,
						  NULL,
						  NULL,
						  hInstance,
						  NULL);
	
	
	if (!hwnd)
	{
		MessageBox(NULL, L"Error creating window",
				   L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	
	
	if (fullscreen)
	{
		SetWindowLong(hwnd, GWL_STYLE, 0);
	}


	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);
    
	return true;
}



int WINAPI WinMain(HINSTANCE hInstance,    //Main windows function
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nShowCmd)
{

	printf("Hello");

	auto ConsoleWindow = GetConsoleWindow();
	if (ConsoleWindow == NULL)
	{
		MessageBox(0, L"There is no associated Console", L"Really?", MB_OK | MB_ICONQUESTION);
		return -1;
	}
	
	ShowWindow(ConsoleWindow, SW_SHOW);

	if (IsWindowVisible(ConsoleWindow) == FALSE )
	{
		MessageBox(0, L"Unable To Show Console Window", L"Really?", MB_OK | MB_ICONQUESTION);
		return -1;
	}
	
    // create the window            /*nShowCmd*/
    if (!InitializeWindow(hInstance, true, Width, Height, FullScreen))
    {
        MessageBox(0, L"Window Initialization - Failed",
				   L"Error", MB_OK);
        return 0;
    }
	
    // start the main loop
	mainloop();
    
    return 0;
}



LRESULT CALLBACK WndProc(HWND hwnd,
						 UINT msg,
						 WPARAM wParam,
						 LPARAM lParam)
{
    switch (msg)
    {
		
		case WM_KEYDOWN:
		{
        	if (wParam == VK_ESCAPE) {
        	    //if (MessageBox(0, L"Are you sure you want to exit?", L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
        	        DestroyWindow(hwnd);
        	}

			// if (wParam == VK_K)
			{
				// printf("Key [%d] is pressed\n", (int)wParam);
			}
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
    }
	
    return DefWindowProc(hwnd, msg, wParam, lParam);
}


// This is absurd
int main()
{
	return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWNORMAL);
}


void mainloop()
{

	IDXGIFactory4* dxgiFactory;
	ID3D12Device2* d3d12Device2;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif


	CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory));

	D3D12CreateDevice(dxgiFactory, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2));

	
	MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    
    while (true)
    {
	
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
			
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // run game code
        }
    }
	
}
