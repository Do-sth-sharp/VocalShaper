#include "ColorEditor.h"
#include "../lookAndFeel/LookAndFeelFactory.h"
#include "../misc/RCManager.h"
#include "../misc/ColorMap.h"
#include "../Utils.h"

class ColorHistory final : private juce::DeletedAtShutdown {
public:
	ColorHistory() {
		this->load();
	}

	void add(const juce::Colour& color) {
		/** Remove If Already In List */
		this->colorList.removeFirstMatchingValue(color);

		/** Add At Head */
		this->colorList.insert(0, color);

		/** Limit List Size */
		if (this->colorList.size() > this->maxSize) {
			this->colorList.resize(this->maxSize);
		}

		/** Save History */
		this->save();
	};

	const juce::Array<juce::Colour>& getList() const {
		return this->colorList;
	};

	void load() {
		this->colorList.clear();

		auto obj = juce::JSON::parse(utils::getEditorDataFile("colorHistory.json"));
		if (obj.isArray()) {
			auto array = obj.getArray();
			for (auto& i : *array) {
				this->colorList.add(
					juce::Colour::fromString(i.toString()).withAlpha(1.f));
			}
		}

		if (this->colorList.size() > this->maxSize) {
			this->colorList.resize(this->maxSize);
		}
	};

	void save() {
		juce::Array<juce::var> array;

		for (auto& i : this->colorList) {
			array.add(juce::var{ i.toDisplayString(false) });
		}

		juce::File file = utils::getEditorDataFile("colorHistory.json");
		file.getParentDirectory().createDirectory();

		juce::FileOutputStream stream(file);
		stream.setPosition(0);
		stream.truncate();
		
		juce::var obj(array);
		juce::JSON::writeToStream(stream, obj, true);
	};

private:
	const int maxSize = 30;
	juce::Array<juce::Colour> colorList;

public:
	static ColorHistory* getInstance();
	static void releaseInstance();

private:
	static ColorHistory* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColorHistory)
};

ColorHistory* ColorHistory::getInstance() {
	return ColorHistory::instance ? ColorHistory::instance
		: (ColorHistory::instance = new ColorHistory{});
}

void ColorHistory::releaseInstance() {
	if (ColorHistory::instance) {
		delete ColorHistory::instance;
		ColorHistory::instance = nullptr;
	}
}

ColorHistory* ColorHistory::instance = nullptr;

ColorEditorContent::ColorEditorContent(
	ColorEditor* parent,
	const Callback& callback,
	const juce::Colour& defaultColor)
	: parent(parent), callback(callback),
	historyList(ColorHistory::getInstance()->getList()),
	themeColors({ ColorMap::getInstance()->get("ThemeColorA2") }),
	themeAlertColors({ ColorMap::getInstance()->get("ThemeColorC1"),
		ColorMap::getInstance()->get("ThemeColorC2"), 
		ColorMap::getInstance()->get("ThemeColorC3"), 
		ColorMap::getInstance()->get("ThemeColorC4"), 
		ColorMap::getInstance()->get("ThemeColorC5"), 
		ColorMap::getInstance()->get("ThemeColorC6") }) {
	this->setWantsKeyboardFocus(true);

	/** Look And Feel */
	this->setLookAndFeel(
		LookAndFeelFactory::getInstance()->forColorEditor());

	/** Title */
	this->commonTitle = TRANS("Common Colors");
	this->historyTitle = TRANS("History Colors");
	this->rTitle = TRANS("Red");
	this->gTitle = TRANS("Green");
	this->bTitle = TRANS("Blue");
	this->hexTitle = TRANS("Hex");

	/** Color Selector */
	this->colorSpace = std::make_unique<ColourSpaceView>(this);
	this->addAndMakeVisible(this->colorSpace.get());

	this->hueSelector = std::make_unique<HueSelectorComp>(this);
	this->addAndMakeVisible(this->hueSelector.get());

	/** Text Editor */
	this->rEditor = std::make_unique<juce::TextEditor>(TRANS("Red"));
	this->rEditor->setHasFocusOutline(false);
	this->rEditor->setMultiLine(false);
	this->rEditor->setJustification(juce::Justification::centredLeft);
	this->rEditor->setClicksOutsideDismissVirtualKeyboard(true);
	this->rEditor->setPopupMenuEnabled(false);
	this->rEditor->setInputFilter(new utils::TextColorRGBFilter, true);
	this->rEditor->onTextChange = [this] { this->rgbValueChanged(); };
	this->addAndMakeVisible(this->rEditor.get());

	this->gEditor = std::make_unique<juce::TextEditor>(TRANS("Green"));
	this->gEditor->setHasFocusOutline(false);
	this->gEditor->setMultiLine(false);
	this->gEditor->setJustification(juce::Justification::centredLeft);
	this->gEditor->setClicksOutsideDismissVirtualKeyboard(true);
	this->gEditor->setPopupMenuEnabled(false);
	this->gEditor->setInputFilter(new utils::TextColorRGBFilter, true);
	this->gEditor->onTextChange = [this] { this->rgbValueChanged(); };
	this->addAndMakeVisible(this->gEditor.get());

	this->bEditor = std::make_unique<juce::TextEditor>(TRANS("Blue"));
	this->bEditor->setHasFocusOutline(false);
	this->bEditor->setMultiLine(false);
	this->bEditor->setJustification(juce::Justification::centredLeft);
	this->bEditor->setClicksOutsideDismissVirtualKeyboard(true);
	this->bEditor->setPopupMenuEnabled(false);
	this->bEditor->setInputFilter(new utils::TextColorRGBFilter, true);
	this->bEditor->onTextChange = [this] { this->rgbValueChanged(); };
	this->addAndMakeVisible(this->bEditor.get());

	this->hexEditor = std::make_unique<juce::TextEditor>(TRANS("Hex"));
	this->hexEditor->setHasFocusOutline(false);
	this->hexEditor->setMultiLine(false);
	this->hexEditor->setJustification(juce::Justification::centredLeft);
	this->hexEditor->setClicksOutsideDismissVirtualKeyboard(true);
	this->hexEditor->setPopupMenuEnabled(false);
	this->hexEditor->setInputFilter(new utils::TextColorHexFilter, true);
	this->hexEditor->onTextChange = [this] { this->hexValueChanged(); };
	this->addAndMakeVisible(this->hexEditor.get());

	/** Color Viewer */
	this->colorViewer = std::make_unique<ColorViewer>();
	this->addAndMakeVisible(this->colorViewer.get());

	/** OK Button */
	this->okButton = std::make_unique<juce::TextButton>(TRANS("OK"));
	this->okButton->setWantsKeyboardFocus(false);
	this->okButton->setMouseCursor(juce::MouseCursor::PointingHandCursor);
	this->okButton->onClick = [this] { this->okButtonPressed(); };
	this->addAndMakeVisible(this->okButton.get());

	/** Set Default Color */
	this->updateColor(defaultColor.withAlpha(1.f));
}

void ColorEditorContent::resized() {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.015;
	int padddingWidth = screenSize.getWidth() * 0.01;
	int splitHeight = screenSize.getHeight() * 0.0075;
	int splitWidth = screenSize.getWidth() * 0.005;

	int colorHeight = screenSize.getHeight() * 0.02;
	int colorWidth = screenSize.getWidth() * 0.015;

	int titleHeight = screenSize.getHeight() * 0.0225;

	constexpr int colorColumn = 6;
	constexpr int colorHistoryRow = 5;

	int colorTotalWidth = colorColumn * colorWidth + (colorColumn - 1) * splitWidth;
	int colorTotalHeight = titleHeight * 2 + colorHeight * (2 + colorHistoryRow) + splitHeight * (4 + colorHistoryRow - 1);

	int colorSpaceHeight = colorTotalHeight;
	int colorSpaceWidth = colorSpaceHeight;

	int hueSelectorHeight = colorSpaceHeight;
	int hueSelectorWidth = screenSize.getWidth() * 0.01;

	int valueEditorHeight = screenSize.getHeight() * 0.025;
	int colorViewerHeight = valueEditorHeight;

	int buttonPaddingWidth = screenSize.getWidth() * 0.025;
	int buttonPaddingHeight = screenSize.getHeight() * 0.015;
	int buttonWidth = screenSize.getWidth() * 0.075;

	/** Color Space */
	juce::Rectangle<int> colorSpaceRect(
		padddingWidth * 3 + colorTotalWidth, paddingHeight,
		colorSpaceWidth, colorSpaceHeight);
	this->colorSpace->setBounds(colorSpaceRect);

	/** Hue Selector */
	juce::Rectangle<int> hueSelectorRect(
		colorSpaceRect.getRight() + splitWidth, colorSpaceRect.getY(),
		hueSelectorWidth, hueSelectorHeight);
	this->hueSelector->setBounds(hueSelectorRect);

	/** Value Editor */
	int valueEditorWidth = this->getWidth() - padddingWidth - (hueSelectorRect.getRight() + splitWidth);
	
	juce::Rectangle<int> rEditorRect(
		hueSelectorRect.getRight() + splitWidth,
		paddingHeight + titleHeight,
		valueEditorWidth, valueEditorHeight);
	this->rEditor->setBounds(rEditorRect);

	juce::Rectangle<int> gEditorRect(
		hueSelectorRect.getRight() + splitWidth,
		rEditorRect.getBottom() + splitHeight + titleHeight,
		valueEditorWidth, valueEditorHeight);
	this->gEditor->setBounds(gEditorRect);

	juce::Rectangle<int> bEditorRect(
		hueSelectorRect.getRight() + splitWidth,
		gEditorRect.getBottom() + splitHeight + titleHeight,
		valueEditorWidth, valueEditorHeight);
	this->bEditor->setBounds(bEditorRect);

	juce::Rectangle<int> hexEditorRect(
		hueSelectorRect.getRight() + splitWidth,
		bEditorRect.getBottom() + splitHeight + titleHeight,
		valueEditorWidth, valueEditorHeight);
	this->hexEditor->setBounds(hexEditorRect);

	/** Color Viewer */
	int colorViewerWidth = valueEditorWidth;
	juce::Rectangle<int> colorViewerRect(
		hueSelectorRect.getRight() + splitWidth,
		hexEditorRect.getBottom() + splitHeight,
		colorViewerWidth, colorViewerHeight);
	this->colorViewer->setBounds(colorViewerRect);

	/** OK Button */
	int buttonAreaPosY = paddingHeight + colorTotalHeight;
	juce::Rectangle<int> okButtonRect(
		this->getWidth() - buttonPaddingWidth - buttonWidth,
		buttonAreaPosY + buttonPaddingHeight,
		buttonWidth,
		this->getHeight() - buttonAreaPosY - buttonPaddingHeight * 2);
	this->okButton->setBounds(okButtonRect);
}

void ColorEditorContent::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.015;
	int padddingWidth = screenSize.getWidth() * 0.01;
	int splitHeight = screenSize.getHeight() * 0.0075;
	int splitWidth = screenSize.getWidth() * 0.005;

	int colorHeight = screenSize.getHeight() * 0.02;
	int colorWidth = screenSize.getWidth() * 0.015;

	float colorCorner = screenSize.getHeight() * 0.0025;

	int titleHeight = screenSize.getHeight() * 0.0225;
	float titleFontHeight = screenSize.getHeight() * 0.02;

	float splitLineThickness = screenSize.getWidth() * 0.0005;

	constexpr int colorColumn = 6;
	constexpr int colorHistoryRow = 5;

	int colorTotalWidth = colorColumn * colorWidth + (colorColumn - 1) * splitWidth;
	int colorTotalHeight = titleHeight * 2 + colorHeight * (2 + colorHistoryRow) + splitHeight * (4 + colorHistoryRow - 1);

	int colorSpaceHeight = colorTotalHeight;
	int colorSpaceWidth = colorSpaceHeight;

	int hueSelectorHeight = colorSpaceHeight;
	int hueSelectorWidth = screenSize.getWidth() * 0.01;

	int valueEditorHeight = screenSize.getHeight() * 0.025;
	int colorViewerHeight = valueEditorHeight;

	/** Color */
	auto& laf = this->getLookAndFeel();
	juce::Colour backgroundColor = laf.findColour(
		juce::Label::ColourIds::backgroundColourId);
	juce::Colour titleColor = laf.findColour(
		juce::Label::ColourIds::textColourId);
	juce::Colour splitLineColor = laf.findColour(
		juce::Label::ColourIds::outlineColourId);

	/** Font */
	juce::Font titleFont(titleFontHeight);

	/** Background */
	g.setColour(backgroundColor);
	g.fillAll();

	/** Common Title */
	juce::Rectangle<int> commonTitleRect(
		padddingWidth, paddingHeight,
		colorTotalWidth, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->commonTitle, commonTitleRect,
		juce::Justification::centredLeft, 1, 0.f);

	/** Theme Color */
	int themeColorNum = std::min(this->themeColors.size(), colorColumn);
	for (int i = 0; i < themeColorNum; i++) {
		juce::Rectangle<int> colorRect(
			padddingWidth + (colorWidth + splitWidth) * i,
			commonTitleRect.getBottom() + splitHeight,
			colorWidth, colorHeight);
		g.setColour(this->themeColors[i]);
		g.fillRoundedRectangle(colorRect.toFloat(), colorCorner);
	}

	/** Theme Alert Color */
	int themeAlertColorNum = std::min(this->themeAlertColors.size(), colorColumn);
	for (int i = 0; i < themeAlertColorNum; i++) {
		juce::Rectangle<int> colorRect(
			padddingWidth + (colorWidth + splitWidth) * i,
			commonTitleRect.getBottom() + splitHeight * 2 + colorHeight,
			colorWidth, colorHeight);
		g.setColour(this->themeAlertColors[i]);
		g.fillRoundedRectangle(colorRect.toFloat(), colorCorner);
	}

	/** History Title */
	juce::Rectangle<int> historyTitleRect(
		padddingWidth,
		commonTitleRect.getBottom() + splitHeight * 3 + colorHeight * 2,
		colorTotalWidth, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->historyTitle, historyTitleRect,
		juce::Justification::centredLeft, 1, 0.f);

	/** History Color */
	for (int i = 0; i < colorHistoryRow; i++) {
		for (int j = 0; j < colorColumn; j++) {
			int historyIndex = i * colorColumn + j;
			if (historyIndex < this->historyList.size()) {
				juce::Rectangle<int> colorRect(
					padddingWidth + (colorWidth + splitWidth) * j,
					historyTitleRect.getBottom() + splitHeight + (splitHeight + colorHeight) * i,
					colorWidth, colorHeight);
				g.setColour(this->historyList[historyIndex]);
				g.fillRoundedRectangle(colorRect.toFloat(), colorCorner);
			}
		}
	}

	/** Split Line */
	juce::Rectangle<float> splitLineRect(
		padddingWidth + colorTotalWidth + padddingWidth - splitLineThickness / 2,
		paddingHeight, splitLineThickness, colorTotalHeight);
	g.setColour(splitLineColor);
	g.fillRect(splitLineRect);

	/** Value Editor Title */
	int valueTitlePosX = padddingWidth * 3 + colorTotalWidth + colorSpaceWidth + hueSelectorWidth + splitWidth * 2;
	int valueTitleWidth = this->getWidth() - padddingWidth - valueTitlePosX;

	juce::Rectangle<int> rTitleRect(
		valueTitlePosX, paddingHeight,
		valueTitleWidth, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->rTitle, rTitleRect,
		juce::Justification::centredLeft, 1, 0.f);

	juce::Rectangle<int> gTitleRect(
		valueTitlePosX,
		rTitleRect.getBottom() + valueEditorHeight + splitHeight,
		valueTitleWidth, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->gTitle, gTitleRect,
		juce::Justification::centredLeft, 1, 0.f);

	juce::Rectangle<int> bTitleRect(
		valueTitlePosX,
		gTitleRect.getBottom() + valueEditorHeight + splitHeight,
		valueTitleWidth, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->bTitle, bTitleRect,
		juce::Justification::centredLeft, 1, 0.f);

	juce::Rectangle<int> hexTitleRect(
		valueTitlePosX,
		bTitleRect.getBottom() + valueEditorHeight + splitHeight,
		valueTitleWidth, titleHeight);
	g.setColour(titleColor);
	g.setFont(titleFont);
	g.drawFittedText(this->hexTitle, hexTitleRect,
		juce::Justification::centredLeft, 1, 0.f);
}

void ColorEditorContent::mouseDrag(const juce::MouseEvent& event) {
	this->mouseMove(event);
}

void ColorEditorContent::mouseMove(const juce::MouseEvent& event) {
	auto pos = event.getPosition();

	int themeIndex = this->getThemeColorIndex(pos);
	if (themeIndex > -1) {
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
		return;
	}

	int themeAlertIndex = this->getThemeAlertColorIndex(pos);
	if (themeAlertIndex > -1) {
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
		return;
	}

	int historyIndex = this->getHistoryColorIndex(pos);
	if (historyIndex > -1) {
		this->setMouseCursor(juce::MouseCursor::PointingHandCursor);
		return;
	}

	this->setMouseCursor(juce::MouseCursor::NormalCursor);
}

void ColorEditorContent::mouseUp(const juce::MouseEvent& event) {
	auto pos = event.getPosition();

	int themeIndex = this->getThemeColorIndex(pos);
	if (themeIndex > -1) {
		auto color = this->themeColors[themeIndex];
		this->selectColor(
			color.getHue(), color.getSaturation(), color.getBrightness());
		return;
	}

	int themeAlertIndex = this->getThemeAlertColorIndex(pos);
	if (themeAlertIndex > -1) {
		auto color = this->themeAlertColors[themeAlertIndex];
		this->selectColor(
			color.getHue(), color.getSaturation(), color.getBrightness());
		return;
	}

	int historyIndex = this->getHistoryColorIndex(pos);
	if (historyIndex > -1) {
		auto color = this->historyList[historyIndex];
		this->selectColor(
			color.getHue(), color.getSaturation(), color.getBrightness());
		return;
	}
}

void ColorEditorContent::selectColor(
	float hue, float sat, float bri) {
	this->updateColor(juce::Colour::fromHSV(hue, sat, bri, 1.f));
}

ColorEditorContent::ColourSpaceView::ColourSpaceView(
	ColorEditorContent* parent) : parent(parent) {
	this->setWantsKeyboardFocus(true);
	this->setMouseCursor(juce::MouseCursor::CrosshairCursor);

	/** Marker */
	this->addAndMakeVisible(this->marker);
}

void ColorEditorContent::ColourSpaceView::setCurrentColor(
	float hue, float sat, float bri) {
	float lastHue = this->h;

	this->h = hue;
	this->s = sat;
	this->b = bri;

	if (lastHue != this->h) {
		this->colours = juce::Image{};
		this->repaint();
	}

	this->updateMarker();
}

void ColorEditorContent::ColourSpaceView::paint(juce::Graphics& g) {
	if (this->colours.isNull()) {
		auto width = getWidth() / 2;
		auto height = getHeight() / 2;
		this->colours = juce::Image{
			juce::Image::RGB, width, height, false };

		juce::Image::BitmapData pixels(
			this->colours, juce::Image::BitmapData::writeOnly);

		for (int y = 0; y < height; ++y) {
			auto val = 1.0f - (float)y / (float)height;

			for (int x = 0; x < width; ++x) {
				auto sat = (float)x / (float)width;
				pixels.setPixelColour(
					x, y, juce::Colour{ h, sat, val, 1.0f });
			}
		}
	}

	g.setOpacity(1.0f);
	g.drawImageTransformed(this->colours,
		juce::RectanglePlacement{juce::RectanglePlacement::stretchToFit}
			.getTransformToFit(this->colours.getBounds().toFloat(),
				this->getLocalBounds().toFloat()), false);
}

void ColorEditorContent::ColourSpaceView::mouseDown(
	const juce::MouseEvent& e) {
	this->mouseDrag(e);
}

void ColorEditorContent::ColourSpaceView::mouseDrag(
	const juce::MouseEvent& e) {
	auto posX = e.x;
	auto posY = e.y;

	if (posX < 0) { posX = 0; }
	if (posX > this->getWidth()) { posX = this->getWidth(); }
	if (posY < 0) { posY = 0; }
	if (posY > this->getHeight()) { posY = this->getHeight(); }

	auto sat = (float)posX / (float)this->getWidth();
	auto val = 1.0f - (float)posY / (float)this->getHeight();

	this->parent->selectColor(this->h, sat, val);
}

void ColorEditorContent::ColourSpaceView::resized() {
	this->colours = juce::Image{};
	this->marker.setBounds(this->getLocalBounds());
	this->updateMarker();
}

void ColorEditorContent::ColourSpaceView::updateMarker() {
	this->marker.setPos(this->s, 1.0f - this->b);
}

ColorEditorContent::ColourSpaceView::ColourSpaceMarker::ColourSpaceMarker() {
	this->setInterceptsMouseClicks(false, false);
}

void ColorEditorContent::ColourSpaceView::ColourSpaceMarker::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float posX = this->pX * this->getWidth();
	float posY = this->pY * this->getHeight();

	float cursorWidth = screenSize.getWidth() * 0.005;
	float cursorLineThickness = screenSize.getWidth() * 0.0025;
	float cursorBorderThickness = screenSize.getWidth() * 0.001;

	/** Color */
	juce::Colour cursorColor = juce::Colours::white;
	juce::Colour cursorLineColor = juce::Colours::black;

	/** Cursor Body */
	juce::Rectangle<float> rectCursor(
		posX - cursorWidth / 2, posY - cursorWidth / 2,
		cursorWidth, cursorWidth);
	g.setColour(cursorColor);
	g.drawEllipse(rectCursor, cursorLineThickness);

	/** Cursor Border */
	juce::Rectangle<float> rectCursorBorderInside(
		posX - cursorWidth / 2 + cursorLineThickness / 2,
		posY - cursorWidth / 2 + cursorLineThickness / 2,
		cursorWidth - cursorLineThickness, cursorWidth - cursorLineThickness);
	juce::Rectangle<float> rectCursorBorderOutside(
		posX - cursorWidth / 2 - cursorLineThickness / 2,
		posY - cursorWidth / 2 - cursorLineThickness / 2,
		cursorWidth + cursorLineThickness, cursorWidth + cursorLineThickness);
	g.setColour(cursorLineColor);
	g.drawEllipse(rectCursorBorderInside, cursorBorderThickness);
	g.drawEllipse(rectCursorBorderOutside, cursorBorderThickness);
}

void ColorEditorContent::ColourSpaceView::ColourSpaceMarker::setPos(float pX, float pY) {
	this->pX = pX;
	this->pY = pY;
	this->repaint();
}

ColorEditorContent::HueSelectorComp::HueSelectorComp(
	ColorEditorContent* parent) : parent(parent) {
	this->setWantsKeyboardFocus(true);
	this->setMouseCursor(juce::MouseCursor::PointingHandCursor);

	/** Marker */
	this->addAndMakeVisible(this->marker);
}

void ColorEditorContent::HueSelectorComp::setCurrentColor(
	float hue, float sat, float bri) {
	this->h = hue;
	this->s = sat;
	this->b = bri;

	this->repaint();
	this->updateMarker();
}

void ColorEditorContent::HueSelectorComp::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float cornerSize = screenSize.getHeight() * 0.005;

	/** Gradient */
	juce::ColourGradient cg;
	cg.isRadial = false;
	cg.point1.setXY(0.0f, (float)0);
	cg.point2.setXY(0.0f, (float)this->getHeight());

	for (float i = 0.0f; i <= 1.0f; i += 0.02f) {
		cg.addColour(i, juce::Colour{ i, 1.0f, 1.0f, 1.0f });
	}

	g.setGradientFill(cg);
	g.fillRoundedRectangle(
		this->getLocalBounds().toFloat(), cornerSize);
}

void ColorEditorContent::HueSelectorComp::resized() {
	this->marker.setBounds(this->getLocalBounds());
	this->updateMarker();
}

void ColorEditorContent::HueSelectorComp::mouseDown(
	const juce::MouseEvent& e) {
	this->mouseDrag(e);
}

void ColorEditorContent::HueSelectorComp::mouseDrag(
	const juce::MouseEvent& e) {
	auto posY = e.y;

	if (posY < 0) { posY = 0; }
	if (posY > this->getHeight()) { posY = this->getHeight(); }

	auto hue = (float)posY / (float)this->getHeight();
	this->parent->selectColor(hue, this->s, this->b);
}

void ColorEditorContent::HueSelectorComp::updateMarker() {
	this->marker.setPos(this->h);
}

ColorEditorContent::HueSelectorComp::HueSelectorMarker::HueSelectorMarker() {
	this->setInterceptsMouseClicks(false, false);
}

void ColorEditorContent::HueSelectorComp::HueSelectorMarker::paint(juce::Graphics& g) {
	/** Size */
	auto screenSize = utils::getScreenSize(this);

	float posX = 0.5 * this->getWidth();
	float posY = this->pY * this->getHeight();

	float cursorLineThickness = screenSize.getWidth() * 0.0025;
	float cursorBorderThickness = screenSize.getWidth() * 0.001;
	float cursorWidth = this->getWidth() - cursorLineThickness - cursorBorderThickness;

	/** Color */
	juce::Colour cursorColor = juce::Colours::white;
	juce::Colour cursorLineColor = juce::Colours::black;

	/** Cursor Body */
	juce::Rectangle<float> rectCursor(
		posX - cursorWidth / 2, posY - cursorWidth / 2,
		cursorWidth, cursorWidth);
	g.setColour(cursorColor);
	g.drawEllipse(rectCursor, cursorLineThickness);

	/** Cursor Border */
	juce::Rectangle<float> rectCursorBorderInside(
		posX - cursorWidth / 2 + cursorLineThickness / 2,
		posY - cursorWidth / 2 + cursorLineThickness / 2,
		cursorWidth - cursorLineThickness, cursorWidth - cursorLineThickness);
	juce::Rectangle<float> rectCursorBorderOutside(
		posX - cursorWidth / 2 - cursorLineThickness / 2,
		posY - cursorWidth / 2 - cursorLineThickness / 2,
		cursorWidth + cursorLineThickness, cursorWidth + cursorLineThickness);
	g.setColour(cursorLineColor);
	g.drawEllipse(rectCursorBorderInside, cursorBorderThickness);
	g.drawEllipse(rectCursorBorderOutside, cursorBorderThickness);
}

void ColorEditorContent::HueSelectorComp::HueSelectorMarker::setPos(float pY) {
	this->pY = pY;
	this->repaint();
}

void ColorEditorContent::ColorViewer::setCurrentColor(float hue, float sat, float bri) {
	this->hue = hue;
	this->sat = sat;
	this->bri = bri;
	this->repaint();
}

void ColorEditorContent::ColorViewer::paint(juce::Graphics& g) {
	g.fillAll(juce::Colour::fromHSV(
		this->hue, this->sat, this->bri, 1.f));
}

void ColorEditorContent::updateColor(
	const juce::Colour& color, bool updateRGB, bool updateHex) {
	this->color = color;
	this->updateComponents(updateRGB, updateHex);
}

void ColorEditorContent::updateComponents(bool updateRGB, bool updateHex) {
	float hue = this->color.getHue();
	float sat = this->color.getSaturation();
	float bri = this->color.getBrightness();

	this->colorSpace->setCurrentColor(hue, sat, bri);
	this->hueSelector->setCurrentColor(hue, sat, bri);
	this->colorViewer->setCurrentColor(hue, sat, bri);

	if (updateRGB) {
		this->rEditor->setText(juce::String{ this->color.getRed() }, false);
		this->gEditor->setText(juce::String{ this->color.getGreen() }, false);
		this->bEditor->setText(juce::String{ this->color.getBlue() }, false);
	}

	if (updateHex) {
		this->hexEditor->setText(this->color.toDisplayString(false), false);
	}
}

int ColorEditorContent::getThemeColorIndex(
	const juce::Point<int>& pos) const {
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.015;
	int padddingWidth = screenSize.getWidth() * 0.01;
	int splitHeight = screenSize.getHeight() * 0.0075;
	int splitWidth = screenSize.getWidth() * 0.005;

	int colorHeight = screenSize.getHeight() * 0.02;
	int colorWidth = screenSize.getWidth() * 0.015;

	int titleHeight = screenSize.getHeight() * 0.0225;

	constexpr int colorColumn = 6;
	constexpr int colorHistoryRow = 5;

	int colorNum = std::min(this->themeColors.size(), colorColumn);
	for (int i = 0; i < colorNum; i++) {
		juce::Rectangle<int> colorRect(
			padddingWidth + (colorWidth + splitWidth) * i,
			paddingHeight + titleHeight + splitHeight,
			colorWidth, colorHeight);
		if (colorRect.contains(pos)) {
			return i;
		}
	}

	return -1;
}

int ColorEditorContent::getThemeAlertColorIndex(
	const juce::Point<int>& pos) const {
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.015;
	int padddingWidth = screenSize.getWidth() * 0.01;
	int splitHeight = screenSize.getHeight() * 0.0075;
	int splitWidth = screenSize.getWidth() * 0.005;

	int colorHeight = screenSize.getHeight() * 0.02;
	int colorWidth = screenSize.getWidth() * 0.015;

	int titleHeight = screenSize.getHeight() * 0.0225;

	constexpr int colorColumn = 6;
	constexpr int colorHistoryRow = 5;

	int colorNum = std::min(this->themeAlertColors.size(), colorColumn);
	for (int i = 0; i < colorNum; i++) {
		juce::Rectangle<int> colorRect(
			padddingWidth + (colorWidth + splitWidth) * i,
			paddingHeight + titleHeight + splitHeight * 2 + colorHeight,
			colorWidth, colorHeight);
		if (colorRect.contains(pos)) {
			return i;
		}
	}

	return -1;
}

int ColorEditorContent::getHistoryColorIndex(
	const juce::Point<int>& pos) const {
	auto screenSize = utils::getScreenSize(this);
	int paddingHeight = screenSize.getHeight() * 0.015;
	int padddingWidth = screenSize.getWidth() * 0.01;
	int splitHeight = screenSize.getHeight() * 0.0075;
	int splitWidth = screenSize.getWidth() * 0.005;

	int colorHeight = screenSize.getHeight() * 0.02;
	int colorWidth = screenSize.getWidth() * 0.015;

	int titleHeight = screenSize.getHeight() * 0.0225;

	constexpr int colorColumn = 6;
	constexpr int colorHistoryRow = 5;

	int colorNum = this->historyList.size();
	for (int i = 0; i < colorHistoryRow; i++) {
		for (int j = 0; j < colorColumn; j++) {
			int index = i * colorColumn + j;
			if (index < colorNum) {
				juce::Rectangle<int> colorRect(
					padddingWidth + (colorWidth + splitWidth) * j,
					paddingHeight + titleHeight * 2 + splitHeight * 4 + colorHeight * 2 + (splitHeight + colorHeight) * i,
					colorWidth, colorHeight);
				if (colorRect.contains(pos)) {
					return index;
				}
			}
		}
	}

	return -1;
}

void ColorEditorContent::rgbValueChanged() {
	int r = this->rEditor->getText().getIntValue();
	int g = this->gEditor->getText().getIntValue();
	int b = this->bEditor->getText().getIntValue();

	this->updateColor(juce::Colour::fromRGB(r, g, b), false, true);
}

void ColorEditorContent::hexValueChanged() {
	auto color = juce::Colour::fromString(this->hexEditor->getText()).withAlpha(1.f);
	this->updateColor(color, true, false);
}

void ColorEditorContent::okButtonPressed() {
	ColorHistory::getInstance()->add(this->color);
	this->callback(this->color);
	this->parent->closeButtonPressed();
}

ColorEditor::ColorEditor(const Callback& callback,
	const juce::Colour& defaultColor)
	: DocumentWindow(TRANS("Color Editor"), juce::LookAndFeel::getDefaultLookAndFeel().findColour(
		juce::ResizableWindow::ColourIds::backgroundColourId),
		juce::DocumentWindow::closeButton, true) {
	this->setUsingNativeTitleBar(true);

	/** Icon */
	auto icon = RCManager::getInstance()->loadImage(
		utils::getResourceFile("logo.png"));
	this->setIcon(icon);
	this->getPeer()->setIcon(icon);

	/** Content */
	this->setContentOwned(
		new ColorEditorContent{ this, callback, defaultColor }, false);

	/** Size */
	juce::MessageManager::callAsync(
		[comp = juce::Component::SafePointer(this)] {
			if (comp) {
				auto screenSize = utils::getScreenSize(comp);
				comp->centreWithSize(screenSize.getWidth() * 0.335,
					screenSize.getHeight() * 0.325);
			}
		}
	);
}

void ColorEditor::closeButtonPressed() {
	this->exitModalState();
}
