#include <promotion.h>
#include <string>
#include "winpp.h"
#include "dde.h"

tProMotion::tProMotion(void)
{
	if (!ddeOpen("PMOTION","ImageServer")) {
		m_isError = true;
		return;
	}
}

tProMotion::~tProMotion(void)
{
	ddeClose();
}

void printBmiHeader(BITMAPINFOHEADER *pHeader) {
	printf("biPlanes: %hu\n", pHeader->biPlanes);
	printf("biBitCount: %hu\n", pHeader->biBitCount);
	printf("biClrImportant: %lu\n", pHeader->biClrImportant);
	printf("biClrUsed: %lu\n", pHeader->biClrUsed);
	printf("biCompression: %lu\n", pHeader->biCompression);
	printf("biWidth: %ld\n", pHeader->biWidth);
	printf("biHeight: %ld\n", pHeader->biHeight);
	printf("biSize: %lu\n", pHeader->biSize);
	printf("biSizeImage: %lu\n", pHeader->biSizeImage);
	printf("biXPelsPerMeter: %ld\n", pHeader->biXPelsPerMeter);
	printf("biYPelsPerMeter: %ld\n", pHeader->biYPelsPerMeter);
}

bool tProMotion::getPaletteFromCurrentImage(std::vector<tRgb> &PaletteOut)
{
	if(
		!ddeQueryOk("SendPalette") || !IsClipboardFormatAvailable(CF_DIB) ||
		!OpenClipboard(NULL)
	) {
		return false;
	}

	// Read palette data
	HANDLE hData = GetClipboardData(CF_DIB);
	if(hData) {
		LPBITMAPINFO pBitmap = (LPBITMAPINFO)GlobalLock(hData);
		if (!pBitmap) {
			return false;
		}
		PaletteOut.clear();
		uint32_t ulColorCount = pBitmap->bmiHeader.biClrUsed;
		uint32_t ulLastNonBlack = 0;

		for(uint32_t i = 0; i < ulColorCount; ++i) {
			tRgb Color(
				pBitmap->bmiColors[i].rgbRed,
				pBitmap->bmiColors[i].rgbGreen,
				pBitmap->bmiColors[i].rgbBlue
			);
			if(Color != tRgb(0, 0, 0)) {
				ulLastNonBlack = i;
			}
			PaletteOut.push_back(std::move(Color));
		}
		GlobalUnlock(hData);

		// PMNG always returns 256 colors so trim to actual size
		PaletteOut.resize(ulLastNonBlack, tRgb(0, 0, 0));
	}
	CloseClipboard();

	return true;
}

bool tProMotion::setPaletteInCurrentImage(const std::vector<tRgb> &Palette) {
	// https://stackoverflow.com/questions/50313607/
	auto PaletteOut = Palette;
	PaletteOut.resize(256, tRgb(0, 0, 0));

	BITMAPINFOHEADER BmHeader = {0};
	BmHeader.biSize = sizeof(BITMAPINFOHEADER);
	BmHeader.biWidth = 1;
	BmHeader.biHeight = 1;
	BmHeader.biPlanes = 1;
	BmHeader.biBitCount = 8;
	BmHeader.biCompression = BI_RGB;
	BmHeader.biSizeImage = 4;
	BmHeader.biClrUsed = PaletteOut.size();
	BmHeader.biClrImportant = PaletteOut.size();

	winpp::tBitmapInfo<256, 1> BmInfo;
	ZeroMemory(&BmInfo, sizeof(BmInfo));
	BmInfo.bmiHeader = BmHeader;
	for(auto i = 0; i < PaletteOut.size(); ++i) {
		BmInfo.bmiColors[i].rgbBlue = PaletteOut.at(i).m_ubR;
		BmInfo.bmiColors[i].rgbGreen = PaletteOut.at(i).m_ubG;
		BmInfo.bmiColors[i].rgbRed = PaletteOut.at(i).m_ubB;
		BmInfo.bmiColors[i].rgbReserved = 0;
	}

	HGLOBAL hClipboardDib = GlobalAlloc( GMEM_MOVEABLE | GMEM_SHARE, sizeof(BmInfo));
	void *pClipboardDib = GlobalLock(hClipboardDib);
	memcpy(pClipboardDib, BmInfo.toBmiPtr(), sizeof(BmInfo));
  GlobalUnlock(hClipboardDib);

	bool isOk = false;
	if(OpenClipboard(NULL)) {
		EmptyClipboard();
		SetClipboardData(CF_DIB, hClipboardDib);
		CloseClipboard();
		isOk = true;
	}

	if(!ddeQueryOk("ReceivePalette")) {
		return false;
	}
	return isOk;
}

bool tProMotion::getError(void)
{
	bool isError = m_isError;
	m_isError = false;
	return m_isError;
}
