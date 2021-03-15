#include "AbstractDialogWidget.h"

AbstractDialogWidget::AbstractDialogWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

}

AbstractDialogWidget::~AbstractDialogWidget()
{
}



void AbstractDialogWidget::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);
	p.fillRect(rect(), QWidget::palette().color(QWidget::backgroundRole()));
}
