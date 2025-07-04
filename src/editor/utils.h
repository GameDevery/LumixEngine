#pragma once

#include "engine/lumix.h"

#include "core/array.h"
#include "core/delegate.h"
#include "core/span.h"
#include "core/string.h"
#include "core/stream.h"

#include <imgui/imgui.h>

namespace Lumix {

namespace os { enum class Keycode : u8; }

struct LUMIX_EDITOR_API ResourceLocator {
	ResourceLocator(StringView path);

	StringView subresource;
	StringView dir;
	StringView basename;
	StringView ext;
	StringView resource;

	StringView full;
};

struct LUMIX_EDITOR_API Action {
	enum Modifiers : u8 {
		NONE = 0,

		SHIFT = 1 << 0,
		ALT = 1 << 1,
		CTRL = 1 << 2
	};

	enum Type {
		NORMAL,
		TOOL,
		WINDOW,
		TEMPORARY
	};

	Action(const char* label_short, const char* label_long, const char* name, const char* font_icon, Type type = NORMAL);
	~Action();
	Action(const Action&) = delete;
	void operator =(const Action&) = delete;
	bool toolbarButton(struct ImFont* font, bool is_selected = false);
	bool isActive() const;
	bool iconButton(bool enabled = true, struct StudioApp* app = nullptr);
	bool shortcutText(Span<char> out) const;

	StaticString<32> name;			// used for serialization
	StaticString<32> label_short;	// used in menus
	StaticString<64> label_long;	// used in shortcut editor
	bool request = false;			// programatically request to invoke the action
	Modifiers modifiers = Modifiers::NONE;
	os::Keycode shortcut;
	StaticString<5> font_icon;
	Type type;

	// linked list of all actions
	static Action* first_action;
	Action* next = nullptr;
	Action* prev = nullptr;
};

struct CommonActions {
	Action save{"Save", "Common - Save", "save", ICON_FA_SAVE};
	Action undo{"Undo", "Common - Undo", "undo", ICON_FA_UNDO};
	Action redo{"Redo", "Common - Redo", "redo", ICON_FA_REDO};
	Action del{"Delete", "Common - Delete", "delete", ICON_FA_MINUS_SQUARE};
	Action cam_orbit{"Orbit", "Camera - orbit with RMB", "orbit_rmb", ""};
	Action cam_forward{"Move forward", "Camera - move forward", "camera_move_forward", ""};
	Action cam_backward{"Move back", "Camera - move backward", "camera_move_back", ""};
	Action cam_left{"Move left", "Camera - move left", "camera_move_left", ""};
	Action cam_right{"Move right", "Camera - move right", "camera_move_right", ""};
	Action cam_up{"Move up", "Camera - move up", "camera_move_up", ""};
	Action cam_down{"Move down", "Camera - move down", "camera_move_down", ""};
	Action select_all{"Select all", "Common - Select all", "select_all", ""};
	Action rename{"Rename", "Common - Rename", "rename", ""};
	Action copy{"Copy", "Common - Copy", "copy", ICON_FA_CLIPBOARD};
	Action paste{"Paste", "Common - Paste", "paste", ICON_FA_PASTE};
	Action close_window{"Close", "Close window", "close_window", ""};
	Action open_externally{"Open externally", "Common - open externally", "open_externally", ICON_FA_EXTERNAL_LINK_ALT};
	Action view_in_browser{"View in browser", "Common - view in asset browser", "view_in_asset_browser", ICON_FA_SEARCH};
};

inline Action::Modifiers operator |(Action::Modifiers a, Action::Modifiers b) { return Action::Modifiers((u8)a | (u8)b); }
inline void operator |= (Action::Modifiers& a, Action::Modifiers b) { a = a | b; }

LUMIX_EDITOR_API void getShortcut(const Action& action, Span<char> buf);
LUMIX_EDITOR_API [[nodiscard]] bool menuItem(const Action& a, bool enabled);
LUMIX_EDITOR_API void getEntityListDisplayName(StudioApp& app, struct World& editor, Span<char> buf, EntityPtr entity, bool force_display_index = false);
LUMIX_EDITOR_API bool inputRotation(const char* label, struct Quat* value);
LUMIX_EDITOR_API bool inputString(const char* label, String* value);
LUMIX_EDITOR_API bool inputString(const char* str_id, const char* label, String* value);
LUMIX_EDITOR_API bool inputStringMultiline(const char* label, String* value, const ImVec2& size = ImVec2(0, 0));
LUMIX_EDITOR_API void openCenterStrip(const char* str_id);
LUMIX_EDITOR_API bool beginCenterStrip(const char* str_id, u32 lines = 5);
LUMIX_EDITOR_API void endCenterStrip();

struct LUMIX_EDITOR_API SimpleUndoRedo {
	enum { NO_MERGE_UNDO = 0xffFFffFF };
	struct Undo {
		Undo(IAllocator& allocator) : blob(allocator) {}
		u32 tag;
		OutputMemoryStream blob;
	};

	SimpleUndoRedo(IAllocator& allocator);
	virtual ~SimpleUndoRedo() {}

	bool canUndo() const;
	bool canRedo() const;
	void undo();
	void redo();
	virtual void pushUndo(u32 tag);
	void clearUndoStack();
	bool isReady() const { return !m_stack.empty(); }

	virtual void deserialize(InputMemoryStream& blob) = 0;
	virtual void serialize(OutputMemoryStream& blob) = 0;

private:
	IAllocator& m_allocator;
	Array<Undo> m_stack;
	i32 m_stack_idx = -1;
};

struct NodeEditorLink {
	u32 from;
	u32 to;
	u32 color = ImGui::GetColorU32(ImGuiCol_Text);

	u16 getToNode() const { return to & 0xffFF; }
	u16 getFromNode() const { return from & 0xffFF; }

	u16 getToPin() const { return (to >> 16) & 0x7fFF; }
	u16 getFromPin() const { return (from >> 16) & 0x7fFF; }
};

struct NodeEditorNode {
	u16 m_id;
	ImVec2 m_pos;

	virtual ~NodeEditorNode() {}
	virtual bool hasInputPins() const = 0;
	virtual bool hasOutputPins() const = 0;
	virtual bool nodeGUI() = 0;
};

struct DirSelector {
	DirSelector(StudioApp& app);
	bool gui(const char* label, bool* open);
	const char* getDir() const { return m_current_dir.c_str(); }

private:
	void fillSubitems();
	bool breadcrumb(StringView path);

	StudioApp& m_app;
	String m_current_dir;
	Array<String> m_subdirs;
	bool m_creating_folder = false;
	char m_new_folder_name[MAX_PATH] = "";
};

struct LUMIX_EDITOR_API FileSelector {
	FileSelector(StudioApp& app);
	FileSelector(const char* ext, StudioApp& app);
	// popup
	bool gui(const char* label, bool* open, const char* extension, bool save);
	// inplace
	bool gui(bool show_breadcrumbs, const char* extension);
	const char* getPath();
	String m_current_dir;

private:
	bool breadcrumb(StringView path);
	void fillSubitems();

	StudioApp& m_app;
	bool m_save;
	String m_filename;
	String m_accepted_extension;
	Array<String> m_subdirs;
	Array<String> m_subfiles;
	String m_full_path;
};

struct LUMIX_EDITOR_API TextFilter {
	bool isActive() const { return count != 0; }
	void clear() { count = 0; filter[0] = 0; }
	bool pass(StringView text) const;
	// if filter is empty, returns 1
	// returns 0 if does not pass, otherwise returns score
	u32 passWithScore(StringView text) const;
	void build();
	// show filter input, returns true if filter changed
	// add `focus_action`'s shortcut to label
	bool gui(const char* label, float width = -1, bool set_keyboard_focus = false, Action* focus_action = nullptr);

	char filter[128] = "";
	StringView subfilters[8];
	u32 count = 0;
};

struct LUMIX_EDITOR_API NodeEditor : SimpleUndoRedo {
	enum { OUTPUT_FLAG = 1 << 31 };

	NodeEditor(IAllocator& allocator);

	virtual void onCanvasClicked(ImVec2 pos, i32 hovered_link) = 0;
	virtual void onLinkDoubleClicked(NodeEditorLink& link, ImVec2 pos) = 0;
	virtual void onNodeDoubleClicked(NodeEditorNode& node) {}
	virtual void onContextMenu(ImVec2 pos) = 0;

	void splitLink(const NodeEditorNode* node, Array<NodeEditorLink>& links, u32 link_idx);
	void nodeEditorGUI(Span<NodeEditorNode*> nodes, Array<NodeEditorLink>& links);

	ImGuiEx::Canvas m_canvas;
	ImVec2 m_offset = ImVec2(0, 0);
	ImVec2 m_mouse_pos_canvas;
	ImGuiID m_half_link_start = 0;
	bool m_is_any_item_active = false;
	u32 m_dragged_node = 0xFFffFFff;
};

struct CodeEditor {
	using Tokenizer = bool (*)(const char* str, u32& token_len, u8& token_type, u8 prev_token_type);

	virtual ~CodeEditor() {}
	
	virtual u32 getCursorLine(u32 cursor_index = 0) = 0;
	virtual u32 getCursorColumn(u32 cursor_index = 0) = 0;
	virtual void setSelection(u32 from_line, u32 from_col, u32 to_line, u32 to_col, bool ensure_visibility) = 0;
	virtual void selectWord() = 0;
	virtual bool canHandleInput() = 0;
	virtual void underlineTokens(u32 line, u32 col_from, u32 col_to, const char* msg) = 0;
	virtual void insertText(const char* text) = 0;
	virtual u32 getNumCursors() = 0;
	virtual ImVec2 getCursorScreenPosition(u32 cursor_index = 0) = 0;
	virtual void focus() = 0;
	// get part of word left of cursor, usable for example for autocomplete
	virtual StringView getPrefix() = 0;
	
	virtual void setReadOnly(bool readonly) = 0;
	virtual void setText(StringView text) = 0;
	virtual void serializeText(OutputMemoryStream& blob) = 0;
	virtual void setTokenColors(Span<const u32> colors) = 0; // keep colors alive while CodeEditor uses them
	virtual void setTokenizer(Tokenizer tokenizer) = 0; // user needs to keep the language alive while CodeEditor uses it
	virtual bool gui(const char* str_id, const ImVec2& size = ImVec2(0, 0), ImFont* ui_font = nullptr) = 0;
};

LUMIX_EDITOR_API UniquePtr<CodeEditor> createCodeEditor(StudioApp& app);
LUMIX_EDITOR_API UniquePtr<CodeEditor> createCppCodeEditor(StudioApp& app);
LUMIX_EDITOR_API UniquePtr<CodeEditor> createLuaCodeEditor(StudioApp& app);
LUMIX_EDITOR_API UniquePtr<CodeEditor> createHLSLCodeEditor(StudioApp& app);

template <typename F> void alignGUI(float align, const F& f) {
	const ImVec2 container_size = ImGui::GetContentRegionAvail();
	const ImVec2 cp = ImGui::GetCursorScreenPos();
	ImGuiStyle& style = ImGui::GetStyle();
	float alpha_backup = style.DisabledAlpha;
	style.DisabledAlpha = 0;
	ImGui::BeginDisabled();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse |
							   ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
							   ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking;
	const char* id = "imgui_lumix_measure__";
	ImGui::Begin(id, nullptr, flags);
	ImGuiEx::SetSkipItems(false);
	ImGui::BeginGroup();
	f();
	ImGui::EndGroup();
	const ImVec2 size = ImGui::GetItemRectSize();
	ImGui::End();
	ImGui::EndDisabled();
	style.DisabledAlpha = alpha_backup;
	ImGui::SetCursorScreenPos(ImVec2(cp.x + (container_size.x - size.x) * align, cp.y));
	f();
}

template <typename F> void alignGUIRight(const F& f) { alignGUI(1, f); }
template <typename F> void alignGUICenter(const F& f) { alignGUI(0.5f, f); }


} // namespace Lumix
