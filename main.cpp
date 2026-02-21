#define _CRT_SECURE_NO_WARNINGS
#include "iGraphics.h"

#include "AppState.h"
#include "UI.h"
#include "Assets.h"
#include "Fight1v1.h"

static AppState gState = HOME;
static UIContext gUI;
static AssetPack gAssets;
static Fight1v1 gFight;

void fixedUpdate(){
	Assets_LoadOnce(gAssets);

	AppState next = gState;
	Fight1v1_Update(gFight, gAssets, gState, next);
	gState = next;
}

static void goToMenuTimer(){
	UI_GoToMenu(gState);
}

void iDraw(){
	iClear();

	if (gState == HOME || gState == MENU1 || gState == PLAYMENU || gState == SHOP || gState == ABOUT){
		UI_Draw(gUI, gState);
		return;
	}

	if (gState == STATE_SELECT_1V1 || gState == STATE_FIGHT_1V1){
		Fight1v1_Draw(gFight, gAssets, gState);
		return;
	}
}

void iPassiveMouseMove(int mx, int my){
	UI_PassiveMouseMove(gUI, mx, my);
}

void iMouseMove(int mx, int my){
	(void)mx; (void)my;
}

void iMouse(int button, int state, int mx, int my){
	if (gState == HOME || gState == MENU1 || gState == PLAYMENU || gState == SHOP || gState == ABOUT){
		AppState before = gState;
		UI_Mouse(gUI, gState, button, state, mx, my);

		if (before != STATE_SELECT_1V1 && gState == STATE_SELECT_1V1){
			Fight1v1_OnEnterSelect(gFight);
		}
		return;
	}
}

void iKeyboard(unsigned char key){
	if (key == 27){
		if (gState == STATE_SELECT_1V1 || gState == STATE_FIGHT_1V1){
			gState = MENU1;
			return;
		}
	}

	if (gState == HOME || gState == MENU1 || gState == PLAYMENU || gState == SHOP || gState == ABOUT){
		UI_Keyboard(gUI, gState, key);
	}
}

void iSpecialKeyboard(unsigned char key){
	(void)key;
}

int main(){
	UI_Init(gUI);
	Assets_Init(gAssets);
	Fight1v1_Init(gFight);

	iSetTimer(20, fixedUpdate);
	iSetTimer(3000, goToMenuTimer);

	iInitialize(1000, 600, (char*)"MythsUNleashed");
	iStart();
	return 0;
}

// ===== UNITY BUILD (these 3 are excluded from build, but compiled here) =====
#include "UI.cpp"
#include "Assets.cpp"
#include "Fight1v1.cpp"