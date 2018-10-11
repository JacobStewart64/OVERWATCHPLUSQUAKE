#pragma once

#include "wcstrtostdstr.h"
#include "Validator.h"
#include "LOOPDATA.h"
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <irrlicht.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>

extern boost::asio::io_service io_service;

class LoginEventReceiver : public irr::IEventReceiver, private Validator
{
	irr::gui::IGUIStaticText* statusmsg;
	irr::gui::IGUIListBox* gameserverlist;
	irr::gui::IGUIEditBox* email;
	irr::gui::IGUIEditBox* password;
	irr::gui::IGUIButton* loginbutton;
	irr::gui::IGUIButton* createaccountbutton;
	irr::video::ITexture* title;
	irr::gui::IGUIFont* font;
	std::unordered_map<unsigned int, std::string> id_to_game_server_name_map;
	irr::video::ITexture* bg;
	LOOP_PARAM_PACK& LOOP_PACK;

	std::string getGameServerNameFromSelected()
	{
		unsigned int id = gameserverlist->getSelected();
		if (id_to_game_server_name_map.find(id) != id_to_game_server_name_map.end())
		{
			std::string selected_game_server_name = id_to_game_server_name_map.at(id);
			return selected_game_server_name;
		}
		exit(EXIT_FAILURE);
	}

	void setupGameServerMenu()
	{
		std::ifstream gameserverini("./resources/Client/gameserver.ini");
		std::string line; getline(gameserverini, line);
		wchar_t buf[32];
		mbstowcs(buf, line.c_str(), line.size());
		unsigned int id = gameserverlist->addItem(buf);
		id_to_game_server_name_map.insert(std::make_pair(id, line));
		gameserverlist->setSelected(buf);
		for (std::string line; getline(gameserverini, line);)
		{
			wchar_t buf[32] = { 0 };
			mbstowcs(buf, line.c_str(), line.size());
			irr::u32 id = gameserverlist->addItem(buf);
			id_to_game_server_name_map.insert(make_pair(id, line));
		}
		gameserverini.close();
	}

	boost::array<char, 128>&& login(const char* host, const char* port,
		const char* account_name, const char* password, const char* game_server_name)
	{
		//buffer that will hold response
		boost::array<char, 128> response;

		try
		{
			//create socket and connect
			boost::asio::ip::tcp::resolver resolver(io_service);
			boost::asio::ip::tcp::resolver::query query(host, port);
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			boost::asio::ip::tcp::resolver::iterator end;

			boost::asio::ip::tcp::socket socket(io_service);
			boost::system::error_code error = boost::asio::error::host_not_found;
			while (error && endpoint_iterator != end)
			{
				socket.close();
				socket.connect(*endpoint_iterator++, error);
			}
			if (error)
				throw boost::system::system_error(error);


			//send login request
			std::string buf("a");
			buf += account_name;
			buf += ":";
			buf += password;
			buf += ":";
			buf += game_server_name;
			buf += ":";

			socket.write_some(boost::asio::buffer(buf), error);

			if (error)
				throw boost::system::system_error(error);

			//receive response
			socket.read_some(boost::asio::buffer(response), error);

			if (error)
				throw boost::system::system_error(error); // Some other error.
		}
		catch (std::exception& e)
		{
			perror(e.what());
		}

		return std::move(response);
	}

	bool create_account(const char* host, const char* port,
		const char* account_name, const char* password, const char* game_server_name)
	{
		//buffer that will hold response
		boost::array<char, 128> response;

		try
		{
			boost::asio::ip::tcp::resolver resolver(io_service);
			boost::asio::ip::tcp::resolver::query query(host, port);
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			boost::asio::ip::tcp::resolver::iterator end;

			boost::asio::ip::tcp::socket socket(io_service);
			boost::system::error_code error = boost::asio::error::host_not_found;
			while (error && endpoint_iterator != end)
			{
				socket.close();
				socket.connect(*endpoint_iterator++, error);
			}
			if (error)
				throw boost::system::system_error(error);

			//send cracc request
			std::string buf("b");
			buf += account_name;
			buf += ":";
			buf += password;
			buf += ":";

			socket.write_some(boost::asio::buffer(buf), error);

			if (error)
				throw boost::system::system_error(error);

			//receive response
			socket.read_some(boost::asio::buffer(response), error);

			if (error)
				throw boost::system::system_error(error); // Some other error.
		}
		catch (std::exception& e)
		{
			perror(e.what());
			return false;
		}

		perror("create account");
		perror(response.data());

		if (response.data()[0] == 'b' && response.data()[1] == '0')
		{
			return true;
		}
		
		return false;
	}

	void loop()
	{
		while (LOOP_PACK.device->run() && LOOP_PACK.driver && LOOP_PACK.runloop)
		{
			LOOP_PACK.driver->beginScene(true, true, irr::video::SColor(0, 120, 102, 136));

			LOOP_PACK.driver->draw2DImage(bg, irr::core::position2d<irr::s32>(0, 0),
				irr::core::rect<irr::s32>(0, 0, 800, 900), 0,
				irr::video::SColor(255, 255, 255, 255), false);

			LOOP_PACK.driver->draw2DImage(title, irr::core::position2d<irr::s32>(50, 10),
				irr::core::rect<irr::s32>(0, 0, 700, 230), 0,
				irr::video::SColor(255, 255, 255, 255), true);

			LOOP_PACK.env->drawAll();

			font->draw(L"email",
				irr::core::rect<irr::s32>(350, 330, 450, 370),
				irr::video::SColor(255, 0, 0, 0), true);

			font->draw(L"password",
				irr::core::rect<irr::s32>(350, 420, 450, 460),
				irr::video::SColor(255, 0, 0, 0), true);

			LOOP_PACK.driver->endScene();
		}

		LOOP_PACK.device->setEventReceiver(nullptr);
	}

public:
	LoginEventReceiver(LOOP_PARAM_PACK& _LOOP_PACK)
		: LOOP_PACK(_LOOP_PACK),
		statusmsg(_LOOP_PACK.env->addStaticText(L"Hello!", irr::core::rect<irr::s32>(150, 290, 650, 320), false, false, 0, -1, false)),
		gameserverlist(gameserverlist = _LOOP_PACK.env->addListBox(irr::core::rect<irr::s32>(300, 210, 500, 290), 0, 1)),
		email(_LOOP_PACK.env->addEditBox(L"", irr::core::rect<irr::s32>(300, 320, 500, 390), true, 0, 1)),
		password(_LOOP_PACK.env->addEditBox(L"", irr::core::rect<irr::s32>(300, 410, 500, 480), true, 0, 2)),
		loginbutton(_LOOP_PACK.env->addButton(irr::core::rect<irr::s32>(350, 500, 450, 530), 0, 1, L"Login", L"Login")),
		createaccountbutton(_LOOP_PACK.env->addButton(irr::core::rect<irr::s32>(350, 540, 450, 570), 0, 2, L"Make Account", L"Make Account")),
		title(_LOOP_PACK.driver->getTexture("./resources/Client/GoldenAgeTitle.png")),
		font(_LOOP_PACK.device->getGUIEnvironment()->getBuiltInFont()),
		bg(_LOOP_PACK.driver->getTexture("./resources/Client/kewlbg.jpg"))
	{
		statusmsg->setTextAlignment(irr::gui::EGUI_ALIGNMENT::EGUIA_CENTER, irr::gui::EGUI_ALIGNMENT::EGUIA_CENTER);
		email->setTextAlignment(irr::gui::EGUI_ALIGNMENT::EGUIA_CENTER, irr::gui::EGUI_ALIGNMENT::EGUIA_CENTER);
		password->setTextAlignment(irr::gui::EGUI_ALIGNMENT::EGUIA_CENTER, irr::gui::EGUI_ALIGNMENT::EGUIA_CENTER);

		setupGameServerMenu();

		LOOP_PACK.mode = LOOPMODE::QUIT;
		LOOP_PACK.runloop = true;
		LOOP_PACK.device->setEventReceiver(this);
		loop();
	}

	virtual bool OnEvent(const irr::SEvent& event)
	{
		if (event.EventType == irr::EET_GUI_EVENT)
		{
			irr::s32 id = event.GUIEvent.Caller->getID();
			if (event.GUIEvent.EventType == irr::gui::EGET_EDITBOX_CHANGED)
			{
				if (id == 1)
				{
					validateInput(email);
				}
				else if (id == 2)
				{
					validateInput(password);
				}
			}
			else if (event.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED)
			{
				if (id == 1)
				{
					std::string email_str = wcstrtostdstr(email->getText());
					std::string password_str = wcstrtostdstr(password->getText());
					std::string game_server_str = getGameServerNameFromSelected();
					boost::array<char, 128> response = login("localhost",
						"1234",
						email_str.c_str(),
						password_str.c_str(),
						game_server_str.c_str());
					//response is my security stuff to send to game server to establish connection
					perror(response.data());
					if (response.data()[0] == 'a')
					{
						if (response.data()[1] == '0')
						{
							perror("success login ~!~");
						}
					}
				}
				else if (id == 2)
				{
					std::string email_str = wcstrtostdstr(email->getText());
					std::string password_str = wcstrtostdstr(password->getText());
					std::string game_server_str = getGameServerNameFromSelected();
					if (create_account("localhost",
						"1234",
						email_str.c_str(),
						password_str.c_str(),
						game_server_str.c_str()))
					{
						perror("create account success");
					}
					else
					{
						perror("create account failure");
					}
				}
				else
				{
					perror("did you forget to add to the event receiver?");
				}
			}
		}
		return false;
	}

};