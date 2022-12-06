#include "hud.h"
#include "ship.h"

typedef struct {
    char model[35];
    char shieldring[35];
    char shieldring02[35];
    char shieldring03[35];
    char shieldring04[35];
    char shieldring05[35];
    char hudship[35];
    char hudburn[35];
    char hudenergy[35];
    char shieldinv[35];
} tCockpitCfgInfo;

void LoadCockpitInfo(char *cockpit_name, tCockpitCfgInfo *cfg)
{
	LoadHUDConfig(cockpit_name);
}

void InitCockpit(int ship_index)
{
	tCockpitCfgInfo cfg;
	LoadCockpitInfo(Ships[ship_index].cockpit_name, &cfg);
}
