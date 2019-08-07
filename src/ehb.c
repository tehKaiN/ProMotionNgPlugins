#include "dde.h"
#include <stdio.h>
#include <windows.h>

int main(void) {
	FILE *pLog = fopen("ehb.log", "wb");

	// Do this only once, for example when starting your program
	if (!ddeOpen("PMOTION","ImageServer")) {
		fprintf(pLog, "ERR: Pro Motion server is inactive\n");
		fclose(pLog);
		return EXIT_FAILURE;
	}
	char str[128];

	if (ddeExecute("ImageServerItem", "SendPalette")) {
		ddeRequest("ImageServerItem", str, sizeof(str), false);
		if(strcmp(str, "OK")) {
			fprintf(pLog, "ERR: Invalid response text: '%s'\n", str);
			fclose(pLog);
			return EXIT_FAILURE;
		}
		if (IsClipboardFormatAvailable(CF_DIB)) {
			if (OpenClipboard(NULL)) {
				HANDLE hData = GetClipboardData(CF_DIB);
				if (hData) {
					LPBITMAPINFO pBitmap = (LPBITMAPINFO)GlobalLock(hData);
					if (!pBitmap) {
						fprintf(pLog, "ERR: Couldn't get bitmap\n");
						fclose(pLog);
						return EXIT_FAILURE;
					}
					uint32_t ulColorCount = pBitmap->bmiHeader.biClrUsed;
					fprintf(pLog, "Color count: %lu", ulColorCount);
					for(uint32_t i = 0; i < ulColorCount; ++i) {
						fprintf(
							pLog, "Color: %lu: %02X %02X %02X\n", i,
							pBitmap->bmiColors[i].rgbRed,
							pBitmap->bmiColors[i].rgbGreen,
							pBitmap->bmiColors[i].rgbBlue
						);
					}
					GlobalUnlock(hData);
				}
			}
			CloseClipboard();
		}
	}

	// do this before terminating your program (or when you no longer
	// need to talk to Pro Motion)
	ddeClose();
	fclose(pLog);
	return EXIT_SUCCESS;
}
