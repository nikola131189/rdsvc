#include "QClient.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <thread>


#include <rd/rd.h>

Q_DECLARE_METATYPE(Rd::FilesEvent)
Q_DECLARE_METATYPE(Rd::ClientsEvent)
Q_DECLARE_METATYPE(Rd::TransferEvent)
Q_DECLARE_METATYPE(Rd::DisplayEvent)
Q_DECLARE_METATYPE(Rd::ScreenInfo)
Q_DECLARE_METATYPE(Rd::CursorInfo)
Q_DECLARE_METATYPE(Rd::TerminalEvent)
Q_DECLARE_METATYPE(Rd::ClipboardEvent)
Q_DECLARE_METATYPE(Rd::ConnectionOpen)
Q_DECLARE_METATYPE(Rd::ConnectionError)


int main(int argc, char *argv[])
{

	//LOG_storage::init("gui.log", 10 * 1024 * 1024);

	//LOG(INFO) << "start";

	setlocale(LC_ALL, ".UTF8");
	qRegisterMetaType<Rd::FilesEvent>();
	qRegisterMetaType<Rd::ClientsEvent>();
	qRegisterMetaType<Rd::TransferEvent>();
	qRegisterMetaType<Rd::DisplayEvent>();
	qRegisterMetaType<Rd::ScreenInfo>();
	qRegisterMetaType<Rd::CursorInfo>();
	qRegisterMetaType<Rd::TerminalEvent>();
	qRegisterMetaType<Rd::ClipboardEvent>();
	qRegisterMetaType<Rd::ConnectionOpen>();
	qRegisterMetaType<Rd::ConnectionError>();

	//new std::thread(networkLoop);

    QApplication a(argc, argv);

	QFile file("styles.css");
	file.open(QIODevice::ReadOnly);
	if (file.isOpen())
	{
		QString m_style = file.readAll();
		a.setStyleSheet(m_style);

	}


    QClient w;
    w.show();
    return a.exec();
}



