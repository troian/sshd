//
// Created by Artur Troian on 10/9/16.
//

#include <ssh/client_session.hh>
#include <ssh/channel.hh>

namespace ssh {

void client_session::auth::none() {
	if (ssh_userauth_none(_session, nullptr) == SSH_ERROR) {
		throw ssh_exception(_session);
	}
}

void client_session::auth::pass(std::string const &pass) {
	if (ssh_userauth_password(_session, nullptr, pass.c_str()) != SSH_OK) {
		throw ssh_exception(_session);
	}
}

void client_session::auth::pubkey(key_pair::sp key) {
	if (ssh_userauth_publickey(_session, nullptr, key->priv().get()) != SSH_OK) {
		throw ssh_exception(_session);
	}
}

void client_session::auth::try_pubkey(ssh_key pubkey) {
	if (ssh_userauth_try_publickey(_session, nullptr, pubkey) != SSH_OK) {
		throw ssh_exception(_session);
	}
}

void client_session::auth::pubkey_auto() {
	if (ssh_userauth_publickey_auto(_session, nullptr, nullptr) != SSH_OK) {
		throw ssh_exception(_session);
	}
}

// --------------------------------------------------------------
// Implemenation of class client_session
// --------------------------------------------------------------
client_session::client_session(boost_io::sp io, const session_signals &sig, int keep_alive)
	: session("SSH.CLIENT", io, sig)
	, _keep_alive_interval(keep_alive)
	, _keep_alive_timer(DEREF_IO(io))
	, _auth()
{
	_auth._session = _session;
}

client_session::~client_session() {
	_keep_alive_timer.cancel();
}

void client_session::connect() {
	int rc = ssh_connect(_session);

	if (rc != SSH_OK) {
		throw ssh_exception(_session);
	}

	_session_alive.store(true);

	if (_keep_alive_interval > boost::posix_time::seconds(0)) {
		_keep_alive_timer.expires_from_now(_keep_alive_interval);
		_keep_alive_timer.async_wait(boost::bind(&client_session::keep_alive, client_session::shared_from_this(), boost::asio::placeholders::error));
	}
}

void client_session::write_known_hosts() {
	if (ssh_write_knownhost(_session) == SSH_ERROR) {
		throw ssh_exception(_session);
	}
}

std::string client_session::remote_banner() {
	std::string banner(ssh_get_serverbanner(_session));

	return banner;
}

bool client_session::listen_forward(const std::string &host, int port, const std::string &fwd_host, int fwd_port, bool multichannel) {
	if (ssh_channel_listen_forward(_session, host.c_str(), port, nullptr) == SSH_ERROR) {
		throw ssh::ssh_exception(_session);
	} else {
		run_fwd_acceptor(fwd_host, fwd_port, multichannel);
	}

	return true;
}

void client_session::keep_alive(const boost::system::error_code& ec) {
	if (ec != boost::asio::error::operation_aborted) {
		if (ssh_send_keepalive(_session) != SSH_OK) {
			LOGE_CLASS() << get().error();
		} else {
			_keep_alive_timer.expires_from_now(_keep_alive_interval);
			_keep_alive_timer.async_wait(boost::bind(&client_session::keep_alive, this, boost::asio::placeholders::error));
		}
	}
}

} // namespace ssh
