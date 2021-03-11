#include "SliderLabelWidget.h"

SliderLabelWidget::SliderLabelWidget(const QString& text, int min, int max,
	Qt::Orientation orientation, QWidget* parent)
	: QWidget(parent),
	_text(text)
{
	ui.setupUi(this);

	_label1 = new QLabel(_text, this);
	_label2 = new QLabel(this);
	_slider = new QSlider(this);
	_slider->setMinimum(min);
	_slider->setMaximum(max);
	_slider->setOrientation(orientation);

	QGridLayout* l = new QGridLayout();
	l->addWidget(_label1, 0, 0, Qt::AlignHCenter);
	l->addWidget(_slider, 1, 0, Qt::AlignHCenter);
	l->addWidget(_label2, 2, 0, Qt::AlignHCenter);

	//l->setAlignment(Qt::AlignHCenter);

	setLayout(l);


	QObject::connect(_slider, &QSlider::valueChanged, [this]() {	
		_label2->setText(std::to_string(_slider->value()).c_str());
		valueChanged(); 
	});
}

SliderLabelWidget::~SliderLabelWidget()
{
}

int SliderLabelWidget::value()
{
	return _slider->value();
}

void SliderLabelWidget::setValue(int v)
{
	_slider->setValue(v);
	_label2->setText(std::to_string(v).c_str());
}


