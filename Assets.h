#pragma once
#include <windows.h>

struct AssetPack {
	bool loaded;

	unsigned int texP1;
	unsigned int texP2;

	static const int BG_COUNT = 4;
	const char* bgFiles[BG_COUNT];
	unsigned int texBG[BG_COUNT];
	int bgIndex;

	char exeDir[MAX_PATH];
	char p1PathUsed[MAX_PATH];
	char p2PathUsed[MAX_PATH];
	char bgPathUsed[BG_COUNT][MAX_PATH];
};

void Assets_Init(AssetPack& A);
void Assets_LoadOnce(AssetPack& A);

// OpenGL texture draw (flip/mirror)
void DrawTexture(int x, int y, int w, int h, unsigned int tex, bool flipY, bool mirrorX);