#include <ssh/types.hpp>
#include <ssh/bind.hpp>
#include <ssh/session.hpp>
#include <ssh/server_session.hpp>

namespace ssh {

void sshbind::incoming_connection_ext(ssh_bind b, void *userdata)
{
	sshbind *bnd = reinterpret_cast<sshbind *>(userdata);

	bnd->inc_sig_();
}

sshbind::sshbind(boost::asio::io_service &io, sp_server_auth auth) :
	  io_(io)
	, auth_(auth)
{
	bind_ = ssh_bind_new();

	if (!bind_) {
		throw std::bad_alloc();
	}
}

sshbind::~sshbind()
{
	inc_sig_.disconnect_all_slots();
	ssh_bind_free(bind_);
}

boost::signals2::connection sshbind::listen(const srv_incoming_connection::slot_type &sig)
{
	boost::signals2::connection conn = inc_sig_.connect(sig);
	if (ssh_bind_listen(bind_) != SSH_OK ) {
		conn.disconnect();
		throw ssh_exception(bind_);
	}

	return conn;
}

sp_srv_session sshbind::accept()
{
	sp_srv_session ses;

	try {
		ses = std::make_shared<ssh::srv_session>(io_, auth_);

		if (ssh_bind_accept(bind_, ses->get_session()) != SSH_OK) {
			throw ssh_exception(bind_);
		}
	} catch (...) {
		throw;
	}

	return ses;
}

void sshbind::option_set(enum ssh_bind_options_e type, const void *value)
{
	if (ssh_bind_options_set(bind_, type, value) != SSH_OK) {
		throw std::runtime_error("Couldn't set ssh bind option");
	}
}

ssh_bind sshbind::c_bind()
{
	return bind_;
}

void sshbind::set_blocking(bool is_blocking)
{
	ssh_bind_set_blocking(bind_, is_blocking ? 1 : 0);
}

} // namespace ssh
