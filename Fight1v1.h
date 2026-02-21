#pragma once
#include "AppState.h"
#include "Assets.h"

struct Fight1v1 {
	int W, H;
	float GRAV, GROUND_Y;

	struct Character { char name[32]; int maxHP; int damage; };
	Character chars[4];

	int p1Char, p2Char;
	bool p1Locked, p2Locked;

	struct Fighter {
		float x, y, vx, vy;
		int w, h;
		int hp, maxHP, damage;
		bool onGround;
		bool facingRight;
		int hurtCD, projCD;
	};

	struct Projectile {
		float x, y, vx;
		int w, h, dmg;
		bool active;
		int owner;
	};

	Fighter P1, P2;
	Projectile proj[6];
	int winner;

	int prevA, prevD, prevF, prevL, prevR, prev0;
	int prevLB, prevRB;
};

void Fight1v1_Init(Fight1v1& G);
void Fight1v1_OnEnterSelect(Fight1v1& G);
void Fight1v1_Update(Fight1v1& G, AssetPack& A, AppState state, AppState& nextState);
void Fight1v1_Draw(const Fight1v1& G, const AssetPack& A, AppState state);