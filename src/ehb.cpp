#include <fstream>
#include <cstring>
#include <windows.h>
#include "promotion.h"

int main(void) {
	try {
		tProMotion ProMotion;
		if(ProMotion.getError()) {
			throw std::runtime_error("ERR: Pro Motion server is inactive\n");
		}

		std::vector<tRgb> Palette;
		if(!ProMotion.getPaletteFromCurrentImage(Palette)) {
			throw std::runtime_error("Couldn't read palette");
		}

		// Resize to 32 colors
		if(Palette.size() != 32) {
			Palette.resize(32, tRgb(0, 0, 0));
		}

		for(auto i = 0; i < 32; ++i) {
			// Normalize old palette to 12BPP
			Palette.at(i) = Palette.at(i).normalize12bpp();
			// Generate EHB colors
			Palette.push_back((Palette.at(i) / 2).normalize12bpp());
		}

		if(!ProMotion.setPaletteInCurrentImage(Palette)) {
			throw std::runtime_error("ERR: Couldn't set palette");
		}

		return EXIT_SUCCESS;
	}
	catch(const std::exception &Exc) {
		MessageBox(
			nullptr, Exc.what(), "EHB Error",
			MB_APPLMODAL | MB_ICONEXCLAMATION | MB_OK
		);
	}
	return EXIT_FAILURE;
}
