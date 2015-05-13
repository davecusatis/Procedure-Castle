#pragma once
#include "BoostGeometry.h"
#include "Debug.h"
#ifdef DEBUG_MAP_GENERATOR
#include "Constants.h"
#include "Painter.h"
#endif

class MapGenerator
{
public:
#ifdef DEBUG_MAP_GENERATOR
	static void setDebugPainter(Painter* debugDraw_);
#endif

	MapGenerator();
	~MapGenerator();
	Polygon2 generate();
	void flush();
private:
#ifdef DEBUG_MAP_GENERATOR
	static Painter* debugDraw;
#endif
	void clean();
	void construct();
	void init();

	static void buildHull(Polygon2 p);
	static void buildHullBaseShape(Polygon2 p);

	
};