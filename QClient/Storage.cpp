#include "Storage.h"

Storage::Storage(const std::string& db)
	: _db(db, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{

}

void Storage::init(const std::string& secret)
{
	std::stringstream s;
	s <<
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


	_db.key(secret);
	int res = _db.exec(s.str());
}

void Storage::rekey(const std::string& secret)
{
	_db.rekey(secret);
}

void Storage::load(std::vector<ServerInfo>& arg)
{
	SQLite::Statement   query(_db, "select * from servers");

	while (query.executeStep())
	{
		ServerInfo inf;
		inf.id = (int)query.getColumn(0);
		inf.description = (const char *)query.getColumn(1);
		inf.connectionType = (const char*)query.getColumn(2);
		inf.address1 = (const char*)query.getColumn(3);
		inf.address2 = (const char*)query.getColumn(4);
		inf.port1 = query.getColumn(5);
		inf.port2 = query.getColumn(6);
		inf.imprint = (const char*)query.getColumn(7);
		inf.secret = (const char*)query.getColumn(8);
		arg.push_back(inf);
	}
}



uint64_t Storage::insert(const ServerInfo& arg)
{
	
	SQLite::Statement query(_db, "insert into servers(description,connectionType,address1,address2,port1,port2,imprint,secret) values (?,?,?,?,?,?,?,?)");

	query.bind(1, arg.description);
	query.bind(2, arg.connectionType);
	query.bind(3, arg.address1);
	query.bind(4, arg.address2);
	query.bind(5, arg.port1);
	query.bind(6, arg.port2);
	query.bind(7, arg.imprint);
	query.bind(8, arg.secret);

	query.exec();
	return _db.getLastInsertRowid();
}

void Storage::update(const ServerInfo& arg)
{
	SQLite::Statement query(_db, "update servers set description=?,connectionType=?,address1=?,address2=?,port1=?,port2=?,imprint=?,secret=? where _id=?");

	query.bind(1, arg.description);
	query.bind(2, arg.connectionType);
	query.bind(3, arg.address1);
	query.bind(4, arg.address2);
	query.bind(5, arg.port1);
	query.bind(6, arg.port2);
	query.bind(7, arg.imprint);
	query.bind(8, arg.secret);
	query.bind(9, (int)arg.id);

	query.exec();
}

void Storage::remove(uint64_t id)
{
	SQLite::Statement query(_db, "delete from servers where _id=?");
	query.bind(1, (int)id);
	query.exec();
}


