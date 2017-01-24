//
// Created by Artur Troian on 10/10/16.
//
#pragma once

#include <ssh/types.hpp>
#include <ssh/server_session.hpp>
#include <ssh/auth.hpp>

namespace ssh {

/**
 * \brief
 */
class sshd : public base_class {
public:
	sshd(boost::asio::io_service &io, int port);
	virtual ~sshd();

public:

private:
	void incoming_connection();
	void acceptor_thread();

private:
	up_sshbind                         bind_;
	std::map<void *, sp_session>       sessions_;
	std::thread                        acceptor_;
	std::atomic_bool                   acceptor_alive_;
	sp_server_auth_add                 auth_;
	sp_event                           evt_;
};

} // namespace ssh
