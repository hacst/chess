#ifndef GLOBALS_H
#define GLOBALS_H

#include "core/GameConfiguration.h"
#include <random>

//! Global configuration for use inside the GUI thread
extern GameConfiguration global_config;
//! Global seed value for use inside GUI to seed subcomponents
extern int global_seed;

#endif //GLOBALS_H
