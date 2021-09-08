#ifndef _PROMOTION_H_
#define _PROMOTION_H_

#include <vector>
#include <cstdint>

struct tRgb {
	uint8_t m_ubR;
	uint8_t m_ubG;
	uint8_t m_ubB;

	tRgb(uint8_t ubR, uint8_t ubG, uint8_t ubB):
		m_ubR(ubR), m_ubG(ubG), m_ubB(ubB)
	{ }

	tRgb normalize12bpp(void) {
		return tRgb(
			((m_ubR + 8) / 17) * 17,
			((m_ubG + 8) / 17) * 17,
			((m_ubB + 8) / 17) * 17
		);
	}

	template<typename T>
	tRgb operator / (T Ratio) {
		return tRgb(
			m_ubR / Ratio,
			m_ubG / Ratio,
			m_ubB / Ratio
		);
	}

};

inline bool operator == (const tRgb &Lhs, const tRgb &Rhs) {
	return (
		Lhs.m_ubR == Rhs.m_ubR &&
		Lhs.m_ubG == Rhs.m_ubG &&
		Lhs.m_ubB == Rhs.m_ubB
	);
}

inline bool operator != (const tRgb &Lhs, const tRgb &Rhs) {
	return !(Lhs == Rhs);
}

class tProMotion {
public:
	tProMotion(void);
	~tProMotion(void);

	bool getPaletteFromCurrentImage(std::vector<tRgb> &PaletteOut);

	bool setPaletteInCurrentImage(const std::vector<tRgb> &Palette);

	bool getError(void);

private:
	bool m_isError;
};

#endif // _PROMOTION_H_
