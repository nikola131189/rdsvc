#include "FilesModels.h"



LocalFilesModel::LocalFilesModel(QObject* p) : AbstractFilesModel(p)
{
	_errDialog = new ErrorDialog("Error", (QWidget *)p);
}

LocalFilesModel::~LocalFilesModel() {}

void LocalFilesModel::dir(const std::filesystem::path& p)
{
	beginResetModel();
	root()->clear();
	endResetModel();

	std::list<files::file_info_t> l;
	std::error_code ec;
	files::dir(p, l, ec);

	checkError(ec);

	beginResetModel();
	for (auto it = l.begin(); it != l.end(); it++)
	{
		root()->emplaceBack(*it);
	}
	endResetModel();

}


void LocalFilesModel::remove(const std::filesystem::path& p)
{
	std::error_code ec;
	std::filesystem::remove_all(p, ec);
	checkError(ec);
}


void LocalFilesModel::rename(const std::filesystem::path& old_name, const std::filesystem::path& new_name)
{
	std::error_code ec;
	std::filesystem::rename(old_name, new_name, ec);
	checkError(ec);
}


void LocalFilesModel::mkdir(const std::filesystem::path& p)
{
	std::error_code ec;
	std::filesystem::create_directories(p, ec);
	checkError(ec);
}


void LocalFilesModel::dirRecursion(const std::filesystem::path& p,
const std::function<void(const std::list<files::file_info_t>&)>& out)
{
	std::list<files::file_info_t> l;
	std::error_code ec;
	files::dir_recursion(p, l, ec);
	checkError(ec);
	out(l);
}

void LocalFilesModel::checkError(const std::error_code& ec)
{
	if (ec) {
		_errDialog->showMessage(QString::fromLocal8Bit(ec.message().c_str()) + ": " + std::to_string(ec.value()).c_str());
		//_glass->show();
	}
}











RemoteFilesModel::RemoteFilesModel(Rd::Client cl, QObject* p) : _client(cl), AbstractFilesModel(p)
{
	_errDialog = new ErrorDialog("Error", (QWidget*)p);
	_id = utility::gen_uuid();
	//Obs::subscribe(this);
	QObject::connect(this, &RemoteFilesModel::notify, this, &RemoteFilesModel::onNotify);
	_conn = EventBus::subscribe<Rd::FilesEvent>([&](const Rd::FilesEvent& e) { notify(e); });
}


RemoteFilesModel::~RemoteFilesModel()
{
	_conn.disconnect();
}

void RemoteFilesModel::dir(const std::filesystem::path& p)
{
	Rd::FilesEvent ev;
	ev.id = _id;
	ev.path1 = p;
	ev.type = Rd::FilesEvent::DIR_REQUEST;
	Rd::Inet::send(_client.id, ev);
	
	beginResetModel();
	root()->clear();
	endResetModel();
}

void RemoteFilesModel::remove(const std::filesystem::path& p)
{
	Rd::FilesEvent ev;
	ev.id = _id;
	ev.path1 = p;
	ev.type = Rd::FilesEvent::REMOVE;
	Rd::Inet::send(_client.id, ev);
}

void RemoteFilesModel::rename(const std::filesystem::path& old_name, const std::filesystem::path& new_name)
{
	Rd::FilesEvent ev;
	ev.id = _id;
	ev.path1 = old_name;
	ev.path2 = new_name;
	ev.type = Rd::FilesEvent::RENAME;
	Rd::Inet::send(_client.id, ev);
}

void RemoteFilesModel::mkdir(const std::filesystem::path& p)
{
	Rd::FilesEvent ev;
	ev.id = _id;
	ev.path1 = p;
	ev.type = Rd::FilesEvent::MAKE_DIR;
	Rd::Inet::send(_client.id, ev);
}

void RemoteFilesModel::dirRecursion(const std::filesystem::path& p,
	const std::function<void(const std::list<files::file_info_t>&)>& out)
{
	_recCbck = out;
	Rd::FilesEvent ev;
	ev.id = _id;
	ev.path1 = p;
	ev.type = Rd::FilesEvent::DIR_RECURSION_REQUEST;
	Rd::Inet::send(_client.id, ev);
}

void RemoteFilesModel::onNotify(const Rd::FilesEvent& e)
{
	if (e.id != _id) return;

	if (e.type == Rd::FilesEvent::DIR_RESPONSE)
	{
		beginResetModel();
		root()->clear();

		for (auto& it : e.fileList)
		{
			root()->emplaceBack(it);
		}
		endResetModel();
	}



	if (e.type == Rd::FilesEvent::DIR_RECURSION_RESPONSE)
	{
		if (_recCbck)
			_recCbck(e.fileList);
	}


	if (e.type == Rd::FilesEvent::ERR)
	{
		_errDialog->showMessage(QString::fromUtf8(e.ec.message.c_str()) + ": " + std::to_string(e.ec.value).c_str());
		//_glass->show();
	}
}
