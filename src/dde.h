#ifndef _DDE_H_
#define _DDE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

bool ddeOpen(const char *szServer, const char *szTopic);

void ddeClose(void);

bool ddeRequest(
	const char *szItem, char *szValue, uint32_t ulMax, bool isCrLf
);

bool ddeExecute(const char *szItem, const char *szCommand);

bool ddeQueryOk(const char *szCommand);

#ifdef __cplusplus
}
#endif

#endif // _DDE_H_
