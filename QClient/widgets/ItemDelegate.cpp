#include "ItemDelegate.h"
namespace fs = std::filesystem;
ItemDelegate::ItemDelegate(QObject* parent)
	: QStyledItemDelegate(parent)
{
}

ItemDelegate::~ItemDelegate()
{
}


std::string ItemDelegate::dateToStr(time_t t) const
{
	if (!t) return "";
	struct tm* timeinfo;
	timeinfo = localtime(&t);
	char buffer[80];
	strftime(buffer, 80, "%d.%m.%Y %H:%M", timeinfo);
	//std::string res = buffer;
	return buffer;
}


std::string ItemDelegate::sizeToStr(uint64_t arg) const
{

	//arg /= 1024;
	//if (arg % 1024) arg++;



	std::string s = std::to_string(arg);
	int k = 0;
	for (auto it = s.end(); it != s.begin(); it--)
	{
		k++;
		if (k == 4)
		{
			s.insert(it++, ' ');
			k = 0;
		}
	}


	return s;
}



std::string ItemDelegate::addressV4ToStr(uint32_t in) const
{
	
	std::stringstream res;
	res << (in >> 24) << "." << (in << 8 >> 24) << "." << (in << 16 >> 24) << "." << (in << 24 >> 24);
	return res.str();
}

std::string ItemDelegate::permsToStr(std::filesystem::perms p) const
{
	std::stringstream res;
	res << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
		<< ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
		<< ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
		<< ((p & fs::perms::group_read) != fs::perms::none ? "r" : "-")
		<< ((p & fs::perms::group_write) != fs::perms::none ? "w" : "-")
		<< ((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
		<< ((p & fs::perms::others_read) != fs::perms::none ? "r" : "-")
		<< ((p & fs::perms::others_write) != fs::perms::none ? "w" : "-")
		<< ((p & fs::perms::others_exec) != fs::perms::none ? "x" : "-");
	return res.str();
}

