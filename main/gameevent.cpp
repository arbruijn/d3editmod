#include "object.h"
#include "spew.h"
#include "gameloop.h"
#include "descent.h"

void ClearAllEvents()
{
	// ...
	SpewInit();
	Render_zoom = D3_DEFAULT_ZOOM;
}

void ProcessNormalEvents() 
{
	SpewEmitAll();
}
