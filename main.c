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
#include <shlobj.h>
#include "haisyo.h"
#include "resource.h"

#if !defined(GWL_HINSTANCE)
# define GWL_HINSTANCE (-6)
#endif

HANDLE g_hMutex;

HRESULT 
CreateShortcut(LPSTR pszLink,
               LPSTR pszFile,
               LPSTR pszDescription,
               LPSTR pszArgs,
               LPSTR pszWorkingDir,
               LPSTR pszIconPath,
               INT iIcon,
               INT iShowCmd)
/*
 * Create shortcut
 *  ref: http://techtips.belution.com/ja/vc/0030/
 */
{
    HRESULT hr;
    IShellLink *spShellLink;
    IPersistFile *spPersistFile;
    WORD wsz[MAX_PATH];

    spShellLink = NULL;
    spPersistFile = NULL;

    /* Create a object implements IShellLink */
    hr = CoCreateInstance(&CLSID_ShellLink, 
                          NULL, 
                          CLSCTX_INPROC_SERVER,              
                          &IID_IShellLink,
                          (void **)&spShellLink);
    if (!SUCCEEDED(hr) || NULL == spShellLink)
    {
        goto end;
    }

    /* get IPersist */
    hr = spShellLink->lpVtbl->QueryInterface(spShellLink,
                                             &IID_IPersistFile,
                                             (void **)&spPersistFile);
    if (!SUCCEEDED(hr) || NULL == spPersistFile)
    {
        goto end;
    }

    spShellLink->lpVtbl->SetPath(spShellLink, pszFile);
    spShellLink->lpVtbl->SetDescription(spShellLink, pszDescription);
    spShellLink->lpVtbl->SetArguments(spShellLink, pszArgs);
    spShellLink->lpVtbl->SetWorkingDirectory(spShellLink, pszWorkingDir);
    spShellLink->lpVtbl->SetIconLocation(spShellLink, pszIconPath, iIcon);
    spShellLink->lpVtbl->SetShowCmd(spShellLink, iShowCmd);

    if (!MultiByteToWideChar(CP_ACP, 0, pszLink, -1, wsz, MAX_PATH))
    {
        hr = E_UNEXPECTED;
        goto end;
    }

    /* persist to a file */
    hr = spPersistFile->lpVtbl->Save(spPersistFile, wsz, TRUE);

end:
    if (spPersistFile)
    {
        spPersistFile->lpVtbl->Release(spPersistFile);
    }
    if (spShellLink)
    {
        spShellLink->lpVtbl->Release(spShellLink);
    }
    return hr;
}


BOOL
ResisterStartUp(HWND    hDlg,
                LPSTR   pszFile,
                LPSTR   pszDescription,
                LPSTR   pszIconPath)
/*
 * Register shortcut to startup directory
 * ref: http://techtips.belution.com/ja/vc/0030/
 */
{
#if defined(HAVE_SHGETFOLDERPATH)
    HRESULT hr = E_FAIL;
#elif defined(HAVE_SHGETSPECIALFOLDERPATH)
    BOOL bRet = FALSE;
#endif
    DWORD dwRet = 0;
    TCHAR szModulePath[_MAX_PATH];
    TCHAR szSysPath[_MAX_PATH];

    /* get full-path for current module */
    dwRet = GetModuleFileName(NULL, szModulePath, sizeof(szModulePath));
    if (dwRet == 0)
    {
        return FALSE;
    }

    /* get the absolute path of startup directory */
#if defined(_MSC_VER) || defined(HAVE_SHGETFOLDERPATH)
    hr = SHGetFolderPath(NULL, CSIDL_COMMON_STARTUP, NULL,
                         0 /* SHGFP_TYPE_CURRENT */, szSysPath);
    /* note that hr may be S_FALSE (CSIDL is valid but the folder is not found) */
    if (!SUCCEEDED(hr))
    {
        return FALSE;
    }
#elif defined(HAVE_SHGETSPECIALFOLDERPATH)
    bRet = SHGetSpecialFolderPath(hDlg, szSysPath, CSIDL_STARTUP, TRUE);
    if (!bRet)
    {
        return FALSE;
    }
#else
    /* TODO: try to do dynamic import using LoadLibrary */
#endif

    /* build the absolute path of target shortcut file
     * (don't use PathCombine for compatibility) */
    strcat(szSysPath, pszFile);

    /* create shortcut file */
    if (SUCCEEDED(CoInitialize(NULL)))
    {
        CreateShortcut(szSysPath,
                       szModulePath,
                       pszFile, 
                       pszDescription, 
                       NULL,
                       pszIconPath,
                       0,
                       0);
        CoUninitialize();
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
    SetRect(
        &rcbar,
        lpDI->rcItem.left, 
        lpDI->rcItem.top,
        lpDI->rcItem.left + barspan,
        lpDI->rcItem.bottom);
    DrawVerticalBar(lpDI, &rcbar);

    /* draw the other region */
    SetRect(
        &rcback,
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
             COLORREF         ActiveTextColor,
             COLORREF         InactiveTextColor,
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
        oldTextColor = SetTextColor(lpDI->hDC, RGB(145, 31, 23)); 
    }
    else if(!(lpDI->itemState & ODS_SELECTED) &&
            (lpDI->itemAction & ODA_SELECT))
    { 
        oldTextColor = SetTextColor(lpDI->hDC, RGB(14, 31, 23));
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
RotateBlt(HINSTANCE hInst,
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
DrawMyMenu(
           HWND              hwnd,
           LPDRAWITEMSTRUCT  lpDI, 
           LPCTSTR           str,
           UINT              iconID)
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
    RotateBlt(hInst,
              lpDI->hDC,
              &lpDI->rcItem,
              iconID);
}

 
BOOL 
InitInstance(HINSTANCE hInst, 
             INT nCmdShow) 
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

    enum { MENU_STARTUP, MENU_END };
    LPCTSTR str[] = { IDS_CONFIG, IDS_CANCEL };
    UINT itemID[] = { IDB_CONFIG, IDB_CANCEL };

    switch (msg) 
    { 
    case WM_MEASUREITEM: 
        HSSetMenuInfo(hWnd, (LPMEASUREITEMSTRUCT)lp, str);
        return TRUE;
        
    case WM_DRAWITEM: 
        {
            LPDRAWITEMSTRUCT lpDI = (LPDRAWITEMSTRUCT)lp;
            if (lpDI->itemID == IDM_STARTUP)
            {
                DrawMyMenu(hWnd, (LPDRAWITEMSTRUCT)lp, str[MENU_STARTUP], itemID[MENU_STARTUP]);
            }
            else if (lpDI->itemID == IDM_END)
            {
                DrawMyMenu(hWnd, (LPDRAWITEMSTRUCT)lp, str[MENU_END], itemID[MENU_END]);
            }
        }
        return TRUE;

    case WM_COMMAND:
        switch(wp)
        {
        case IDM_STARTUP:            
            ResisterStartUp(hWnd, "\\" IDS_HAISYO ".lnk", "", NULL);
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
                       (LPCTSTR)MENU_STARTUP);
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
        case WM_RBUTTONUP:
            /* detect right click and show popup menu */
            EmergeLButtonMenu(hWnd, hmenu);
            break;
        }
        break;

    case WM_DESTROY:
        /* clean up  */

        /* unregister hook */
        UnsetHaisyoHook(); 

        /* release Mutex */
        CloseHandle(g_hMutex);
        
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

INT WINAPI
WinMain(HINSTANCE hCurInst, 
        HINSTANCE hPrevInst, 
        LPSTR     lpsCmdLine, 
        INT       nCmdShow) 
{ 
    MSG msg; 

    g_hMutex = CreateMutex(NULL, TRUE, "f98c3702-c97b-4b8a-a6e8-d038a583b30b");

    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return FALSE;
    }
        
    if (!InitApp(hCurInst)) 
    {
        return FALSE;
    }

    if (!InitInstance(hCurInst, nCmdShow)) 
    {
        return FALSE;
    } 

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 
    return msg.wParam; 
}

/* EOF */
