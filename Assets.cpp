#define _CRT_SECURE_NO_WARNINGS
#include "Assets.h"
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>

// NOTE: Unity build: iGraphics.h will be included in main.cpp before this file is included.

static int myFileExists(const char* path){
	FILE* fp = fopen(path, "rb");
	if (fp){ fclose(fp); return 1; }
	return 0;
}

static void getExeDir(char out[MAX_PATH]){
	char full[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, full, MAX_PATH);
	char* last = strrchr(full, '\\');
	if (last) *last = '\0';
	strcpy(out, full);
}

static void joinPath(char* out, const char* dir, const char* file){
	strcpy(out, dir);
	strcat(out, "\\");
	strcat(out, file);
}

static unsigned int loadFirstMatch(char* outUsed, const char* const* list, int n){
	for (int i = 0; i < n; i++){
		if (myFileExists(list[i])){
			strcpy(outUsed, list[i]);
			return iLoadImage((char*)list[i]);
		}
	}
	return 0;
}

void Assets_Init(AssetPack& A){
	A.loaded = false;
	A.texP1 = 0;
	A.texP2 = 0;

	A.bgFiles[0] = "darkcastlebg.jpg";
	A.bgFiles[1] = "bg2.jpg";
	A.bgFiles[2] = "bg3.jpg";
	A.bgFiles[3] = "bg4.jpg";

	for (int i = 0; i < AssetPack::BG_COUNT; i++){
		A.texBG[i] = 0;
		A.bgPathUsed[i][0] = '\0';
	}
	A.bgIndex = 0;

	strcpy(A.p1PathUsed, "none");
	strcpy(A.p2PathUsed, "none");
	A.exeDir[0] = '\0';
}

void Assets_LoadOnce(AssetPack& A){
	if (A.loaded) return;

	getExeDir(A.exeDir);

	char p1a[MAX_PATH], p1b[MAX_PATH], p2a[MAX_PATH], p2b[MAX_PATH];
	joinPath(p1a, A.exeDir, "Intes.jpg");
	joinPath(p1b, A.exeDir, "Intes.png");
	joinPath(p2a, A.exeDir, "Mai.jpg");
	joinPath(p2b, A.exeDir, "Mai.png");

	const char* p1List[] = {
		"Intes.jpg", "Intes.png",
		p1a, p1b,
		".\\Intes.jpg", ".\\Intes.png",
		"Debug\\Intes.jpg", "Debug\\Intes.png",
		"Debug\\Debug\\Intes.jpg", "Debug\\Debug\\Intes.png"
	};

	const char* p2List[] = {
		"Mai.jpg", "Mai.png",
		p2a, p2b,
		".\\Mai.jpg", ".\\Mai.png",
		"Debug\\Mai.jpg", "Debug\\Mai.png",
		"Debug\\Debug\\Mai.jpg", "Debug\\Debug\\Mai.png"
	};

	A.texP1 = loadFirstMatch(A.p1PathUsed, p1List, (int)(sizeof(p1List) / sizeof(p1List[0])));
	A.texP2 = loadFirstMatch(A.p2PathUsed, p2List, (int)(sizeof(p2List) / sizeof(p2List[0])));

	for (int k = 0; k < AssetPack::BG_COUNT; k++){
		char a[MAX_PATH];
		joinPath(a, A.exeDir, A.bgFiles[k]);

		const char* tryList[] = {
			A.bgFiles[k],
			a,
			".\\darkcastlebg.jpg",
			"Debug\\darkcastlebg.jpg",
			"Debug\\Debug\\darkcastlebg.jpg"
		};

		A.texBG[k] = loadFirstMatch(A.bgPathUsed[k], tryList, (int)(sizeof(tryList) / sizeof(tryList[0])));
	}

	A.loaded = true;
}

void DrawTexture(int x, int y, int w, int h, unsigned int tex, bool flipY, bool mirrorX){
	if (!tex) return;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	float u0 = mirrorX ? 1.0f : 0.0f;
	float u1 = mirrorX ? 0.0f : 1.0f;
	float v0 = flipY ? 1.0f : 0.0f;
	float v1 = flipY ? 0.0f : 1.0f;

	glBegin(GL_QUADS);
	glTexCoord2f(u0, v0); glVertex2f((float)x, (float)y);
	glTexCoord2f(u1, v0); glVertex2f((float)(x + w), (float)y);
	glTexCoord2f(u1, v1); glVertex2f((float)(x + w), (float)(y + h));
	glTexCoord2f(u0, v1); glVertex2f((float)x, (float)(y + h));
	glEnd();

	glDisable(GL_TEXTURE_2D);
}