#pragma once

#include <QWidget>
#include "ui_DialogBase.h"
#include "GlassDialogContainer.h"

class DialogBase : public QWidget
{
	Q_OBJECT

public:
	DialogBase(const QString& title, QWidget *parent = Q_NULLPTR, bool isGlassClickable = true);
	~DialogBase();
	void show();
	void hide();
private:
	Ui::DialogBase ui;
	GlassDialogContainer* _glass;
};
