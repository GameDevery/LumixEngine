#include <eeui/ui/uicheckbox.hpp>
#include <eeui/ui/uicodeeditor.hpp>
#include <eeui/ui/uicombobox.hpp>
#include <eeui/ui/uidropdownlist.hpp>
#include <eeui/ui/uigridlayout.hpp>
#include <eeui/ui/uiimage.hpp>
#include <eeui/ui/uilinearlayout.hpp>
#include <eeui/ui/uilistbox.hpp>
#include <eeui/ui/uilistview.hpp>
#include <eeui/ui/uiloader.hpp>
#include <eeui/ui/uimenubar.hpp>
#include <eeui/ui/uiprogressbar.hpp>
#include <eeui/ui/uipushbutton.hpp>
#include <eeui/ui/uiradiobutton.hpp>
#include <eeui/ui/uirelativelayout.hpp>
#include <eeui/ui/uiscrollbar.hpp>
#include <eeui/ui/uiscrollview.hpp>
#include <eeui/ui/uiselectbutton.hpp>
#include <eeui/ui/uislider.hpp>
#include <eeui/ui/uispinbox.hpp>
#include <eeui/ui/uisplitter.hpp>
#include <eeui/ui/uisprite.hpp>
#include <eeui/ui/uistackwidget.hpp>
#include <eeui/ui/uitab.hpp>
#include <eeui/ui/uitableview.hpp>
#include <eeui/ui/uitabwidget.hpp>
#include <eeui/ui/uitextedit.hpp>
#include <eeui/ui/uitextinput.hpp>
#include <eeui/ui/uitextinputpassword.hpp>
#include <eeui/ui/uitextureregion.hpp>
#include <eeui/ui/uitextview.hpp>
#include <eeui/ui/uitouchdraggablewidget.hpp>
#include <eeui/ui/uitreeview.hpp>
#include <eeui/ui/uiviewpager.hpp>
#include <eeui/ui/uiwidgetcreator.hpp>
#include <eeui/ui/uiwidgettable.hpp>
#include <eeui/ui/uiwidgettablerow.hpp>
#include <eeui/ui/uiwindow.hpp>

namespace EE { namespace UI {

static bool sBaseListCreated = false;

UIWidgetCreator::WidgetCallbackMap UIWidgetCreator::widgetCallback =
	UIWidgetCreator::WidgetCallbackMap();

UIWidgetCreator::RegisteredWidgetCallbackMap UIWidgetCreator::registeredWidget =
	UIWidgetCreator::RegisteredWidgetCallbackMap();

void UIWidgetCreator::createBaseWidgetList() {
	if ( !sBaseListCreated ) {
		registeredWidget["widget"] = UIWidget::New;
		registeredWidget["linearlayout"] = UILinearLayout::NewVertical;
		registeredWidget["relativelayout"] = UIRelativeLayout::New;
		registeredWidget["textview"] = UITextView::New;
		registeredWidget["pushbutton"] = UIPushButton::New;
		registeredWidget["checkbox"] = UICheckBox::New;
		registeredWidget["radiobutton"] = UIRadioButton::New;
		registeredWidget["combobox"] = UIComboBox::New;
		registeredWidget["dropdownlist"] = UIDropDownList::New;
		registeredWidget["image"] = UIImage::New;
		registeredWidget["listbox"] = UIListBox::New;
		registeredWidget["menubar"] = UIMenuBar::New;
		registeredWidget["progressbar"] = UIProgressBar::New;
		registeredWidget["scrollbar"] = UIScrollBar::New;
		registeredWidget["slider"] = UISlider::New;
		registeredWidget["spinbox"] = UISpinBox::New;
		registeredWidget["sprite"] = UISprite::New;
		registeredWidget["tab"] = UITab::New;
		registeredWidget["widgettable"] = UIWidgetTable::New;
		registeredWidget["widgettablerow"] = UIWidgetTableRow::New;
		registeredWidget["tabwidget"] = UITabWidget::New;
		registeredWidget["textedit"] = UITextEdit::New;
		registeredWidget["textinput"] = UITextInput::New;
		registeredWidget["textinputpassword"] = UITextInputPassword::New;
		registeredWidget["loader"] = UILoader::New;
		registeredWidget["selectbutton"] = UISelectButton::New;
		registeredWidget["window"] = UIWindow::New;
		registeredWidget["scrollview"] = UIScrollView::New;
		registeredWidget["textureregion"] = UITextureRegion::New;
		registeredWidget["touchdraggable"] = UITouchDraggableWidget::New;
		registeredWidget["gridlayout"] = UIGridLayout::New;
		registeredWidget["layout"] = UILayout::New;
		registeredWidget["viewpager"] = UIViewPager::New;
		registeredWidget["codeeditor"] = UICodeEditor::New;
		registeredWidget["splitter"] = UISplitter::New;
		registeredWidget["treeview"] = UITreeView::New;
		registeredWidget["tableview"] = UITableView::New;
		registeredWidget["listview"] = UIListView::New;
		registeredWidget["stackwidget"] = UIStackWidget::New;

		registeredWidget["hbox"] = UILinearLayout::NewHorizontal;
		registeredWidget["vbox"] = UILinearLayout::NewVertical;
		registeredWidget["input"] = UITextInput::New;
		registeredWidget["inputpassword"] = UITextInputPassword::New;
		registeredWidget["viewpagerhorizontal"] = UIViewPager::NewHorizontal;
		registeredWidget["viewpagervertical"] = UIViewPager::NewHorizontal;
		registeredWidget["vslider"] = UISlider::NewHorizontal;
		registeredWidget["hslider"] = UISlider::NewHorizontal;
		registeredWidget["vscrollbar"] = UIScrollBar::NewVertical;
		registeredWidget["hscrollbar"] = UIScrollBar::NewHorizontal;

		sBaseListCreated = true;
	}
}

UIWidget* UIWidgetCreator::createFromName( std::string widgetName ) {
	createBaseWidgetList();

	String::toLowerInPlace( widgetName );

	if ( registeredWidget.find( widgetName ) != registeredWidget.end() ) {
		return registeredWidget[widgetName]();
	}

	if ( widgetCallback.find( widgetName ) != widgetCallback.end() ) {
		return widgetCallback[widgetName]( widgetName );
	}

	return NULL;
}

void UIWidgetCreator::addCustomWidgetCallback( std::string widgetName,
											   const UIWidgetCreator::CustomWidgetCb& cb ) {
	widgetCallback[String::toLower( widgetName )] = cb;
}

void UIWidgetCreator::removeCustomWidgetCallback( std::string widgetName ) {
	widgetCallback.erase( String::toLower( widgetName ) );
}

bool UIWidgetCreator::existsCustomWidgetCallback( std::string widgetName ) {
	return widgetCallback.find( String::toLower( widgetName ) ) != widgetCallback.end();
}

void UIWidgetCreator::registerWidget( std::string widgetName,
									  const UIWidgetCreator::RegisterWidgetCb& cb ) {
	registeredWidget[String::toLower( widgetName )] = cb;
}

void UIWidgetCreator::unregisterWidget( std::string widgetName ) {
	registeredWidget.erase( String::toLower( widgetName ) );
}

bool UIWidgetCreator::isWidgetRegistered( std::string widgetName ) {
	return registeredWidget.find( String::toLower( widgetName ) ) != registeredWidget.end();
}

const UIWidgetCreator::RegisteredWidgetCallbackMap& UIWidgetCreator::getRegisteredWidgets() {
	return registeredWidget;
}

std::vector<std::string> UIWidgetCreator::getWidgetNames() {
	std::vector<std::string> names;
	createBaseWidgetList();
	for ( auto& widgetIt : registeredWidget ) {
		names.push_back( widgetIt.first );
	}
	return names;
}

}} // namespace EE::UI
