/*******************************************************************************
 *
 *       hook.c
 *                          ver.0.0    2005/09/24   Hayaki Saito
 *                          ver.0.1    2005/09/25   Hayaki Saito
 *                          ver.0.2    2012/09/02   Hayaki Saito
 *
 ******************************************************************************/

#include <windows.h>
#include <winuser.h>
#include <shlwapi.h>
#include "hook.h"
#include "resource.h"

/* shared instance in system global */
#pragma data_seg(".hook")
HHOOK        hCBTHook = 0; 
#pragma data_seg()

HINSTANCE hInst;      /* dll instance */
BOOL      setflag;

BOOL CALLBACK
EnumWindowsProc1(HWND hwnd , LPARAM lp)
{
	CHAR buffer[4096];

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
					char *p = strstr(buffer, IDS_YES);
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
				char *p = strstr(buffer, IDS_KUDASAI);
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
			EnumChildWindows(hwnd, (void *)EnumWindowsProc1, (LONG)NULL);
		}
		else
		{
			EnumChildWindows(hwnd, (void *)EnumWindowsProc1, (LONG)NULL);
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
	if (nCode == HCBT_ACTIVATE || nCode == HCBT_QS)
	{
		EnumWindowsProc1((HWND)wp, (LONG)NULL);
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
	if(setflag) 
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
} 

EXPORT BOOL CALLBACK 
MySetHook(void) 
/*
 * start to hooking
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
	setflag = TRUE; 
	return TRUE;
} 

EXPORT BOOL CALLBACK 
MyEndHook(void) 
/*
 * exit to hooking
 */
{
	/* reset hook */
	UnhookWindowsHookEx(hCBTHook);
	setflag = FALSE; 	
  
	return TRUE;
} 

