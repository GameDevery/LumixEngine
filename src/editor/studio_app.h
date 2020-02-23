#pragma once


#include "engine/lumix.h"
#include "engine/os.h"
#include "engine/array.h"


struct ImFont;

#ifdef STATIC_PLUGINS
	#define LUMIX_STUDIO_ENTRY(plugin_name) \
		extern "C" StudioApp::IPlugin* setStudioApp_##plugin_name(StudioApp& app); \
		extern "C" { StudioApp::StaticPluginRegister s_##plugin_name##_editor_register(#plugin_name, setStudioApp_##plugin_name); } \
		extern "C" StudioApp::IPlugin* setStudioApp_##plugin_name(StudioApp& app)
#else
	#define LUMIX_STUDIO_ENTRY(plugin_name) \
		extern "C" LUMIX_LIBRARY_EXPORT StudioApp::IPlugin* setStudioApp(StudioApp& app)
#endif


namespace Lumix
{

namespace Reflection { struct  PropertyBase; }


struct Action;
struct ComponentUID;
struct ResourceType;
struct Vec2;
struct WorldEditor;


struct LUMIX_EDITOR_API StudioApp : OS::Interface
{
	struct IPlugin
	{
		virtual ~IPlugin() {}
		virtual void init() = 0;
		virtual bool dependsOn(IPlugin& plugin) const { return false; }
		virtual const char* getName() const = 0;
	};

	struct GUIPlugin
	{
		virtual ~GUIPlugin() {}

		virtual void onWindowGUI() = 0;
		virtual bool hasFocus() { return false; }
		virtual void update(float) {}
		virtual void pluginAdded(GUIPlugin& plugin) {}
		virtual const char* getName() const = 0;
		virtual bool onDropFile(const char* file) { return false; }
		virtual bool packData(const char* dest_dir) { return true; }
		virtual void guiEndFrame() {}
		virtual void onSettingsLoaded() {}
		virtual void onBeforeSettingsSaved() {}
	};

	struct IAddComponentPlugin
	{
		virtual ~IAddComponentPlugin() {}
		virtual void onGUI(bool create_entity, bool from_filter) = 0;
		virtual const char* getLabel() const = 0;
	};

	struct AddCmpTreeNode
	{
		IAddComponentPlugin* plugin = nullptr;
		AddCmpTreeNode* child = nullptr;
		AddCmpTreeNode* next = nullptr;
		char label[50];
	};

	struct LUMIX_EDITOR_API StaticPluginRegister
	{
		using Creator = IPlugin* (*)(StudioApp& app);
		StaticPluginRegister(const char* name, Creator creator);

		static void create(StudioApp& app);

		StaticPluginRegister* next;
		Creator creator;
		const char* name;
	};

	static StudioApp* create();
	static void destroy(StudioApp& app);

	virtual IAllocator& getAllocator() = 0;
	virtual struct Engine& getEngine() = 0;
	virtual void run() = 0;
	virtual struct PropertyGrid& getPropertyGrid() = 0;
	virtual struct LogUI& getLogUI() = 0;
	virtual struct AssetBrowser& getAssetBrowser() = 0;
	virtual struct AssetCompiler& getAssetCompiler() = 0;
	virtual WorldEditor& getWorldEditor() = 0;
	virtual void initPlugins() = 0;
	virtual void addPlugin(IPlugin& plugin) = 0;
	virtual void addPlugin(GUIPlugin& plugin) = 0;
	virtual void removePlugin(GUIPlugin& plugin) = 0;
	virtual GUIPlugin* getPlugin(const char* name) = 0;
	virtual const char* getComponentTypeName(ComponentType cmp_type) const = 0;
	virtual void registerComponent(const char* id, const char* label) = 0;
	virtual void registerComponent(const char* id, IAddComponentPlugin& plugin) = 0;
	virtual void registerComponentWithResource(const char* id,
		const char* label,
		ResourceType resource_type,
		const char* property) = 0;
	virtual const AddCmpTreeNode& getAddComponentTreeRoot() const = 0;
	virtual int getExitCode() const = 0;
	virtual void runScript(const char* src, const char* script_name) = 0;
	virtual const Array<Action*>& getActions() = 0;
	virtual Array<Action*>& getToolbarActions() = 0;
	virtual void addAction(Action* action) = 0;
	virtual void removeAction(Action* action) = 0;
	virtual void addWindowAction(Action* action) = 0;
	virtual Action* getAction(const char* name) = 0;
	virtual void setFullscreen(bool fullscreen) = 0;
	virtual bool makeFile(const char* path, const char* content) = 0;
	virtual struct Settings& getSettings() = 0;

	virtual const OS::Event* getEvents() const = 0;
	virtual int getEventsCount() const = 0;
	virtual ~StudioApp() {}
	virtual ImFont* getBoldFont() = 0;
};


} // namespace Lumix
