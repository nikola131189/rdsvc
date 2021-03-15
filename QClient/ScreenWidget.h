#pragma once

#include "widgets/TabItem.h"
#include "ui_ScreenWidget.h"
#include <rd/rd.h>
#include "ScreenSettingsWidget.h"
#include "ScreenCtrlWidget.h"
#include <rd/codec/CodecFactory.h>
#include <QTimer>
#include <QClipboard>
#include <QMimeData>
#include <QThread>
#include "widgets/GlassWidget.h"

class ScreenWidget :
	public TabItem
{
	Q_OBJECT

public:
	ScreenWidget(const Rd::Client& cl, const Rd::ScreenInfo& scr, QWidget* parent = Q_NULLPTR);
	~ScreenWidget();
signals:
	void setScreen(const QImage& im);
	void setCursor(const Rd::CursorInfo& curs);
	void notifyClipboard(const Rd::ClipboardEvent& ev);
public slots:
	void onClipboardChanged();
	void onNotifyClipboard(const Rd::ClipboardEvent& ev);
private:
	void notifyScreen(const Rd::VideoEvent& ev);
	void showSettings(const QPoint& pos);
	void showEvent(QShowEvent* event);
	void hideEvent(QHideEvent* event);
	void sendOpen(const codec::VideoFormat& f);
	void sendClose();
	void prepareToDestroy();
private:
	Rd::Client _client;
	Rd::ScreenInfo _screenInfo;
	ScreenSettingsWidget* _screenSettings;
	ScreenCtrlWidget* _screenCtrlWidget;

	std::vector<uint8_t> _buffer;
	std::shared_ptr<codec::Decoder> _decoder;
	codec::VideoFormat _fmt;

	bool _clipboardLock = false;
	boost::signals2::connection _screenConn;
	boost::signals2::connection _notifyClipboardConn;
	GlassWidget* _glass;
private:
	Ui::ScreenWidget ui;
};
