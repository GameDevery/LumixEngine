#pragma once


#include "engine/plugin.h"
#include "renderer/gpu/gpu.h"

namespace Lumix {
struct EEUIScene : IScene {
	static UniquePtr<EEUIScene> createInstance(struct EEUISystem& system, Universe& universe, struct IAllocator& allocator);
	static void reflect();

	virtual EEUISystem* getSystem() = 0;
};
} // namespace Lumix