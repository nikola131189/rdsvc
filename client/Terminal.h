#pragma once
#include <boost/process.hpp>
#include <boost/asio/io_service.hpp>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <queue>


namespace bp = boost::process;
struct Terminal : public std::enable_shared_from_this<Terminal>
{
	Terminal(boost::asio::io_context& ctx)
		: _ctx(ctx), out_pipe(ctx), in_pipe(ctx), err_pipe(ctx)
	{}

	~Terminal()
	{}

	void start(const std::string& app)
	{
		//boost::process::imbue(std::locale("Windows-1251"));
		std::error_code ec;
		c = bp::child(app, bp::std_out > out_pipe, bp::std_in < in_pipe, bp::std_err > err_pipe, ec);
		if (ec)
		{
			on_error(ec);
		}
		else
		{
			do_out();
			do_error();
		}

	}


	void terminate()
	{
		out_pipe.cancel();
		in_pipe.cancel();
		err_pipe.cancel();
		c.terminate();
#ifdef __linux__ 
		waitpid(c.id(), 0, 0);
#endif
	}

	void write(const std::string& comm)
	{
		if (!c.running()) return;
		auto self(shared_from_this());
		boost::asio::post(_ctx, [this, self, comm]() {
			bool empt = in_buf.empty();
			in_buf.push(comm + "\n");
			if (empt) do_write();
			});
	}

private:


	void do_write()
	{
		auto& b = in_buf.front();
		auto self(shared_from_this());
		boost::asio::async_write(in_pipe, boost::asio::buffer(b),
			[this, self](const boost::system::error_code& ec, std::size_t size)
			{
				if (ec)
				{
					on_error(ec);
				}
				else
				{
					in_buf.pop();
					if (!in_buf.empty()) do_write();
				}
			});
	}




	void do_out()
	{
		auto self(shared_from_this());
		boost::asio::async_read_until(out_pipe, out_buf, "\0",
			[this, self](const boost::system::error_code& ec, std::size_t size)
			{
				if (!ec)
				{
					std::istream stream(&out_buf);
					std::string str;
					std::getline(stream, str, '\0');
					//std::cout << str;
					on_std_out(str);
					do_out();
				}
				else
				{
					on_error(ec);
				}
			});

	}


	void do_error()
	{
		auto self(shared_from_this());
		boost::asio::async_read_until(err_pipe, err_buf, "\0",
			[this, self](const boost::system::error_code& ec, std::size_t size)
			{
				if (!ec)
				{
					std::istream stream(&err_buf);
					std::string str;
					std::getline(stream, str, '\0');
					on_std_error(str);
					do_error();
				}
				else
				{
					on_error(ec);
				}
			});
	}

protected:
	virtual void on_std_out(const std::string& str) = 0;
	virtual void on_std_error(const std::string& str) = 0;
	virtual void on_error(const boost::system::error_code& ec) = 0;
	virtual void on_error(const std::error_code& ec) = 0;
private:
	boost::asio::io_context& _ctx;
	bp::async_pipe out_pipe, in_pipe, err_pipe;
	boost::asio::streambuf out_buf, err_buf;
	std::queue<std::string> in_buf;
	bp::child c;
};







