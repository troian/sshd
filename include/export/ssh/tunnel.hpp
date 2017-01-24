#pragma once

#include <thread>
#include <atomic>
#include <memory>

#include <ssh/sshkey.hpp>
#include <ssh/session.hpp>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

namespace ssh {

/**
 * \brief
 */
class ssh_tunnel final : public base_class {
	/**
	 * \brief
	 *
	 * \param io
	 * \param host
	 * \param port
	 * \param user
	 * \param timeout
	 */
	explicit ssh_tunnel(
				  boost::asio::io_service &io
				, const std::string &host
				, int port
				, const std::string &user
				, int timeout);

public:
	/**
	 * \brief   Create tunnel to server with user:password
	 *
	 * \param io
	 * \param host
	 * \param port
	 * \param user
	 * \param pass
	 * \param timeout
	 */
	explicit ssh_tunnel(
				  boost::asio::io_service &io
				, const std::string &host
				, int port
				, const std::string &user
				, const std::string &pass
				, int timeout);

	/**
	 * \brief   Create tunnel to the server using user:privkey
	 *
	 * \param io
	 * \param host
	 * \param port
	 * \param user
	 * \param key
	 * \param timeout
	 */
	explicit ssh_tunnel(
				  boost::asio::io_service &io
				, const std::string &host
				, int port
				, const std::string &user
				, sp_ssh_key_pair key
				, int timeout);

	virtual ~ssh_tunnel();

public:
	/**
	 * \brief
	 *
	 * \param login
	 * \param pass
	 * \param host
	 * \param local_port
	 * \param remote_port
	 */
	void create(const std::string &login, const std::string &pass, const std::string &host, int local_port, int remote_port);

	/**
	 * \brief
	 *
	 * \param host
	 * \param port
	 * \param localhost
	 * \param local_port
	 */
	void create_forwarding(const std::string &host, int port, const std::string &localhost, int local_port);

	/**
	 * \brief
	 *
	 * \param host
	 * \param port
	 * \param fwd_host
	 * \param bind_port
	 * \param multichannel
	 */
	void create_reverse(const std::string &host, int port, const std::string &fwd_host, int bind_port, bool multichannel = false);

	/**
	 * \brief
	 */
	void cancel();

private: // session signals
	void session_connected(bool, const std::string &msg);
	void session_disconnected();
	void channel_connect(uintptr_t id, bool is_connected);

private:
	sp_client_session             session_;
	boost::asio::io_service      &io_;
};

} // namespace ssh
