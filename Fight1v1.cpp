#define _CRT_SECURE_NO_WARNINGS
#include "Fight1v1.h"
#include <stdio.h>
#include <string.h>

// NOTE: Unity build: iGraphics.h will be included in main.cpp before this file is included.

static float clampf(float v, float lo, float hi){ return (v < lo) ? lo : ((v > hi) ? hi : v); }
static bool rectOverlap(float ax, float ay, int aw, int ah, float bx, float by, int bw, int bh){
	return (ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by);
}
static bool pressedEdge(int now, int& prev){
	bool edge = (now && !prev);
	prev = now;
	return edge;
}

static void initCharacters(Fight1v1& G){
	sprintf(G.chars[0].name, "Intes");  G.chars[0].maxHP = 120; G.chars[0].damage = 10;
	sprintf(G.chars[1].name, "Ran");    G.chars[1].maxHP = 110; G.chars[1].damage = 12;
	sprintf(G.chars[2].name, "Mai");    G.chars[2].maxHP = 95;  G.chars[2].damage = 9;
	sprintf(G.chars[3].name, "Ryo");    G.chars[3].maxHP = 140; G.chars[3].damage = 8;
}

static void resetProjectiles(Fight1v1& G){
	for (int i = 0; i < 6; i++) G.proj[i].active = false;
}

static void setupFighter(Fight1v1::Fighter& F, const Fight1v1::Character& C, int side, float groundY){
	F.w = 60; F.h = 120;
	F.y = groundY; F.vx = 0; F.vy = 0;

	if (side == 1){ F.x = 220; F.facingRight = true; }
	else          { F.x = 720; F.facingRight = false; }

	F.maxHP = C.maxHP;
	F.hp = F.maxHP;
	F.damage = C.damage;
	F.onGround = true;
	F.hurtCD = 0;
	F.projCD = 0;
}

static void startMatch(Fight1v1& G, AppState& nextState){
	setupFighter(G.P1, G.chars[G.p1Char], 1, G.GROUND_Y);
	setupFighter(G.P2, G.chars[G.p2Char], 2, G.GROUND_Y);
	resetProjectiles(G);
	G.winner = 0;
	nextState = STATE_FIGHT_1V1;
}

static void applyHit(Fight1v1::Fighter& v, int dmg){
	if (v.hurtCD > 0) return;
	v.hp -= dmg;
	if (v.hp < 0) v.hp = 0;
	v.hurtCD = 15;
}

static void spawnProjectile(Fight1v1& G, int owner){
	Fight1v1::Fighter& A = (owner == 1) ? G.P1 : G.P2;
	if (A.projCD > 0) return;
	A.projCD = 45;

	for (int i = 0; i < 6; i++){
		if (!G.proj[i].active){
			G.proj[i].active = true;
			G.proj[i].owner = owner;
			G.proj[i].w = 18; G.proj[i].h = 10;
			G.proj[i].dmg = 14;
			G.proj[i].y = A.y + A.h * 0.55f;

			if (A.facingRight){
				G.proj[i].x = A.x + A.w + 5;
				G.proj[i].vx = 10;
			}
			else {
				G.proj[i].x = A.x - G.proj[i].w - 5;
				G.proj[i].vx = -10;
			}
			return;
		}
	}
}

static void updateProjectiles(Fight1v1& G){
	for (int i = 0; i < 6; i++){
		if (!G.proj[i].active) continue;

		G.proj[i].x += G.proj[i].vx;

		if (G.proj[i].x < -50 || G.proj[i].x > G.W + 50){
			G.proj[i].active = false;
			continue;
		}

		Fight1v1::Fighter& B = (G.proj[i].owner == 1) ? G.P2 : G.P1;
		if (rectOverlap(G.proj[i].x, G.proj[i].y, G.proj[i].w, G.proj[i].h, B.x, B.y, B.w, B.h)){
			applyHit(B, G.proj[i].dmg);
			G.proj[i].active = false;
		}
	}
}

static void updatePhysics(Fight1v1& G, Fight1v1::Fighter& F){
	if (!F.onGround) F.vy -= G.GRAV;
	F.y += F.vy;

	if (F.y <= G.GROUND_Y){
		F.y = G.GROUND_Y;
		F.vy = 0;
		F.onGround = true;
	}

	F.x += F.vx;
	F.vx *= 0.80f;

	F.x = clampf(F.x, 0, (float)(G.W - F.w));

	if (F.hurtCD > 0) F.hurtCD--;
	if (F.projCD > 0) F.projCD--;
}

void Fight1v1_Init(Fight1v1& G){
	G.W = 1000; G.H = 600;
	G.GRAV = 0.7f;
	G.GROUND_Y = 80.0f;

	initCharacters(G);
	Fight1v1_OnEnterSelect(G);
}

void Fight1v1_OnEnterSelect(Fight1v1& G){
	G.p1Char = 0; G.p2Char = 1;
	G.p1Locked = false; G.p2Locked = false;

	G.winner = 0;
	resetProjectiles(G);

	G.prevA = G.prevD = G.prevF = 0;
	G.prevL = G.prevR = G.prev0 = 0;
	G.prevLB = G.prevRB = 0;
}

static void updateSelect(Fight1v1& G, AssetPack& A, AppState& nextState){
	int nowA = (isKeyPressed('a') || isKeyPressed('A')) ? 1 : 0;
	int nowD = (isKeyPressed('d') || isKeyPressed('D')) ? 1 : 0;
	int nowF = (isKeyPressed('f') || isKeyPressed('F')) ? 1 : 0;

	if (!G.p1Locked){
		if (pressedEdge(nowA, G.prevA)) G.p1Char = (G.p1Char + 4 - 1) % 4;
		if (pressedEdge(nowD, G.prevD)) G.p1Char = (G.p1Char + 1) % 4;
		if (pressedEdge(nowF, G.prevF)) G.p1Locked = true;
	}
	else {
		if (pressedEdge(nowF, G.prevF)) G.p1Locked = false;
	}

	int nowL = isSpecialKeyPressed(GLUT_KEY_LEFT) ? 1 : 0;
	int nowR = isSpecialKeyPressed(GLUT_KEY_RIGHT) ? 1 : 0;
	int now0 = isKeyPressed('0') ? 1 : 0;

	if (!G.p2Locked){
		if (pressedEdge(nowL, G.prevL)) G.p2Char = (G.p2Char + 4 - 1) % 4;
		if (pressedEdge(nowR, G.prevR)) G.p2Char = (G.p2Char + 1) % 4;
		if (pressedEdge(now0, G.prev0)) G.p2Locked = true;
	}
	else {
		if (pressedEdge(now0, G.prev0)) G.p2Locked = false;
	}

	// BG change [ ]
	int nowLB = isKeyPressed('[') ? 1 : 0;
	int nowRB = isKeyPressed(']') ? 1 : 0;

	if (pressedEdge(nowLB, G.prevLB)) A.bgIndex = (A.bgIndex + AssetPack::BG_COUNT - 1) % AssetPack::BG_COUNT;
	if (pressedEdge(nowRB, G.prevRB)) A.bgIndex = (A.bgIndex + 1) % AssetPack::BG_COUNT;

	if (G.p1Locked && G.p2Locked) startMatch(G, nextState);
}

static void updateFight(Fight1v1& G){
	if (G.winner) return;

	float speed = 6.0f;

	// P1
	if (isKeyPressed('a') || isKeyPressed('A')) { G.P1.vx = -speed; G.P1.facingRight = false; }
	if (isKeyPressed('d') || isKeyPressed('D')) { G.P1.vx = speed;  G.P1.facingRight = true; }
	if ((isKeyPressed('w') || isKeyPressed('W')) && G.P1.onGround){ G.P1.vy = 13.5f; G.P1.onGround = false; }
	if (isKeyPressed('g') || isKeyPressed('G')) spawnProjectile(G, 1);

	// P2
	if (isSpecialKeyPressed(GLUT_KEY_LEFT))  { G.P2.vx = -speed; G.P2.facingRight = false; }
	if (isSpecialKeyPressed(GLUT_KEY_RIGHT)) { G.P2.vx = speed;  G.P2.facingRight = true; }
	if (isSpecialKeyPressed(GLUT_KEY_UP) && G.P2.onGround){ G.P2.vy = 13.5f; G.P2.onGround = false; }
	if (isKeyPressed('5')) spawnProjectile(G, 2);

	updatePhysics(G, G.P1);
	updatePhysics(G, G.P2);
	updateProjectiles(G);

	if (G.P1.hp <= 0) G.winner = 2;
	if (G.P2.hp <= 0) G.winner = 1;
}

void Fight1v1_Update(Fight1v1& G, AssetPack& A, AppState state, AppState& nextState){
	nextState = state;
	if (state == STATE_SELECT_1V1) updateSelect(G, A, nextState);
	else if (state == STATE_FIGHT_1V1) updateFight(G);
}

static void drawHealthBar(int x, int y, int w, int h, int hp, int maxHP){
	iRectangle(x, y, w, h);
	float r = (maxHP > 0) ? (float)hp / (float)maxHP : 0.0f;
	r = clampf(r, 0.0f, 1.0f);

	iSetColor(255 * (1.0f - r), 255 * r, 0);
	iFilledRectangle(x, y, (int)(w * r), h);

	iSetColor(255, 255, 255);
}

static void drawSelectScreen(const Fight1v1& G, const AssetPack& A){
	if (A.texBG[A.bgIndex]) DrawTexture(0, 0, G.W, G.H, A.texBG[A.bgIndex], true, false);

	iSetColor(255, 255, 255);
	iText(G.W / 2 - 120, G.H - 60, (char*)"CHARACTER SELECT", GLUT_BITMAP_HELVETICA_18);

	iRectangle(120, 250, 300, 200);
	iText(130, 420, (char*)"P1: A/D choose, F lock", GLUT_BITMAP_HELVETICA_12);
	iText(130, 380, (char*)"Selected:", GLUT_BITMAP_HELVETICA_12);
	iText(210, 380, (char*)G.chars[G.p1Char].name, GLUT_BITMAP_HELVETICA_18);

	iRectangle(580, 250, 300, 200);
	iText(590, 420, (char*)"P2: <-/-> choose, 0 lock", GLUT_BITMAP_HELVETICA_12);
	iText(590, 380, (char*)"Selected:", GLUT_BITMAP_HELVETICA_12);
	iText(670, 380, (char*)G.chars[G.p2Char].name, GLUT_BITMAP_HELVETICA_18);

	if (A.texP1) DrawTexture(320, 270, 90, 130, A.texP1, true, false);
	if (A.texP2) DrawTexture(780, 270, 90, 130, A.texP2, true, true);

	char bgtxt[180];
	sprintf(bgtxt, "BG: %s   ([ / ] to change)", A.bgFiles[A.bgIndex]);
	iText(20, 20, bgtxt, GLUT_BITMAP_HELVETICA_12);
}

static void drawFightScreen(const Fight1v1& G, const AssetPack& A){
	if (A.texBG[A.bgIndex]) DrawTexture(0, 0, G.W, G.H, A.texBG[A.bgIndex], true, false);

	iSetColor(255, 255, 255);
	iText(20, G.H - 40, (char*)"P1 HP", GLUT_BITMAP_HELVETICA_12);
	drawHealthBar(80, G.H - 45, 300, 18, G.P1.hp, G.P1.maxHP);

	iText(G.W - 380, G.H - 40, (char*)"P2 HP", GLUT_BITMAP_HELVETICA_12);
	drawHealthBar(G.W - 300, G.H - 45, 300, 18, G.P2.hp, G.P2.maxHP);

	int drawW = 150, drawH = 130;

	if (A.texP1) DrawTexture((int)G.P1.x, (int)G.P1.y, drawW, drawH, A.texP1, true, false);
	else iRectangle((int)G.P1.x, (int)G.P1.y, G.P1.w, G.P1.h);

	if (A.texP2) DrawTexture((int)G.P2.x, (int)G.P2.y, drawW, drawH, A.texP2, true, true);
	else iRectangle((int)G.P2.x, (int)G.P2.y, G.P2.w, G.P2.h);

	for (int i = 0; i < 6; i++){
		if (G.proj[i].active) iRectangle((int)G.proj[i].x, (int)G.proj[i].y, G.proj[i].w, G.proj[i].h);
	}

	if (G.winner == 1) iText(G.W / 2 - 60, G.H / 2, (char*)"P1 WINS!", GLUT_BITMAP_HELVETICA_18);
	if (G.winner == 2) iText(G.W / 2 - 60, G.H / 2, (char*)"P2 WINS!", GLUT_BITMAP_HELVETICA_18);
}

void Fight1v1_Draw(const Fight1v1& G, const AssetPack& A, AppState state){
	if (state == STATE_SELECT_1V1) drawSelectScreen(G, A);
	else if (state == STATE_FIGHT_1V1) drawFightScreen(G, A);
}