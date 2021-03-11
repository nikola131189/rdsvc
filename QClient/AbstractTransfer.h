#pragma once
#include <filesystem>
#include <rd/rd.h>


class TransferHandler;

struct AbstractTransfer : public std::enable_shared_from_this<AbstractTransfer>
{
	enum State
	{
		running,
		stopped,
		ready,
		error,
		success
	};

	AbstractTransfer(Rd::Client cl, uint32_t id_, const std::filesystem::path& p1, const std::filesystem::path& p2,
		file_system::FileBuffer& fb, TransferHandler& transferHandler)
		: 
		_client(cl), _id(id_), _path1(p1), _path2(p2), 
		_fileBuffer(fb), _state(ready), _lastActive(0), _offset(0),
		_transferHandler(transferHandler)
	{}
	const Rd::Client& client() const { return _client; }
	State state() { return _state; }
	uint32_t id() { return _id; }
	const std::filesystem::path& path1() const { return _path1; }
	const std::filesystem::path& path2() const { return _path2; }
	uint64_t lastActive() { return _lastActive; }
	int64_t offset() { return _offset; }



	double speed() { return _speed; }

	void snapshot(double dt)
	{
		double dist = (_offset - _lastSnapshot) / 1024.0;
		_speed = dist / (dt / 1000.0);
		_lastSnapshot = _offset;
	}

	uint32_t last_active() { return _lastActive; }

	const Rd::ErrorCode& ec() const { return _ec; }
private:
	double _speed = 0.0;
	uint64_t _lastSnapshot = 0;

public:
	virtual void start() = 0;
	virtual void onTransfer(const Rd::TransferEvent& ev) = 0;

protected:
	TransferHandler& _transferHandler;
	State _state;
	uint32_t _id;
	std::filesystem::path _path1, _path2;
	uint64_t _lastActive;
	int64_t _offset;
	Rd::Client _client;
	file_system::FileBuffer& _fileBuffer;
	std::array<uint8_t, 64 * 1024> _buffer;

	Rd::ErrorCode _ec;

};


typedef std::shared_ptr<AbstractTransfer> TransferPtr;


class TransferHandler
{
public:
	virtual void onSuccess(TransferPtr ptr) = 0;
	virtual void onError(TransferPtr ptr) = 0;
	virtual ~TransferHandler() {}
};