#include "stdafx.h"

#include <QUAKERWATCH/tcp_connection.h>
#include <QUAKERWATCH/tcp_listen.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>

boost::asio::io_service io_service;

void message_handler(tcp_connection* connection, char* msg);

bool match_password_from_db(const char* account,
	const char* password)
{
	boost::array<char, 128> response;

	boost::shared_ptr<tcp_connection> new_connection =
		tcp_connection::create(io_service, message_handler, 4321, "localhost");

	std::string packet("a");
	packet += account;
	packet += ":";
	packet += password;
	packet += ":";

	boost::system::error_code error;
	new_connection.get()->socket().write_some(boost::asio::buffer(packet), error);

	if (error)
		throw boost::system::system_error(error);

	new_connection.get()->socket().read_some(boost::asio::buffer(response), error);

	if (error)
		throw boost::system::system_error(error);

	if (response.data()[0] == 'a' && response.data()[1] == '0')
	{
		return true;
	}

	return false;
}

bool accountdb_create_account(const char* account,
	const char* password)
{
	boost::array<char, 128> response;

	boost::shared_ptr<tcp_connection> new_connection =
		tcp_connection::create(io_service, message_handler, 4321, "localhost");

	std::string packet("b");
	packet += account;
	packet += ":";
	packet += password;
	packet += ":";

	boost::system::error_code error;
	new_connection.get()->socket().write_some(boost::asio::buffer(packet), error);

	if (error)
		throw boost::system::system_error(error);

	new_connection.get()->socket().read_some(boost::asio::buffer(response), error);

	if (error)
		throw boost::system::system_error(error);

	if (response.data()[1] == '0')
	{
		return true;
	}

	return false;
}

void message_handler(tcp_connection* connection, char* msg)
{
	if (msg[0] == 'a')
	{
		perror("got the login packet");
		perror(msg);
		std::string account;
		std::string password;
		std::string gameserver;
		int i = 1;
		int last_i = i;
		for (; msg[i] != ':'; ++i);
		account = std::string(&msg[last_i], i - last_i);
		++i;
		last_i = i;
		for (; msg[i] != ':'; ++i);
		password = std::string(&msg[last_i], i - last_i);
		++i;
		last_i = i;
		for (; msg[i] != ':'; ++i);
		gameserver = std::string(&msg[last_i], i - last_i);

		perror(account.c_str());
		perror(password.c_str());
		perror(gameserver.c_str());

		if (match_password_from_db(account.c_str(), password.c_str()))
		{
			//and tell the client
			connection->post_send("a0");
			//tell the game server
		}
		else
		{
			perror(account.c_str());
			perror("invalid password");
			connection->post_send("a1");
		}
	}
	else if (msg[0] == 'b')
	{
		perror("got the create account packet");
		perror(msg);
		std::string account;
		std::string password;
		int i = 1;
		int last_i = i;
		for (; msg[i] != ':'; ++i);
		account = std::string(&msg[last_i], i - last_i);
		++i;
		last_i = i;
		for (; msg[i] != ':'; ++i);
		password = std::string(&msg[last_i], i - last_i);

		perror(account.c_str());
		perror(password.c_str());

		if (accountdb_create_account(account.c_str(), password.c_str()))
		{
			//inform the client of success
			connection->post_send("b0");
		}
		else
		{
			perror(account.c_str());
			perror("failed to create account");
			connection->post_send("b1");
		}
	}
	else
	{
		perror("msg failure!");
	}
}

int main()
{
	tcp_listen(io_service, 1234, message_handler);
	return 0;
}

