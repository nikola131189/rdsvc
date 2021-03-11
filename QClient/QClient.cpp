#include "QClient.h"
#include "ScreenWidget.h"
#include "TransferWidget.h"
#include "TerminalWidget.h"

QClient::QClient(QWidget *parent)
    : QMainWindow(parent), 
	_settings("sett.ini", QSettings::IniFormat)
{
    ui.setupUi(this);

	QList<QToolBar*> allToolBars = findChildren<QToolBar*>();
	foreach(QToolBar * tb, allToolBars) {
		removeToolBar(tb);
	}
	delete ui.statusBar;
	delete ui.menuBar;

	restoreGeometry(_settings.value("client/geometry").toByteArray());
	restoreState(_settings.value("client/windowState").toByteArray());

	_transferModel = new TransferModel(this);


	_tabWidget = new TabWidget(this);
	_tabWidget->tabBar()->setMovable(true);
	_tabWidget->setTabsClosable(true);
	//_tabWidget->setElideMode(Qt::ElideRight);
	_tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	_tabWidget->setFocusPolicy(Qt::ClickFocus);

	setCentralWidget(_tabWidget);


	_homeWidget = new HomeWidget(_tabWidget);
	_homeWidget->setState(_settings);
	_tabWidget->addTab(_homeWidget, "home");
	_tabWidget->pin(0);

	QObject::connect(_tabWidget, &QTabWidget::tabCloseRequested, [&](int n) {
		TabItem* w = (TabItem*)_tabWidget->widget(n);
		_tabWidget->removeTab(n);
		w->getState(_settings);
		w->prepareToDestroy();
		QTimer::singleShot(2000, [w]() {w->deleteLater();});

		});

	QObject::connect(_homeWidget->control(), &ClientControlWidget::filesClick, [this](const Rd::Client& cl) {

		FilesTabWidget* w = new FilesTabWidget(cl, _transferModel, this);
		//w->setState(_settings);


		std::stringstream ss;
		ss << "(F:) " << cl.name;
		int i = _tabWidget->addTab(w, ss.str().c_str());
		_tabWidget->setCurrentIndex(i);
	});


	QObject::connect(_homeWidget->control(), &ClientControlWidget::terminalClick, [this](const Rd::Client& cl) {

		TerminalWidget* w = new TerminalWidget(cl, this);
		//w->setState(_settings);


		std::stringstream ss;
		ss << "(T:) " << cl.name;
		int i = _tabWidget->addTab(w, ss.str().c_str());
		_tabWidget->setCurrentIndex(i);
		});


	QObject::connect(_homeWidget->control(), &ClientControlWidget::screenClick, 
		[this](const Rd::Client& cl, Rd::ScreenInfo scr) {

		ScreenWidget* w = new ScreenWidget(cl, scr, this);
		//w->setState(_settings);


		std::stringstream ss;
		ss << "(S:) " << cl.name;
		int i = _tabWidget->addTab(w, ss.str().c_str());
		_tabWidget->setCurrentIndex(i);
		});


	QObject::connect(_homeWidget->control(), &ClientControlWidget::downloadsClick, [this]() {

		TransferWidget* w = new TransferWidget(_transferModel, this);
		//w->setState(_settings);


		std::stringstream ss;
		ss << "downloads";
		int i = _tabWidget->addTab(w, ss.str().c_str());
		_tabWidget->setCurrentIndex(i);
	});

	QObject::connect(_tabWidget, &QTabWidget::customContextMenuRequested, [=](const QPoint& pos) {
		if (_tabWidget->currentIndex() == _tabWidget->tabBar()->tabAt(pos))
		{
			TabItem* w = (TabItem*)_tabWidget->widget(_tabWidget->currentIndex());
			QPoint p = QCursor::pos();
			w->showSettings(p);
		}
		});



	QObject::connect(_homeWidget->control(), &ClientControlWidget::changeClient, [this](const Rd::Client& cl) {
		setWindowTitle(QString::fromUtf8(cl.name.c_str()));
		});


	installEventFilter(_homeWidget->control());
}



void QClient::closeEvent(QCloseEvent* event)
{
	_settings.setValue("client/geometry", saveGeometry());
	_settings.setValue("client/windowState", saveState());
	_homeWidget->getState(_settings);
	
	_settings.sync();

	QMainWindow::closeEvent(event);
}


