#include "MapGenerator.h"
#include "NumericUtility.h"
#include <algorithm>
#include <tuple>
#include <math.h>


#define SCALE_STEP 0.01
#define DIVET_BUFFER 0.2
#define DIVET_PENETRATION_DEPTH 0.2
#define DIVET_MIN_DOUBLE_WIDTH 0.4

using namespace boost::geometry;
#ifdef DEBUG_MAP_GENERATOR
Painter* MapGenerator::debugDraw = 0;
void MapGenerator::setDebugPainter(Painter* debugDraw_)
{
	debugDraw = debugDraw_;
}
#endif

MapGenerator::MapGenerator()
{
#ifdef DEBUG_MAP_GENERATOR
	if (!debugDraw)
	{
		setDebugPainter(&DebugDraw);
	}
#endif 


}

MapGenerator::~MapGenerator()
{

}

void MapGenerator::flush()
{

}


void MapGenerator::clean()
{

}
void MapGenerator::construct()
{

}
void MapGenerator::init()
{

}

Polygon2 MapGenerator::generate()
{
	Polygon2 p;

	buildHull(p);

	return p;
}


void MapGenerator::buildHullBaseShape(Polygon2 p)
{
	double	 baseTotalLength;
	double	 baseBackStartX;
	double	 baseBackEndX;
	double	 baseBackScale;
	double	 baseBackLength;
	double	 baseBackPScale;
	double	 baseMidLength;
	double	 baseFrontStartX;
	double	 baseFrontEndX;
	double	 baseFrontScale;
	double	 baseFrontPScale;
	double	 baseFrontLength;
	Polygon2 hull;
	Box2	 divet;
	Point2	 hullPoint;
	double	 curPointStep;
	double	 curFunctionStep;
	double	 curFunctionParameter;
	double	 curFunctionValue;
	double	 totalPScale;
	int		 i;
	int      q;
	int		 numSamples;
	int		 numDivets;
	int      divetSide;
	double	 divetWidth;
	double	 divetHeight;
	double	 divetGap;
	double	 divetCenter;
	double	 divetMinY;
	double	 divetMaxY;
	bool     noIntersect;
	double   minCenter;
	double   maxCenter;
	double   cornerDepthL;
	double   cornerDepthR;
	double   minCornerDepth;
	Polygon2Container diffOuput;
	std::vector<std::pair<double, double>> upDivets;
	std::vector<std::pair<double, double>> downDivets;

	baseBackStartX = rnd_floatRange(-1.0, -0.5);
	baseBackEndX = rnd_floatRange(-0.5, 0.0);
	baseBackLength = rnd();

	baseMidLength = rnd();

	baseFrontStartX = rnd_floatRange(0.0, 0.4);
	baseFrontEndX = rnd_floatRange(0.9, 1);
	baseFrontLength = rnd_floatRange(0.9, 1);

	if (baseBackEndX < baseBackStartX) std::swap(baseBackStartX, baseBackEndX);
	baseBackScale = 1.0 / sqrt(1.0 - baseBackEndX*baseBackEndX);
	baseBackPScale = (baseBackEndX - baseBackStartX) / baseBackLength;

	if (baseFrontEndX < baseFrontStartX) std::swap(baseFrontStartX, baseFrontEndX);
	baseFrontScale = 1.0 / sqrt(1.0 - baseFrontStartX*baseFrontStartX);
	baseFrontPScale = (baseFrontEndX - baseFrontStartX) / baseFrontLength;

	baseTotalLength = baseBackLength + baseMidLength + baseFrontLength;
	totalPScale = 2.0 / baseTotalLength;

	curPointStep = 0;
	while ((curFunctionStep = curPointStep*SCALE_STEP) <= baseTotalLength)
	{
		if (curFunctionStep < baseBackLength)
		{
			curFunctionParameter = (curFunctionStep*baseBackPScale + baseBackStartX);
			curFunctionValue = sqrt(1 - curFunctionParameter*curFunctionParameter)*baseBackScale;
		}
		else if (curFunctionStep < (baseBackLength + baseMidLength))
		{
			curFunctionValue = 1.0;
		}
		else
		{
			curFunctionParameter = ((curFunctionStep - (baseBackLength + baseMidLength))*baseFrontPScale + baseFrontStartX);
			curFunctionValue = sqrt(1 - curFunctionParameter*curFunctionParameter)*baseFrontScale;
		}

		hullPoint = Point2(curFunctionStep * totalPScale - 1, curFunctionValue);
		append(hull, hullPoint);

		curPointStep += 1.0;
	}

	numSamples = hull.outer().size();
	for (i = numSamples - 1; i >= 0; i--)
	{
		hullPoint = hull.outer()[i];
		append(hull, Point2(hullPoint.x(), -hullPoint.y()));
	}
	hullPoint = hull.outer()[0];
	append(hull, hullPoint);

	upDivets.clear();
	downDivets.clear();
	numDivets = rnd_intRange(2, 4);
	for (i = 0; i < numDivets; i++)
	{
		do
		{
			noIntersect = true;
			divetWidth = rnd_floatRange(0.2, 0.8);
			divetHeight = rnd_floatRange(0.1, 0.5);
			minCenter = -1.0 + divetWidth*0.5 + DIVET_BUFFER;
			maxCenter =  1.0 - divetWidth*0.5 - DIVET_BUFFER;
			divetCenter = rnd_floatRange(minCenter, maxCenter);
			if (rnd_oneInXChance(2))
			{
				for (q = 0; q < upDivets.size(); q++)
				{
					if ((upDivets[q].first <= divetCenter + divetWidth*0.5) && (upDivets[q].second >= divetCenter - divetWidth*0.5))
					{
						noIntersect = false;
						break;
					}
				}
				divetSide = -1;
			}
			else
			{
				for (q = 0; q < downDivets.size(); q++)
				{
					if ((downDivets[q].first <= divetCenter + divetWidth*0.5) && (downDivets[q].second >= divetCenter - divetWidth*0.5))
					{
						noIntersect = false;
						break;
					}
				}
				divetSide = 1;
			}
		} while (!noIntersect);

		curFunctionStep = ((divetCenter - divetWidth*0.5) + 1) / totalPScale;
		if (curFunctionStep < baseBackLength)
		{
			curFunctionParameter = (curFunctionStep*baseBackPScale + baseBackStartX);
			curFunctionValue = sqrt(1 - curFunctionParameter*curFunctionParameter)*baseBackScale;
		}
		else if (curFunctionStep < (baseBackLength + baseMidLength))
		{
			curFunctionValue = 1.0;
		}
		else
		{
			curFunctionParameter = ((curFunctionStep - (baseBackLength + baseMidLength))*baseFrontPScale + baseFrontStartX);
			curFunctionValue = sqrt(1 - curFunctionParameter*curFunctionParameter)*baseFrontScale;
		}
		cornerDepthL = curFunctionValue;
		curFunctionStep = ((divetCenter + divetWidth*0.5) + 1) / totalPScale;
		if (curFunctionStep < baseBackLength)
		{
			curFunctionParameter = (curFunctionStep*baseBackPScale + baseBackStartX);
			curFunctionValue = sqrt(1 - curFunctionParameter*curFunctionParameter)*baseBackScale;
		}
		else if (curFunctionStep < (baseBackLength + baseMidLength))
		{
			curFunctionValue = 1.0;
		}
		else
		{
			curFunctionParameter = ((curFunctionStep - (baseBackLength + baseMidLength))*baseFrontPScale + baseFrontStartX);
			curFunctionValue = sqrt(1 - curFunctionParameter*curFunctionParameter)*baseFrontScale;
		}
		cornerDepthR = curFunctionValue;
		minCornerDepth = fmin(cornerDepthL, cornerDepthR);
		divetHeight = (1.0 - minCornerDepth) + DIVET_PENETRATION_DEPTH + rnd_floatRange(0.0, 0.2);

		if (divetSide == -1)
		{
			divetMinY = -1.0;
			divetMaxY = -1.0 + divetHeight;
		}
		else
		{
			divetMinY = 1.0 - divetHeight;
			divetMaxY = 1.0;
		}


		if (rnd_oneInXChance(4) && (divetWidth > DIVET_MIN_DOUBLE_WIDTH))
		{
			divetGap = rnd_floatRange(divetWidth*0.2, divetWidth*0.4);

			divet.min_corner() = Point2(divetCenter - divetWidth*0.5, divetMinY);
			divet.max_corner() = Point2(divetCenter - divetGap*0.5, divetMaxY);
			diffOuput.clear();
			difference(hull, divet, diffOuput);
			assign(hull, diffOuput[0]);

			divet.min_corner() = Point2(divetCenter + divetGap*0.5, divetMinY);
			divet.max_corner() = Point2(divetCenter + divetWidth*0.5, divetMaxY);
			diffOuput.clear();
			difference(hull, divet, diffOuput);
			assign(hull, diffOuput[0]);
		}
		else
		{
			divet.min_corner() = Point2(divetCenter - divetWidth*0.5, divetMinY);
			divet.max_corner() = Point2(divetCenter + divetWidth*0.5, divetMaxY);
			diffOuput.clear();
			difference(hull, divet, diffOuput);
			assign(hull, diffOuput[0]);
		}


		if (divetSide == -1)
		{
			upDivets.push_back(std::pair<double, double>(divetCenter - divetWidth*0.5 - DIVET_BUFFER, divetCenter + divetWidth*0.5 + DIVET_BUFFER));
		}
		else
		{
			downDivets.push_back(std::pair<double, double>(divetCenter - divetWidth*0.5 - DIVET_BUFFER, divetCenter + divetWidth*0.5 + DIVET_BUFFER));
		}
	}

#ifdef DEBUG_MAP_GENERATOR
	debugDraw->print("Hull Base");
	Point2 a, b;
	a = hull.outer()[0];
	for (i = 1; i < hull.outer().size(); i++)
	{
		b = hull.outer()[i];

		debugDraw->line(a.x() * 150 + SCRX*0.5, a.y() * 150 + SCRY*0.5, b.x() * 150 + SCRX*0.5, b.y() * 150 + SCRY*0.5, 0xffffff, "");

		a = b;
	}
#endif // DEBUG_MAP_GENERATOR

	clear(p);
	assign(p, hull);
}

void MapGenerator::buildHull(Polygon2 p)
{

	buildHullBaseShape(p);

}