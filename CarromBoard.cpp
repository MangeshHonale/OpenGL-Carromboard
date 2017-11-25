#include <windows.h>
#include<math.h>
#include <gl/GL.h>

#define WIN_WIDTH 700
#define WIN_HEIGHT 700
#define PI 3.1415926535898f
#define SPEED 1.0f;
#pragma comment(lib,"opengl32.lib")


//Prototype Of WndProc() declared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
int giX, giY;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;
GLfloat gfStrikerCenter = 25.66f;

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function prototype
	void initialize(void);
	void uninitialize(void);
	void display(void);

	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("RTROGL");
	bool bDone = false;

	//code
	//initializing members of struct WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	//Registering Class
	RegisterClassEx(&wndclass);

	giX = GetSystemMetrics(SM_CXFULLSCREEN) / 2 - WIN_WIDTH / 2;
	giY = GetSystemMetrics(SM_CYFULLSCREEN) / 2 - WIN_HEIGHT / 2;

	//Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("Let's Play!"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		giX,
		giY,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);


	ghwnd = hwnd;

	//initialize
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Message Loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
				display();
			}

		}
	}

	uninitialize();
	return((int)msg.wParam);
}

//WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function prototype

	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;


	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;
		case 0x46: //for 'f' or 'F'
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	void initialize(void);
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);

			}
		}
		ShowCursor(FALSE);
	}

	else
	{
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	//function prototypes
	void resize(int, int);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//Initialization of structure 'PIXELFORMATDESCRIPTOR'
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{
	void drawCarromBoard();
	//code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	drawCarromBoard();

	SwapBuffers(ghdc);
}

void drawCarromBoard() {
	
	
	//Edges
	glBegin(GL_QUADS);
	glColor3f((GLfloat)133 / (GLfloat)255, (GLfloat)94/ (GLfloat)255, (GLfloat)66 / (GLfloat)255);
	glVertex3f(45.0f, 45.0f, 0.0f);
	glVertex3f(-45.0f, 45.0f, 0.0f);
	glVertex3f(-45.0f, -45.0f, 0.0f);
	glVertex3f(45.0f, -45.0f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f((GLfloat)235 / (GLfloat)255, (GLfloat)199 / (GLfloat)255, (GLfloat)158 / (GLfloat)255);
	glVertex3f(37.4f, 37.4f, 0.0f);
	glVertex3f(-37.4f, 37.4f, 0.0f);
	glVertex3f(-37.4f, -37.4f, 0.0f);
	glVertex3f(37.4f, -37.4f, 0.0f);
	glEnd();

	//bottom left hole
	glTranslatef(-34.78f, -34.78f, 0.0f);
	glBegin(GL_LINES);
	glColor3f((GLfloat)51 / (GLfloat)255, (GLfloat)51 / (GLfloat)255, (GLfloat)51 / (GLfloat)255);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 2.22, sin(angle)* 2.22, 0.0f);
	}
	glEnd();

	//top left hole
	glTranslatef(0.0f, 34.78f * 2, 0.0f);
	glBegin(GL_LINES);
	glColor3f((GLfloat)51 / (GLfloat)255, (GLfloat)51 / (GLfloat)255, (GLfloat)51 / (GLfloat)255);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 2.22, sin(angle)* 2.22, 0.0f);
	}
	glEnd();

	//top right hole
	glTranslatef(34.78f * 2, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glColor3f((GLfloat)51 / (GLfloat)255, (GLfloat)51 / (GLfloat)255, (GLfloat)51 / (GLfloat)255);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 2.22, sin(angle)* 2.22, 0.0f);
	}
	glEnd();

	//bottom right hole
	glTranslatef(0.0f, -34.78f * 2, 0.0f);
	glBegin(GL_LINES);
	glColor3f((GLfloat)51 / (GLfloat)255, (GLfloat)51 / (GLfloat)255, (GLfloat)51 / (GLfloat)255);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 2.22, sin(angle)* 2.22, 0.0f);
	}
	glEnd();

	//center red spot
	glTranslatef(-34.78f, 34.78f, 0.0f);
	glBegin(GL_LINES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.59f, sin(angle) * 1.59f, 0.0f);
	}
	glEnd();

	//center circle
	glPointSize(1.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 9.5f, sin(angle) * 9.5f , 0.0f);
	}
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 9.6f, sin(angle) * 9.6f, 0.0f);
	}
	glEnd();

	//black circle covering red spot
	glPointSize(3.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 1.6f, sin(angle) * 1.6f, 0.0f);
	}
	glEnd();

	//triangles in central circles starting from +ve x axis clockwise
	glLineWidth(2.0f);
	//1st triangle
	glBegin(GL_LINES);
	glVertex3f(cos(0.0f) * 9.5f, sin(0.0f) * 9.5f, 0.0f);
	glVertex3f(1.3f, 1.3f, 0.0f);
	glVertex3f(cos(0.0f) * 9.5f, sin(0.0f) * 9.5f, 0.0f);
	glVertex3f(1.3f, -1.3f, 0.0f);
	
	//2nd triangle
	glVertex3f(cos(PI / 4) * 9.5f, sin(PI / 4) * 9.5f, 0.0f);
	glVertex3f(0.0f, 1.9f, 0.0f);
	glVertex3f(cos(PI / 4) * 9.5f, sin(PI / 4) * 9.5f, 0.0f);
	glVertex3f(1.9f, 0.0f, 0.0f);

	//3rd triangle
	glVertex3f(cos(PI / 2) * 9.5f, sin(PI / 2) * 9.5f, 0.0f);
	glVertex3f(1.3f, 1.3f, 0.0f);
	glVertex3f(cos(PI / 2) * 9.5f, sin(PI / 2) * 9.5f, 0.0f);
	glVertex3f(-1.3f, 1.3f, 0.0f);

	//4th triangle
	glVertex3f(cos(PI * 3 / 4) * 9.5f, sin(PI * 3 / 4) * 9.5f, 0.0f);
	glVertex3f(0.0f, 1.9f, 0.0f);
	glVertex3f(cos(PI * 3 / 4) * 9.5f, sin(PI * 3 / 4) * 9.5f, 0.0f);
	glVertex3f(-1.9f, 0.0f, 0.0f);

	//5th triangle
	glVertex3f(cos(PI) * 9.5f, sin(PI) * 9.5f, 0.0f);
	glVertex3f(-1.3f, 1.3f, 0.0f);
	glVertex3f(cos(PI) * 9.5f, sin(PI) * 9.5f, 0.0f);
	glVertex3f(-1.3f, -1.3f, 0.0f);

	//6th triangle
	glVertex3f(cos(PI * 5 / 4) * 9.5f, sin(PI * 5 / 4) * 9.5f, 0.0f);
	glVertex3f(0.0f, -1.9f, 0.0f);
	glVertex3f(cos(PI * 5 / 4) * 9.5f, sin(PI * 5 / 4) * 9.5f, 0.0f);
	glVertex3f(-1.9f, 0.0f, 0.0f);

	//7th triangle
	glVertex3f(cos(PI * 3 / 2) * 9.5f, sin(PI * 3 / 2) * 9.5f, 0.0f);
	glVertex3f(-1.3f, -1.3f, 0.0f);
	glVertex3f(cos(PI * 3 / 2) * 9.5f, sin(PI * 3 / 2) * 9.5f, 0.0f);
	glVertex3f(1.3f, -1.3f, 0.0f);

	//8th triangle
	glVertex3f(cos(PI * 7 / 4) * 9.5f, sin(PI * 7 / 4) * 9.5f, 0.0f);
	glVertex3f(0.0f, -1.9f, 0.0f);
	glVertex3f(cos(PI * 7 / 4) * 9.5f, sin(PI * 7 / 4) * 9.5f, 0.0f);
	glVertex3f(1.9f, 0.0f, 0.0f);
	glEnd();

	//filled triangles in circle
	//1st red triangle
	glBegin(GL_TRIANGLES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	glVertex3f(cos(0.0f) * 9.5f, sin(0.0f) * 9.5f, 0.0f);
	glVertex3f(1.9, 0.0f, 0.0f);
	glVertex3f(2.83f, -1.4f, 0.0f);
	glEnd();

	//2nd black triangle
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(cos(PI / 4) * 9.5f, sin(PI / 4) * 9.5f, 0.0f);
	glVertex3f(1.4f, 1.4f, 0.0f);
	glVertex3f(2.6f, 1.2f, 0.0f);
	glEnd();

	//3rd triangle
	glBegin(GL_TRIANGLES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	glVertex3f(cos(PI / 2) * 9.5f, sin(PI / 2) * 9.5f, 0.0f);
	glVertex3f(1.1f, 2.5f, 0.0f);
	glVertex3f(0.0f, 1.9f, 0.0f);
	glEnd();

	//4th black triangle
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(cos(PI * 3 / 4) * 9.5f, sin(PI * 3 / 4) * 9.5f, 0.0f);
	glVertex3f(-1.4f, 1.4f, 0.0f);
	glVertex3f(-1.1f, 2.55f, 0.0f);
	glEnd();

	//5th red triangle
	glBegin(GL_TRIANGLES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	glVertex3f(cos(PI) * 9.5f, sin(PI) * 9.5f, 0.0f);
	glVertex3f(-1.9f, 0.0f, 0.0f);
	glVertex3f(-2.75f, 1.35f, 0.0f);
	glEnd();

	//6th black triangle
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(cos(PI * 5 / 4) * 9.5f, sin(PI * 5 / 4) * 9.5f, 0.0f);
	glVertex3f(-1.4f, -1.4f, 0.0f);
	glVertex3f(-2.6f, -1.2f, 0.0f);
	glEnd();

	//7th red triangle
	glBegin(GL_TRIANGLES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	glVertex3f(cos(PI * 3 / 2) * 9.5f, sin(PI * 3 / 2) * 9.5f, 0.0f);
	glVertex3f(0.0f, -1.9f, 0.0f);
	glVertex3f(-1.1f, -2.55f, 0.0f);
	glEnd();

	//8th black triangle
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(cos(PI * 7 / 4) * 9.5f, sin(PI * 7 / 4) * 9.5f, 0.0f);
	glVertex3f(1.4f, -1.4f, 0.0f);
	glVertex3f(1.1f, -2.55f, 0.0f);
	glEnd();

	//right player service plate
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(27.25f, 22.5f, 0.0f);
	glVertex3f(27.25f, -22.5f, 0.0f);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(24.07f, 22.5f, 0.0f);
	glVertex3f(24.07f, -22.5f, 0.0f);
	glEnd();

	//top player service plate
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(22.5f, 27.25f, 0.0f);
	glVertex3f(-22.5f, 27.25f, 0.0f);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(22.5f, 24.07f, 0.0f);
	glVertex3f(-22.5f, 24.07f, 0.0f);
	glEnd();

	//left player service plate
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-27.25f, 22.5f, 0.0f);
	glVertex3f(-27.25f, -22.5f, 0.0f);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-24.07f, 22.5f, 0.0f);
	glVertex3f(-24.07f, -22.5f, 0.0f);
	glEnd();

	//bottom player service plate
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-22.5f, -27.25f, 0.0f);
	glVertex3f(22.5f, -27.25f, 0.0f);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-22.5f, -24.07f, 0.0f);
	glVertex3f(22.5f, -24.07f, 0.0f);
	glEnd();

	//top right cross line circle
	glTranslatef(cos(PI / 4) * 19.92f, sin(PI / 4) * 19.92f, 0.0f);
	glPointSize(1.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = (PI / 2); angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 3.175f, sin(angle) * 3.175f, 0.0f);
	}
	glEnd();

	//top left cross line circle
	glTranslatef(cos(PI / 4) * 19.92f * -2.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
			glVertex3f(cos(angle) * 3.175f, sin(angle) * 3.175f, 0.0f);
	}
	glEnd();

	glBegin(GL_POINTS);
	glColor3f((GLfloat)235 / (GLfloat)255, (GLfloat)199 / (GLfloat)255, (GLfloat)158 / (GLfloat)255);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		if(angle > (PI / 2) && angle < PI)
		glVertex3f(cos(angle) * 3.175f, sin(angle) * 3.175f, 0.0f);
	}
	glEnd();

	//bottom left cross line circle
	glTranslatef(0.0f, cos(PI / 4) * 19.92f * -2.0f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 3.175f, sin(angle) * 3.175f, 0.0f);
	}
	glEnd();

	glBegin(GL_POINTS);
	glColor3f((GLfloat)235 / (GLfloat)255, (GLfloat)199 / (GLfloat)255, (GLfloat)158 / (GLfloat)255);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		if (angle >(PI) && angle < (PI * 3 / 2))
			glVertex3f(cos(angle) * 3.175f, sin(angle) * 3.175f, 0.0f);
	}
	glEnd();

	//bottom right cross line circle
	glTranslatef(cos(PI / 4) * 19.92f * 2.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < (PI * 3 / 2); angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 3.175f, sin(angle) * 3.175f, 0.0f);
	}
	glEnd();

	//bring back the origin to the center of screen
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	//top right cross line
	glVertex3f(cos(PI / 4) * 16.75f, sin(PI / 4) * 16.75f, 0.0f);
	glVertex3f(cos(PI / 4) * 43.45f, sin(PI / 4) * 43.45f, 0.0f);

	//top left cross line
	glVertex3f(cos(PI * 3 / 4) * 16.75f, sin(PI * 3 / 4) * 16.75f, 0.0f);
	glVertex3f(cos(PI * 3 / 4) * 43.45f, sin(PI * 3 / 4) * 43.45f, 0.0f);

	//bottom left cross line
	glVertex3f(cos(PI * 5 / 4) * 16.75f, sin(PI * 5 / 4) * 16.75f, 0.0f);
	glVertex3f(cos(PI * 5 / 4) * 43.45f, sin(PI * 5 / 4) * 43.45f, 0.0f);

	//bottom right cross line
	glVertex3f(cos(PI * 7 / 4) * 16.75f, sin(PI * 7 / 4) * 16.75f, 0.0f);
	glVertex3f(cos(PI * 7 / 4) * 43.45f, sin(PI * 7 / 4) * 43.45f, 0.0f);
	glEnd();
	//right player service area circles
	glTranslatef(25.66f, 22.5f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 1.59f, sin(angle) * 1.59f, 0.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.22f, sin(angle) * 1.22f, 0.0f);
	}
	glEnd();
	
	glTranslatef(0.0f, -45.0f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 1.59f, sin(angle) * 1.59f, 0.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.22f, sin(angle) * 1.22f, 0.0f);
	}
	glEnd();

	//left player service area circles
	glTranslatef(-51.32f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 1.59f, sin(angle) * 1.59f, 0.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.22f, sin(angle) * 1.22f, 0.0f);
	}
	glEnd();

	glTranslatef(0.0f, 45.0f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 1.59f, sin(angle) * 1.59f, 0.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.22f, sin(angle) * 1.22f, 0.0f);
	}
	glEnd();

	//bring back the origin to center
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//top player service area circles
	glTranslatef(22.5f, 25.66f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 1.59f, sin(angle) * 1.59f, 0.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.22f, sin(angle) * 1.22f, 0.0f);
	}
	glEnd();

	glTranslatef(-45.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 1.59f, sin(angle) * 1.59f, 0.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.22f, sin(angle) * 1.22f, 0.0f);
	}
	glEnd();

	//bottom player service area circles
	glTranslatef(0.0f, -51.32f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 1.59f, sin(angle) * 1.59f, 0.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.22f, sin(angle) * 1.22f, 0.0f);
	}
	glEnd();

	glTranslatef(45.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(cos(angle) * 1.59f, sin(angle) * 1.59f, 0.0f);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f((GLfloat)139 / (GLfloat)255, 0.0f, 0.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.22f, sin(angle) * 1.22f, 0.0f);
	}
	glEnd();


	//striker
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*glTranslatef(gfStrikerCenter, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (float angle = 0.0f; angle < 2.0f * PI; angle = angle + 0.0001f) {
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(cos(angle) * 1.9, sin(angle)* 1.9, 0.0f);
	}
	glEnd();
	gfStrikerCenter = gfStrikerCenter - SPEED;
	if (gfStrikerCenter <= 0)
		gfStrikerCenter = 25.66f;*/
}


void createLineVertices(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
	glVertex3f(x1, y1, 0.0f);
	glVertex3f(x2, y2, 0.0f);
}
void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	if(width == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//to maintain the scale of object when window is resized
	if (width <= height)
		glOrtho(-50.0f, 50.0f, -50.0f * (GLfloat)height / (GLfloat)width, 50.0f * (GLfloat)height / (GLfloat)width, -50.0f, 50.0f);
	else
		glOrtho(-50.0f * (GLfloat)width / (GLfloat)height, 50.0f * (GLfloat)width / (GLfloat)height, -50.0f, 50.0f, -50.0f, 50.0f);
}

void uninitialize(void)
{
	//UNINITIALIZATION CODE

	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);

	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;
}
