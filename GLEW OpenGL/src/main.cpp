#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/wglew.h>
#include <Windows.h>
#include <sstream>

bool GameOn;

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};

// Prints the latest system error to the console output screen in debug mode
bool PrintSystemError(DWORD error)
{
	std::ostringstream oss;
	oss << "System Error: " << error;
	OutputDebugString(oss.str().c_str());

	return false;
}

// Creates a default windowprocedure
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (msg)
	{
		case WM_CREATE:
		{
			GameOn = true;
		} break;

		case WM_CLOSE:
		{
			GameOn = false;
		} break;

		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_ESCAPE:
				{
					// Process escape key
					GameOn = false;
				} break;
			}
		} break;

		case WM_LBUTTONDOWN:
		{
			OutputDebugString("Works!");
		} break;

		default:
		{
			result = DefWindowProc(hWnd, msg, wParam, lParam);
		} break;
	}
	
	return result;
}

// Windows main entry point
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX WindowsClass = {};
	HDC hdc = {};
	int pixelFormatNumber = 0;

	// Initialize windows class extended structure 
	WindowsClass.cbSize = sizeof(WNDCLASSEX);
	WindowsClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowsClass.lpfnWndProc = WindowProcedure;
	WindowsClass.hInstance = hInstance;
	WindowsClass.lpszClassName = "Pong";
	WindowsClass.hbrBackground = (HBRUSH)LTGRAY_BRUSH;

	if (!RegisterClassEx(&WindowsClass))
		return PrintSystemError(GetLastError());

	// Create window handle
	HWND hwndMain = CreateWindowEx(
		0,
		WindowsClass.lpszClassName,
		"Pong",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		(HWND)NULL,
		(HMENU)NULL,
		hInstance,
		NULL
	);

	if (!hwndMain)
		return PrintSystemError(GetLastError());

	// Device Context Handle
	hdc = GetDC(hwndMain);

	// Pixel Format to assign to hdc
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.bReserved = PFD_MAIN_PLANE;

	pixelFormatNumber = ChoosePixelFormat(hdc, &pfd);
	if (!pixelFormatNumber)
		return PrintSystemError(GetLastError());

	if (!SetPixelFormat(hdc, pixelFormatNumber, &pfd))
		return PrintSystemError(GetLastError());

	// Creates temporary rendering context with the device context handle to access openGL functions
	HGLRC tempContext = wglCreateContext(hdc);

	// Make the device context current
	wglMakeCurrent(hdc, tempContext);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		return PrintSystemError(err);
		return false;
	}

	if (GLEW_VERSION_4_6)
	{
		OutputDebugString("OpenGL 4.6 supported!");
	}

	const int pixelFormatAttribList[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0
	};

	int pixelFormat;
	UINT numFormats;

	wglChoosePixelFormatARB(hdc, pixelFormatAttribList, NULL, 1, &pixelFormat, &numFormats);

	const int contextAttribList[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_FLAGS_ARB,
		0
	};

	HGLRC wglRenderingContext = wglCreateContextAttribsARB(hdc, 0, contextAttribList);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);
	wglMakeCurrent(hdc, wglRenderingContext);

	while (GameOn)
	{
		// Handle events. Dispatches message to window procedure
		MSG message;
		BOOL msgResult = GetMessage(&message, 0, 0, 0);
		{
			if (msgResult > 0)
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}

		ShowWindow(hwndMain, SW_SHOWDEFAULT);
		UpdateWindow(hwndMain);
	}

	return 0;
}