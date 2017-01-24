#include <ssh/ssh.hpp>
#include <tools/base64.hpp>
#include <ssh/server_session.hpp>
#include <ssh/channel.hpp>

namespace ssh {

int srv_session::auth_pass_cb_ext(ssh_session session, const char *user, const char *pass, void *userdata)
{
	srv_session *ses = reinterpret_cast<srv_session *>(userdata);
 	return ses->auth_pass_cb(user, pass);
}

int srv_session::auth_none_cb_ext(ssh_session session, const char *user, void *userdata)
{
	srv_session *ses = reinterpret_cast<srv_session *>(userdata);
	return ses->auth_none_cb(user);
}

int srv_session::auth_pubkey_cb_ext(ssh_session session, const char *user, struct ssh_key_struct *pubkey, char signature_state, void *userdata)
{
	srv_session *ses = reinterpret_cast<srv_session *>(userdata);
	return ses->auth_pubkey_cb(user, pubkey, signature_state);
}

int srv_session::service_req_cb_ext(ssh_session session, const char *service, void *userdata)
{
	srv_session *ses = reinterpret_cast<srv_session *>(userdata);
	return ses->service_req_cb(service);
}

ssh_channel srv_session::channel_open_req_session_cb_ext(ssh_session session, void *userdata)
{
	srv_session *ses = reinterpret_cast<srv_session *>(userdata);
	return ses->channel_open_req_session_cb();
}

srv_session::srv_session(boost::asio::io_service &io, sp_server_auth auth) :
	  session("SSH.SERVER", io)
	, is_authorized_(false)
	, auth_(auth)
{
	server_cb_.userdata = this;

	server_cb_.auth_none_function                    = auth_none_cb_ext;
	server_cb_.auth_password_function                = auth_pass_cb_ext;
	server_cb_.auth_pubkey_function                  = auth_pubkey_cb_ext;
	server_cb_.channel_open_request_session_function = channel_open_req_session_cb_ext;
	server_cb_.service_request_function              = service_req_cb_ext;
	ssh_callbacks_init(&server_cb_);

	ssh_set_server_callbacks(session_, &server_cb_);

	ssh_set_auth_methods(session_, SSH_AUTH_METHOD_PASSWORD | SSH_AUTH_METHOD_PUBLICKEY);
}

srv_session::~srv_session()
{

}

//void srv_session::set_auth_pass(const std::string &user, const std::string &pass)
//{
//	std::string digest;
//	if (sha2_digest(pass, digest) == false) {
//		throw std::runtime_error("Couldn't created digest for password");
//	}
//
//	sp_auth_type        auth;
//	sp_auth_method_data method;
//
//	try {
//		auth = user_auth_.at(user);
//	} catch (const std::exception &e) {
//		try {
//			auth = std::make_shared<auth_type>();
//			method = std::make_shared<auth_method_data>();
//			auth->creds.insert(std::pair<int, sp_auth_method_data>(SSH_AUTH_METHOD_PASSWORD, method));
//			user_auth_.insert(std::pair<std::string, sp_auth_type>(user, auth));
//		} catch (...) {
//			throw;
//		}
//		auth->methods = SSH_AUTH_METHOD_PASSWORD;
//	}
//
//	LOGD_CLASS() << "Check if password needs update";
//
//	if (!method->sha2_pass.empty() && method->sha2_pass.compare(digest) == 0) {
//		LOGD_CLASS() << "Password is same";
//	} else {
//		LOGD_CLASS() << "Setting new password";
//		method->sha2_pass = digest;
//	}
//}

void srv_session::key_exchange()
{
	if (ssh_handle_key_exchange(session_) != SSH_OK) {
		throw ssh_exception(session_);
	}
}

int srv_session::auth_pass_cb(const char *user, const char *pass)
{
	enum ssh_auth_e auth = SSH_AUTH_DENIED;

	try {
		if (auth_->authorize(user, pass, strlen(pass))) {
			auth = SSH_AUTH_SUCCESS;
		}
	} catch (const std::exception &e) {
		LOGE_CLASS() << "User [" << user << "] not found. Access denied!!!";
	}

	return auth;
}

int srv_session::auth_none_cb(const char *user)
{
	enum ssh_auth_e auth = SSH_AUTH_DENIED;

	return auth;
}

int srv_session::auth_pubkey_cb(const char *user, struct ssh_key_struct *pubkey, char signature_state)
{
	enum ssh_auth_e auth = SSH_AUTH_DENIED;

	return auth;
}

ssh_channel srv_session::channel_open_req_session_cb()
{
	sp_channel chan = std::make_shared<channel>(this, nullptr, &channel_cb_);

	u_lock lock(channels_lock_);
	channels_.insert(std::pair<void *, sp_channel>(chan->c_channel(), chan));
	return chan->c_channel();
}

int srv_session::service_req_cb(const char *service)
{
	LOGI_CLASS() << "SVC req cb: " << service;

	return SSH_OK;
}

//int srv_session::handle_msg(ssh_message message)
//{
//	if (message) {
//		int type = ssh_message_type(message);
//		int sub_type = ssh_message_subtype(message);
//
//		switch (type) {
//		case SSH_REQUEST_AUTH: {
//			// check if auth user exists and supports requested auth
//			const char *user = ssh_message_auth_user(message);
//			sp_auth_type auth;
//			try {
//				auth = user_auth_.at(user);
//			} catch (const std::exception &e) {
//				LOGE_CLASS() << "Unknown user: " << user;
//				ssh_message_reply_default(message);
//				break;
//			}
//
//			if ((auth->methods & sub_type) == 0) {
//				LOGW_CLASS() << "User [" << user
//				             << "] wants login with unsupported auth type: "
//				             << sub_type;
//				ssh_message_auth_set_methods(message, auth->methods);
//				ssh_message_reply_default(message);
//			} else {
//				if (sub_type == SSH_AUTH_METHOD_PASSWORD) {
//					std::string digest;
//					if (sha2_digest(user, digest) == true) {
//						if (auth->creds.at(sub_type)->sha2_pass.compare(digest) == 0) {
//							ssh_message_auth_reply_success(message, 0);
//							is_authorized_ = true;
//						} else {
//							LOGE_CLASS() << "User [" << user << "] with wrong password";
//							ssh_message_reply_default(message);
//						}
//					} else {
//						ssh_message_reply_default(message);
//					}
//				} else if (sub_type == SSH_AUTH_METHOD_PUBLICKEY) {
//
//				} else if (sub_type == SSH_AUTH_METHOD_INTERACTIVE) {
//
//				}
//			}
//			break;
//		}
//		case SSH_REQUEST_CHANNEL_OPEN: {
//			if (is_authorized_ == false) {
//				LOGE_CLASS() << "Declined";
//				ssh_message_reply_default(message);
//			}
//			switch (sub_type) {
//			case SSH_CHANNEL_SESSION: {
//				break;
//			}
//			case SSH_CHANNEL_DIRECT_TCPIP:
//			case SSH_CHANNEL_FORWARDED_TCPIP:
//			case SSH_CHANNEL_X11:
//			case SSH_CHANNEL_UNKNOWN:
//			default:
//				LOGW_CLASS() << "Unsupported Channel type: " << sub_type;
//				ssh_message_reply_default(message);
//				break;
//			}
//			break;
//		}
//		case SSH_REQUEST_CHANNEL: {
//			switch (sub_type) {
//			case SSH_CHANNEL_REQUEST_SHELL:
//			case SSH_CHANNEL_REQUEST_PTY: {
//				ssh_message_channel_request_reply_success(message);
//				break;
//			}
//			case SSH_CHANNEL_REQUEST_EXEC:
//			case SSH_CHANNEL_REQUEST_ENV:
//			case SSH_CHANNEL_REQUEST_SUBSYSTEM:
//			case SSH_CHANNEL_REQUEST_WINDOW_CHANGE:
//			case SSH_CHANNEL_REQUEST_X11:
//			case SSH_CHANNEL_REQUEST_UNKNOWN:
//			default:
//				LOGW_CLASS() << "Unsupported Channel request: " << sub_type;
//				ssh_message_reply_default(message);
//				break;
//			}
//			break;
//		}
//		case SSH_REQUEST_SERVICE:
//		case SSH_REQUEST_GLOBAL:
//		default:
//					ssh_message_auth_set_methods(message,
//					                             SSH_AUTH_METHOD_PASSWORD |
//					                             SSH_AUTH_METHOD_INTERACTIVE);
//					ssh_message_reply_default(message);
////			LOGE_CLASS() << "Unsupported SSH request type: " << type;
////			ssh_message_reply_default(message);
//		}
//		ssh_message_free(message);
//		return 0;
//	} else {
//		return 1;
//	}
//}

bool srv_session::is_authorized()
{
	return is_authorized_;
}

//int srv_session::handle_msg_auth(ssh_message message)
//{
//	int ret = 1;
//
//	if (message) {
//		int type = ssh_message_type(message);
//		int sub_type = ssh_message_subtype(message);
//
//		switch (type) {
//		case SSH_REQUEST_AUTH: {
//			// check if auth user exists and supports requested auth
//			const char *user = ssh_message_auth_user(message);
//			sp_auth_type auth;
//			try {
//				auth = user_auth_.at(user);
//			} catch (const std::exception &e) {
//				LOGE_CLASS() << "Unknown user: " << user;
//				ssh_message_reply_default(message);
//				break;
//			}
//
//			if ((auth->methods & sub_type) == 0) {
//				LOGW_CLASS() << "User [" << user
//				             << "] wants login with unsupported auth type: "
//				             << sub_type;
//				ssh_message_auth_set_methods(message, auth->methods);
//				ssh_message_reply_default(message);
//			} else {
//				if (sub_type == SSH_AUTH_METHOD_PASSWORD) {
//					std::string digest;
//					const char *pass = ssh_message_auth_password(message);
//					if (sha2_digest(pass, digest) == true) {
//						if (auth->creds.at(sub_type)->sha2_pass.compare(digest) == 0) {
//							ssh_message_auth_reply_success(message, 0);
//							is_authorized_ = true;
//							ret = 0;
//						} else {
//							LOGE_CLASS() << "User [" << user << "] with wrong password";
//							ssh_message_reply_default(message);
//						}
//					} else {
//						ssh_message_reply_default(message);
//					}
//				} else if (sub_type == SSH_AUTH_METHOD_PUBLICKEY) {
//
//				} else if (sub_type == SSH_AUTH_METHOD_INTERACTIVE) {
//
//				}
//			}
//
//			if (is_authorized_ == true) {
//				LOGI_CLASS() << "Authorized user [" << user << "] with method [" << sub_type << "]";
//			}
//			break;
//		}
//		default:
//			ssh_message_auth_set_methods(message,
//			                             SSH_AUTH_METHOD_PASSWORD
//			                             | SSH_AUTH_METHOD_INTERACTIVE
//										 | SSH_AUTH_METHOD_PUBLICKEY);
//			ssh_message_reply_default(message);
//		}
//		ssh_message_free(message);
//	}
//
//	return ret;
//}
//
//int srv_session::handle_msg_open_channel(ssh_message message, sp_session s, sp_channel &chan)
//{
//	int ret = 1;
//
//	if (message) {
//		int type = ssh_message_type(message);
//		int sub_type = ssh_message_subtype(message);
//
//		switch (type) {
//		case SSH_REQUEST_CHANNEL_OPEN: {
//			switch (sub_type) {
//			case SSH_CHANNEL_SESSION: {
//				ssh_channel c = ssh_message_channel_request_open_reply_accept(message);
//
//				if (c == nullptr) {
//					LOGE_CLASS() << s->get_error();
//					ssh_message_reply_default(message);
//				} else {
//					chan = std::make_shared<ssh::channel>(s, c);
//					ret = 0;
//				}
//				break;
//			}
//			case SSH_CHANNEL_DIRECT_TCPIP:
//			case SSH_CHANNEL_FORWARDED_TCPIP:
//			case SSH_CHANNEL_X11:
//			case SSH_CHANNEL_UNKNOWN:
//			default:
//				LOGE_CLASS() << "Unexpected channel type: " << sub_type;
//				ssh_message_reply_default(message);
//				break;
//			}
//			break;
//		}
//		default:
//			LOGE_CLASS() << "Unexpected request type: " << type;
//			ssh_message_reply_default(message);
//		}
//		ssh_message_free(message);
//	}
//
//	return ret;
//}
//
//int srv_session::handle_msg_channel_req(ssh_message message, tty_env &env)
//{
//	int ret = 1;
//
//	if (message) {
//		int type = ssh_message_type(message);
//		int sub_type = ssh_message_subtype(message);
//
//		switch (type) {
//		case SSH_REQUEST_CHANNEL: {
//			switch (sub_type) {
//			case SSH_CHANNEL_REQUEST_SHELL:
//			case SSH_CHANNEL_REQUEST_PTY: {
//				ssh_message_channel_request_reply_success(message);
//				ret = 0;
//				break;
//			}
//			case SSH_CHANNEL_REQUEST_ENV: {
//				const char *name = ssh_message_channel_request_env_name(message);
//				const char *value = ssh_message_channel_request_env_value(message);
//				env.insert(std::pair<std::string, std::string>(name, value));
////				ssh_message_channel_request_reply_success(message);
//				break;
//			}
//			default:
//				LOGE_CLASS() << "Unexpected channel type: " << sub_type;
//				ssh_message_reply_default(message);
//				break;
//			}
//			break;
//		}
//		default:
//			LOGE_CLASS() << "Unexpected request type: " << type;
//			ssh_message_reply_default(message);
//		}
//		ssh_message_free(message);
//	}
//
//	return ret;
//}

} // namespace ssh
