#include "Painter.h"
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <sprig.h>
#include <SDL_ttf.h>
#include "Linhash.h"
#include <windows.h>
#include <time.h>

const int    PT_Painter_FONT_PIXEL_HEIGHT = 16;
const double PT_Painter_DEFAULT_Z = 256;

Painter DebugDraw;
int FONT_main;

Painter::Painter(){
	error_free = true;
	srand((unsigned int)time(NULL));
	hasSyncd = true;
}

Painter::~Painter(){
	SDL_Surface** img_ret;
	SDL_Surface*  img;
	TTF_Font**    font_ret;
	TTF_Font*     font;

	while (img_ret = images.roll()){
		img = *img_ret;
		SDL_FreeSurface(img);
	}

	while (font_ret = fonts.roll()){
		font = *font_ret;
		TTF_CloseFont(font);
	}

	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void Painter::print(const char * text){
	screenText(0, printingLine * PT_Painter_FONT_PIXEL_HEIGHT, text, FONT_main, 0xffffff);
	printingLine++;
}

void Painter::pause(){
	SDL_Event event;
	do
	{
		SDL_WaitEvent(&event);
	} while(event.type != SDL_KEYDOWN);
}

bool Painter::ok(){
	return error_free;
}

void Painter::getMouse(int* mx, int* my){
	SDL_GetMouseState(mx, my);
}


bool Painter::key_quit(){
	SDL_Event key;
	SDL_PollEvent(&key);
	if (key.type == SDL_QUIT) return true;
	return false;
}

void Painter::getKeys(){
	if (hasSyncd) keystate = SDL_GetKeyState(NULL);
	hasSyncd = false;
}
bool Painter::key_up(){
	getKeys();
	if (keystate[SDLK_UP]) return true;
	return false;
}
bool Painter::key_right(){
	getKeys();
	if (keystate[SDLK_RIGHT]) return true;
	return false;
}
bool Painter::key_down(){
	getKeys();
	if (keystate[SDLK_DOWN]) return true;
	return false;
}
bool Painter::key_left(){
	getKeys();
	if (keystate[SDLK_LEFT]) return true;
	return false;
}

bool Painter::key_space(){
	getKeys();
	if (keystate[SDLK_SPACE]) return true;
	return false;
}



void Painter::sync(){
	SDL_Flip(screen);
	hasSyncd = true;
}

double Painter::getCamX(){
	return camX;
}
double Painter::getCamY(){
	return camY;
}
double Painter::getCamZ(){
	return camZ;
}

int Painter::getTimer(){
	return SDL_GetTicks();
}

void Painter::scaleToViewport(int &x, int &y){
	x = ((((double)x) - camX) * width) / (camZ * viewW) + width * 0.5;
	y = ((((double)y) - camY) * height) / (camZ * viewH) + height * 0.5;
}

void Painter::scaleToViewport(double &x, double &y){
	x = ((x - camX) * width) / (camZ * viewW) + width * 0.5;
	y = ((y - camY) * height) / (camZ * viewH) + height * 0.5;
}

void Painter::setCamera(double x, double y, double z){
	camX = x;
	camY = y;
	camZ = z;
}

void Painter::setViewport(double w, double h){
	viewW = w;
	viewH = h;
}

void Painter::init(int scrx, int scry, const char* title, bool fs){
	error_free = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		error_free = false;
		return;
	}

	SPG_PushAA(true);

	if (!(screen = SDL_SetVideoMode(scrx, scry, 0, SDL_HWSURFACE | SDL_DOUBLEBUF | (fs ? SDL_FULLSCREEN : 0)))){
		SDL_Quit();
		error_free = false;
		return;
	}

	if ((IMG_INIT_PNG & IMG_Init(IMG_INIT_PNG)) != IMG_INIT_PNG){
		error_free = false;
		SDL_Quit();
	}

	if (TTF_Init()){
		error_free = false;
		SDL_Quit();
	}

	width = scrx;
	height = scry;
	camX = scrx * 0.5;
	camY = scry * 0.5;
	camZ = PT_Painter_DEFAULT_Z;
	viewW = scrx / camZ;
	viewH = scry / camZ;
	SDL_WM_SetCaption(title, 0);
	return;
}

void Painter::pset(int x0, int y0, int col)
{
	scaleToViewport(x0, y0);
	SPG_Pixel(screen, x0, y0, col);
}


void Painter::screenPoly(int n, SPG_Point* points, int col, int alpha, const char* mode){
	if (strcmp(mode, "F") == 0){
		SPG_PolygonFilledBlend(screen, n, points, col, alpha);
	}
	else
	{
		SPG_PolygonBlend(screen, n, points, col, alpha);
	}
}

void Painter::screenLine(int x0, int y0, int x1, int y1, int col, const char* mode){
	if (strcmp(mode, "BF") == 0){
		SPG_RectFilled(screen, x0, y0, x1, y1, col);
	}
	else if (strcmp(mode, "B") == 0){
		SPG_Rect(screen, x0, y0, x1, y1, col);
	}
	else{
		SPG_Line(screen, x0, y0, x1, y1, col);
	}
}

void Painter::line(int x0, int y0, int x1, int y1, int col, const char* mode){
	scaleToViewport(x0, y0);
	scaleToViewport(x1, y1);
	if (strcmp(mode, "BF") == 0){
		SPG_RectFilled(screen, x0, y0, x1, y1, col);
	}
	else if (strcmp(mode, "B") == 0){
		SPG_Rect(screen, x0, y0, x1, y1, col);
	}
	else{
		SPG_Line(screen, x0, y0, x1, y1, col);
	}
}

void Painter::circle(int x0, int y0, double r, int col, const char* mode){
	double rx;
	double ry;
	scaleToViewport(x0, y0);
	rx = r + camX;
	ry = r + camY;
	scaleToViewport(rx, ry);
	rx -= width * 0.5;
	ry -= height * 0.5;
	if (strcmp(mode, "F") == 0){
		SPG_EllipseFilled(screen, x0, y0, rx, ry, col);
	}
	else{
		SPG_Ellipse(screen, x0, y0, rx, ry, col);
	}
}

void Painter::screenText(int x0, int y0, const char* text, int font, int col){
	TTF_Font*    fontUsed;
	SDL_Surface* bitmap;
	SDL_Color    color;
	SDL_Rect     dest;

	if (!(fontUsed = fonts[font])){
		error_free = false;
		MessageBox(NULL, "Invalid font key", "Error", MB_OK);
		return;
	}


	color.r = (col & 0xFF0000) >> 16;
	color.g = (col & 0x00FF00) >> 8;
	color.b = (col & 0x0000FF);

	dest.x = x0;
	dest.y = y0;
	dest.w = 0;
	dest.h = 0;

	bitmap = TTF_RenderText_Blended(fontUsed, text, color);
	SDL_BlitSurface(bitmap, NULL, screen, &dest);
	SDL_FreeSurface(bitmap);
}
void Painter::text(int x0, int y0, const char* text, int font, int col){
	TTF_Font*    fontUsed;
	SDL_Surface* bitmap;
	SDL_Color    color;
	SDL_Rect     dest;

	if (!(fontUsed = fonts[font])){
		error_free = false;
		MessageBox(NULL, "Invalid font key", "Error", MB_OK);
		return;
	}


	color.r = (col & 0xFF0000) >> 16;
	color.g = (col & 0x00FF00) >> 8;
	color.b = (col & 0x0000FF);

	scaleToViewport(x0, y0);

	dest.x = x0;
	dest.y = y0;
	dest.w = 0;
	dest.h = 0;

	bitmap = TTF_RenderText_Blended(fontUsed, text, color);
	SDL_BlitSurface(bitmap, NULL, screen, &dest);
	SDL_FreeSurface(bitmap);
}

int Painter::loadFont(const char* fontFile, int size){
	TTF_Font* font;
	int       key;
	char*     errmsg;

	error_free = true;

	if (!(font = TTF_OpenFont(fontFile, size))){
		error_free = false;
		errmsg = TTF_GetError();
		if (!errmsg) errmsg = "Could not open font file";
		MessageBox(NULL, errmsg, "Error", MB_OK);
		return 0;
	}

	while (!(key = rand()));

	fonts.insert(key, font);

	return key;
}

void Painter::put(int imgNum, int startX, int startY, int bx0, int by0, int bx1, int by1){
	SDL_Surface* bitmap;
	SDL_Rect     source;
	SDL_Rect     dest;

	error_free = true;

	if (!(bitmap = images[imgNum])){
		error_free = false;
		MessageBox(NULL, "Invalid img key", "Error", MB_OK);
		return;
	}

	source.x = bx0;
	source.y = by0;
	source.w = bx1 - bx0 + 1;
	source.h = by1 - by0 + 1;

	scaleToViewport(startX, startY);

	dest.x = startX;
	dest.y = startY;
	dest.w = source.w;
	dest.h = source.h;

	SDL_BlitSurface(bitmap, &source, screen, &dest);
}



void Painter::cls(){
	SDL_FillRect(screen, NULL, 0);
	printingLine = 0;
}

int Painter::loadImage(const char* filename){
	SDL_Surface* bitmap;
	int          key;
	char*        errmsg;

	error_free = true;

	bitmap = IMG_Load(filename);
	if (bitmap == NULL){
		error_free = false;
		errmsg = IMG_GetError();
		if (!errmsg) errmsg = "Unable to determine SDL_Image error";
		MessageBox(NULL, errmsg, "Error", MB_OK);
		return 0;
	}

	while (!(key = rand()));

	images.insert(key, bitmap);

	return key;
}