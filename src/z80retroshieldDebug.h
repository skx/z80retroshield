/*
 * This z80retroshieldDebug class has some debugging features enabled that are
 * conditionally compiled with #ifdef Z80RetroShield_DEBUG.
 * The z80retroshieldDebug class is almost identical to the z80retroshield.
 * Therefore, the source code .h and .cpp share the same file. The reason
 * debugging functions are not enabled by inheritance and virtual functions is
 * to prioritize the efficiency of Tick() execution.
 */

#define Z80RetroShield_DEBUG
#include "z80retroshield.h"
#undef Z80RetroShield_DEBUG
