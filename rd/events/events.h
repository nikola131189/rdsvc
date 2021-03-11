#pragma once

#include <string>
#include <vector>
#include <variant>
#include <array>
#include <boost/system/error_code.hpp>
#include <filesystem>
#include "../Client.hpp"
#include "../files/file_system.h"
#include "../codec/Codec.h"

namespace Rd
{
	struct ClientNotFound
	{
		ClientNotFound(uint32_t id_) : id(id_) {}
		uint32_t id;
	};

	struct ConnectionError
	{
		/*ConnectionError(const ConnectionError&)
		{
			std::cout << "copy\n";
		}*/
		ConnectionError() 
		{}
		ConnectionError(const boost::system::error_code& e) : ec(e)
		{}
		boost::system::error_code ec;

	};

	struct ConnectionOpen
	{
		ConnectionOpen(){}
	};

	struct ErrorCode
	{
		ErrorCode() : value(0) {}
		int value;
		std::string message;
	};


	struct EventBase
	{
		uint32_t source;
	};


	//-----------------------------------------------------------------------------------------------------client


	struct ClientsRequestEvent : public EventBase
	{
		ClientsRequestEvent()
		{}
	};

	struct ClientsEvent : public EventBase
	{
		std::vector<Client> clients;
	};

	struct HelloEvent : public EventBase
	{
		Client client;
		std::string imprint;
		std::string password;
		uint32_t connectionType = 0;
	};

	///----------------------------------------------------------------------------------------------------terminal

	struct TerminalEvent : public EventBase
	{
		enum Type
		{
			OPEN,
			CLOSE,
			CTRLC,
			STDIN,
			STDOUT,
			STDERR,
			ERR
		};

		Type type = Type::OPEN;
		std::string id;
		std::string data;
		ErrorCode ec;
	};




	///----------------------------------------------------------------------------------------------------FilesEvent
	struct FilesEvent : public EventBase
	{
		enum Type
		{
			DIR_REQUEST,
			DIR_RESPONSE,
			DIR_RECURSION_REQUEST,
			DIR_RECURSION_RESPONSE,
			REMOVE,
			MAKE_DIR,
			RENAME,
			ERR
		};

		Type type = Type::DIR_REQUEST;
		std::string id;
		std::filesystem::path path1, path2;
		std::list<files::file_info_t> fileList;
		ErrorCode ec;
	};














	///----------------------------------------------------------------------------------------------------TransferEvent


	struct TransferEvent : public EventBase
	{
		struct Post
		{
			int64_t offset = 0;
			std::filesystem::path path;
			std::vector<uint8_t> data;
		};


		struct PostOk
		{
			int64_t offset = 0;
			uint32_t size = 0;
		};

		struct Get
		{
			int64_t offset = 0;
			uint32_t size = 0;
			std::filesystem::path path;
		};

		struct GetOk
		{
			int64_t offset = 0;
			std::vector<uint8_t> data;
			bool eof = 0;
		};

		struct Error
		{
			int64_t offset = 0;
			int value = 0;
			std::string message;
		};

		uint32_t id = 0;
		std::variant<Post, PostOk, Get, GetOk, Error> val;
	};
















	




	///----------------------------------------------------------------------------------------------------VideoEvent





	struct CursorInfo
	{
		CursorInfo() :
			width(0), height(0), x(0), y(0), xhot(0), yhot(0), visible(0)
		{}

		int width, height, x, y, xhot, yhot;
		std::vector<uint8_t> data;
		bool visible;
	};



	struct VideoEvent : public EventBase
	{
		std::string display;
		int32_t screenId = 0;

		std::vector<uint8_t> data;
		uint32_t size = 0;
		codec::VideoFormat videoFormat;
		CursorInfo cursor;
	
	};























	///----------------------------------------------------------------------------------------------------DisplayEvent




	struct Resolution
	{
		Resolution()
			: width(0), height(0), mwidth(0), mheight(0)
		{}
		uint32_t width, height, mwidth, mheight;
	};


	struct ScreenInfo
	{
		uint32_t id = 0;
		std::vector<Rd::Resolution>	resolutions;
		Rd::Resolution current;
	};


	struct DisplayInfo
	{
		std::string name;
		std::vector<ScreenInfo> screens;
	};




	struct DisplayEvent : public EventBase
	{
		struct Request
		{
			std::string name;
		};

		struct Response
		{
			DisplayInfo display;
		};


		struct Open
		{
			std::string display;
			int32_t screenId = 0;
			codec::VideoFormat videoFormat;
			bool blockInput = 0;
		};

		struct Close
		{
			std::string display;
			int32_t screenId = 0;
		};


		std::variant<Request, Open, Close, Response, ErrorCode> val;
	};














	///----------------------------------------------------------------------------------------------------ActionEvent



	struct ActionEvent : public EventBase
	{

		struct Mouse
		{
			enum Type
			{
				move,
				wheel_forward,
				wheel_backward,
				button_1,  // left
				button_2,  // right
				button_3,  // middle
				button_4,
				button_5,
				button_6,
				button_7,
				button_8,
				button_9,
				button_10,
				button_11,
				button_12,
			};
			bool pressed;
			int32_t x, y;
			Type type;
		};



		struct Keybd
		{
			uint32_t key;
			bool pressed;
			bool extended;
		};


		int32_t screenId = 0;
		std::variant<Mouse, Keybd> val;
	};








	///----------------------------------------------------------------------------------------------------ClipboardEvent



	struct ClipboardEvent : public EventBase
	{
		std::string text;
	};
}