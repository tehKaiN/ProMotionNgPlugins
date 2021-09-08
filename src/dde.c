#include "dde.h"
#include <windows.h>
#include <ddeml.h>

// Based on code of Thiadmer Riemersma from PMNG sample plugin zip

#if !defined(__WIN32__)
static HINSTANCE s_hInst;
#endif

#ifndef _export
// GCC hack
#define _export
#endif

#define DDE_RETRIES 5
#define DDE_TIMEOUT 10000

static FARPROC s_cbDde = NULL;
static DWORD s_dwDdeInst = 0L; // DDE instance id
static HCONV s_hConvDde = 0; // DDE conversation id
static HSZ s_hServer, s_hTopic; // usually constant for a conversation
static bool s_isDdeReady;

#pragma argsused
HDDEDATA CALLBACK _export ddeCallback(
	WORD wType, WORD wFmt, HCONV hConv, HSZ hsz1, HSZ hsz2, HDDEDATA hData,
  DWORD dwData1, DWORD dwData2
) {
  switch (wType) {
  case XTYP_DISCONNECT:
		// Disconnect, handle no longer valid
    s_hConvDde = 0;
    if (s_hServer != 0 && s_hTopic != 0) {
      DdeFreeStringHandle(s_dwDdeInst, s_hServer);
      DdeFreeStringHandle(s_dwDdeInst, s_hTopic);
      s_hServer = NULL;
      s_hTopic = NULL;
    }
    break;
  case XTYP_XACT_COMPLETE:
    s_isDdeReady = true;
    break;
  }
  return NULL;
}

bool ddeOpen(const char *szServer, const char *szTopic) {
  #if defined __WIN32__
    s_cbDde = (FARPROC)ddeCallback;
  #else
		#error FIXME: pass hInst somehow without including windows.h and its types to .h
		// In 32-bit, this call is unnecessary, so do it only on 16-bit.
    s_cbDde = MakeProcInstance((FARPROC)ddeCallback, s_hInst);
  #endif
  if (DdeInitialize(
		&s_dwDdeInst, (PFNCALLBACK)s_cbDde, APPCMD_CLIENTONLY, 0) != DMLERR_NO_ERROR
	) {
    return false;
	}

  s_hServer = DdeCreateStringHandle(s_dwDdeInst, szServer, CP_WINANSI);
  s_hTopic  = DdeCreateStringHandle(s_dwDdeInst, szTopic, CP_WINANSI);

  s_hConvDde = DdeConnect(s_dwDdeInst, s_hServer, s_hTopic, NULL);
  if (s_hConvDde != 0)
   return true;

  /* failure, clean up */
  DdeFreeStringHandle(s_dwDdeInst, s_hServer);
  DdeFreeStringHandle(s_dwDdeInst, s_hTopic);
  s_hServer = NULL;
  s_hTopic = NULL;
  return false;
}

void ddeClose(void) {
  if (s_hConvDde != 0) {
    DdeDisconnect(s_hConvDde);
    s_hConvDde = 0;
  }
  if (s_hServer != 0 && s_hTopic != 0) {
    DdeFreeStringHandle(s_dwDdeInst, s_hServer);
    DdeFreeStringHandle(s_dwDdeInst, s_hTopic);
    s_hServer = 0;
    s_hTopic = 0;
  }
  DdeUninitialize(s_dwDdeInst);
  #if !defined __WIN32__
    FreeProcInstance(s_cbDde);
  #endif
}

bool ddeRequest(
	const char *szItem, char *szValue, uint32_t ulMax, bool isCrLf
) {
  HSZ hszItem;
  HDDEDATA hData;
  DWORD dwSize;
  UINT Err;
  short retry = 0;

  *szValue = '\0';
  hszItem = DdeCreateStringHandle(s_dwDdeInst, szItem, CP_WINANSI);

  do {
    Err = DMLERR_NO_ERROR;
    hData = DdeClientTransaction(NULL, 0, s_hConvDde, hszItem, CF_TEXT, XTYP_REQUEST, 1000, NULL);
    if (!hData) {
      Err = DdeGetLastError(s_dwDdeInst);
		}
    retry++;
  } while (Err!=DMLERR_NO_ERROR && retry < DDE_RETRIES);
  DdeFreeStringHandle(s_dwDdeInst, hszItem);

  if (Err)
    return false;

  dwSize = DdeGetData(hData,NULL,0,0);
  if (dwSize > ulMax) {
    dwSize = ulMax;
	}
  DdeGetData(hData, (LPBYTE)szValue, dwSize, 0L);
  szValue[(int)dwSize] = '\0'; // make sure it is zero terminated
  if (!isCrLf) {
    int length = lstrlen(szValue);
    if (szValue[length - 2] == '\r') {
      szValue[length - 2] = '\0';
		}
  }
  DdeFreeDataHandle(hData);

  return true;
}

bool ddeExecute(const char *szItem, const char *szCommand) {
  HSZ hszItem;
  DWORD time;
  MSG msg;

  hszItem = DdeCreateStringHandle(s_dwDdeInst, szItem, CP_WINANSI);
  DdeClientTransaction(
		(LPBYTE)szCommand,strlen(szCommand)+1 , s_hConvDde, hszItem, CF_TEXT,
    XTYP_EXECUTE, TIMEOUT_ASYNC, NULL
	);

  s_isDdeReady = false;
  time = GetTickCount();
  while (!s_isDdeReady) {
		// wait 10 seconds, fail otherwise
    if (GetTickCount() > time + DDE_TIMEOUT) {
      break;
		}
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  DdeFreeStringHandle(s_dwDdeInst, hszItem);

  return s_isDdeReady;
}

bool ddeQueryOk(const char *szCommand) {
	if(!ddeExecute("ImageServerItem", szCommand)) {
		return false;
	}
	char szResult[128];
	ddeRequest("ImageServerItem", szResult, sizeof(szResult), false);
	if(strcmp(szResult, "OK")) {
		return false;
	}
	return true;
}
