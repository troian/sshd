//
// Created by Artur Troian on 10/8/16.
//
#pragma once

#include <ssh/types.hpp>

namespace ssh {

class channel_callbacks {
public:
	explicit channel_callbacks();

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
	 * @param core a boolean telling wether a core has been dumped or not
	 * @param errmsg the description of the exception
	 * @param lang the language of the description (format: RFC 3066)
	 * @param userdata Userdata to be passed to the callback function.
	 */
	static
	void chan_exit_sig_cb_ext(ssh_session s, ssh_channel c, const char *signal, int core, const char *errmsg, const char *lang, void *userdata);

//	/**
//	 * @brief SSH channel PTY request from a client.
//	 * @param channel the channel
//	 * @param term The type of terminal emulation
//	 * @param width width of the terminal, in characters
//	 * @param height height of the terminal, in characters
//	 * @param pxwidth width of the terminal, in pixels
//	 * @param pxheight height of the terminal, in pixels
//	 * @param userdata Userdata to be passed to the callback function.
//	 * @returns 0 if the pty request is accepted
//	 * @returns -1 if the request is denied
//	 */
//	static
//	int chan_pty_req_cb_ext(ssh_session s, ssh_channel c, const char *term, int width, int height, int pxwidth, int pwheight, void *userdata);
//
//	/**
//	 * @brief SSH channel Shell request from a client.
//	 * @param channel the channel
//	 * @param userdata Userdata to be passed to the callback function.
//	 * @returns 0 if the shell request is accepted
//	 * @returns 1 if the request is denied
//	 */
//	static
//	int chan_shell_req_cb_ext(ssh_session s, ssh_channel c, void *userdata);
//
//	/**
//	 * @brief SSH auth-agent-request from the client. This request is
//	 * sent by a client when agent forwarding is available.
//	 * Server is free to ignore this callback, no answer is expected.
//	 * @param channel the channel
//	 * @param userdata Userdata to be passed to the callback function.
//	 */
//	static
//	void chan_auth_agent_req_cb_ext(ssh_session s, ssh_channel c, void *userdata);
//
//	/**
//	 * @brief SSH X11 request from the client. This request is
//	 * sent by a client when X11 forwarding is requested(and available).
//	 * Server is free to ignore this callback, no answer is expected.
//	 * @param channel the channel
//	 * @param userdata Userdata to be passed to the callback function.
//	 */
//	static
//	void chan_x11_req_cb_ext(ssh_session s, ssh_channel c, int single_connection, const char *auth_protocol, const char *auth_cookie, uint32_t screen_number, void *userdata);
//
//	/**
//		 * @brief SSH channel PTY windows change (terminal size) from a client.
//		 * @param channel the channel
//		 * @param width width of the terminal, in characters
//		 * @param height height of the terminal, in characters
//		 * @param pxwidth width of the terminal, in pixels
//		 * @param pxheight height of the terminal, in pixels
//		 * @param userdata Userdata to be passed to the callback function.
//		 * @returns 0 if the pty request is accepted
//		 * @returns -1 if the request is denied
//		 */
//	static
//	int chan_pty_window_change_cb_ext(ssh_session s, ssh_channel c, int width, int height, int pxwidth, int pwheight, void *userdata);
//
//	/**
//	 * @brief SSH channel Exec request from a client.
//	 * @param channel the channel
//	 * @param command the shell command to be executed
//	 * @param userdata Userdata to be passed to the callback function.
//	 * @returns 0 if the exec request is accepted
//	 * @returns 1 if the request is denied
//	 */
//	static
//	int chan_exec_req_cb_ext(ssh_session s, ssh_channel c, const char *command, void *userdata);
//
//	/**
//	 * @brief SSH channel environment request from a client.
//	 * @param channel the channel
//	 * @param env_name name of the environment value to be set
//	 * @param env_value value of the environment value to be set
//	 * @param userdata Userdata to be passed to the callback function.
//	 * @returns 0 if the env request is accepted
//	 * @returns 1 if the request is denied
//	 * @warning some environment variables can be dangerous if changed (e.g.
//	 * 			LD_PRELOAD) and should not be fulfilled.
//	 */
//	static
//	int chan_env_req_cb_ext(ssh_session s, ssh_channel c,const char *env_name, const char *env_value, void *userdata);
//
//	/**
//	 * @brief SSH channel subsystem request from a client.
//	 *
//	 * @param channel the channel
//	 * @param subsystem the subsystem required
//	 * @param userdata Userdata to be passed to the callback function.
//	 * @returns 0 if the subsystem request is accepted
//	 * @returns 1 if the request is denied
//	 */
//	static
//	int chan_subsystem_req_cb_ext(ssh_session s, ssh_channel c, const char *subsystem, void *userdata);

protected:
	struct ssh_channel_callbacks_struct channel_cb_;
};

/**
 * \brief
 */
class session : public base_class, public obj_management<class session>, public channel_callbacks {
private:
	class methods_base {
	private:
		friend class session;
	protected:
		ssh_session session_;
	};
public:

	class set : public methods_base {
	public:
		void option(ssh::options type, long int option);
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
	explicit session(const std::string &log_name, sp_boost_io io, const session_signals &sig);

	/**
	 * \brief
	 *
	 * \param io
	 */
	explicit session(sp_boost_io io, const session_signals &sig);

	virtual ~session() = 0;

public:
	class set &set() {
		return set_;
	}

	class get &get() {
		return get_;
	}

	class copy &copy() {
		return copy_;
	}

	class parse &parse() {
		return parse_;
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
	ssh_session                         session_;
	sp_boost_io                         io_;
	std::mutex                          channels_lock_;
	std::map<ssh_channel, sp_channel>   channels_;
	std::atomic_bool                    session_alive_;
	std::thread                         fwd_acceptor_;

protected: // signals;
//	conn_signal                         sig_conn_;
	disconn_signal                      sig_dis_;
	chan_conn_signal                    sig_chan_;

private:
	class set                           set_;
	class get                           get_;
	class copy                          copy_;
	class parse                         parse_;
};

}
