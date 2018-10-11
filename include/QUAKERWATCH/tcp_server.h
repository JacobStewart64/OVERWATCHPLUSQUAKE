#pragma once

#include <QUAKERWATCH/tcp_connection.h>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <functional>

class tcp_server
{
public:
	tcp_server(boost::asio::io_service& io_service, int port, std::function<void(tcp_connection*, char*)> _message_handler)
		: acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
		message_handler_(_message_handler)
	{
		post_accept();
	}

private:
	void post_accept()
	{
		boost::shared_ptr<tcp_connection> new_connection =
			tcp_connection::create(acceptor_.get_io_service(), message_handler_);

		acceptor_.async_accept(new_connection->socket(),
			boost::bind(&tcp_server::handle_accept, this, new_connection,
				boost::asio::placeholders::error));
	}

	void handle_accept(boost::shared_ptr<tcp_connection> new_connection,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			//this is login server
			//they just connected
			//this means they are going to send
			//login data
			//post a receive to handle their login data
			perror("got connection from client!");
			new_connection->post_receive();
			post_accept();
		}
	}

	std::function<void(tcp_connection*, char*)> message_handler_;
	boost::asio::ip::tcp::acceptor acceptor_;
};