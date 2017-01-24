//
// Created by Artur Troian on 10/9/16.
//
#pragma once

#include <ssh/session.hpp>
#include <ssh/sshkey.hpp>

#include <boost/asio.hpp>

namespace ssh {

/**
 * \brief
 */
class client_session : public session {
private:
	class auth {
	private:
		friend class client_session;
	public:
		void none();
		void pubkey_auto();
		void pass(std::string const &pass);
		void try_pubkey(ssh_key pubkey);
		void pubkey(sp_key_pair key);

	private:
		ssh_session session_;
	};

public:
	explicit client_session(sp_boost_io io, const session_signals &sig, int keep_alive = 0);

	virtual ~client_session();
public:
	template <typename... _Args>
	static std::shared_ptr<class client_session> shared(_Args&&... __args) {
		return std::make_shared<class client_session>(__args...);
	}

public:
	/**
	 * \brief
	 */
	void connect();

	class auth &auth() {
		return auth_;
	}

public:
	/**
	 * \brief
	 */
	void write_known_hosts();

	/**
	 * \brief
	 *
	 * \return
	 */
	virtual std::string remote_banner();

public:
	/**
	 * \brief
	 *
	 * \param[in]   host
	 * \param[in]   port
	 * \param[in]   fwd_host
	 * \param[in]   fwd_port
	 * \param[in]   multichannel
	 *
	 * \return
	 */
	bool listen_forward(const std::string &host, int port, const std::string &fwd_host, int fwd_port, bool multichannel);

private:
	void keep_alive(const boost::system::error_code& ec);

private:
	boost::posix_time::seconds                 keep_alive_interval_;
	boost::asio::deadline_timer                keep_alive_timer_;
	class auth                                 auth_;
};

} // namespace ssh
