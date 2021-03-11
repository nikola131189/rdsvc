#pragma once

#include <unordered_map>
#include <rd/rd.h>
#include "ImprintStore.h"

namespace Net
{
	class Application : public SessionHandler
	{
	public:
		Application();
		void onStart(session_ptr sess);
		void onError(session_ptr sess, std::string msg);
		void onRead(session_ptr sess, Packet&& pack);

		awaitable<void> pingLoop(int timeout);

		~Application() {}
	private:
		
		void sendPing(session_ptr sess);
		void onPong(session_ptr sess);
		void onHello(session_ptr sess, Packet&& pack);
		void printSessions();
		void notifyDisconnected(uint32_t id);

		template<typename T>
		inline void onData(session_ptr sess, const T& t)
		{
			if constexpr (std::is_same_v<T, Rd::ClientsRequestEvent>)
			{
				Rd::ClientsEvent ev;
				for (auto& it : _sessions1)
					ev.clients.push_back(it.second->client());

				if (Rd::Serializer::encode(ev, _buff))
				{
					PacketHelper::divide(0, sess->id(), _buff, [&](Packet& p, bool eof) {
							sess->write(p);
						});
				}
			}
		}


	private:
		std::unordered_map<uint32_t, session_ptr> _sessions1, _sessions2;
		

		std::vector<uint8_t> _buff;
		PacketHelper _packetHelper;
		ImprintStore _imprintStore;
	};

}
