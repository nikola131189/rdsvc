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
void networkLoop();

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





void networkLoop()
{
	Rd::Config cfg("C:\\Users\\user\\source\\repos\\rd\\x64\\Release\\config.cfg");

	cfg.name = "";

	Rd::Inet::init(cfg.name, utility::gen_uuid());


	boost::asio::io_context ctx(1);

	bool running = true;
	boost::asio::signal_set sign(ctx, SIGINT, SIGTERM);
	sign.async_wait([&](auto, auto) {
		running = false;
		ctx.stop();
		});


	std::shared_ptr<Net::Connection> conn1 = nullptr, conn2 = nullptr;

	while (running)
	{

		if (!conn1 || utility::get_tick_count() - conn1->lastActive() > cfg.connectionTimeout)
		{
			for (Net::Connector* c : cfg.connectors)
			{
				boost::system::error_code ec;
				auto sock = c->connect(ctx, ec);
				if (!ec)
				{
					conn1 = std::make_shared<Net::Connection>(Rd::Inet::sess1(), std::move(sock), cfg.secret, crypto::RsaEncryptor());
					conn1->start();
					break;
				}
			}
		}


		if (!conn2 || utility::get_tick_count() - conn2->lastActive() > cfg.connectionTimeout)
		{
			for (Net::Connector* c : cfg.connectors)
			{
				boost::system::error_code ec;
				auto sock = c->connect(ctx, ec);
				if (!ec)
				{
					conn2 = std::make_shared<Net::Connection>(Rd::Inet::sess2(), std::move(sock), cfg.secret, crypto::RsaEncryptor());
					conn2->start();
					break;
				}
			}
		}

		ctx.run_for(std::chrono::milliseconds(1000));
	}
}