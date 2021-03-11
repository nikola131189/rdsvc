#pragma once

#include <QObject>
#include <QStyledItemDelegate>
#include <sstream>
#include <filesystem>

class ItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	ItemDelegate(QObject* parent = 0);
	~ItemDelegate();
protected:
	std::string dateToStr(time_t t) const;
	std::string sizeToStr(uint64_t arg) const;
	std::string addressV4ToStr(uint32_t in) const;
	std::string permsToStr(std::filesystem::perms in) const;
};
