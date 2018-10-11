#pragma once

#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/beast/core/multi_buffer.hpp>

#define MAX_PACKET_LEN 2048

class tcp_connection
	: public boost::enable_shared_from_this<tcp_connection>
{
public:
	static boost::shared_ptr<tcp_connection> create(boost::asio::io_service& io_service)
	{
		return boost::shared_ptr<tcp_connection>(new tcp_connection(io_service));
	}

	boost::asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

	void post_receive()
	{
		socket_.async_read_some(boost::asio::buffer(message_, MAX_PACKET_LEN),
			boost::bind(&tcp_connection::handle_receive, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void post_send()
	{
		socket_.async_write_some(boost::asio::buffer(message_, MAX_PACKET_LEN),
			boost::bind(&tcp_connection::handle_send, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

private:
	tcp_connection(boost::asio::io_service& io_service)
		: socket_(io_service)
	{
	}

	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			//switch over the message type, simple message for noa
			if (message_[0] == 'a')
			{
				//login packet
				perror("we got the login packet!");
			}
			perror("we received a packet from client!");
			post_receive();
		}
		else
		{
			perror("receive error!!!!!!");
		}
	}

	void handle_send(const boost::system::error_code& ec, size_t bytes_transferred)
	{
		post_receive();
	}

	boost::asio::ip::tcp::socket socket_;
	char message_[MAX_PACKET_LEN];
};