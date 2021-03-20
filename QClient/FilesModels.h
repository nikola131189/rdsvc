#pragma once

#include "AbstractFilesModel.h"

#include "widgets/ErrorDialog.h"
class LocalFilesModel : public AbstractFilesModel
{
	Q_OBJECT
public:
	LocalFilesModel(QObject* p = nullptr);
	~LocalFilesModel();
	void dir(const std::filesystem::path& p);
	void remove(const std::filesystem::path& p);
	void rename(const std::filesystem::path& old_name, const std::filesystem::path& new_name);
	void mkdir(const std::filesystem::path& p);
	void dirRecursion(const std::filesystem::path& p,
		const std::function<void(const std::list<files::file_info_t>&)>& out);
private:
	std::function<void(const std::list<files::file_info_t>&)> _recCbck;
	ErrorDialog *_errDialog;
	void checkError(const std::error_code& ec);
};


#include <rd/rd.h>

class RemoteFilesModel 
	: public
	AbstractFilesModel
	//ObServer<Rd::FilesEvent>
{
	Q_OBJECT
public:
	RemoteFilesModel(Rd::Client cl, QObject* p = nullptr);
	~RemoteFilesModel();
	void dir(const std::filesystem::path& p);
	void remove(const std::filesystem::path& p);
	void rename(const std::filesystem::path& old_name, const std::filesystem::path& new_name);
	void mkdir(const std::filesystem::path& p);
	void dirRecursion(const std::filesystem::path& p,
		const std::function<void(const std::list<files::file_info_t>&)>& out);
signals:
	void notify(const Rd::FilesEvent& ev);
public slots:
	void onNotify(const Rd::FilesEvent& ev);
private:
	Rd::Client _client;
	std::string _id;

	std::function<void(const std::list<files::file_info_t>&)> _recCbck;
	ErrorDialog *_errDialog;
	boost::signals2::connection _conn;
};