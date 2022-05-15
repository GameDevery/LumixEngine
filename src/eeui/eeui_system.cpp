#include "eeui_system.h"
#include "eeui/eeui_scene.h"
#include "engine/allocator.h"
#include "engine/engine.h"
#include "engine/reflection.h"
#include "engine/universe.h"

namespace Lumix {
struct EEUISystemImpl final : EEUISystem {
	explicit EEUISystemImpl(Engine& engine)
		: m_engine(engine) {
		EEUIScene::reflect();
	}


	~EEUISystemImpl() {}

	Engine& getEngine() override { return m_engine; }

	void stopGame() override {}


	const char* getName() const override { return "eeui"; }

	u32 getVersion() const override { return 0; }
	void serialize(OutputMemoryStream& stream) const override {}
	bool deserialize(u32 version, InputMemoryStream& stream) override { return version == 0; }

	Engine& m_engine;
};


LUMIX_PLUGIN_ENTRY(eeui) {
	return LUMIX_NEW(engine.getAllocator(), EEUISystemImpl)(engine);
}
} // namespace Lumix