#include "ServersModel.h"


ServersModel::ServersModel(const std::string& db, QObject* p)
	:
	_storage(db),
	TreeModel(p)
{
	
}

ServersModel::~ServersModel()
{

}

void ServersModel::init(const std::string& secret)
{
	_storage.init(secret);
	sync();
}

void ServersModel::rekey(const std::string& secret)
{
	_storage.rekey(secret);
}

ServerInfo ServersModel::make()
{
	ServerInfo inf;
	inf.description = "new server";
	inf.id = _storage.insert(inf);
	sync();
	return inf;
}

void ServersModel::update(const ServerInfo& arg)
{
	_storage.update(arg);
	auto it = std::find_if(std::begin(_servers), std::end(_servers), [&](const ServerInfo& in) { return in.id == arg.id; });
	if (it != std::end(_servers))
	{
		*it = arg;
		//beginResetModel();

		for (int i = 0; i < root()->size(); i++)
		{
			if (root()->get(i)->data<ServerInfo>().id == arg.id)
			{
				root()->get(i)->setData(arg);
				break;
			}
		}

		//endResetModel();
	}
	dataChanged(index(0), index(root()->size()));
}

void ServersModel::remove(uint64_t id)
{
	_storage.remove(id);
	sync();
}

std::vector<ServerInfo>& ServersModel::servers()
{
	return _servers;
}

void ServersModel::sync()
{
	_servers.clear();
	_storage.load(_servers);
	beginResetModel();
	root()->clear();

	for (auto &it : _servers)
	{
		root()->emplaceBack(it);
	}

	endResetModel();
}
