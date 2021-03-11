#pragma once
#include "widgets/TreeModel.h"
#include <filesystem>

#include <rd/files/file_system.h>

struct AbstractFilesModel : public TreeModel
{
	Q_OBJECT
public:
	AbstractFilesModel(QObject* p = nullptr) :TreeModel(p) {}
	virtual ~AbstractFilesModel() {}
	virtual void dir(const std::filesystem::path& p) = 0;
	virtual void remove(const std::filesystem::path& p) = 0;
	virtual void rename(const std::filesystem::path& old_name, const std::filesystem::path& new_name) = 0;
	virtual void mkdir(const std::filesystem::path& p) = 0;
	virtual void dirRecursion(const std::filesystem::path& p,
		const std::function<void(const std::list<files::file_info_t>&)>& out) = 0;
};