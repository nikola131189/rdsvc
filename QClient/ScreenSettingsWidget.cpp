#include "ScreenSettingsWidget.h"

ScreenSettingsWidget::ScreenSettingsWidget(const Rd::ScreenInfo& scr, QWidget *parent)
	: _screenInfo(scr),
	AbstractDialogWidget(parent),
	_disableInput(false),
	_clipboardSync(true),
	_spyMode(false),
	_cursorVisible(false)
{

	std::sort(_screenInfo.resolutions.begin(), _screenInfo.resolutions.end(),
		[this](Rd::Resolution r1, Rd::Resolution r2) {
			if (r1.width == r2.width)
				return r1.height > r2.height;
			return r1.width > r2.width;
		});


	ui.setupUi(this);

	QWidget* w1 = new QWidget(this);
	l1 = new QGridLayout(this);
	w1->setLayout(l1);
	{
		_resolutionWidget = new StringListWidget(QStringList(), "resolution", this);
		_resolutionWidget->setMinimumWidth(150);
		l1->addWidget(_resolutionWidget, 0, 0);
		setScreenInfo(_screenInfo);
	}

	{
		_codecIdWidget = new StringListWidget({"zstd", "vp8", "h264", "mpeg1", "mpeg2", "mpeg4" }, "codec id", this);
		_codecIdWidget->setMinimumWidth(150);
		l1->addWidget(_codecIdWidget, 0, 1);
		
	}

	{
		_profileWidget = new StringListWidget({ "1", "2", "3", "4", "5", "6", "7", "8",
			 "9", "10", "11", "12", "13", "14", "15", "16",
			 "17", "18", "19", "20", "21", "22" }, "profile", this);

		_profileWidget->setMinimumWidth(150);
		_profileWidget->selectRow(0);
		l1->addWidget(_profileWidget, 0, 2);
	}

	{
		_threadCountWidget = new StringListWidget({ "1", "2", "3", "4", "5", "6", "7", "8", 
			"9", "10", "11", "12", "13", "14", "15", "16" }, "thread count", this);

		_threadCountWidget->setMinimumWidth(150);
		_threadCountWidget->selectRow(0);
		l1->addWidget(_threadCountWidget, 0, 3);
	}
	
	
	{
		_bitrateWidget = new SliderLabelWidget("bitrate", 0, 10000, Qt::Vertical, this);
		l1->addWidget(_bitrateWidget, 0, 4);
	}

	{
		_quantizationWidget = new SliderLabelWidget("quant", 0, 100, Qt::Vertical, this);
		l1->addWidget(_quantizationWidget, 0, 5);
	}

	QWidget* w2 = new QWidget(this);
	l2 = new QHBoxLayout(this);
	w2->setLayout(l2);
	{
		_spyModeWidget = new QCheckBox("spy mode", this);
		_disableInputWidget = new QCheckBox("disable input", this);
		_clipboardSyncWidget = new QCheckBox("clipboard sync", this);
		_cursorVisibleWidget = new QCheckBox("cursor visible", this);

		l2->addWidget(_spyModeWidget);
		l2->addWidget(_disableInputWidget);
		l2->addWidget(_clipboardSyncWidget);
		l2->addWidget(_cursorVisibleWidget);
		l2->setAlignment(Qt::AlignLeft);
	}




	l3 = new QHBoxLayout(this);
	QWidget* w3 = new QWidget(this);
	{
		QPushButton* btn = new QPushButton("ok", this);
		QObject::connect(btn, &QPushButton::clicked, [&](){
			setState();
			paramChanged(_fmt);
			hideDialog();
		});



		QPushButton* btn2 = new QPushButton("apply", this);
		QObject::connect(btn2, &QPushButton::clicked, [&](){
			setState();
			paramChanged(_fmt);
		});



		QPushButton* btn3 = new QPushButton("cancel", this);
		QObject::connect(btn3, &QPushButton::clicked, [&](){
			hideDialog();
		});



		l3->addWidget(btn);
		l3->addWidget(btn2);
		l3->addWidget(btn3);
		//l3->setAlignment(Qt::AlignLeft);
		w3->setLayout(l3);
	}



	_fmt.id = codec::VideoFormat::ZSTD;
	_fmt.profile = 1;
	_fmt.bitarte = 3000;
	_fmt.quantization = 18;
	_fmt.threadCount = 4;


	QLabel* title = new QLabel("Screen settings", this);
	title->setAlignment(Qt::AlignCenter);

	l5 = new QVBoxLayout(this);
	l5->addWidget(title);
	l5->addWidget(w1);
	l5->addWidget(w2);
	l5->addWidget(w3);

	//l1->setRowMinimumHeight(0, 250);
	
	setLayout(l5);

	this->setMinimumWidth(800);
	this->setMinimumHeight(500);
}

ScreenSettingsWidget::~ScreenSettingsWidget()
{
}




void ScreenSettingsWidget::setScreenInfo(const Rd::ScreenInfo& scr)
{
	int n = 0, i = 0;
	QStringList resolutions;
	for (auto& r : scr.resolutions)
	{
		std::stringstream s;
		s << r.width << " x " << r.height;
		resolutions.push_back(s.str().c_str());

		if (r.width == scr.current.width && r.height == scr.current.height)
		{
			n = i;
		}
		i++;
	}
	_resolutionWidget->setData(resolutions);
	_resolutionWidget->selectRow(n);
	_resolution = n;
}






void ScreenSettingsWidget::resolutionFromString(const std::string& in, int& w, int& h)
{
	if (in.empty()) return;
	char* str = (char*)in.c_str();
	const char* sep = " x ";
	char* istr;
	istr = strtok(str, sep);
	w = std::atoi(istr);
	istr = strtok(NULL, sep);
	h = std::atoi(istr);
}




void ScreenSettingsWidget::setState()
{
	{
		resolutionFromString(_resolutionWidget->value(_resolutionWidget->selectedRow()).toStdString(),_fmt.w, _fmt.h);
	}

	{
		/*QString val = _profileWidget->value(_profileWidget->selectedRow());
		_fmt.profile = val.toInt();*/
	}
	_fmt.bitarte = _bitrateWidget->value();
	_fmt.quantization = _quantizationWidget->value();
	_fmt.threadCount = _threadCountWidget->value(_threadCountWidget->selectedRow()).toUInt();
	_fmt.profile = _profileWidget->value(_profileWidget->selectedRow()).toUInt();


	_spyMode = _spyModeWidget->isChecked();
	_resolution = _resolutionWidget->selectedRow();
	_disableInput = _disableInputWidget->isChecked();
	_clipboardSync = _clipboardSyncWidget->isChecked();
	_cursorVisible = _cursorVisibleWidget->isChecked();
	
	if (_codecIdWidget->value(_codecIdWidget->selectedRow()) == "vp8")
		_fmt.id = codec::VideoFormat::VP8;

	if (_codecIdWidget->value(_codecIdWidget->selectedRow()) == "zstd")
		_fmt.id = codec::VideoFormat::ZSTD;

	if (_codecIdWidget->value(_codecIdWidget->selectedRow()) == "h264")
		_fmt.id = codec::VideoFormat::H264;

	if (_codecIdWidget->value(_codecIdWidget->selectedRow()) == "mpeg1")
		_fmt.id = codec::VideoFormat::MPEG1;

	if (_codecIdWidget->value(_codecIdWidget->selectedRow()) == "mpeg2")
		_fmt.id = codec::VideoFormat::MPEG2;

	if (_codecIdWidget->value(_codecIdWidget->selectedRow()) == "mpeg4")
		_fmt.id = codec::VideoFormat::MPEG4;

}


void ScreenSettingsWidget::loadState()
{
	_profileWidget->selectRow(_fmt.profile - 1);
	_threadCountWidget->selectRow(_fmt.threadCount - 1);
	_quantizationWidget->setValue(_fmt.quantization);
	_bitrateWidget->setValue(_fmt.bitarte);
	_spyModeWidget->setChecked(_spyMode);
	_resolutionWidget->selectRow(_resolution);
	_disableInputWidget->setChecked(_disableInput);
	_clipboardSyncWidget->setChecked(_clipboardSync);
	_cursorVisibleWidget->setChecked(_cursorVisible);

	if (_fmt.id == codec::VideoFormat::ZSTD)
		_codecIdWidget->selectRow(0);
	if (_fmt.id == codec::VideoFormat::VP8)
		_codecIdWidget->selectRow(1);
	if (_fmt.id == codec::VideoFormat::H264)
		_codecIdWidget->selectRow(2);
	if (_fmt.id == codec::VideoFormat::MPEG1)
		_codecIdWidget->selectRow(3);
	if (_fmt.id == codec::VideoFormat::MPEG2)
		_codecIdWidget->selectRow(4);
	if (_fmt.id == codec::VideoFormat::MPEG4)
		_codecIdWidget->selectRow(5);
}


void ScreenSettingsWidget::showEvent(QShowEvent* event)
{
	loadState();
	AbstractDialogWidget::showEvent(event);
}