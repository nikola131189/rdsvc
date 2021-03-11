#pragma once
#include "events/events.h"
#include <array>

namespace Rd
{
	class Native
	{
	public:
		class Display
		{
		public:
			virtual void getDisplay(Rd::DisplayInfo& inf) = 0;
			virtual void openScreen(const std::string& src, uint32_t dest, const std::string& disp, int id, const codec::VideoFormat& fmt, bool block_input) = 0;
			virtual void closeScreen(int id) = 0;
			virtual void init() {}
			virtual ~Display() {}
		};


		class Action
		{
		public:
			virtual void onMouse(const Rd::ActionEvent::Mouse& mouse) = 0;
			virtual void onKeybd(const Rd::ActionEvent::Keybd& keybd) = 0;
			virtual void init(){}
			virtual ~Action() {}
		};


		class ScreenCapture
		{
		public:
			virtual bool init(uint32_t width, uint32_t height) = 0;
			virtual int frame(uint8_t* data, uint32_t size, Rd::CursorInfo& inf) = 0;
			virtual void setResolution(uint32_t w, uint32_t h) = 0;
			virtual ~ScreenCapture() {}
		};


		class System
		{
		public:
			virtual std::string terminalApp() = 0;
			virtual ~System() {}
		};



		class ClipBoard
		{
		public:			
			typedef std::function<void(const std::string&)> ClipBoardCbck;
			virtual void setCbck(const ClipBoardCbck& cbck) = 0;
			virtual void setData(const std::string& str) = 0;
			virtual ~ClipBoard() {}
		};


		class EventLoop
		{
		public:	
			virtual void blockInput(bool block) = 0;
			virtual void loop() = 0;
			virtual ~EventLoop() {}
		};


		virtual Display& display() = 0;
		virtual Action& action() = 0;
		virtual System& system() = 0;
		virtual ClipBoard& clipBoard() = 0;
		virtual ScreenCapture& screenCapture() = 0;
		virtual EventLoop& eventLoop() = 0;
		virtual ~Native() {}
	};
}
