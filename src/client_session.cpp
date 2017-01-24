//
// Created by Artur Troian on 10/9/16.
//

#include <ssh/client_session.hpp>
#include <ssh/channel.hpp>

namespace ssh {

void client_session::auth::none()
{
	if (ssh_userauth_none(session_, NULL) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}

void client_session::auth::pass(std::string const &pass)
{
	if (ssh_userauth_password(session_, NULL, pass.c_str()) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}

void client_session::auth::pubkey(sp_key_pair key)
{
	if (ssh_userauth_publickey(session_, NULL, key->priv()) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}

void client_session::auth::try_pubkey(ssh_key pubkey)
{
	if (ssh_userauth_try_publickey(session_, NULL, pubkey) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}

void client_session::auth::pubkey_auto()
{
	if (ssh_userauth_publickey_auto(session_, NULL, NULL) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}
// --------------------------------------------------------------
// Implemenation of class client_session
// --------------------------------------------------------------
client_session::client_session(sp_boost_io io, const session_signals &sig, int keep_alive) :
	  session("SSH.CLIENT", io, sig)
	, keep_alive_interval_(keep_alive)
	, keep_alive_timer_(DEREF_IO(io))
{
	auth_.session_ = session_;
}

client_session::~client_session()
{
	keep_alive_timer_.cancel();
}

void client_session::connect()
{
	int rc = ssh_connect(session_);

	if (rc != SSH_OK) {
		throw ssh_exception(session_);
	}

	session_alive_.store(true);

	if (keep_alive_interval_ > boost::posix_time::seconds(0)) {
		keep_alive_timer_.expires_from_now(keep_alive_interval_);
		keep_alive_timer_.async_wait(boost::bind(&client_session::keep_alive, this, boost::asio::placeholders::error));
	}
}

void client_session::write_known_hosts()
{
	if (ssh_write_knownhost(session_) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}

std::string client_session::remote_banner()
{
	std::string banner(ssh_get_serverbanner(session_));

	return std::move(banner);
}

bool client_session::listen_forward(const std::string &host, int port, const std::string &fwd_host, int fwd_port, bool multichannel)
{
	if (ssh_channel_listen_forward(session_, host.c_str(), port, NULL) == SSH_ERROR) {
		throw ssh::ssh_exception(session_);
	} else {
		run_fwd_acceptor(fwd_host, fwd_port, multichannel);
	}

	return true;
}

void client_session::keep_alive(const boost::system::error_code& ec)
{
	if (ec != boost::asio::error::operation_aborted) {
		if (ssh_send_keepalive(session_) != SSH_OK) {
			LOGE_CLASS() << get().error();
		} else {
			keep_alive_timer_.expires_from_now(keep_alive_interval_);
			keep_alive_timer_.async_wait(boost::bind(&client_session::keep_alive, this, boost::asio::placeholders::error));
		}
	}
}

} // namespace ssh
