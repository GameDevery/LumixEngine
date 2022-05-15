#ifndef EE_UITOOLSCTEXTUREATLASTEXTUREREGIONEDITOR_HPP
#define EE_UITOOLSCTEXTUREATLASTEXTUREREGIONEDITOR_HPP

#include <eepp/graphics/textureregion.hpp>
#include <eeui/ui/base.hpp>
#include <eeui/ui/uitextureregion.hpp>
#include <eeui/ui/uiwidget.hpp>

namespace EE { namespace UI { namespace Tools {

class TextureAtlasEditor;

class EE_API TextureAtlasTextureRegionEditor : public UIWidget {
  public:
	static TextureAtlasTextureRegionEditor* New( TextureAtlasEditor* Editor );

	TextureAtlasTextureRegionEditor( TextureAtlasEditor* Editor );

	virtual ~TextureAtlasTextureRegionEditor();

	virtual void draw();

	Graphics::TextureRegion* getTextureRegion() const;

	void setTextureRegion( Graphics::TextureRegion* TextureRegion );

	UITextureRegion* getGfx() const;

  protected:
	UITextureRegion* mGfx;
	UINode* mDrag;
	Vector2f mUICenter;
	TextureAtlasEditor* mEditor;
	Vector2f mDragPos;

	virtual void onSizeChange();

	void getCenter();
};

}}} // namespace EE::UI::Tools

#endif
