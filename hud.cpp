#include <stdio.h>
#include <stdarg.h>
#include "hud.h"

bool AddHUDMessage(char *msg, ...) {
	va_list vp;
	va_start(vp, msg);
	vprintf(msg, vp);
	va_end(vp);
	return false;
}

bool AddFilteredHUDMessage(char *msg, ...) {
	va_list vp;
	va_start(vp, msg);
	vprintf(msg, vp);
	va_end(vp);
	return false;
}
