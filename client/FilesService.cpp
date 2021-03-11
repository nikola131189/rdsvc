#include "FilesService.h"

FilesService::FilesService(boost::asio::io_context& ctx)
	: 
	_ctx(ctx)
{
}

void FilesService::notify(const Rd::FilesEvent& e)
{
	boost::asio::post([this, e]()
		{
			std::error_code ec;
			Rd::FilesEvent ev = e;
			if (ev.type == Rd::FilesEvent::DIR_REQUEST)
			{
				files::dir(ev.path1, ev.fileList, ec);
				ev.type = Rd::FilesEvent::DIR_RESPONSE;
				checkError(ev, ec);
				Rd::Inet::send(ev.source, ev);
			}



			if (ev.type == Rd::FilesEvent::DIR_RECURSION_REQUEST)
			{
				files::dir_recursion(ev.path1, ev.fileList, ec);
				ev.type = Rd::FilesEvent::DIR_RECURSION_RESPONSE;
				checkError(ev, ec);
				Rd::Inet::send(ev.source, ev);
			}



			if (ev.type == Rd::FilesEvent::MAKE_DIR)
			{
				std::filesystem::create_directories(ev.path1, ec);
				if (checkError(ev, ec))
				{
					Rd::Inet::send(ev.source, ev);
				}
			}


			if (ev.type == Rd::FilesEvent::REMOVE)
			{
				std::filesystem::remove_all(ev.path1, ec);
				if (checkError(ev, ec))
				{
					Rd::Inet::send(ev.source, ev);
				}
			}


			if (ev.type == Rd::FilesEvent::RENAME)
			{
				std::error_code ec;
				std::filesystem::rename(ev.path1, ev.path2, ec);

				if (checkError(ev, ec))
				{
					Rd::Inet::send(ev.source, ev);
				}
			}
		}
	);
}

bool FilesService::checkError(Rd::FilesEvent& ev, std::error_code ec)
{
	if (ec) {
		ev.type = Rd::FilesEvent::ERR;
		ev.ec.value = ec.value();
		ev.ec.message = utility::localToU8(ec.message());
		return 1;
	}
	return 0;
}

