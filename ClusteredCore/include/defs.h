#ifndef DEFS_H
#define DEFS_H

#include <iostream>
#include "diagnostics.h"

namespace PSim
{
// GLOBAL VARIABLES
extern PSim::Diagnostics chatterBox;
}
// Defined Functions
#define tos(a) std::to_string(a)
// TYPE DEFS
typedef unsigned int uint;

#endif
