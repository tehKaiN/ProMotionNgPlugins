#ifndef _WINPP_H_
#define _WINPP_H_

#include <windows.h>
#include <cstdint>

namespace winpp {

template <uint32_t t_ulColorCount, uint32_t t_ulPixelDataSize>
struct tBitmapInfo {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[t_ulColorCount];
	uint8_t pixels[t_ulPixelDataSize];

	BITMAPINFO *toBmiPtr(void) {
		return reinterpret_cast<BITMAPINFO*>(&bmiHeader);
	}
};

} // namespace winpp

#endif // _WINPP_H_
