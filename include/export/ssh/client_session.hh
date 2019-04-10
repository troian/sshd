//
// Created by Artur Troian on 10/9/16.
//
#pragma once

#include <string>

#include <ssh/session.hh>
#include <ssh/sshkey.hh>

#include <boost/asio.hpp>

#include <util/types.hh>

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
		void pubkey(key_pair::sp key);

	private:
		ssh_session _session;
	};

public:
	explicit client_session(boost_io::sp io, const session_signals &sig, int keep_alive = 0);

	~client_session() override;

public:
	template <typename... _Args>
	static std::shared_ptr<class client_session> shared(_Args&&... __args) {
		return std::make_shared<class client_session>(__args...);
	}

	auto shared_from_this() {
		return shared_from(this);
	}

public:
	/**
	 * \brief
	 */
	void connect();

	class auth &auth() {
		return _auth;
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
	std::string remote_banner() override;

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
	boost::posix_time::seconds  _keep_alive_interval;
	boost::asio::deadline_timer _keep_alive_timer;
	class auth                  _auth;
};

} // namespace ssh
