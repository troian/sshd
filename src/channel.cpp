#include <ssh/channel.hh>
#include <ssh/session.hh>

namespace ssh {

//#define BUF_SIZE 1024

ssh_channel channel::get::channel() {
	return _channel;
}

bool channel::status::is_open() {
	return ssh_channel_is_open(_channel) != 0;
}

bool channel::status::is_eof() {
	return ssh_channel_is_eof(_channel) != 0;
}

// --------------------------------------------------------------
// Implementation of class channel
// --------------------------------------------------------------
channel::channel(
			  boost_io::sp io
			, const std::string &log_name
			, std::shared_ptr<session> s
			, ssh_channel c
			, ssh_channel_callbacks cb
			, chan_conn_signal conn_sig)
	: base_class(log_name)
	, _session(s)
	, _channel(c)
	, _foreign_chan(c != nullptr)
	, _io(io)
	, _conn_sig(conn_sig)
	, _get()
	, _status()
{
	if (!_foreign_chan) {
		_channel = ssh_channel_new(s->get().session());
		if (_channel == nullptr) {
			throw std::runtime_error("Couldn't allocate ssh channel");
		}
	}

	if (cb) {
		if (ssh_set_channel_callbacks(_channel, cb) != SSH_OK) {
			if (!_foreign_chan) {
				// Free channel if we created the one
				ssh_channel_free(_channel);
			}
			throw ssh_exception(_session->get().session());
		}
	}

	_get._channel    = _channel;
	_status._channel = _channel;
}

channel::channel(boost_io::sp io, std::shared_ptr<session> s, ssh_channel c, ssh_channel_callbacks cb, chan_conn_signal conn_sig) :
	channel(io, "SSH.CHANNEL", s, c, cb, conn_sig)
{}

channel::~channel() {
	if (ssh_channel_is_eof(_channel) == 0) {
		ssh_channel_send_eof(_channel);

		// Notify channel closed
		if (_conn_sig) {
			auto dis = [](uintptr_t id, chan_conn_signal conn_sig) {
				conn_sig(id, false);
			};

			DEREF_IO(_io).post(boost::bind<void>(dis, reinterpret_cast<uintptr_t>(_channel), _conn_sig));
		}
	}

	if (!_foreign_chan) {
		ssh_channel_free(_channel);
	}
}

int channel::write(const void *data, size_t len, bool is_stderr) {
	int ret;

	if (is_stderr) {
		ret = ssh_channel_write_stderr(_channel, data, len);
	} else {
		ret = ssh_channel_write(_channel, data, len);
	}

	if (ret == SSH_ERROR) {
		throw ssh_exception(_session->get().session());
	}

	return ret;
}

size_t channel::on_data(void *data, uint32_t len, int is_stderr) {
//	if (pty_)
//		return ::write(pty_->fd(), (char *)data, len);
	return 0;
}

} // namespace ssh
