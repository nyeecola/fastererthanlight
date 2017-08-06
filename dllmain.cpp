#define NTDDI_VERSION NTDDI_WIN7
#define _WIN32_WINNT _WIN32_WINNT_WIN7

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLu32.lib")

#include "afxwin.h"
#include <windows.h>
#include <Wingdi.h>
#include <stdio.h>
#include <string>
#include <tlhelp32.h>
#include <iostream>
#include <gl\gl.h>
#include <gl\glu.h>

typedef void ( APIENTRY *glBegin_t )( GLenum );
typedef void ( APIENTRY *glEnd_t )( void );
typedef void ( APIENTRY *glClear_t )( GLbitfield );
typedef void ( APIENTRY *glVertex3fv_t )( const GLfloat *v );
typedef void ( APIENTRY *glVertex3f_t )( GLfloat x,  GLfloat y,  GLfloat z );
typedef void ( APIENTRY *glVertex2fv_t )( const GLfloat *v );
typedef void ( APIENTRY *glVertex2f_t )( GLfloat x,  GLfloat y);
typedef void ( APIENTRY *glDrawArrays_t )( GLenum mode, GLint first, GLsizei count);
typedef void ( APIENTRY *glDrawElements_t )( GLenum mode, GLint first, GLsizei count, const GLvoid *indices);
typedef void ( APIENTRY *glEnable_t )( GLenum );
typedef void ( APIENTRY *wglSwapBuffers_t )( HDC h );

glBegin_t pglBegin = NULL;
glEnd_t pglEnd = NULL;
glClear_t pglClear = NULL;
glVertex3fv_t pglVertex3fv = NULL;
glVertex3f_t pglVertex3f = NULL;
glVertex2fv_t pglVertex2fv = NULL;
glVertex2f_t pglVertex2f = NULL;
glDrawArrays_t pglDrawArrays = NULL;
glDrawElements_t pglDrawElements = NULL;
glEnable_t pglEnable = NULL;
wglSwapBuffers_t pwglSwapBuffers = NULL;

typedef struct
{
	float start_x;
	float start_y;
	float line_height;
	int max_lines;
	int cur_lines;
	char **lines;
	int oldest_line;
} console_t;

static console_t global_console;

void draw_text(char *str, float x, float y)
{
	HDC hdc = wglGetCurrentDC();
	SelectObject (hdc, GetStockObject (SYSTEM_FONT)); 
	wglUseFontBitmaps(hdc, 0, 255, 3000);

	glRasterPos2f(x, y);

	glPushAttrib(GL_LIST_BIT);
	glListBase(3000); 
	glCallLists (strlen(str), GL_UNSIGNED_BYTE, str);
	glPopAttrib();
}

void console_log(char *str)
{
	if (global_console.cur_lines < global_console.max_lines)
	{
		strcpy(global_console.lines[global_console.cur_lines], str);
		++global_console.cur_lines;
	}
	else
	{
		strcpy(global_console.lines[global_console.oldest_line], str);
		global_console.oldest_line = (global_console.oldest_line + 1) % global_console.max_lines;
	}
}

void console_draw()
{
	glColor4f(0x00, 0xcf, 0x00, 0xff);

	HDC hdc = wglGetCurrentDC();
	SelectObject (hdc, GetStockObject (SYSTEM_FONT)); 
	wglUseFontBitmaps(hdc, 0, 255, 3000);

	glPushAttrib(GL_LIST_BIT);
	glListBase(3000); 

	for (int i = 0; i < global_console.cur_lines; i++)
	{
		int line_index = (global_console.oldest_line + i) % global_console.max_lines;
		glRasterPos2f(global_console.start_x, global_console.start_y + i * global_console.line_height);

		glCallLists (strlen(global_console.lines[line_index]), GL_UNSIGNED_BYTE, global_console.lines[line_index]);
	}

	glPopAttrib();
}

void DrawBox(GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height) 
{ 
	glBegin(GL_QUADS); 
	glVertex3f(x,y,z); 
	glVertex3f(x+width,y,z); 
	glVertex3f(x+width,y+height,z); 
	glVertex3f(x,y+height,z); 
	glEnd(); 
} 

void APIENTRY Hooked_glEnable (GLenum cap)
{
	(*pglEnable)(cap);
}

void APIENTRY Hooked_glBegin(GLenum e)
{
	if (GetAsyncKeyState(VK_F3))
	{
		glColor4f(0xff, 0x00, 0x00, 0xff);
	}

	(*pglBegin)(e);
}

void APIENTRY Hooked_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	if (GetAsyncKeyState(VK_F4))
	{
		glColor4f(0x00, 0x00, 0xff, 0xff);
	}

	(*pglDrawArrays)(mode, first, count);
}

void APIENTRY Hooked_glDrawElements(GLenum mode, GLint first, GLsizei count, const GLvoid *indices)
{
	if (GetAsyncKeyState(VK_F4))
	{
		glColor4f(0x00, 0x00, 0xff, 0xff);
	}

	(*pglDrawElements)(mode, first, count, indices);
}

void APIENTRY Hooked_glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
	if (GetAsyncKeyState(VK_F4))
	{
		glColor4f(0x00, 0x00, 0xff, 0xff);
	}

	(*pglVertex3f)(x, y, z);
}

void APIENTRY Hooked_glVertex2f(GLfloat x, GLfloat y)
{
	if (GetAsyncKeyState(VK_F4))
	{
		glColor4f(0x00, 0x00, 0xff, 0xff);
	}

	(*pglVertex2f)(x, y);
}

void APIENTRY Hooked_glVertex2fv(GLfloat *v)
{
	if (GetAsyncKeyState(VK_F4))
	{
		glColor4f(0x00, 0x00, 0xff, 0xff);
	}

	(*pglVertex2fv)(v);
}

void APIENTRY Hooked_glVertex3fv(GLfloat *v)
{
	if (GetAsyncKeyState(VK_F4))
	{
		glColor4f(0x00, 0x00, 0xff, 0xff);
	}

	(*pglVertex3fv)(v);
}

void APIENTRY Hooked_wglSwapBuffers (HDC h)
{
	if (GetAsyncKeyState(VK_F2))
	{
		glBlendFunc(GL_ONE, GL_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
		DrawBox(global_console.start_x - 10, global_console.start_y - 24, 0, 500, global_console.max_lines * 14 + 20);
		glDisable(GL_BLEND);

		console_draw();
	}

	if (GetAsyncKeyState(VK_F9))
	{
		static int n = 0;
		char str[400];
		sprintf(str, "TEST %d!", n++);
		console_log(str);
	}

	glColor4f(0xff, 0xff, 0x00, 0xff);
	HWND window_handle = GetActiveWindow();
	RECT window_rect;
	GetWindowRect(window_handle, &window_rect);

	draw_text("Fasterer than Light v0.2.0", window_rect.right-250, 14);

	(*pwglSwapBuffers)(h);
}

void *DetourFunc( BYTE *src, const BYTE *dst, const int len )
{
	BYTE *jmp = (BYTE*)malloc( len + 5 );
	DWORD dwback;
	VirtualProtect( src, len, PAGE_READWRITE, &dwback );
	memcpy( jmp, src, len );
	jmp += len;
	jmp[0] = 0xE9;
	*(DWORD*)( jmp + 1 ) = (DWORD)( src + len - jmp ) - 5;
	src[0] = 0xE9;
	*(DWORD*)( src + 1 ) = (DWORD)( dst - src ) - 5;
	VirtualProtect( src, len, dwback, &dwback );
	return ( jmp - len );
}

void HookGL()
{
	HMODULE hOpenGL = GetModuleHandle("opengl32.dll");
	/*
	pglEnd       =   (glEnd_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glEnd" ), (LPBYTE)&Hooked_glEnd, 6 );
	pglClear     =   (glClear_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glClear" ), (LPBYTE)&Hooked_glClear, 7 );
	*/
	pglEnable = (glEnable_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glEnable"), (LPBYTE)&Hooked_glEnable, 6);
	pwglSwapBuffers = (wglSwapBuffers_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "wglSwapBuffers"), (LPBYTE)&Hooked_wglSwapBuffers, 5);
	pglBegin = (glBegin_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glBegin"),(LPBYTE)&Hooked_glBegin,6);
	pglVertex3f = (glVertex3f_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glVertex3f" ), (LPBYTE)&Hooked_glVertex3f, 6);
	pglVertex3fv = (glVertex3fv_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glVertex3fv" ), (LPBYTE)&Hooked_glVertex3fv, 6 );
	pglVertex2f = (glVertex2f_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glVertex2f" ), (LPBYTE)&Hooked_glVertex2f, 6);
	pglVertex2fv = (glVertex2fv_t)DetourFunc( (LPBYTE)GetProcAddress(hOpenGL, "glVertex2fv" ), (LPBYTE)&Hooked_glVertex2fv, 6);
	pglDrawArrays = (glDrawArrays_t) DetourFunc((LPBYTE)GetProcAddress(hOpenGL, "glDrawArrays"), (LPBYTE) &Hooked_glDrawArrays, 6);
	pglDrawElements = (glDrawElements_t) DetourFunc((LPBYTE)GetProcAddress(hOpenGL, "glDrawElements"), (LPBYTE) &Hooked_glDrawElements, 6);

	global_console.max_lines = 10;
	global_console.cur_lines = 0;
	global_console.start_x = 200;
	global_console.start_y = 200;
	global_console.line_height = 14;
	global_console.oldest_line = 0;
	global_console.lines = (char **) malloc(global_console.max_lines * sizeof(char *));
	for (int i = 0; i <= global_console.max_lines; i++)
	{
		global_console.lines[i] = (char *) calloc(300, sizeof(char));
	}
}

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved) {

	/* open file */
	//FILE *file;
	//fopen_s(&file, "C:\\Users\\italo\\Documents\\howtogetinside\\temp.txt", "a+");

	bool loop = false;
	switch(Reason) {
		case DLL_PROCESS_ATTACH:
			//fprintf(file, "DLL attach function called.\n");
			loop = true;
			HookGL();
			break;
		case DLL_PROCESS_DETACH:
			//fprintf(file, "DLL detach function called.\n");
			break;
		case DLL_THREAD_ATTACH:
			//fprintf(file, "DLL thread attach function called.\n");
			break;
		case DLL_THREAD_DETACH:
			//fprintf(file, "DLL thread detach function called.\n");
			break;
	}

	int **player_data = (int **) 0x0079ba90;
	int offset_fuel = 0x410;
	int offset_money = 0x450;
	int offset_crew = 0xc4;

	HANDLE process_handle = GetCurrentProcess();
	HWND window_handle = GetActiveWindow();

	bool active = true;
	MSG msg;
	while(loop && GetAsyncKeyState(VK_END) >= 0)
	{
		Sleep(20); // TODO: create cooldown

		PeekMessage(&msg, window_handle, 0, 0, PM_REMOVE);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if(msg.message == WM_CLOSE || msg.message == WM_DESTROY || msg.message == WM_ENDSESSION || msg.message == WM_QUERYENDSESSION) break;
		//fprintf(file, "MESSAGE: %d\n", msg.message);
		//fprintf(file, "W PARAM: %d\n", msg.wParam);
		//fprintf(file, "L PARAM: %d\n", msg.lParam);
		//fprintf(file, "WM_QUIT: %d\n", WM_QUIT);

		// toggle cheat
		if (GetAsyncKeyState('H') < 0)
		{
			active = !active;
		}

		if(!active) continue;

		// skip menus
		if(!(*player_data)) continue;

		// A
		if (GetAsyncKeyState('A') < 0)
		{
			(*player_data)[offset_fuel/4] += 10;
		}

		// B
		if (GetAsyncKeyState('B') < 0)
		{
			(*player_data)[offset_money/4] += 22;
		}

		// C
		if (GetAsyncKeyState('C') < 0)
		{
			float **crew_member = (float **) (*player_data)[offset_crew/4];
			if (!(*crew_member)) continue;

			int health_offset = 0x2c;

			(*crew_member)[health_offset/sizeof(float)] -= 4;
		}

		// D
		if (GetAsyncKeyState('D') < 0)
		{
			float **crew_member = (float **) (*player_data)[offset_crew/4];
			if (!(*crew_member)) continue;

			int health_offset = 0x2c;

			(*crew_member)[health_offset/sizeof(float)] += 4;
		}

		// E and F
		if (GetAsyncKeyState('E') < 0 || GetAsyncKeyState('F') < 0)
		{
			int **outer_selected_characters = (int **) 0x79ba98;
			if (!outer_selected_characters) continue;
			if (!(*outer_selected_characters)) continue;
			int *selected_characters = (int *) (*outer_selected_characters)[1];
			int *selected_characters_end = (int *) (*outer_selected_characters)[2];

			if (!selected_characters || !selected_characters_end) continue;
			int debug_counter = 0;

			while (selected_characters != selected_characters_end)
			{
				float *character = (float *) *selected_characters;
				if (!character) break;

				int health_offset = 0x2c;

				int amount;
				if (GetAsyncKeyState('E') < 0) amount = -4;
				else if (GetAsyncKeyState('F') < 0) amount = 4;

				character[health_offset/sizeof(float)] += amount;

				selected_characters = &selected_characters[1];
			}
		}
	}

	/* close file */
	//fclose(file);

	return TRUE;
}
