#include "eeui_scene.h"
#include "eeui_system.h"
#include "engine/allocator.h"
#include "engine/associative_array.h"
#include "engine/crt.h"
#include "engine/engine.h"
#include "engine/flag_set.h"
#include "engine/input_system.h"
#include "engine/log.h"
#include "engine/os.h"
#include "engine/reflection.h"
#include "engine/resource_manager.h"
#include "engine/string.h"
#include "engine/universe.h"
#include "renderer/draw2d.h"
#include "renderer/font.h"
#include "renderer/pipeline.h"
#include "renderer/texture.h"

namespace Lumix {

struct EEUISceneImpl final : EEUIScene {
	enum class Version : i32 { LATEST };
	EEUISceneImpl(EEUISystem& system, Universe& context, IAllocator& allocator)
		: m_allocator(allocator)
		, m_universe(context)
		, m_system(system)
		, m_draw_2d(allocator) {}


	void clear() override {}


	void update(float time_delta, bool paused) override {
		if (paused) return;
	}


	void serialize(OutputMemoryStream& serializer) override {}


	void deserialize(InputMemoryStream& serializer, const EntityMap& entity_map, i32 version) override {}

	EEUISystem* getSystem() override { return &m_system; }

	Universe& getUniverse() override { return m_universe; }
	IPlugin& getPlugin() const override { return m_system; }

	IAllocator& m_allocator;
	Universe& m_universe;
	EEUISystem& m_system;
	Draw2D m_draw_2d;
};

UniquePtr<EEUIScene> EEUIScene::createInstance(EEUISystem& system, Universe& universe, IAllocator& allocator) {
	return UniquePtr<EEUISceneImpl>::create(allocator, system, universe, allocator);
}

void EEUIScene::reflect() {
	LUMIX_SCENE(EEUISceneImpl, "eeui");
}

} // namespace Lumix