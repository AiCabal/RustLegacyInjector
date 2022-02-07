#ifndef SUBHWID_H
#define SUBHWID_H

#include <Windows.h>
#include "processhelper.h"
#include "include/hardwareid.h"
#define WINSEVEN_GETSYSTEM_OFFSET 0x94
#define WINSEVEN_ENUMSYSTEM_OFFSET 0x95
#define WINTEN_GETSYSTEM_OFFSET 0x9A
#define WINTEN_ENUMSYSTEM_OFFSET 0x8D
						 

void ActivateHWIDChanger(p_helper*);


#endif // SUBHWID_H
