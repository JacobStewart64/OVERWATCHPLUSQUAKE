#pragma once

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <functional>

#define MAX_PACKET_LEN 1024

class tcp_connection
	: public boost::enable_shared_from_this<tcp_connection>
{
public:
	static boost::shared_ptr<tcp_connection> create(boost::asio::io_service& io_service,
		std::function<void(tcp_connection*, char*)> _message_handler)
	{
		return boost::shared_ptr<tcp_connection>(new tcp_connection(io_service, _message_handler));
	}

	static boost::shared_ptr<tcp_connection> create(boost::asio::io_service& io_service,
		std::function<void(tcp_connection*, char*)> _message_handler,
		int port,
		std::string host)
	{
		return boost::shared_ptr<tcp_connection>(new tcp_connection(io_service,
			_message_handler,
			port,
			host));
	}

	boost::asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

	void connect(boost::asio::io_service& io_service,
		int port,
		std::string host)
	{
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;

		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			socket_.close();
			socket_.connect(*endpoint_iterator++, error);
		}
		if (error)
			throw boost::system::system_error(error);
	}

	void post_receive()
	{
		socket_.async_read_some(boost::asio::buffer(message_, MAX_PACKET_LEN),
			boost::bind(&tcp_connection::handle_receive, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void post_send(const char* msg)
	{
		socket_.async_write_some(boost::asio::buffer(msg, MAX_PACKET_LEN),
			boost::bind(&tcp_connection::handle_send, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

private:
	tcp_connection(boost::asio::io_service& io_service, std::function<void(tcp_connection*, char*)> _message_handler)
		: socket_(io_service), message_handler_(_message_handler)
	{
	}

	tcp_connection(boost::asio::io_service& io_service,
		std::function<void(tcp_connection*, char*)> _message_handler,
		int port,
		std::string host)
		: socket_(io_service), message_handler_(_message_handler)
	{
		connect(io_service, port, host);
	}

	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			message_handler_(this, message_);
		}
		else
		{
			perror("receive error!!!!!!");
		}
	}

	void handle_send(const boost::system::error_code& ec, size_t bytes_transferred)
	{
	}

	std::function<void(tcp_connection*, char*)> message_handler_;
	boost::asio::ip::tcp::socket socket_;
	char message_[MAX_PACKET_LEN];
};