#include <ssh/channel.hpp>
#include <ssh/session.hpp>

namespace ssh {

//#define BUF_SIZE 1024

ssh_channel channel::get::channel() {
	return channel_;
}

bool channel::status::is_open()
{
	return ssh_channel_is_open(channel_) != 0;
}

bool channel::status::is_eof()
{
	return ssh_channel_is_eof(channel_) != 0;
}

// --------------------------------------------------------------
// Implementation of class channel
// --------------------------------------------------------------
channel::channel(
			  sp_boost_io io
			, const std::string &log_name
			, session *s
			, ssh_channel c
			, ssh_channel_callbacks cb
			, chan_conn_signal conn_sig) :
	  base_class(log_name)
	, session_(s)
	, channel_(c)
	, foreign_chan_(c != nullptr)
	, io_(io)
	, conn_sig_(conn_sig)
{
	if (!foreign_chan_) {
		channel_ = ssh_channel_new(s->get().session());
		if (channel_ == NULL) {
			throw std::runtime_error("Couldn't allocate ssh channel");
		}
	}

	if (cb) {
		if (ssh_set_channel_callbacks(channel_, cb) != SSH_OK) {
			if (!foreign_chan_) {
				// Free channel if we created the one
				ssh_channel_free(channel_);
			}
			throw ssh_exception(session_->get().session());
		}
	}

	get_.channel_    = channel_;
	status_.channel_ = channel_;
}

channel::channel(sp_boost_io io, session *s, ssh_channel c, ssh_channel_callbacks cb, chan_conn_signal conn_sig) :
	channel(io, "SSH.CHANNEL", s, c, cb, conn_sig)
{}

channel::~channel()
{
	if (ssh_channel_is_eof(channel_) == 0) {
		ssh_channel_send_eof(channel_);

		// Notify channel closed
		if (conn_sig_) {
			auto dis = [](uintptr_t id, chan_conn_signal conn_sig) {
				conn_sig(id, false);
			};

			DEREF_IO(io_).post(boost::bind<void>(dis, (uintptr_t)channel_, conn_sig_));
		}
	}

	if (!foreign_chan_) {
		ssh_channel_free(channel_);
	}
}

int channel::write(const void *data, size_t len, bool is_stderr)
{
	int ret;

	if (is_stderr) {
		ret = ssh_channel_write_stderr(channel_, data, len);
	} else {
		ret = ssh_channel_write(channel_, data, len);
	}

	if (ret == SSH_ERROR) {
		throw ssh_exception(session_->get().session());
	}

	return ret;
}

int channel::on_data(void *data, uint32_t len, int is_stderr)
{
//	if (pty_)
//		return ::write(pty_->fd(), (char *)data, len);
	return 0;
}

} // namespace ssh
