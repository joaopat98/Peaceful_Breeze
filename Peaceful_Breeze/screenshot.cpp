#include "screenshot.h"

const char* filename;
string location = "Screenshots/";
int numberScreenshots = 0;
bool createdFolder = _mkdir("Screenshots");

inline bool fileExists(const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

void getFilename() {
	string numberScreenshotsString = to_string(numberScreenshots);
	string filenameString = location + "Screenshot" + numberScreenshotsString + ".bmp";
	filename = filenameString.c_str();
}

void ScreenShot::takeScreenShot() {
	GLint screen[4];
	glGetIntegerv(GL_VIEWPORT, screen);
	int width = screen[2];
	int height = screen[3];

	// Make the BYTE array, factor of 4 because it's RBGA.
	BYTE* pixels = new BYTE[(size_t)4 * width * height];

	glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

	// Filename
	getFilename();
	while (fileExists(filename)) {
		numberScreenshots++;
		getFilename();
	}

	// Convert to FreeImage format & save to file
	FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, width, height, 4 * width, 32, 0x0000FF, 0xFF0000, 0x00FF00, false);
	FreeImage_Save(FIF_BMP, image, filename, 0);
	cout << filename << endl;

	// Free resources
	FreeImage_Unload(image);
	delete[] pixels;
}