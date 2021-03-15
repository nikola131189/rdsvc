#pragma once
#include "Storage.h"
#include "widgets/TreeModel.h"


struct ServersModel : public TreeModel
{
	Q_OBJECT
public:
	ServersModel(QObject* p = nullptr);
	~ServersModel();
	ServerInfo make();
	void update(const ServerInfo& arg);
	void remove(uint64_t id);
	std::vector<ServerInfo>& servers();
private:
	void sync();
	Storage _storage;

	std::vector<ServerInfo> _servers;
};
