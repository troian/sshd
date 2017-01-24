//
// Created by Artur Troian on 10/9/16.
//
#pragma once

#include <ssh/types.hpp>
#include <ssh/auth.hpp>

namespace ssh {

/**
 * \brief
 */
class sshbind final {
public:
	/**
	 * \brief
	 *
	 * \param[in]   io
	 */
	explicit sshbind(boost::asio::io_service &io, sp_server_auth auth);

	~sshbind();

	/**
	 * \brief
	 *
	 * \param[in]   sig
	 *
	 * \return
	 */
	boost::signals2::connection listen(const srv_incoming_connection::slot_type &sig);

	/**
	 * \brief
	 *
	 * \return
	 */
	sp_srv_session accept();

	/**
	 * \brief
	 *
	 * \param[in]   type
	 * \param[in]   value
	 */
	void option_set(enum ssh_bind_options_e type, const void *value);

	/**
	 * \brief
	 *
	 * \return
	 */
	ssh_bind c_bind();

	/**
	 * \brief
	 *
	 * \param[in]   is_blocking
	 */
	void set_blocking(bool is_blocking);

private:
	/**
	 * \brief
	 *
	 * \param[in]   b
	 * \param[in]   userdata
	 */
	static void incoming_connection_ext(ssh_bind b, void *userdata);
	friend void incoming_connection_ext(ssh_bind b, void *userdata);

private:
	boost::asio::io_service &io_;
	ssh_bind                 bind_;
	srv_incoming_connection  inc_sig_;
	sp_server_auth           auth_;
};

} // namespace ssh
