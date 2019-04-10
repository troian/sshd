//
// Created by Artur Troian on 1/19/17.
//

#include <ssh/session.hh>

namespace ssh {

// --------------------------------------------------------------
// Implementation of class channel_callbacks
// --------------------------------------------------------------
channel_callbacks::channel_callbacks() {
	_channel_cb.userdata                           = this;
	_channel_cb.channel_data_function              = chan_data_cb_ext;
	_channel_cb.channel_eof_function               = chan_eof_cb_ext;
	_channel_cb.channel_close_function             = chan_close_cb_ext;
	_channel_cb.channel_signal_function            = chan_sig_cb_ext;
	_channel_cb.channel_exit_status_function       = chan_exit_status_cb_ext;
	_channel_cb.channel_exit_signal_function       = chan_exit_sig_cb_ext;
	_channel_cb.channel_pty_request_function       = nullptr;
	_channel_cb.channel_shell_request_function     = nullptr;
	_channel_cb.channel_auth_agent_req_function    = nullptr;
	_channel_cb.channel_x11_req_function           = nullptr;
	_channel_cb.channel_pty_window_change_function = nullptr;
	_channel_cb.channel_exec_request_function      = nullptr;
	_channel_cb.channel_env_request_function       = nullptr;
	_channel_cb.channel_subsystem_request_function = nullptr;

	ssh_callbacks_init(&_channel_cb);
}

channel_callbacks::~channel_callbacks()
{}

int channel_callbacks::chan_data_cb_ext(ssh_session s, ssh_channel c, void *data, uint32_t len, int is_stderr, void *userdata) {
	auto cbs = reinterpret_cast<channel_callbacks *>(userdata);
	auto ses = dynamic_cast<session *>(cbs);

	if (ses == nullptr) {
		LOG(ERROR) << "dereference of null channel";
		return -1;
	}

	return ses->chan_data(c, data, len, is_stderr);
}

void channel_callbacks::chan_eof_cb_ext(ssh_session s, ssh_channel c, void *userdata) {
	auto cbs = reinterpret_cast<channel_callbacks *>(userdata);
	auto ses = dynamic_cast<session *>(cbs);

	if (ses == nullptr) {
		LOG(ERROR) << "dereference of null channel";
		return;
	}

	ses->chan_eof(c);
}

void channel_callbacks::chan_close_cb_ext(ssh_session s, ssh_channel c, void *userdata) {
	auto cbs = reinterpret_cast<channel_callbacks *>(userdata);
	auto ses = dynamic_cast<session *>(cbs);

	if (ses == nullptr) {
		LOG(ERROR) << "dereference of null channel";
		return;
	}

	LOG(INFO) << "Channel close";
	ses->chan_close(c);
}

void channel_callbacks::chan_sig_cb_ext(ssh_session s, ssh_channel c, const char *signal, void *userdata) {
	LOG(INFO) << "Channel signal";
}

void channel_callbacks::chan_exit_status_cb_ext(ssh_session s, ssh_channel c, int exit_status, void *userdata) {
	LOG(INFO) << "Channel exit";
}

void channel_callbacks::chan_exit_sig_cb_ext(ssh_session s, ssh_channel c, const char *signal, int core, const char *errmsg, const char *lang, void *userdata) {
	LOG(INFO) << "Received exit signal";
}

} // namespace ssh
