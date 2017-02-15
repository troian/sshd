#include <ssh/session.hpp>
#include <ssh/channel.hpp>

//#include <libssh/callbacks.h>
//#include <libssh/libssh.h>

namespace ssh {

// --------------------------------------------------------------
// Implemenation of class session::set
// --------------------------------------------------------------
void session::set::option(ssh::options type, long int option)
{
	if (ssh_options_set(session_, (enum ssh_options_e)type, (const void *)&option) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}

void session::set::option(ssh::options type, const void *option)
{
	if (ssh_options_set(session_, (enum ssh_options_e)type, option) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}

// --------------------------------------------------------------
// Implemenation of class session::get
// --------------------------------------------------------------
ssh_session session::get::session()
{
	return session_;
}

int session::get::status()
{
	return 0;
}

int session::get::version()
{
	return ssh_get_version(session_);
}

const char *session::get::error()
{
	return ssh_get_error(session_);
}

int session::get::error_code()
{
	return ssh_get_error_code(session_);
}

int session::get::openssh_version()
{
	return 0;
}

// --------------------------------------------------------------
// Implemenation of class session::copy
// --------------------------------------------------------------
void session::copy::options(const session &src)
{
	if (ssh_options_copy(src.session_, &session_) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}

// --------------------------------------------------------------
// Implemenation of class session::parse
// --------------------------------------------------------------
void session::parse::options(std::string const &ops)
{
	if (ssh_options_parse_config(session_, ops.c_str()) == SSH_ERROR) {
		throw ssh_exception(session_);
	}
}

// --------------------------------------------------------------
// Implemenation of class session
// --------------------------------------------------------------
int session::chan_data(ssh_channel c, void *data, uint32_t len, int is_stderr)
{
	sp_channel chan = (std::shared_ptr<channel> &&)channels_.at(c);

	if (!chan->status().is_open()) {
		LOG(WARNING) << "Channel is closed";
		return len;
	}

	return chan->on_data(data, len, is_stderr);
}

void session::chan_eof(ssh_channel c)
{
	DEREF_IO(io_).post([this, c]() {
		chan_close(c);
	});
}

void session::chan_close(ssh_channel c)
{
	LOGI_CLASS() << "Deleting channel";

	try {
		u_lock lock(channels_lock_);
		channels_.erase(c);
		sig_chan_((uintptr_t)c, false);
	} catch (const std::exception &e) {
		LOG(ERROR) << e.what();
	}
}

session::session(const std::string &log_name, sp_boost_io io, const session_signals &sig) :
	  base_class(log_name)
	, channel_callbacks()
	, io_(io)
	, session_alive_(false)
{
	session_ = ssh_new();
	if (session_ == NULL) {
		throw std::runtime_error("Couldn't allocate ssh object");
	}

//	sig_conn_ = sig.conn;
	sig_dis_  = sig.dis;
	sig_chan_ = sig.chan;

	set_.session_   = session_;
	get_.session_   = session_;
	copy_.session_  = session_;
	parse_.session_ = session_;
}

session::session(sp_boost_io io, const session_signals &sig) :
	session("SSH.SESSION", io, sig)
{}

session::~session()
{
	session_alive_.store(false, std::memory_order_release);
	if (fwd_acceptor_.joinable()) {
		fwd_acceptor_.join();
	}

	// Cleaning up channels
	channels_.clear();

	if (ssh_is_connected(session_) == SSH_OK) {
		ssh_disconnect(session_);
	}

	ssh_free(session_);
}

void session::disconnect()
{
	ssh_disconnect(session_);
}

void session::disconnect_silent()
{
	ssh_silent_disconnect(session_);
}

void session::run_fwd_acceptor(std::string const &host, int port, bool multichannel)
{
	fwd_acceptor_ = std::thread(std::bind(&session::fwd_acceptor, this, host, port, multichannel));
}

void session::fwd_acceptor(const std::string &host, int port, bool multichannel)
{
	while (session_alive_.load(std::memory_order_acquire) && multichannel) {
		ssh_channel forward = ssh_channel_accept_forward(session_, 3000, NULL);

		if (forward) {
			sp_channel chan;

			try {
				chan = proxy_channel::shared(io_, this, host, port, forward, &channel_cb_, sig_chan_);
			} catch (const std::exception &e) {
				ssh_channel_free(forward);
				LOGE_CLASS() << "Couldn't create channel: " << e.what();
				continue;
			}

			u_lock lock(channels_lock_);
			channels_.insert(std::pair<ssh_channel, sp_channel>(forward, chan));

			sig_chan_((uintptr_t) forward, true);
		}
	}
}

} // namespace ssh
