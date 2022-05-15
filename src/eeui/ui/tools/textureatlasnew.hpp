#ifndef EE_UITOOLSCTEXTUREATLASNEW_HPP
#define EE_UITOOLSCTEXTUREATLASNEW_HPP

#include <eepp/graphics/texturepacker.hpp>
#include <eeui/ui/base.hpp>
#include <eeui/ui/uicheckbox.hpp>
#include <eeui/ui/uicombobox.hpp>
#include <eeui/ui/uipushbutton.hpp>
#include <eeui/ui/uispinbox.hpp>
#include <eeui/ui/uiwindow.hpp>

namespace EE { namespace UI { namespace Tools {

class EE_API TextureAtlasNew {
  public:
	typedef std::function<void( TexturePacker* )> TGCreateCb;

	TextureAtlasNew( TGCreateCb NewTGCb = TGCreateCb() );

	virtual ~TextureAtlasNew();

  protected:
	UIWindow* mUIWindow;
	TGCreateCb mNewTGCb;
	UIComboBox* mComboWidth;
	UIComboBox* mComboHeight;
	UISpinBox* mPixelSpace;
	UITextInput* mTGPath;
	UIPushButton* mSetPathButton;
	UIDropDownList* mSaveFileType;
	UIDropDownList* mPixelDensity;
	UIDropDownList* mTextureFilter;
	UICheckBox* mForcePow2;
	UICheckBox* mScalableSVG;
	UICheckBox* mSaveExtensions;
	UICheckBox* mAllowChilds;

	void windowClose( const Event* Event );

	void cancelClick( const Event* Event );

	void okClick( const Event* Event );

	void onDialogFolderSelect( const Event* Event );

	void onSelectFolder( const Event* Event );

	void textureAtlasSave( const Event* Event );
};

}}} // namespace EE::UI::Tools

#endif
