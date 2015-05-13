#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Linhash.h"
#include <sprig.h>
extern int FONT_main;

class Painter{
public:
	Painter();
	~Painter();
	void init(int, int, const char*, bool fs = false);
	int loadImage(const char*);
	void put(int imgNum, int startX, int startY, int bx0, int by0, int bx1, int by1);
	void line(int x0, int y0, int x1, int y1, int col, const char* mode);
	void screenLine(int x0, int y0, int x1, int y1, int col, const char* mode);
	void circle(int x0, int y0, double r, int col, const char* mode);
	void pset(int x0, int y0, int col);
	void screenPoly(int n, SPG_Point* points, int col, int alpha, const char* mode);
	void text(int x0, int y0, const char*, int font, int col);
	void screenText(int x0, int y0, const char*, int font, int col);
	int  loadFont(const char*, int);
	void getMouse(int* mx, int* my);
	void pause();
	void sync();
	void print(const char*);
	void cls();
	int  get_width();
	int  get_height;
	bool ok();
	bool key_quit();
	bool key_up();
	bool key_right();
	bool key_down();
	bool key_left();
	bool key_space();
	double getCamX();
	double getCamY();
	double getCamZ();
	int getTimer();
	void setCamera(double x, double y, double z);
	void setViewport(double w, double h);
	void scaleToViewport(int &x, int &y);
	void scaleToViewport(double &x, double &y);
private:
	void getKeys();
	SDL_Surface* screen;
	bool error_free;
	Linhash<int, SDL_Surface*> images;
	Linhash<int, TTF_Font*> fonts;
	int width;
	int height;
	bool hasSyncd;
	Uint8* keystate;
	double camX;
	double camY;
	double camZ;
	double viewW;
	double viewH;
	int printingLine;
};


extern Painter DebugDraw;

