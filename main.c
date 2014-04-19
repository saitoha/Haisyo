/*****************************************************************************
 * Copyright 2005-2014  Hayaki Saito <user@zuse.jp>
 *
 * ##### BEGIN GPL LICENSE BLOCK #####
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ##### END GPL LICENSE BLOCK #####
 *
 *       main.c
 *                          ver.0.0    2005/09/24   Hayaki Saito
 *                          ver.0.1    2005/09/25   Hayaki Saito
 *                          var 0.2    2012/09/02   Hayaki Saito
 *
 *****************************************************************************/

#include "config.h"
#include <windows.h>
#include "haisyo.h"
#include "resource.h"

#if !defined(WIN64)

# if !defined(GWL_HINSTANCE)
#  define GWL_HINSTANCE (-6)
# endif

BOOL
ResisterStartUp()
{
    DWORD dwRet = 0;
    LONG ret;
    CHAR szModulePath[_MAX_PATH];
    HKEY hKey;

    /* get full-path for current module */
    dwRet = GetModuleFileNameA(NULL, szModulePath, sizeof(szModulePath));
    if (dwRet == 0)
    {
        return FALSE;
    }

    ret = RegOpenKeyExA(HKEY_CURRENT_USER,
                        IDS_REGKEY_RUN,
                        0, /* reserved */
                        KEY_SET_VALUE,
                        &hKey);

    if (ret != ERROR_SUCCESS) {
        MessageBox(NULL, "#-", IDS_REGISTER_TITLE, MB_OK);
        return FALSE;
    }

    ret = RegSetValueExA(hKey,
                         "haisyo.exe",
                         0, /* reserved */
                         REG_SZ,
                         (const unsigned char*)szModulePath,
                         strlen(szModulePath));
    RegCloseKey(hKey);

    if (ret != ERROR_SUCCESS) {
        MessageBox(NULL, "-", IDS_REGISTER_TITLE, MB_OK);
        return FALSE;
    }

    MessageBox(NULL, IDS_REGISTERED, IDS_REGISTER_TITLE, MB_OK);

    return TRUE;
}

BOOL
UnresisterStartUp()
{
    HKEY hKey;
    LONG ret;

    ret = RegOpenKeyExA(HKEY_CURRENT_USER,
                        IDS_REGKEY_RUN,
                        0, /* reserved */
                        KEY_ALL_ACCESS,
                        &hKey);

    if (ret != ERROR_SUCCESS) {
        return FALSE;
    }

    ret = RegDeleteValue(hKey, "haisyo.exe");

    RegCloseKey(hKey);

    if (ret != ERROR_SUCCESS) {
        return FALSE;
    }

    MessageBox(NULL, IDS_UNREGISTERED, IDS_REGISTER_TITLE, MB_OK);

    return TRUE;
}

LONG IsRegistered()
{
    LONG ret;
    HKEY hKey;

    ret = RegOpenKeyExA(HKEY_CURRENT_USER,
                        IDS_REGKEY_RUN,
                        0, /* reserved */
                        KEY_QUERY_VALUE,
                        &hKey);

    if (ret != ERROR_SUCCESS) {
        return FALSE;
    }

    ret = RegQueryValueExA(hKey,
                           "haisyo.exe",
                           0, /* reserved */
                           NULL,
                           NULL,
                           0);
    RegCloseKey(hKey);

    if (ret != ERROR_SUCCESS) {
        return FALSE;
    }
    return TRUE;
}

EXPORT BOOL
TransparentBltEx(
                 HDC       hdc,        /* device context */
                 PRECT     prc,        /* target rect */
                 HBITMAP   hBmp,       /* source bitmap handle */
                 LONG      leftmargin, /* left margin of drawing region */
                 LONG      topmargin,  /* right margin of drawing rigion */
                 COLORREF  bkcolor     /* background color */
                )
/*
 * Do chroma key drawing.
 */
{
    BITMAP     bm;
    HBITMAP    hMaskBmp;
    HDC        hBmpDC,  hMaskDC;
    COLORREF   oldBkColor;
    COLORREF   oldTextColor;
    INT        oldBkMode;
    BOOL       bRet;

    oldBkMode  = SetBkMode(hdc, TRANSPARENT);
    oldBkColor = SetBkColor(hdc, bkcolor);

    hBmpDC     = CreateCompatibleDC(hdc);
    SelectObject(hBmpDC, hBmp);
    SetBkColor(hBmpDC, bkcolor);
    GetObject(hBmp, sizeof(BITMAP), &bm);

    oldTextColor = SetTextColor(hdc, RGB(0, 0, 0));

    /* craete mask */
    hMaskDC = CreateCompatibleDC(hdc);

    /* create a monochrome bitmap */
    hMaskBmp = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
    SelectObject(hMaskDC, hMaskBmp);

    bRet = BitBlt(hMaskDC, 0, 0, bm.bmWidth, bm.bmHeight,
                  hBmpDC, 0, 0, SRCCOPY);

#if !defined(HAVE_DSTERASE)
/* DSTERASE: dest = dest & (~src) : DSna */
#define DSTERASE 0x00220326
#endif
    /* fill background black */
    bRet = BitBlt(hBmpDC, 0, 0, bm.bmWidth, bm.bmHeight,
                  hMaskDC, 0, 0, DSTERASE);

    /* draw background */
    bRet = BitBlt(hdc,
                  prc->left + leftmargin,
                  prc->top  + topmargin,
                  prc->right,
                  prc->bottom,
                  hMaskDC, 0, 0, SRCAND);

    /* draw foreground */
    bRet = BitBlt(hdc,
                  prc->left + leftmargin,
                  prc->top  + topmargin,
                  prc->right,
                  prc->bottom,
                  hBmpDC, 0, 0, SRCPAINT);

    DeleteObject(&bm);
    DeleteObject(hMaskBmp);
    DeleteDC(hBmpDC);
    DeleteDC(hMaskDC);

    SetTextColor(hdc, oldTextColor);
    SetBkColor(hdc, oldBkColor);
    SetBkMode(hdc, oldBkMode);

    return bRet;
}

EXPORT void
SetAbsoluteForegroundWindow(HWND hWnd) /* target window handle */
/*
 * Bring target window to the front
 * ref: http://techtips.belution.com/ja/vc/0012/
 */
{
    INT nTargetID, nForegroundID;
    DWORD sp_time;

    /* get the thread ID of the current foreground window */
    nForegroundID = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

    /* get the thread ID of foreground foreground window */
    nTargetID = GetWindowThreadProcessId(hWnd, NULL);

    AttachThreadInput(nTargetID, nForegroundID, TRUE);

    if(SPI_SETFOREGROUNDLOCKTIMEOUT)
    {
        /* save the current setting to sp_time */
        SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &sp_time, 0);

        /* change the waiting time of switching window to 0ms */
        SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, 0);
    }

    /* bring target window to the front */
    SetForegroundWindow(hWnd);

    if(SPI_SETFOREGROUNDLOCKTIMEOUT)
    {
        /* restore the setting */
        SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (void*)sp_time, 0);
    }

    AttachThreadInput(nTargetID, nForegroundID, FALSE);
}


void
EmergeLButtonMenu(HWND  hwnd,   /* parent window handle */
                  HMENU hmenu)  /* the menu handle */
/*
 * emerge a menu at the current cursor position.
 */
{
    POINT pt;
    GetCursorPos(&pt);

    /* bring target menu to the front */
    SetAbsoluteForegroundWindow(hwnd);

    /* show menu */
    TrackPopupMenu(hmenu,
                   TPM_RIGHTALIGN,
                   pt.x,
                   pt.y,
                   0,
                   hwnd,
                   NULL);

    /* hide menu */
    PostMessage(hwnd, WM_NULL, 0, 0);
}

EXPORT HFONT
SetMenuFont(LPCTSTR face, /* font face */
            INT height)   /* font height */
/*
http://www.kumei.ne.jp/c_lang/sdk/sdk_46.htm
*/
{
    HFONT hFont;

    hFont = CreateFont(height,
                       0,                       /* width */
                       0,                       /* escapement */
                       0,                       /* orientation */
                       FW_REGULAR,              /* font weight */
                       FALSE,                   /* italic */
                       FALSE,                   /* underline */
                       FALSE,                   /* strike out */
                       SHIFTJIS_CHARSET,        /* character set */
                       OUT_DEFAULT_PRECIS,      /* output precision */
                       CLIP_DEFAULT_PRECIS,     /* cliping precision */
                       PROOF_QUALITY,           /* output quality */
                       FIXED_PITCH | FF_MODERN, /* pitch and family */
                       face);
    return hFont;
}

void
HSSetMenuInfo(HWND                hwnd,   /* parent window handle */
              LPMEASUREITEMSTRUCT lpMI,   /* information of menu size */
              LPCTSTR             *lpstr) /* a string list of menu items */
/*
 * create and initialize a new menu
 */
{
    HDC hdc = GetDC(hwnd);
    SIZE sz;
    HFONT hFont = SetMenuFont("MS Gothic", 14);
    HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);

    GetTextExtentPoint32(hdc,
                         lpstr[lpMI->itemData],
                         lstrlen(lpstr[lpMI->itemData]) - 1,
                         &sz);
    lpMI->itemWidth  = sz.cx + 50;
    lpMI->itemHeight = sz.cy + 16;
    SelectObject(hdc, hFontOld);
    DeleteObject(hFont);
    ReleaseDC(hwnd, hdc);
}

BOOL
DrawVerticalBar(LPDRAWITEMSTRUCT  lpDI,
                RECT              *prcbar)
{
    INT i;

    for (i = 0; i < prcbar->right; i++)
    {
        COLORREF color = RGB(255, 255 - i * 3, 255 - i * 5);
        HPEN hPen = CreatePen(PS_SOLID, 1, color);
        HPEN hOldPen   = SelectObject(lpDI->hDC, hPen);
        MoveToEx(lpDI->hDC, i, prcbar->top, NULL);
        LineTo(lpDI->hDC, i, prcbar->bottom);
        SelectObject(lpDI->hDC, hOldPen);
        DeleteObject(hPen);
    }
    return  TRUE;
}

BOOL
DrawOfficeMenuBack(LPDRAWITEMSTRUCT  lpDI,
                   LONG              barspan)
{
    RECT rcbar;
    RECT rcback;

    /*
     * validate the width of specified rect
     */
    if(lpDI->rcItem.right - lpDI->rcItem.left <= barspan)
    {
        return FALSE;
    }

    /* draw vertical gradation bar */
    SetRect(&rcbar,
            lpDI->rcItem.left,
            lpDI->rcItem.top,
            lpDI->rcItem.left + barspan,
            lpDI->rcItem.bottom);
    DrawVerticalBar(lpDI, &rcbar);

    /* draw the other region */
    SetRect(&rcback,
            rcbar.right,
            lpDI->rcItem.top,
            lpDI->rcItem.right,
            lpDI->rcItem.bottom);
    FillRect(lpDI->hDC, &rcback, WHITE_BRUSH);
    return TRUE;
}


BOOL
DrawActiveRect(LPDRAWITEMSTRUCT  lpDI,
               LONG              marginleft,
               LONG              margintop,
               LONG              marginright,
               LONG              marginbottom)
{
    HPEN   hPen      = CreatePen(PS_INSIDEFRAME, 1, RGB(116, 116, 116));
    HPEN   hOldPen   = SelectObject(lpDI->hDC, hPen);
    HBRUSH hBrush    = CreateSolidBrush(RGB(240, 236, 234));
    HBRUSH hOldBrush = SelectObject(lpDI->hDC, hBrush);

    (BOOL) Rectangle(lpDI->hDC,
                     lpDI->rcItem.left   + marginleft,
                     lpDI->rcItem.top    + margintop,
                     lpDI->rcItem.right  - marginright,
                     lpDI->rcItem.bottom - marginbottom);
    SelectObject(lpDI->hDC, hOldPen);
    DeleteObject(hPen);
    SelectObject(lpDI->hDC, hOldBrush);
    DeleteObject(hBrush);
    return TRUE;
}

BOOL
DrawMenuText(LPDRAWITEMSTRUCT lpDI,
             LPCTSTR          str,
             LPCTSTR          fontface,
             UINT             fontsize,
             COLORREF         activeTextColor,
             COLORREF         inactiveTextColor,
             LONG             marginleft,
             LONG             margintop)
{
    INT oldBkMode;
    COLORREF oldTextColor = 0;
    HFONT hFont = SetMenuFont(fontface, fontsize);
    HFONT hFontOld = (HFONT)SelectObject(lpDI->hDC, hFont);

    if ((lpDI->itemState  & ODS_SELECTED) &&
        (lpDI->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
    {
        DrawActiveRect(lpDI, 4, 2, 4, 2);
        oldTextColor = SetTextColor(lpDI->hDC, activeTextColor);
    }
    else if(!(lpDI->itemState & ODS_SELECTED) &&
            (lpDI->itemAction & ODA_SELECT))
    {
        oldTextColor = SetTextColor(lpDI->hDC, inactiveTextColor);
    }

    oldBkMode = SetBkMode(lpDI->hDC, TRANSPARENT);
    TextOut(lpDI->hDC,
            lpDI->rcItem.left + marginleft,
            lpDI->rcItem.top  + margintop,
            str,
            lstrlen(str));
    SetBkMode(lpDI->hDC, oldBkMode);
    SetTextColor(lpDI->hDC, oldTextColor);
    SelectObject(lpDI->hDC, hFontOld);
    DeleteObject(hFont);

    return TRUE;
}

void
DrawMenuIcon(HINSTANCE hInst,
             HDC     hdc,
             PRECT   prc,
             UINT    iconID)
{
    BITMAP  bm;
    HBITMAP hBmp;

    hBmp = LoadImage(hInst,
                     MAKEINTRESOURCE(iconID),
                     IMAGE_BITMAP,
                     0,
                     0,
                     LR_CREATEDIBSECTION);

    GetObject(hBmp, sizeof(BITMAP), &bm);
    TransparentBltEx(hdc,
                     prc,
                     hBmp,
                     8,
                     5,
                     RGB(255, 255, 255));
    DeleteObject(&bm);
    DeleteObject(hBmp);
}

void
DrawMenu(
         HWND              hwnd,
         LPDRAWITEMSTRUCT  lpDI,
         LPCTSTR           str,
         UINT              iconID
        )
{
    HINSTANCE hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);

    /* draw background */
    DrawOfficeMenuBack(lpDI, 35);

    /* draw text */
    DrawMenuText(lpDI,
                 str,
                 "MS Gothic",
                 14,
                 RGB(145, 31, 23),
                 RGB(14, 31, 23),
                 44,
                 8);

    /* draw icon image */
    DrawMenuIcon(hInst,
                 lpDI->hDC,
                 &lpDI->rcItem,
                 iconID);
}

BOOL
InitInstance(HINSTANCE hInst)
/*
 * create main window
 */
{
    HWND hWnd;
    NOTIFYICONDATA nIcon;

    hWnd = CreateWindow(IDS_CLASS,    /* window class */
                        IDS_TITLE,    /* title bar */
                        WS_DISABLED,  /* kind of window */
                        0,            /* x position */
                        -20,          /* y position */
                        0,            /* width */
                        0,            /* height */
                        NULL,         /* parent window handle */
                        NULL,         /* menu handle */
                        hInst,        /* instance handle */
                        NULL);
    if (!hWnd)
    {
        return FALSE;
    }

    nIcon.cbSize = sizeof(NOTIFYICONDATA);
    nIcon.uID = 1;
    wsprintf(nIcon.szTip, IDS_HAISYO); /* tooltip */
    nIcon.hWnd = hWnd;
    nIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nIcon.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HAISYO));
    nIcon.uCallbackMessage = WM_TRYCLK;

    return Shell_NotifyIcon(NIM_ADD, &nIcon);
}

LRESULT CALLBACK
WndProc(HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp)
/*
 * the window procedure
 */
{
    static HMENU hmenu;
    enum            { MENU_REGISTER, MENU_UNREGISTER, MENU_END   };
    LPCTSTR str[] = { IDS_REGISTER,  IDS_UNREGISTER,  IDS_CANCEL };
    UINT itemID[] = { IDB_CONFIG,    IDB_CONFIG,      IDB_CANCEL };

    switch (msg)
    {
    case WM_MEASUREITEM:
        HSSetMenuInfo(hWnd, (LPMEASUREITEMSTRUCT)lp, str);
        return TRUE;

    case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT lpDI = (LPDRAWITEMSTRUCT)lp;
            switch (lpDI->itemID)
            {
            case IDM_STARTUP:
                if (IsRegistered()) {
                  DrawMenu(hWnd, (LPDRAWITEMSTRUCT)lp, str[MENU_UNREGISTER], itemID[MENU_UNREGISTER]);
                } else {
                  DrawMenu(hWnd, (LPDRAWITEMSTRUCT)lp, str[MENU_REGISTER], itemID[MENU_REGISTER]);
                }
                break;
            case IDM_END:
                DrawMenu(hWnd, (LPDRAWITEMSTRUCT)lp, str[MENU_END], itemID[MENU_END]);
                break;
            default:
                break;
            }
        }
        return TRUE;

    case WM_COMMAND:
        switch(wp)
        {
        case IDM_STARTUP:
            if (IsRegistered()) {
              UnresisterStartUp(hWnd, "", NULL);
            } else {
              ResisterStartUp(hWnd, "", NULL);
            }
            break;

        case IDM_END:
            DestroyWindow(hWnd);
            break;
        }
        break;

    case WM_CREATE:
        {
            if(!SetHaisyoHook())
            {
                /* if fook is failed */
                DestroyWindow(hWnd);
                break;
            }

            /* create task tray menu */
            hmenu = CreatePopupMenu();
            AppendMenu(hmenu,
                       MF_OWNERDRAW,
                       IDM_STARTUP,
                       (LPCTSTR)MENU_REGISTER);

            AppendMenu(hmenu,
                       MF_OWNERDRAW,
                       IDM_END,
                       (LPCTSTR)MENU_END);
        }
        break;

    case WM_TRYCLK:
        /* process click event at tray icon */
        switch(lp)
        {
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
            /* detect click and show popup menu */
            EmergeLButtonMenu(hWnd, hmenu);
            break;
        }
        break;

    case WM_DESTROY:
        /* clean up  */

        /* unregister hook */
        UnsetHaisyoHook();

        /* destroy popup menu */
        DestroyMenu(hmenu);

        PostQuitMessage(0);
        break;

    default:
        return (DefWindowProc(hWnd, msg, wp, lp));

    }
    return 0;

}

ATOM InitApp(HINSTANCE hInst)
/*
 * Register a window class
 */
{
    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_BYTEALIGNWINDOW;
    wc.lpfnWndProc = WndProc;                    /* window procedure name */
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;                        /* module instance */
    wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HAISYO));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;                      /* the name of menu */
    wc.lpszClassName = (LPCSTR)IDS_CLASS;
    wc.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_HAISYO));

    return RegisterClassEx(&wc);
}

/* ref: http://blog.monoweb.info/blog/2009/09/13/64bit-hook */
void Launch64bitSurrogate()
{
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    STARTUPINFO lpStartupInfo;
    PROCESS_INFORMATION lpProcessInformation;
    CHAR szPath[_MAX_PATH * 2];
    CHAR szDir[_MAX_PATH * 2];
    CHAR sz64BinaryPath[_MAX_PATH * 2];
    BOOL is_wow64;
    HMODULE hKernel32;

    hKernel32 = GetModuleHandleA("kernel32");
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hKernel32, "IsWow64Process");
    is_wow64 = FALSE;
    if (fnIsWow64Process && fnIsWow64Process(GetCurrentProcess(), &is_wow64) && is_wow64) {
        if (GetModuleFileNameA(NULL, szPath, sizeof(szPath))) {
            _splitpath(szPath, sz64BinaryPath, szDir, NULL, NULL); 
            if (strcat(sz64BinaryPath, szDir) && strcat(sz64BinaryPath, "haisyo64surrogate.exe")) {
                GetStartupInfo(&lpStartupInfo);
                CreateProcess(NULL, sz64BinaryPath,
                              0, 0, FALSE, NORMAL_PRIORITY_CLASS,
                              0, 0, &lpStartupInfo, &lpProcessInformation);
            }
        }
    }
}

#endif  /* !defined(WIN64) */

INT WINAPI
WinMain(HINSTANCE hCurInst,
        HINSTANCE hPrevInst,
        LPSTR     lpsCmdLine,
        INT       nCmdShow)
{
    HANDLE hMutex;

    UNUSED_VARIABLE(hPrevInst);
    UNUSED_VARIABLE(lpsCmdLine);
    UNUSED_VARIABLE(nCmdShow);
#if WIN64
    UNUSED_VARIABLE(hCurInst);
    hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, IDS_LOCKSTRING);
    if (hMutex) {
        if(SetHaisyoHook()) {
            WaitForSingleObject(hMutex , INFINITE);
            UnsetHaisyoHook();
        }
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }
    return 0;
#else
    MSG msg;

    hMutex = CreateMutex(NULL, TRUE, IDS_LOCKSTRING);

    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return FALSE;
    }

    if (!InitApp(hCurInst))
    {
        return FALSE;
    }

    if (!InitInstance(hCurInst))
    {
        return FALSE;
    }

    Launch64bitSurrogate();

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    return msg.wParam;
#endif  /* WIN64 */
}

/* EOF */
