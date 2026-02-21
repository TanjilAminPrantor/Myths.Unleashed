#define _CRT_SECURE_NO_WARNINGS
#include "UI.h"
#include <stdio.h>

// NOTE: Unity build: iGraphics.h will be included in main.cpp before this file is included.

static bool inBox(int mx, int my, int x1, int y1, int x2, int y2){
	return (mx >= x1 && mx <= x2 && my >= y1 && my <= y2);
}

void UI_Init(UIContext& ui){
	ui.mxPos = 0;
	ui.myPos = 0;
}

void UI_GoToMenu(AppState& state){
	if (state == HOME) state = MENU1;
}

// ---------- SAFE BMP DRAW ----------
static void safeShowBMP2(int x, int y, const char* path){
	FILE* fp = fopen(path, "rb");
	if (fp){
		fclose(fp);
		iShowBMP2(x, y, (char*)path, 0);
		return;
	}

	// fallback: show error text instead of crashing
	iSetColor(255, 0, 0);
	iText(20, 20, (char*)"IMAGE NOT FOUND:", GLUT_BITMAP_HELVETICA_18);
	iText(20, 50, (char*)path, GLUT_BITMAP_HELVETICA_18);

	// also show hint
	iSetColor(255, 255, 0);
	iText(20, 80, (char*)"Put images in: Debug\\image\\", GLUT_BITMAP_HELVETICA_18);
}

// pages
static void drawHomePage(){ safeShowBMP2(0, 0, "image\\homepage.bmp"); }
static void drawMenuPage(){ safeShowBMP2(0, 0, "image\\menu.bmp"); }
static void drawPlayMenuPage(){ safeShowBMP2(0, 0, "image\\playmenu.bmp"); }
static void drawShopPage(){ safeShowBMP2(0, 0, "image\\shop.bmp"); }
static void drawAboutPage(){ safeShowBMP2(0, 0, "image\\aboutus.bmp"); }

void UI_Draw(const UIContext& ui, AppState state){
	if (state == HOME) drawHomePage();
	else if (state == MENU1) drawMenuPage();
	else if (state == PLAYMENU) drawPlayMenuPage();
	else if (state == SHOP) drawShopPage();
	else if (state == ABOUT) drawAboutPage();

	// debug mouse
	char str[64];
	sprintf(str, "mx=%d my=%d", ui.mxPos, ui.myPos);
	iSetColor(255, 255, 0);
	iText(10, 10, str, GLUT_BITMAP_HELVETICA_18);
}

void UI_PassiveMouseMove(UIContext& ui, int mx, int my){
	ui.mxPos = mx;
	ui.myPos = my;
}

void UI_Keyboard(UIContext& ui, AppState& state, unsigned char key){
	(void)ui;
	if (key == 27){ // ESC
		if (state == PLAYMENU || state == SHOP || state == ABOUT) state = MENU1;
		else if (state == MENU1) state = HOME;
	}
}

void UI_Mouse(UIContext& ui, AppState& state, int button, int mstate, int mx, int my){
	(void)ui;
	if (button != GLUT_LEFT_BUTTON || mstate != GLUT_DOWN) return;

	if (state == MENU1){
		int px1 = 363, py1 = 159, px2 = 637, py2 = 208; // Play
		int sx1 = 363, sy1 = 83, sx2 = 637, sy2 = 132; // Shop
		int ax1 = 363, ay1 = 13, ax2 = 637, ay2 = 59;  // About

		if (inBox(mx, my, px1, py1, px2, py2)) state = PLAYMENU;
		else if (inBox(mx, my, sx1, sy1, sx2, sy2)) state = SHOP;
		else if (inBox(mx, my, ax1, ay1, ax2, ay2)) state = ABOUT;
	}
	else if (state == PLAYMENU){
		// top option -> 1v1
		int o1x1 = 363, o1y1 = 145, o1x2 = 636, o1y2 = 213;

		// others back for now
		int stx1 = 363, sty1 = 93, stx2 = 637, sty2 = 145;
		int trx1 = 363, try1 = 31, trx2 = 637, try2 = 78;

		if (inBox(mx, my, o1x1, o1y1, o1x2, o1y2)) state = STATE_SELECT_1V1;
		else if (inBox(mx, my, stx1, sty1, stx2, sty2)) state = MENU1;
		else if (inBox(mx, my, trx1, try1, trx2, try2)) state = MENU1;
	}
	else if (state == SHOP || state == ABOUT){
		state = MENU1;
	}
}