#pragma comment(lib, "opengl32.lib")

#include <Windows.h>
#include <sstream>
#include <gl/GL.h>

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#define GLAPI extern
#endif

// OpenGL function prototypes
// (P)ointer to the (F)unctio(N) (function name) (PROC)edure
// Example: PFNGLUSEPROGRAMPROC: PFN glUseProgram PROC
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLGETINTEGERVPROC)(GLenum pname, GLint *params);
//typedef void (APIENTRYP PFNGLGETSTRINGPROC)(GLenum name);
typedef void (APIENTRYP PFNGLGETSTRINGIPROC)(GLenum name, GLuint index);

// Prints the latest system error to the console output screen in debug mode
bool PrintSystemError()
{
	DWORD error = GetLastError();
	std::ostringstream oss;
	oss << "System Error: " << GetLastError();
	OutputDebugString(oss.str().c_str());

	return false;
}

// Creates a default windowprocedure
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return (DefWindowProc(hWnd, Msg, wParam, lParam));
}

// Checks for OpenGL function pointer using wglGetProcAddress, and if not found checks for the function using Win32's GetProcAddress
// wglGetProcAddress is responsible for retrieving modern opengl functions. Any older functions that need to be accessed can be done using GetProcAddress
void *GetAnyGLFuncAddress(const char *name)
{
	void *functionPointer = (void *)wglGetProcAddress(name);
	if ((functionPointer == 0) ||
		(functionPointer == (void*)0x1) ||
		(functionPointer == (void*)0x2) ||
		(functionPointer == (void*)0x3) ||
		(functionPointer == (void*)-1))
	{
		HMODULE module = LoadLibraryA("opengl32.dll");
		functionPointer = (void *)GetProcAddress(module, name);
	}

	return functionPointer;
}

// Windows main entry point
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX WindowsClass = {};
	HDC hdc = {};
	int pixelFormatNumber = 0;

	PFNGLGETINTEGERVPROC glGetIntegerv;
	GLenum glEnum = 0;
	GLint *glParms = 0;


	// Initialize windows class extended structure 
	WindowsClass.cbSize = sizeof(WNDCLASSEX);
	WindowsClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WindowsClass.lpfnWndProc = WindowProcedure;
	WindowsClass.hInstance = hInstance;
	WindowsClass.lpszClassName = "Pong";
	WindowsClass.hbrBackground = (HBRUSH)LTGRAY_BRUSH;

	if (!RegisterClassEx(&WindowsClass))
		return PrintSystemError();

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
		(HWND) NULL,
		(HMENU)NULL,
		hInstance,
		NULL

	);

	if (!hwndMain)
		return PrintSystemError();

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
		return PrintSystemError();
	
	if (!SetPixelFormat(hdc, pixelFormatNumber, &pfd))
		return PrintSystemError();

	// Creates rendering context with the device context handle
	HGLRC renderingContext = wglCreateContext(hdc);

	// Make the device context current
	wglMakeCurrent(hdc, renderingContext);

	glGetIntegerv = (PFNGLGETINTEGERVPROC)GetAnyGLFuncAddress("glGetIntegerv");

	glGetIntegerv(glEnum, glParms);

	while (true)
	{
		ShowWindow(hwndMain, SW_SHOWDEFAULT);
		UpdateWindow(hwndMain);
	}
}