#include "Constants.h"
#include "Debug.h"
#include "Painter.h"
#include "MapGenerator.h"

void main(int argc, char *argv[])
{
	MapGenerator G;

	DebugDraw.init(SCRX, SCRY, "ProcedureCastle");
	FONT_main = DebugDraw.loadFont("Resources/Fonts/ProggyClean.ttf", 16);

	while(true)
	{
		DebugDraw.cls();
		G.generate();

		DebugDraw.sync();
		DebugDraw.pause();

	}
	return;
}