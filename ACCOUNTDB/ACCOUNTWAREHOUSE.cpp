// ACCOUNTDB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <QUAKERWATCH/tcp_connection.h>
#include <QUAKERWATCH/tcp_listen.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <fstream>
#include <string>

boost::asio::io_service io_service;

//perhaps we will need to respond to the type of error
//from getlasterror to get the behavior we want
bool is_file_exist(const wchar_t* file_path)
{
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(file_path))
	{
		return false;
	}

	return true;
}

void message_handler(tcp_connection* connection, char* msg);

//reading file from client chosen stringzor?????????????????????????
bool match_password(const char* account, const char* password)
{
	std::wstring path(L"./resources/AccountDB/");
	wchar_t buf[17] = { 0 };
	mbstowcs(buf, account, strlen(account));
	path += buf;
	path += L".ini";

	if (is_file_exist(path.c_str()))
	{
		std::ifstream passfile(path.c_str());
		char buf[17] = { 0 };
		passfile >> buf;
		if (strcmp(password, buf) == 0)
		{
			return true;
		}
	}

	return false;
}

bool create_account(const char* account,
	const char* password)
{
	std::wstring path(L"./resources/AccountDB/");
	wchar_t buf[17] = { 0 };
	mbstowcs(buf, account, strlen(account));
	path += buf;
	path += L".ini";
	if (is_file_exist(path.c_str()))
	{
		return false;
	}

	std::ofstream new_account(path.c_str());
	new_account << password;
	new_account.close();

	return true;
}

void message_handler(tcp_connection* connection, char* msg)
{
	if (msg[0] == 'a')
	{
		perror("got the login packet");
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

		if (match_password(account.c_str(), password.c_str()))
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

		if (create_account(account.c_str(), password.c_str()))
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
	tcp_listen(io_service, 4321, message_handler);
	return 0;
}

