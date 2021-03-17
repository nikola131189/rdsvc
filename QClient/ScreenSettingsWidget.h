#pragma once

#include <QWidget>
#include "ui_ScreenSettingsWidget.h"
#include <rd/rd.h>
#include "widgets/StringListWidget.h"
#include "widgets/SliderLabelWidget.h"
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QPointer>
#include "widgets/DialogBase.h"

class ScreenSettingsWidget : public DialogBase
{
	Q_OBJECT

public:
	ScreenSettingsWidget(const Rd::ScreenInfo& scr, QWidget *parent = Q_NULLPTR);
	~ScreenSettingsWidget();
	bool isDisableInput() { return _disableInput; }
	bool isSpyMode() { return _spyMode; }
	bool isClipboardSync() { return _clipboardSync; }
	bool isCursorVisible() { return _cursorVisible; }
	codec::VideoFormat getVideoFormat() { return _fmt; }
signals:
	void paramChanged(const codec::VideoFormat& fmt);
private:
	void setScreenInfo(const Rd::ScreenInfo& scr);
	void showEvent(QShowEvent* event);
	void loadState();
	void setState();
	void resolutionFromString(const std::string& in, int& w, int& h);
private:




	Rd::ScreenInfo _screenInfo;
	codec::VideoFormat _fmt;
	bool _spyMode;
	bool _disableInput;
	bool _clipboardSync;
	int _resolution;
	bool _cursorVisible;



	StringListWidget* _resolutionWidget;
	StringListWidget* _codecIdWidget;
	StringListWidget* _profileWidget;
	StringListWidget* _threadCountWidget;
	SliderLabelWidget* _bitrateWidget;
	SliderLabelWidget* _quantizationWidget;
	QCheckBox* _spyModeWidget;
	QCheckBox* _disableInputWidget;
	QCheckBox* _clipboardSyncWidget;
	QCheckBox* _cursorVisibleWidget;

	QPointer<QGridLayout> l1;
	QPointer<QHBoxLayout> l2, l3;
	QPointer<QVBoxLayout> l5;
private:
	Ui::ScreenSettingsWidget ui;

};
