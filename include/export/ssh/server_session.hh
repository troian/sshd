//
// Created by Artur Troian on 10/9/16.
//
#pragma once

#include <ssh/types.hpp>
#include <ssh/session.hpp>
#include <ssh/auth.hh>

namespace ssh {

/**
 * \brief
 */
class srv_session : public session {
public:
	explicit srv_session(boost::asio::io_service &io, sp_server_auth auth);

	virtual ~srv_session();

public:
	void key_exchange();

	bool is_authorized();

private:
	int auth_pass_cb(const char *user, const char *pass);

	int auth_none_cb(const char *user);

	int auth_pubkey_cb(const char *user, struct ssh_key_struct *pubkey, char signature_state);

	int service_req_cb(const char *service);

	ssh_channel channel_open_req_session_cb();

private:
	static int auth_pass_cb_ext(ssh_session session, const char *user, const char *pass, void *userdata);

	static int auth_none_cb_ext(ssh_session session, const char *user, void *userdata);

	static int auth_pubkey_cb_ext(ssh_session session, const char *user, struct ssh_key_struct *pubkey, char signature_state, void *userdata);

	static int service_req_cb_ext(ssh_session session, const char *service, void *userdata);

	static ssh_channel channel_open_req_session_cb_ext(ssh_session session, void *userdata);

private:
	bool                                is_authorized_;
	int                                 auth_methods_;
	sp_server_auth                      auth_;
	struct ssh_server_callbacks_struct  server_cb_;
};

} // namespace ssh
