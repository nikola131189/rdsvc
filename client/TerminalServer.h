#pragma once
#include "Terminal.h"
#include <rd/rd.h>
#include <boost/locale.hpp>

struct terminal_session : public Terminal
{
	terminal_session(uint32_t cid, std::string sid, boost::asio::io_context& ctx)
		: m_cid(cid), m_sid(sid), Terminal(ctx)
	{}

	void on_std_out(const std::string& str)
	{
		Rd::TerminalEvent ev;
		ev.id = m_sid;
		ev.type = Rd::TerminalEvent::STDOUT;
		ev.data = str;

		Rd::Inet::send(m_cid, ev);
	}

	void on_std_error(const std::string& str)
	{
		Rd::TerminalEvent ev;
		ev.id = m_sid;
		ev.type = Rd::TerminalEvent::STDERR;
		ev.data = str;
		Rd::Inet::send(m_cid, ev);
	}


	void on_error(const boost::system::error_code& ec)
	{
		Rd::TerminalEvent ev;
		ev.id = m_sid;
		ev.type = Rd::TerminalEvent::ERR;
		ev.ec.message = utility::localToU8(ec.message());
		ev.ec.value = ec.value();

		Rd::Inet::send(m_cid, ev);
	}

	void on_error(const std::error_code& ec)
	{
		Rd::TerminalEvent ev;
		ev.id = m_sid;
		ev.type = Rd::TerminalEvent::ERR;
		ev.ec.message = utility::localToU8(ec.message());
		ev.ec.value = ec.value();

		Rd::Inet::send(m_cid, ev);
	}
	uint32_t clientId() { return m_cid; }
	std::string id() { return m_sid; }
private:
	std::string m_sid;
	uint32_t m_cid;
};



struct TerminalServer
{

	TerminalServer(boost::asio::io_context& ctx, const std::string& term_path)
		: _ctx(ctx), app_path(term_path)
	{
		EventBus::subscribe<Rd::TerminalEvent>(std::bind(&TerminalServer::notify, this, std::placeholders::_1));

		EventBus::subscribe<Rd::ClientNotFound>([&](const Rd::ClientNotFound& ev) {
				
			for(auto it = sessions.begin(); it != sessions.end();)
				if ((*it)->clientId() == ev.id)
				{
					(*it)->terminate();
					it = sessions.erase(it);
				}
				else
				{
					it++;
				}
			});
	}

	~TerminalServer()
	{
		stop();
	}

private:


	void notify(const Rd::TerminalEvent& ev)
	{
		switch (ev.type)
		{
		case Rd::TerminalEvent::OPEN:
			initSession(ev.source, ev.id);
			break;
		case Rd::TerminalEvent::CTRLC:
			terminateSession(ev.id);
			initSession(ev.source, ev.id);
			break;
		case Rd::TerminalEvent::STDIN:
			command(ev.id, ev.data);
			break;
		case Rd::TerminalEvent::CLOSE:
			terminateSession(ev.id);
			break;
		default:
			break;
		}
	}


	void stop()
	{
		for (auto it = sessions.begin(); it != sessions.end(); it++)
		{
			(*it)->terminate();
		}
	}

	void initSession(uint32_t cid, std::string sid)
	{
		sessions.emplace_back(std::make_shared<terminal_session>(cid, sid, _ctx))->start(app_path);
	}


	void terminateSession(std::string sid)
	{
		auto it = std::find_if(sessions.begin(), sessions.end(), [&](const std::shared_ptr<terminal_session>& s) {return s->id() == sid; });
		if (it == sessions.end()) return;
		(*it)->terminate();
		sessions.erase(it);
	}

	void command(std::string sid, const std::string& com)
	{
		auto it = std::find_if(sessions.begin(), sessions.end(), [&](const std::shared_ptr<terminal_session>& s) {return s->id() == sid; });
		if (it == sessions.end()) return;
		(*it)->write(com);
	}
private:
	std::list<std::shared_ptr<terminal_session>> sessions;
	boost::asio::io_context& _ctx;
	std::string app_path;
};
