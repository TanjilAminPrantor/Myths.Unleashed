#pragma once
#include "AppState.h"

struct UIContext {
	int mxPos;
	int myPos;
};

void UI_Init(UIContext& ui);
void UI_Draw(const UIContext& ui, AppState state);
void UI_PassiveMouseMove(UIContext& ui, int mx, int my);
void UI_Mouse(UIContext& ui, AppState& state, int button, int mstate, int mx, int my);
void UI_Keyboard(UIContext& ui, AppState& state, unsigned char key);
void UI_GoToMenu(AppState& state);