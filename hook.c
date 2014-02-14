/* Hello, Emacs, this is -*-C-*- */

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
 *       hook.c
 *                          ver.0.0    2005/09/24   Hayaki Saito
 *                          ver.0.1    2005/09/25   Hayaki Saito
 *                          ver.0.2    2012/09/02   Hayaki Saito
 *                          ver.0.3    2014/02/12   Hayaki Saito
 *
 *****************************************************************************/

#include <windows.h>
#include <shlwapi.h>
#include "hook.h"
#include "resource.h"

/* shared instance in system global */
#if defined(__GNUC__)
HHOOK hCBTHook __attribute__((section(".hook"), shared)) = NULL;
#else
# pragma data_seg(".hook")
HHOOK hCBTHook = NULL; 
# pragma data_seg()
#endif

HINSTANCE hInst;      /* dll instance */
BOOL is_hooking;

BOOL CALLBACK
HaisyoniseWindow(HWND hwnd , LPARAM lp)
{
    CHAR buffer[4096];
    CHAR *p = NULL;

    if (GetClassName(hwnd, buffer, 8))
    {
        if (strcmp(buffer, "Button") == 0) 
        {
            if (GetWindowText(hwnd, buffer, sizeof(buffer)))
            {
                if (strcmp(buffer, IDS_OK) == 0)
                {
                    SetWindowText(hwnd, IDS_HAISYO);
                }
                else
                {
                    p = strstr(buffer, IDS_YES);
                    if (p) 
                    {
                        memcpy(p, IDS_HAISYO, sizeof(IDS_YES));
                    }
                    SetWindowText(hwnd, buffer);
                }
            }
        }
        else if (strcmp(buffer, "Static") == 0)
        {
            if (GetWindowText(hwnd, buffer, sizeof(buffer)))
            {
                p = strstr(buffer, IDS_KUDASAI);
                if (p) 
                {
                    memcpy(p, IDS_ITADAKITAKU, sizeof(IDS_KUDASAI));
                }
                p = strstr(buffer, IDS_SHIMASU);
                if (p) 
                {
                    memcpy(p, IDS_SHITAKU, sizeof(IDS_SHIMASU));
                }
                SetWindowText(hwnd, buffer);
            }
            EnumChildWindows(hwnd, HaisyoniseWindow, (LONG)NULL);
        }
        else
        {
            EnumChildWindows(hwnd, HaisyoniseWindow, (LONG)NULL);
        }
    }

    return TRUE;
}

EXPORT LRESULT CALLBACK
CBTHookProc(
            int nCode,
            WPARAM wp,
            LPARAM lp
           )
/*
 * The hook procedure for CBT actions
 */
{ 
    switch (nCode)
    {
    case HCBT_ACTIVATE:
    case HCBT_QS:
        HaisyoniseWindow((HWND)wp, (LONG)NULL);
        break;
    default:
        break;
    }

    return CallNextHookEx(hCBTHook, nCode, wp, lp); 
}


int __stdcall
DllMain(
        HINSTANCE hInstance,    /* dll instance handle */
        DWORD     dwReason,     /* reason for calling */
        LPVOID    lpReserved    /* reserved */
       )
/*
 * save module instance handle
 */
{    
    switch(dwReason) 
    {
    case DLL_PROCESS_ATTACH:
        /* when attached */
        hInst        = hInstance;
        break;
    return TRUE;
    }
    return TRUE;
}


EXPORT BOOL CALLBACK 
IsHooking(void) 
/*
 * detect if this process is hooked
 */
{ 
    return is_hooking;
} 

EXPORT BOOL CALLBACK 
SetHaisyoHook(void) 
/*
 * start hooking
 */
{
    if(!hInst) 
    { 
        return FALSE;
    } 
    /* set hook */
    hCBTHook = SetWindowsHookEx(WH_CBT, 
                                CBTHookProc, 
                                hInst, 
                                (ULONG)NULL);
    is_hooking = TRUE; 
    return TRUE;
} 

EXPORT BOOL CALLBACK 
UnsetHaisyoHook(void) 
/*
 * exit hooking
 */
{
    /* reset hook */
    UnhookWindowsHookEx(hCBTHook);
    is_hooking = FALSE;     
  
    return TRUE;
} 

