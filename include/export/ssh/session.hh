//
// Created by Artur Troian on 10/8/16.
//
#pragma once

#include <map>
#include <string>

#include <ssh/types.hh>

namespace ssh {

class channel_callbacks {
public:
	channel_callbacks();

	virtual ~channel_callbacks() = 0;

private:
	/**
	 * @brief SSH channel data callback. Called when data is available on a channel
	 * @param s Current session handler
	 * @param c the actual channel
	 * @param data the data that has been read on the channel
	 * @param len the length of the data
	 * @param is_stderr is 0 for stdout or 1 for stderr
	 * @param userdata Userdata to be passed to the callback function.
	 * @returns number of bytes processed by the callee. The remaining bytes will
	 * be sent in the next callback message, when more data is available.
	 */
	static
	int chan_data_cb_ext(ssh_session s, ssh_channel c, void *data, uint32_t len, int is_stderr, void *userdata);
	/**
	 * @brief SSH channel eof callback. Called when a channel receives EOF
	 * @param s Current session handler
	 * @param c the actual channel
	 * @param userdata Userdata to be passed to the callback function.
	 */
	static
	void chan_eof_cb_ext(ssh_session s, ssh_channel c, void *userdata);

	/**
	 * @brief SSH channel close callback. Called when a channel is closed by remote peer
	 * @param session Current session handler
	 * @param channel the actual channel
	 * @param userdata Userdata to be passed to the callback function.
	 */
	static
	void chan_close_cb_ext(ssh_session s, ssh_channel c, void *userdata);

	/**
	 * @brief SSH channel signal callback. Called when a channel has received a signal
	 * @param session Current session handler
	 * @param channel the actual channel
	 * @param signal the signal name (without the SIG prefix)
	 * @param userdata Userdata to be passed to the callback function.
	 */
	static
	void chan_sig_cb_ext(ssh_session s, ssh_channel c, const char *signal, void *userdata);

	/**
	 * @brief SSH channel exit status callback. Called when a channel has received an exit status
	 * @param session Current session handler
	 * @param channel the actual channel
	 * @param userdata Userdata to be passed to the callback function.
	 */
	static
	void chan_exit_status_cb_ext(ssh_session s, ssh_channel c, int exit_status, void *userdata);

	/**
	 * @brief SSH channel exit signal callback. Called when a channel has received an exit signal
	 * @param session Current session handler
	 * @param channel the actual channel
	 * @param signal the signal name (without the SIG prefix)
	 * @param core a boolean telling whether a core has been dumped or not
	 * @param errmsg the description of the exception
	 * @param lang the language of the description (format: RFC 3066)
	 * @param userdata Userdata to be passed to the callback function.
	 */
	static
	void chan_exit_sig_cb_ext(ssh_session s, ssh_channel c, const char *signal, int core, const char *errmsg, const char *lang, void *userdata);

protected:
	struct ssh_channel_callbacks_struct _channel_cb;
};

/**
 * \brief
 */
class session : public base_class, public channel_callbacks, public std::enable_shared_from_this<session> {
private:
	class methods_base {
	public:
		virtual ~methods_base() = default;
	private:
		friend class session;
	protected:
		ssh_session _session;
	};

public:
	class set : public methods_base {
	public:
		void option(ssh::options type, int32_t option);
		void option(ssh::options type, const void *option);
	};

	class get : public methods_base {
	public:
		ssh_session session();
		int status();
		int version();
		const char *error();
		int error_code();
		int openssh_version();
	};

	class copy : public methods_base {
	public:
		void options(const session &src);
	};

	class parse : public methods_base {
	public:
		void options(std::string const &ops);
	};

private:
	friend class channel_callbacks;
	friend class copy;

public:
	explicit session(const std::string &log_name, boost_io::sp io, const session_signals &sig);

	/**
	 * \brief
	 *
	 * \param io
	 */
	explicit session(boost_io::sp io, const session_signals &sig);

	~session() override = 0;

	session(const session &) = delete;
	session &operator=(const session &) = delete;

public:
	class set &set() {
		return _set;
	}

	class get &get() {
		return _get;
	}

	class copy &copy() {
		return _copy;
	}

	class parse &parse() {
		return _parse;
	}

	/**
	 * \brief
	 */
	void disconnect();

	/**
	 * \brief
	 */
	void disconnect_silent();

public:
	virtual std::string remote_banner() = 0;

private: // Channel callbacks
	/**
	 * \brief
	 *
	 * \param c
	 * \param data
	 * \param len
	 * \param is_stderr
	 * \return
	 */
	int chan_data(ssh_channel c, void *data, uint32_t len, int is_stderr);

	/**
	 * \brief
	 *
	 * \param c
	 */
	void chan_eof(ssh_channel c);

	/**
	 * \brief
	 *
	 * \param c
	 */
	void chan_close(ssh_channel c);

protected:
	/**
	 * \brief
	 *
	 * \param host
	 * \param port
	 * \param multichannel
	 */
	void run_fwd_acceptor(std::string const &host, int port, bool multichannel);

private:
	/**
	 * \brief
	 *
	 * \param host
	 * \param port
	 * \param multichannel
	 */
	void fwd_acceptor(const std::string &host, int port, bool multichannel);

protected:
	ssh_session                                           _session;
	boost_io::sp                                          _io;
	std::mutex                                            _channels_lock;
	std::map<ssh_channel, std::shared_ptr<class channel>> _channels;
	std::atomic_bool                                      _session_alive;
	std::thread                                           _fwd_acceptor;

protected: // signals;
	disconn_signal                      _sig_dis;
	chan_conn_signal                    _sig_chan;

private:
	class set                           _set;
	class get                           _get;
	class copy                          _copy;
	class parse                         _parse;
};

} // namespace ssh
