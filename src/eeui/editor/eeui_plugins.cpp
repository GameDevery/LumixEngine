#include <imgui/imgui.h>

#include "editor/asset_browser.h"
#include "editor/asset_compiler.h"
#include "editor/settings.h"
#include "editor/studio_app.h"
#include "editor/utils.h"
#include "editor/world_editor.h"
#include "engine/crt.h"
#include "engine/engine.h"
#include "engine/geometry.h"
#include "engine/log.h"
#include "engine/math.h"
#include "engine/os.h"
#include "engine/path.h"
#include "engine/reflection.h"
#include "engine/resource_manager.h"
#include "engine/universe.h"
#include "eeui/eeui_scene.h"
#include "renderer/draw2d.h"
#include "renderer/gpu/gpu.h"
#include "renderer/pipeline.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"

using namespace Lumix;


namespace
{

struct StudioAppPlugin : StudioApp::IPlugin
{
	StudioAppPlugin(StudioApp& app)
		: m_app(app)
	{
	}


	const char* getName() const override { return "eeui"; }
	bool dependsOn(IPlugin& plugin) const override { return equalStrings(plugin.getName(), "renderer"); }


	void init() override {
	}

	bool showGizmo(UniverseView&, ComponentUID) override { return false; }
	
	~StudioAppPlugin() {
	}


	StudioApp& m_app;
};



} // anonymous namespace


LUMIX_STUDIO_ENTRY(eeui)
{
	IAllocator& allocator = app.getAllocator();
	return LUMIX_NEW(allocator, StudioAppPlugin)(app);
}
