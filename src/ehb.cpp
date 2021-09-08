#include <fstream>
#include "promotion.h"

int main(void) {
	std::ofstream Log("ehb.log");
	try {
		tProMotion ProMotion;
		if(ProMotion.getError()) {
			Log << "ERR: Pro Motion server is inactive\n";
			return EXIT_FAILURE;
		}

		std::vector<tRgb> Palette = {
			tRgb(0x11, 0x22, 0x33),
			tRgb(0x44, 0x55, 0x66),
			tRgb(7, 8, 9)
		};
		if(!ProMotion.getPaletteFromCurrentImage(Palette)) {
			Log << "ERR: Couldn't read palette\n";
			return EXIT_FAILURE;
		}

		Log << "Before\n\n";
		for(const auto &Rgb: Palette) {
			Log << "Color: " <<
				std::to_string(Rgb.m_ubR) << " " <<
				std::to_string(Rgb.m_ubG) << " " <<
				std::to_string(Rgb.m_ubB) << "\n";
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

		Log << "After\n\n";
		for(const auto &Rgb: Palette) {
			Log << "Color: " <<
				std::to_string(Rgb.m_ubR) << " " <<
				std::to_string(Rgb.m_ubG) << " " <<
				std::to_string(Rgb.m_ubB) << "\n";
		}

		if(!ProMotion.setPaletteInCurrentImage(Palette)) {
			Log << "ERR: Couldn't set palette";
		}

		return EXIT_SUCCESS;
	}
	catch(const std::exception &Exc) {
		Log << "ERR: Unhandled exception: " << Exc.what();
	}
}
