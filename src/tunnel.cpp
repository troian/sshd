/**
 * \file
 *
 * \author
 *
 * \Copyright(c)  "Artur Troian <troian dot ap at gmail dot com>"
 */

#include <poll.h>

#include <ssh/tunnel.hpp>
#include <ssh/client_session.hpp>
#include <boost/asio.hpp>

namespace ssh {

ssh_tunnel::ssh_tunnel(
				  boost::asio::io_service &io
				, const std::string &host
				, int port
				, const std::string &user
				, int timeout) :
	  base_class("SSH.TUNNEL")
	, io_(io)
{
	try {
		session_signals sig;

		session_ = std::make_shared<ssh::client_session>(io_, sig);

		session_->set_option(SSH_OPTIONS_HOST, host.c_str());
		session_->set_option(SSH_OPTIONS_USER, user.c_str());
		session_->set_option(SSH_OPTIONS_PORT, &port);
		session_->set_option(SSH_OPTIONS_TIMEOUT, &timeout);
	} catch (...) {
	    throw;
	}
}

ssh_tunnel::ssh_tunnel(
				  boost::asio::io_service &io
				, const std::string &host
				, int port
				, const std::string &user
				, const std::string &pass
				, int timeout) :
	ssh_tunnel(io, host, port, user, timeout)
{
	try {
		session_->connect();
	} catch (...) {
		throw;
	}
}

ssh_tunnel::ssh_tunnel(
				  boost::asio::io_service &io
				, const std::string &host
				, int port
				, const std::string &user
				, sp_ssh_key_pair key
				, int timeout) :
	ssh_tunnel(io, host, port, user, timeout)
{
	try {
		session_->connect();
		session_->auth_pubkey(key);
	} catch (const std::exception &e) {
		throw;
	}
}

ssh_tunnel::~ssh_tunnel()
{
	session_->disconnect();

	session_.reset();
}

void ssh_tunnel::create_reverse(const std::string &host, int port, const std::string &fwd_host, int bind_port, bool multichannel)
{
	try {
		session_->listen_forward(host, port, fwd_host, bind_port, multichannel);
	} catch (...) {
		throw;
	}
}

void ssh_tunnel::cancel()
{
	session_->disconnect();
}

} // namespace ssh
