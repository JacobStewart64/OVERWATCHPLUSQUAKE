#pragma once

#include <QUAKERWATCH/tcp_server.h>
#include <QUAKERWATCH/tcp_connection.h>
#include <boost/asio.hpp>
#include <functional>

void tcp_listen(boost::asio::io_service& io_service,
	int port,
	std::function<void(tcp_connection*, char*)> _message_handler)
{
	try
	{
		tcp_server server(io_service, port, _message_handler);
		io_service.run();
	}
	catch (std::exception& e)
	{
		perror(e.what());
	}
}