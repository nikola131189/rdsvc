#include "ServersModel.h"


ServersModel::ServersModel(QObject* p)
	:
	TreeModel(p)
{
	sync();
}

ServersModel::~ServersModel()
{

}

ServerInfo ServersModel::make()
{
	ServerInfo inf;
	inf.description = "fg";
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
