#pragma once


#include "engine/os.h"
#include "engine/plugin.h"


namespace Lumix {


struct EEUISystem : IPlugin {
	virtual Engine& getEngine() = 0;
};


} // namespace Lumix