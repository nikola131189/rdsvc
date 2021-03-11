#pragma once
#include "Session.h"
#include "PacketHelper.h"
#include "../events/events.h"
#include "../Client.hpp"
#include "../events/Serializer.hpp"
#include "../EventBus.hpp"
#include "../Log.h"

namespace Net
{
	class ClientSessionHandler : public SessionHandler
	{
	public:
		ClientSessionHandler();
		ClientSessionHandler(const std::string& name, const std::string& imprint, int connectionType);
		~ClientSessionHandler();
		ClientSessionHandler& operator=(const ClientSessionHandler& other);

		template<typename T>
		void write(uint32_t dest, const T& t, Net::packet_cbck cbck = nullptr);

	private:
		void onStart(session_ptr sess);
		void onError(session_ptr sess, std::string msg);
		void onRead(session_ptr sess, Packet&& pack);

	private:
		session_ptr _sess;
		PacketHelper _packetHelper;
		std::vector<uint8_t> _readBuff, _writeBuff;
		Rd::HelloEvent _helloEvent;
		std::mutex _mut;
	};



	template<typename T>
	inline void ClientSessionHandler::write(uint32_t dest, const T& t, packet_cbck cbck)
	{
		if (!_sess)
			return;

		std::unique_lock<std::mutex> lk(_mut);

		if (Rd::Serializer::encode(t, _writeBuff))
		{
			PacketHelper::divide(_sess->id(), dest, _writeBuff, [&](Packet& p, bool eof) {
				if (eof)
					p.cbck = cbck;	
				/*auto fn = [this, pack = std::move(p)]() {
					_sess->write(std::move(pack));
				};

				boost::asio::post(_sess->get_executor(), []() {});*/

				boost::asio::post(_sess->get_executor(), [p, this]() { _sess->write(p); });
			});
		} 
		else
		{
			//LOG(INFO) << "err encode";
		}
		//LOG(INFO) << typeid(t).name();
	}

}