/*******************************************************************************
 *
 *       hooker.h  -- hooker.c の関数プロトタイプ・グローバル変数定義等
 *
 *                          ver.0.0    2005/09/24   H. Saito
 *                          ver.0.1    2005/09/25   H. Saito
 *
 *******************************************************************************/

#pragma once


#define EXPORT __declspec(dllexport) 

//
//グローバル変数
//
char      tmp[256];


//
// 関数宣言
//
EXPORT long CALLBACK 
ReadIniFile(void); 

EXPORT BOOL CALLBACK 
IsHooking(void); 

EXPORT BOOL CALLBACK 
MySetHook(void); 

EXPORT BOOL CALLBACK 
MyEndHook(void); 

EXPORT LRESULT CALLBACK 
CBTHookProc(
			int nCode, 
			WPARAM wp, 
			LPARAM lp
			);

EXPORT LRESULT CALLBACK 
WndHookProc(
			int nCode, 
			WPARAM wp, 
			LPARAM lp
			);

EXPORT BOOL CALLBACK
SlideWindow(
			HWND  hwnd,
			int   spd,
			DWORD dwFlags
			);

EXPORT BOOL CALLBACK
ThrowWindow(
				  HWND hwnd,
				  RECT *ptwdrc,
				  int  spd
				  );

BOOL CALLBACK 
EnumWindowsProc(
                HWND hwnd,
                LPARAM lParam
				);

EXPORT BOOL CALLBACK 
EnumActivateProc(
				 HWND hwnd,
				 LPARAM lParam
				 );

BOOL 
HideWindow(
		   HWND hWnd,
		   int vx,
		   int vy
		   );

BOOL 
EmergeWindow(
			 HWND hwnd,
			 long spd        //スピード
			 );

