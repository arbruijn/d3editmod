#include "gamedata.h"

void load_game_data(bool do_init) {
	if (!LoadTableFile(!do_init))
		Error("Cannot load table file.");
}