#pragma once

#include "proto/serialize.pb.h"
#include "events.h"
#include <memory>
#include "../network/packet.h"

//#include "../utility/utility.h"
namespace Rd
{
	struct ProtoSerializer
	{
		enum rd_event_type
		{
			HELLO_EVENT,
			CLIENTS_REQUEST_EVENT,
			CLIENTS_EVENT,
			DISPLAY_EVENT,
			VIDEO_EVENT,
			ACTION_EVENT,
			TERMINAL_EVENT,
			FILES_EVENT,
			FILE_TRANSFER_EVENT,
			CLIPBOARD_EVENT
		};

		template<typename T>
		static bool encode(const T& in, std::vector<uint8_t>& res)
		{
			if constexpr (std::is_same_v<T, Rd::HelloEvent>)
			{
				::HelloEvent ev;
				
				ev.mutable_client()->set_id(in.client.id);
				ev.mutable_client()->set_name(in.client.name);
				ev.mutable_client()->set_ping(in.client.ping);
				ev.mutable_client()->set_connection_time(in.client.connectionTime);
				ev.mutable_client()->set_v4address(in.client.addressV4);
				ev.mutable_client()->set_v6address(in.client.addressV6.data(), 16);
				ev.set_imprint(in.imprint);
				ev.set_password(in.password);
				ev.set_connection_type(in.connectionType);

				res.resize(ev.ByteSizeLong() + 1);				
				res[0] = HELLO_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
				
			}
			if constexpr (std::is_same_v<T, Rd::ClientsRequestEvent>)
			{
				::ClientsRequestEvent ev;
				res.resize(ev.ByteSizeLong() + 1);
				res[0] = CLIENTS_REQUEST_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
			}

			if constexpr (std::is_same_v<T, Rd::ClientsEvent>)
			{

				::ClientsEvent ev;

				for (auto& it : in.clients)
				{
					auto cl = ev.mutable_clients()->Add();

					
					cl->set_id(it.id);
					cl->set_name(it.name);
					cl->set_ping(it.ping);
					cl->set_connection_time(it.connectionTime);

					cl->set_v4address(it.addressV4);
					cl->set_v6address(it.addressV6.data(), 16);
				}

				res.resize(ev.ByteSizeLong() + 1);
				res[0] = CLIENTS_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
			}

			if constexpr (std::is_same_v<T, Rd::TerminalEvent>)
			{

				::TerminalEvent ev;
				ev.set_id(in.id);

				switch (in.type)
				{
				case Rd::TerminalEvent::OPEN:
					ev.mutable_open()->CopyFrom(TerminalEvent_Open());
					break;
				case Rd::TerminalEvent::CLOSE:
					ev.mutable_terminate()->CopyFrom(TerminalEvent_Terminate());
					break;
				case Rd::TerminalEvent::CTRLC:
					ev.mutable_ctrl_c()->CopyFrom(TerminalEvent_CtrlC());
					break;
				case Rd::TerminalEvent::STDIN:
					ev.mutable_std_in()->set_data(in.data);
					break;
				case Rd::TerminalEvent::STDOUT:
					ev.mutable_std_out()->set_data(in.data);
					break;
				case Rd::TerminalEvent::STDERR:
					ev.mutable_std_err()->set_data(in.data);
					break;
				case Rd::TerminalEvent::ERR:
					ev.mutable_error()->set_message(in.ec.message);
					ev.mutable_error()->set_value(in.ec.value);
					break;
				default:
					break;
				}


				res.resize(ev.ByteSizeLong() + 1);
				res[0] = TERMINAL_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
			}

			if constexpr (std::is_same_v<T, Rd::FilesEvent>)
			{

				::FilesEvent ev;
				ev.set_id(in.id);
				if (in.type == Rd::FilesEvent::DIR_REQUEST)
					ev.mutable_dir_request()->set_path(in.path1.generic_u8string());

				if (in.type == Rd::FilesEvent::DIR_RESPONSE)
				{
					for (auto& it : in.fileList)
					{
						auto f = ev.mutable_dir_response()->mutable_data()->Add();
						f->set_path(it.path.generic_u8string());
						f->set_size(it.size);
						f->set_time(it.time);
						f->set_type((int)it.type);
						f->set_perm((uint32_t)it.perm);
					}
				}

				if (in.type == Rd::FilesEvent::DIR_RECURSION_REQUEST)
					ev.mutable_dir_recursion_request()->set_path(in.path1.generic_u8string());


				if (in.type == Rd::FilesEvent::DIR_RECURSION_RESPONSE)
				{
					for (auto& it : in.fileList)
					{
						auto f = ev.mutable_dir_recursion_response()->mutable_data()->Add();
						f->set_path(it.path.generic_u8string());
						f->set_size(it.size);
						f->set_time(it.time);
						f->set_type((int)it.type);
						f->set_perm((uint32_t)it.perm);
					}
				}


				if (in.type == Rd::FilesEvent::REMOVE)
					ev.mutable_remove()->set_path(in.path1.generic_u8string());

				if (in.type == Rd::FilesEvent::MAKE_DIR)
					ev.mutable_make_dir()->set_path(in.path1.generic_u8string());

				if (in.type == Rd::FilesEvent::RENAME)
				{
					ev.mutable_rename()->set_src(in.path1.generic_u8string());
					ev.mutable_rename()->set_dest(in.path2.generic_u8string());
				}

				if (in.type == Rd::FilesEvent::ERR)
				{
					ev.mutable_error()->set_message(in.ec.message);
					ev.mutable_error()->set_value(in.ec.value);
				}


				res.resize(ev.ByteSizeLong() + 1);
				res[0] = FILES_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
			}

			if constexpr (std::is_same_v<T, Rd::TransferEvent>)
			{
				::FileTransferEvent ev;
				ev.set_id(in.id);

				if (auto pval = std::get_if<TransferEvent::Post>(&in.val))
				{
					ev.mutable_post()->set_offset(pval->offset);
					ev.mutable_post()->set_path(pval->path.generic_u8string());
					if (pval->data.size())
						ev.mutable_post()->set_data(&pval->data[0], pval->data.size());
				}

				if (auto pval = std::get_if<TransferEvent::PostOk>(&in.val))
				{
					ev.mutable_postok()->set_offset(pval->offset);
					ev.mutable_postok()->set_size(pval->size);
				}

				if (auto pval = std::get_if<TransferEvent::Get>(&in.val))
				{
					ev.mutable_get()->set_offset(pval->offset);
					ev.mutable_get()->set_size(pval->size);
					ev.mutable_get()->set_path(pval->path.generic_u8string());
				}


				if (auto pval = std::get_if<TransferEvent::GetOk>(&in.val))
				{
					ev.mutable_getok()->set_offset(pval->offset);
					if (pval->data.size())
						ev.mutable_getok()->set_data(&pval->data[0], pval->data.size());
					ev.mutable_getok()->set_eof(pval->eof);
				}

			
				if (auto pval = std::get_if<TransferEvent::Error>(&in.val))
				{
					ev.mutable_error()->set_message(pval->message);
					ev.mutable_error()->set_value(pval->value);
					ev.mutable_error()->set_offset(pval->offset);
				}

				res.resize(ev.ByteSizeLong() + 1);
				res[0] = FILE_TRANSFER_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
			}

			if constexpr (std::is_same_v<T, Rd::VideoEvent>)
			{
				::VideoEvent ev;
				ev.set_screen_id(in.screenId);
				ev.set_display(in.display);
				ev.set_data(&in.data[0], in.size);
				ev.mutable_video_format()->set_bitrate(in.videoFormat.bitarte);
				ev.mutable_video_format()->set_id(in.videoFormat.id);
				ev.mutable_video_format()->set_w(in.videoFormat.w);
				ev.mutable_video_format()->set_h(in.videoFormat.h);
				ev.mutable_video_format()->set_dw(in.videoFormat.dw);
				ev.mutable_video_format()->set_dh(in.videoFormat.dh);
				ev.mutable_video_format()->set_profile(in.videoFormat.profile);
				ev.mutable_video_format()->set_quantization(in.videoFormat.quantization);
				ev.mutable_video_format()->set_thread_count(in.videoFormat.threadCount);

				ev.mutable_cursor()->set_width(in.cursor.width);
				ev.mutable_cursor()->set_height(in.cursor.height);
				if (!in.cursor.data.empty())
					ev.mutable_cursor()->set_data(&in.cursor.data[0], in.cursor.data.size());
				ev.mutable_cursor()->set_visible(in.cursor.visible);
				ev.mutable_cursor()->set_hotspot_x(in.cursor.xhot);
				ev.mutable_cursor()->set_hotspot_y(in.cursor.yhot);
				ev.mutable_cursor()->set_x(in.cursor.x);
				ev.mutable_cursor()->set_y(in.cursor.y);


				res.resize(ev.ByteSizeLong() + 1);
				res[0] = VIDEO_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
			}

			if constexpr (std::is_same_v<T, Rd::DisplayEvent>)
			{
				::DisplayEvent ev;

				if (auto pval = std::get_if<Rd::DisplayEvent::Request>(&in.val))
				{
					ev.mutable_request()->set_name(pval->name);
				}

				if (auto pval = std::get_if<Rd::DisplayEvent::Response>(&in.val))
				{
					ev.mutable_response()->mutable_display()->set_name(pval->display.name);
					for (auto it : pval->display.screens)
					{
						auto s = ev.mutable_response()->mutable_display()->mutable_screens()->Add();
						s->set_id(it.id);
						s->mutable_current()->set_width(it.current.width);
						s->mutable_current()->set_height(it.current.height);
						s->mutable_current()->set_mwidth(it.current.mwidth);
						s->mutable_current()->set_mheight(it.current.mheight);

						for (auto it1 : it.resolutions)
						{
							auto r = s->mutable_resolutions()->Add();
							r->set_width(it1.width);
							r->set_height(it1.height);
							r->set_mwidth(it1.mwidth);
							r->set_mheight(it1.mheight);
						}

					}
				}


				if (auto pval = std::get_if<Rd::DisplayEvent::Open>(&in.val))
				{
					ev.mutable_open()->set_display(pval->display);
					ev.mutable_open()->set_screen_id(pval->screenId);
					ev.mutable_open()->set_block_input(pval->blockInput);
					ev.mutable_open()->mutable_video_format()->set_bitrate(pval->videoFormat.bitarte);
					ev.mutable_open()->mutable_video_format()->set_id(pval->videoFormat.id);
					ev.mutable_open()->mutable_video_format()->set_w(pval->videoFormat.w);
					ev.mutable_open()->mutable_video_format()->set_h(pval->videoFormat.h);
					ev.mutable_open()->mutable_video_format()->set_dw(pval->videoFormat.dw);
					ev.mutable_open()->mutable_video_format()->set_dh(pval->videoFormat.dh);
					ev.mutable_open()->mutable_video_format()->set_profile(pval->videoFormat.profile);
					ev.mutable_open()->mutable_video_format()->set_quantization(pval->videoFormat.quantization);
					ev.mutable_open()->mutable_video_format()->set_thread_count(pval->videoFormat.threadCount);
				}

				if (auto pval = std::get_if<Rd::DisplayEvent::Close>(&in.val))
				{
					ev.mutable_close()->set_display(pval->display);
					ev.mutable_close()->set_screen_id(pval->screenId);
				}
				res.resize(ev.ByteSizeLong() + 1);
				res[0] = DISPLAY_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
			}

			if constexpr (std::is_same_v<T, Rd::ActionEvent>)
			{
				::ActionEvent ev;
				ev.set_screen_id(in.screenId);
				if (auto pval = std::get_if<Rd::ActionEvent::Mouse>(&in.val))
				{
					ev.mutable_mouse()->set_type(pval->type);
					ev.mutable_mouse()->set_x(pval->x);
					ev.mutable_mouse()->set_y(pval->y);
					ev.mutable_mouse()->set_pressed(pval->pressed);
				}

				if (auto pval = std::get_if<Rd::ActionEvent::Keybd>(&in.val))
				{
					ev.mutable_keybd()->set_key(pval->key);
					ev.mutable_keybd()->set_extended(pval->extended);
					ev.mutable_keybd()->set_pressed(pval->pressed);
				}


				res.resize(ev.ByteSizeLong() + 1);
				res[0] = ACTION_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
			}


			if constexpr (std::is_same_v<T, Rd::ClipboardEvent>)
			{
				::ClipboardEvent ev;
				ev.set_text(in.text);
				
				res.resize(ev.ByteSizeLong() + 1);
				res[0] = CLIPBOARD_EVENT;
				ev.SerializeToArray(res.data() + 1, res.size() - 1);
			}
			return 1;
		}
		
		template <typename T>
		static void decode(const std::vector<uint8_t>& data, T t)
		{
			if (data[0] == HELLO_EVENT)
			{
				::HelloEvent ev;
				ev.ParseFromArray(data.data() + 1, data.size() - 1);
				Rd::HelloEvent ret;
				ret.connectionType = ev.connection_type();
				ret.client.connectionTime = ev.client().connection_time();
				ret.client.id = ev.client().id();
				ret.client.name = ev.client().name();
				ret.client.ping = ev.client().ping();
				ret.client.addressV4 = ev.client().v4address();
				std::copy(ev.client().v6address().begin(), ev.client().v6address().end(), ret.client.addressV6.begin());
				ret.imprint = ev.imprint();
				ret.password = ev.password();
				t(std::move(ret));
			}
			
			if (data[0] == CLIENTS_REQUEST_EVENT)
			{
				Rd::ClientsRequestEvent ret;
				t(std::move(ret));
			}
			
			if (data[0] == CLIENTS_EVENT)
			{
				::ClientsEvent ev;
				ev.ParseFromArray(data.data() + 1, data.size() - 1);
				Rd::ClientsEvent ret;
				for (int i = 0; i < ev.clients().size(); i++)
				{
					Rd::Client cl;
					auto cl1 = ev.clients().Get(i);
					cl.connectionTime = cl1.connection_time();
					cl.id = cl1.id();
					cl.name = cl1.name();
					cl.ping = cl1.ping();


					cl.addressV4 = cl1.v4address();
					std::copy(cl1.v6address().begin(), cl1.v6address().end(), cl.addressV6.begin());
					ret.clients.push_back(cl);
				}
				t(std::move(ret));
			}
			
			if (data[0] == TERMINAL_EVENT)
			{
				::TerminalEvent ev;
				ev.ParseFromArray(data.data() + 1, data.size() - 1);
				Rd::TerminalEvent ret;
				ret.id = ev.id();
				if (ev.has_ctrl_c())
					ret.type = TerminalEvent::CTRLC;
				if (ev.has_open())
					ret.type = TerminalEvent::OPEN;
				if (ev.has_terminate())
					ret.type = TerminalEvent::CLOSE;
				if (ev.has_std_in()) {
					ret.type = TerminalEvent::STDIN;
					ret.data = ev.std_in().data();
				}
				if (ev.has_std_out()) {
					ret.type = TerminalEvent::STDOUT;
					ret.data = ev.std_out().data();
				}
				if (ev.has_std_err()) {
					ret.type = TerminalEvent::STDERR;
					ret.data = ev.std_err().data();
				}
				if (ev.has_error())
				{
					ret.type = TerminalEvent::ERR;
					ret.ec.message = ev.mutable_error()->message();
					ret.ec.value = ev.mutable_error()->value();
				}
				t(std::move(ret));
			}

			if (data[0] == FILES_EVENT)
			{
				::FilesEvent ev;
				ev.ParseFromArray(data.data() + 1, data.size() - 1);
				Rd::FilesEvent ret;
				ret.id = ev.id();


				if (ev.has_dir_request()) {
					ret.type = Rd::FilesEvent::DIR_REQUEST;
					ret.path1 = ev.dir_request().path();
				}


				if (ev.has_dir_response()) {
					ret.type = Rd::FilesEvent::DIR_RESPONSE;

					for (int i = 0; i < ev.dir_response().data().size(); i++)
					{
						auto f = ev.dir_response().data().Get(i);
						ret.fileList.push_back({ f.path(), f.size(), (time_t)f.time(),
							(files::file_type)f.type(), (std::filesystem::perms)f.perm() });
					}
				}

				if (ev.has_dir_recursion_request()) {
					ret.type = Rd::FilesEvent::DIR_RECURSION_REQUEST;
					ret.path1 = ev.dir_recursion_request().path();
				}


				if (ev.has_dir_recursion_response()) {
					ret.type = Rd::FilesEvent::DIR_RECURSION_RESPONSE;

					for (int i = 0; i < ev.dir_recursion_response().data().size(); i++)
					{
						auto f = ev.dir_recursion_response().data().Get(i);
						ret.fileList.push_back({ f.path(), f.size(), (time_t)f.time(),
							(files::file_type)f.type(), (std::filesystem::perms)f.perm() });
					}
				}


				if (ev.has_remove()) {
					ret.type = Rd::FilesEvent::REMOVE;
					ret.path1 = ev.remove().path();
				}

				if (ev.has_make_dir()) {
					ret.type = Rd::FilesEvent::MAKE_DIR;
					ret.path1 = ev.make_dir().path();
				}

				if (ev.has_rename()) {
					ret.type = Rd::FilesEvent::RENAME;
					ret.path1 = ev.rename().src();
					ret.path2 = ev.rename().dest();
				}


				if (ev.has_error()) {

					ret.type = Rd::FilesEvent::ERR;
					ret.ec.message = ev.mutable_error()->message();
					ret.ec.value = ev.mutable_error()->value();
				}
				t(std::move(ret));
			}

			if (data[0] == FILE_TRANSFER_EVENT)
			{
				::FileTransferEvent ev;
				ev.ParseFromArray(data.data() + 1, data.size() - 1);
				Rd::TransferEvent ret;
				ret.id = ev.id();

				if (ev.has_post())
				{
					TransferEvent::Post v;
					v.offset = ev.post().offset();
					v.path = ev.post().path();
					v.data = std::vector<uint8_t>(ev.post().data().begin(), ev.post().data().end());
					ret.val = v;
				}


				if (ev.has_postok())
				{
					TransferEvent::PostOk v;
					v.offset = ev.postok().offset();
					v.size = ev.postok().size();
					ret.val = v;
				}


				if (ev.has_get())
				{
					TransferEvent::Get v;
					v.offset = ev.get().offset();
					v.size = ev.get().size();
					v.path = ev.get().path();
					ret.val = v;
				}


				if (ev.has_getok())
				{
					TransferEvent::GetOk v;
					v.offset = ev.getok().offset();
					v.data = std::vector<uint8_t>(ev.getok().data().begin(), ev.getok().data().end());
					v.eof = ev.getok().eof();
					ret.val = v;
				}


				if (ev.has_error())
				{
					TransferEvent::Error ec;
					ec.message = ev.mutable_error()->message();
					ec.value = ev.mutable_error()->value();
					ec.offset = ev.mutable_error()->offset();
					ret.val = ec;
				}

				t(std::move(ret));
			}

			if (data[0] == VIDEO_EVENT)
			{
				::VideoEvent ev;
				ev.ParseFromArray(data.data() + 1, data.size() - 1);
				Rd::VideoEvent ret;
				ret.screenId = ev.screen_id();
				ret.display = ev.display();
				ret.data = std::vector<uint8_t>(ev.data().begin(), ev.data().end());
				ret.videoFormat.bitarte = ev.video_format().bitrate();
				ret.videoFormat.id = (codec::VideoFormat::CodecId)ev.video_format().id();
				ret.videoFormat.h = ev.video_format().h();
				ret.videoFormat.w = ev.video_format().w();
				ret.videoFormat.dh = ev.video_format().dh();
				ret.videoFormat.dw = ev.video_format().dw();
				ret.videoFormat.profile = ev.video_format().profile();
				ret.videoFormat.quantization = ev.video_format().quantization();
				ret.videoFormat.threadCount = ev.video_format().thread_count();
				   
				ret.cursor.data = std::vector<uint8_t>(ev.cursor().data().begin(), ev.cursor().data().end());
				ret.cursor.width = ev.cursor().width();
				ret.cursor.height = ev.cursor().height();
				ret.cursor.x = ev.cursor().x();
				ret.cursor.y = ev.cursor().y();
				ret.cursor.xhot = ev.cursor().hotspot_x();
				ret.cursor.yhot = ev.cursor().hotspot_y();
				ret.cursor.visible = ev.cursor().visible();

				t(std::move(ret));
			}

			if (data[0] == DISPLAY_EVENT)
			{
				::DisplayEvent ev;
				ev.ParseFromArray(data.data() + 1, data.size() - 1);
				Rd::DisplayEvent ret;

				if (ev.has_request())
				{
					Rd::DisplayEvent::Request v;
					v.name = ev.request().name();
					ret.val = v;
				}

				if (ev.has_response())
				{
					Rd::DisplayEvent::Response v;
					v.display.name = ev.response().display().name();
					for (int i = 0; i < ev.response().display().screens().size(); i++)
					{
						Rd::ScreenInfo s;
						s.id = ev.response().display().screens().Get(i).id();
						s.current.width = ev.response().display().screens().Get(i).current().width();
						s.current.height = ev.response().display().screens().Get(i).current().height();
						s.current.mwidth = ev.response().display().screens().Get(i).current().mwidth();
						s.current.mheight = ev.response().display().screens().Get(i).current().mheight();

						for (int j = 0; j < ev.response().display().screens().Get(i).resolutions().size(); j++)
						{
							Rd::Resolution r;
							r.width = ev.response().display().screens().Get(i).resolutions().Get(j).width();
							r.height = ev.response().display().screens().Get(i).resolutions().Get(j).height();
							r.mwidth = ev.response().display().screens().Get(i).resolutions().Get(j).mwidth();
							r.mheight = ev.response().display().screens().Get(i).resolutions().Get(j).mheight();
							s.resolutions.push_back(r);
						}
						v.display.screens.push_back(s);
					}
					ret.val = v;
				}


				if (ev.has_open())
				{
					Rd::DisplayEvent::Open v;
					v.display = ev.mutable_open()->display();
					v.screenId = ev.mutable_open()->screen_id();
					v.blockInput = ev.mutable_open()->block_input();
					v.videoFormat.bitarte = ev.mutable_open()->video_format().bitrate();
					v.videoFormat.id = (codec::VideoFormat::CodecId)ev.mutable_open()->video_format().id();
					v.videoFormat.h = ev.mutable_open()->video_format().h();
					v.videoFormat.w = ev.mutable_open()->video_format().w();
					v.videoFormat.dh = ev.mutable_open()->video_format().dh();
					v.videoFormat.dw = ev.mutable_open()->video_format().dw();
					v.videoFormat.profile = ev.mutable_open()->video_format().profile();
					v.videoFormat.quantization = ev.mutable_open()->video_format().quantization();
					v.videoFormat.threadCount = ev.mutable_open()->video_format().thread_count();
					ret.val = v;
				}


				if (ev.has_close())
				{
					Rd::DisplayEvent::Close v;
					v.display = ev.mutable_close()->display();
					v.screenId = ev.mutable_close()->screen_id();
					ret.val = v;
				}


				t(std::move(ret));
			}

			if (data[0] == ACTION_EVENT)
			{
				::ActionEvent ev;
				ev.ParseFromArray(data.data() + 1, data.size() - 1);
				Rd::ActionEvent ret;

				ev.set_screen_id(ret.screenId);

				if (ev.has_mouse())
				{
					Rd::ActionEvent::Mouse v;
					v.type = (Rd::ActionEvent::Mouse::Type)ev.mouse().type();
					v.x = ev.mouse().x();
					v.y = ev.mouse().y();
					v.pressed = ev.mouse().pressed();
					ret.val = v;
				}

				if (ev.has_keybd())
				{
					Rd::ActionEvent::Keybd v;
					v.extended = ev.keybd().extended();
					v.key = ev.keybd().key();
					v.pressed = ev.keybd().pressed();
					ret.val = v;
				}
				t(std::move(ret));
			}


			if (data[0] == CLIPBOARD_EVENT)
			{
				::ClipboardEvent ev;
				ev.ParseFromArray(data.data() + 1, data.size() - 1);
				Rd::ClipboardEvent ret;

				ret.text = ev.text();
				t(std::move(ret));
			}
		}
	};

}