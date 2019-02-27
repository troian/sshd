#include <ssh/session.hh>
#include <ssh/channel.hh>

//#include <libssh/callbacks.h>
//#include <libssh/libssh.h>

namespace ssh {

// --------------------------------------------------------------
// Implemenation of class session::set
// --------------------------------------------------------------
void session::set::option(ssh::options type, int32_t option) {
	if (ssh_options_set(_session, static_cast<enum ssh_options_e>(type), static_cast<const void *>(&option)) == SSH_ERROR) {
		throw ssh_exception(_session);
	}
}

void session::set::option(ssh::options type, const void *option) {
	if (ssh_options_set(_session, static_cast<enum ssh_options_e>(type), option) == SSH_ERROR) {
		throw ssh_exception(_session);
	}
}

// --------------------------------------------------------------
// Implemenation of class session::get
// --------------------------------------------------------------
ssh_session session::get::session() {
	return _session;
}

int session::get::status() {
	return 0;
}

int session::get::version() {
	return ssh_get_version(_session);
}

const char *session::get::error() {
	return ssh_get_error(_session);
}

int session::get::error_code() {
	return ssh_get_error_code(_session);
}

int session::get::openssh_version() {
	return 0;
}

// --------------------------------------------------------------
// Implemenation of class session::copy
// --------------------------------------------------------------
void session::copy::options(const session &src) {
	if (ssh_options_copy(src._session, &_session) == SSH_ERROR) {
		throw ssh_exception(_session);
	}
}

// --------------------------------------------------------------
// Implemenation of class session::parse
// --------------------------------------------------------------
void session::parse::options(std::string const &ops) {
	if (ssh_options_parse_config(_session, ops.c_str()) == SSH_ERROR) {
		throw ssh_exception(_session);
	}
}

// --------------------------------------------------------------
// Implemenation of class session
// --------------------------------------------------------------
int session::chan_data(ssh_channel c, void *data, uint32_t len, int is_stderr) {
	auto chan = _channels.at(c);

	if (!chan->status().is_open()) {
		LOG(WARNING) << "Channel is closed";
		return len;
	}

	return chan->on_data(data, len, is_stderr);
}

void session::chan_eof(ssh_channel c) {
	DEREF_IO(_io).post([this, c]() {
		chan_close(c);
	});
}

void session::chan_close(ssh_channel c) {
	LOGI_CLASS() << "Deleting channel";

	try {
		u_lock lock(_channels_lock);
		_channels.erase(c);
		_sig_chan(reinterpret_cast<uintptr_t>(c), false);
	} catch (const std::exception &e) {
		LOG(ERROR) << e.what();
	}
}

session::session(const std::string &log_name, boost_io::sp io, const session_signals &sig)
	: base_class(log_name)
	, channel_callbacks()
	, _io(io)
	, _session_alive(false)
	, _sig_dis()
	, _sig_chan()
	, _set()
	, _get()
	, _copy()
	, _parse()
{
	_session = ssh_new();
	if (_session == nullptr) {
		throw std::runtime_error("Couldn't allocate ssh object");
	}

//	_sig_conn = sig.conn;
	_sig_dis  = sig.dis;
	_sig_chan = sig.chan;

	_set._session   = _session;
	_get._session   = _session;
	_copy._session  = _session;
	_parse._session = _session;
}

session::session(boost_io::sp io, const session_signals &sig) :
	session("SSH.SESSION", io, sig)
{}

session::~session() {
	_session_alive.store(false);
	if (_fwd_acceptor.joinable()) {
		_fwd_acceptor.join();
	}

	// Cleaning up channels
	_channels.clear();

	if (ssh_is_connected(_session) == SSH_OK) {
		ssh_disconnect(_session);
	}

	ssh_free(_session);
}

void session::disconnect() {
	ssh_disconnect(_session);
}

void session::disconnect_silent() {
	ssh_silent_disconnect(_session);
}

void session::run_fwd_acceptor(std::string const &host, int port, bool multichannel) {
	_fwd_acceptor = std::thread(std::bind(&session::fwd_acceptor, this, host, port, multichannel));
}

void session::fwd_acceptor(const std::string &host, int port, bool multichannel)
{
	do {
		ssh_channel forward = ssh_channel_accept_forward(_session, 3000, nullptr);

		if (forward) {
			LOGI_CLASS() << "accepted channel. starting...";
			try {
				auto chan = std::make_shared<proxy_channel>(_io, shared_from_this(), forward, &_channel_cb, _sig_chan);

				u_lock lock(_channels_lock);
				_channels.insert(std::pair<ssh_channel, std::shared_ptr<class channel>>(forward, chan));
				LOGI_CLASS() << "inserted channel";

				chan->start(host, port);
			} catch (const std::exception &e) {
				ssh_channel_free(forward);
				LOGE_CLASS() << "Couldn't create channel: " << e.what();
				continue;
			}

			_sig_chan(reinterpret_cast<uintptr_t>(forward), true);
		}
	} while (_session_alive.load() && multichannel);
}

} // namespace ssh
