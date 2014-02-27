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
 *       haisyo.c
 *                          ver.0.0    2005/09/24   Hayaki Saito
 *                          ver.0.1    2005/09/25   Hayaki Saito
 *                          ver.0.2    2012/09/02   Hayaki Saito
 *                          ver.0.3    2014/02/12   Hayaki Saito
 *
 *****************************************************************************/

#include "config.h"
#include <windows.h>
#include "haisyo.h"
#include "resource.h"

/* shared instance in system global */
#if defined(__GNUC__)
HHOOK hHook __attribute__((section(".haisyo"), shared)) = NULL;
#else
# pragma data_seg(".haisyo")
HHOOK hHook = NULL;
# pragma data_seg()
#endif

HINSTANCE hInst;      /* dll instance */
BOOL is_hooking;

BOOL CALLBACK
HaisyoniseWindow(HWND hwnd , LPARAM lp)
{
    UINT i;
    INT origlen, newlen, postlen;
    CHAR buffer[4096];
    CHAR *p = NULL;
    CHAR *orig_words[] = {
        IDS_KUDASAI1,
        IDS_SHIMASUKA1,
        IDS_SHIMASU1,
        IDS_DEKIMASU1,
        IDS_ARIMASU1
    };
    CHAR *new_words[] = {
        IDS_KUDASAI2,
        IDS_SHIMASUKA2,
        IDS_SHIMASU2,
        IDS_DEKIMASU2,
        IDS_ARIMASU2
    };
    UNUSED_VARIABLE(lp);

    if (GetClassName(hwnd, buffer, 8))
    {
        if (strcmp(buffer, "Button") == 0)
        {
            if (GetWindowText(hwnd, buffer, sizeof(buffer) - 32))
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
        else /* if (strcmp(buffer, "Static") == 0) */
        {
            if (GetWindowText(hwnd, buffer, sizeof(buffer) - 32))
            {
                for (i = 0; i < sizeof(orig_words) / sizeof(*orig_words); i++)
                {
                    p = strstr(buffer, orig_words[i]);
                    if (p)
                    {
                        origlen = strlen(orig_words[i]);
                        newlen = strlen(new_words[i]);
                        postlen = strlen(p + origlen);
                        memmove(p + newlen, p + origlen, postlen + 1);
                        memcpy(p, new_words[i], newlen);
                    }
                }

                SetWindowText(hwnd, buffer);
            }
            EnumChildWindows(hwnd, HaisyoniseWindow, (LONG)NULL);
        }
    }

    return TRUE;
}

EXPORT LRESULT CALLBACK
HookProc(
         int nCode,
         WPARAM wp,
         LPARAM lp
        )
/*
 * The hook procedure for CBT actions
 */
{
    CWPRETSTRUCT *msg;

    msg = (CWPRETSTRUCT *)lp;
    switch (msg->message) {
        case WM_ACTIVATE:
        case WM_CREATE:
            HaisyoniseWindow((HWND)msg->hwnd, (LONG)NULL);
        default:
            break;
    }

    return CallNextHookEx(hHook, nCode, wp, lp);
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
    UNUSED_VARIABLE(lpReserved);

    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
        /* when attached */
        hInst = hInstance;
        break;
    default:
        break;
    }
    return TRUE;
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

    if (is_hooking)
    {
        return FALSE;
    }

    /* set hook */
    hHook = SetWindowsHookEx(WH_CALLWNDPROCRET,
                             HookProc,
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
    if (is_hooking)
    {
        (BOOL) UnhookWindowsHookEx(hHook);
    }

    hHook = NULL;
    is_hooking = FALSE;
    return TRUE;
}

/* EOF */
