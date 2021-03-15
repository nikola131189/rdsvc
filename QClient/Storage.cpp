#include "Storage.h"

Storage::Storage() 
	:
	_db("C:\\Users\\user\\source\\repos\\rd\\x64\\Release\\db.db")
{
	_db <<
		"create table if not exists servers ("
		"   _id integer primary key autoincrement not null,"
		"   description text,"
		"	connectionType text,"
		"   address1 text,"
		"   address2 text,"
		"   port1 int,"
		"   port2 int,"
		"   imprint text,"
		"   secret text"
		");";

}

void Storage::load(std::vector<ServerInfo>& arg)
{
	_db << "select * from servers;"
		>> [&](int id, std::string description, std::string connectionType, std::string address1, std::string address2,
			 int port1, int port2, std::string imprint, std::string secret)
	{
		ServerInfo inf = { id, description, connectionType, address1, address2, port1, port2, imprint, secret };
		arg.push_back(inf);
	};
}



uint64_t Storage::insert(const ServerInfo& arg)
{
	_db << "insert into servers(description,connectionType,address1,address2,port1,port2,imprint,secret) values (?,?,?,?,?,?,?,?);"
		<< arg.description
		<< arg.connectionType
		<< arg.address1
		<< arg.address2
		<< arg.port1
		<< arg.port2
		<< arg.imprint
		<< arg.secret;

	return _db.last_insert_rowid();
}

void Storage::update(const ServerInfo& arg)
{
	_db << "update servers set description=?,connectionType=?,address1=?,address2=?,port1=?,port2=?,imprint=?,secret=? where _id=?;"
		<< arg.description
		<< arg.connectionType
		<< arg.address1
		<< arg.address2
		<< arg.port1
		<< arg.port2
		<< arg.imprint
		<< arg.secret
		<< arg.id;
}

void Storage::remove(uint64_t id)
{
	_db << "delete from servers where _id=?;"
		<< id;
}

