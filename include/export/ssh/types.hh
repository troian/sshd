//
// Created by Artur Troian on 10/9/16.
//
#pragma once

#include <easylogging++.h>

#include <sys/types.h>
#include <libssh/libssh.h>
#include <libssh/callbacks.h>
#include <libssh/server.h>

#include <map>
#include <memory>
#include <string>

// Boost include
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/bind.hpp>

#include <ssh/exception.hh>
#include <util/types.hh>

struct fd {
private:
	struct file_descriptor_closer {
		void operator()(void *fd) noexcept {
			if (fd) {
				close(reinterpret_cast< int >(fd));
			}
		}
	};

public:
	fd(const std::string &pathname, int flags)
		: m_fd(initialize(pathname, flags)) {
	}

	explicit fd(const int opened_fd)
		: m_fd(initialize(opened_fd)) {
	}

	explicit operator int() const { return reinterpret_cast< int >(m_fd.get()); }

private:
	std::shared_ptr<void> initialize(int f) {
		try {
			return std::shared_ptr<void>(reinterpret_cast<void *>(f), file_descriptor_closer());
		} catch (std::bad_alloc &) {
			close(f);
			throw;
		}
	}

	std::shared_ptr<void> initialize(const std::string &pathname, int flags) {
		const int fd = open(pathname.c_str(), flags);
		if (fd < 0) {
			throw std::system_error(std::error_code(errno, std::system_category()), "cannot create file descriptor");
		}

		return initialize(fd);
	}

	std::shared_ptr<void> m_fd;
};

namespace ssh {

enum class connect_status {
	CONNECTED = 0,
	DISCONNECTED,
	ERROR
};

//typedef std::function<void (bool, const std::string &msg)>  conn_signal;
typedef std::function<void ()>                              disconn_signal;
typedef std::function<void (uintptr_t, bool)>               chan_conn_signal;

typedef struct {
//	conn_signal       conn;
	disconn_signal    dis;
	chan_conn_signal  chan;
} session_signals;

/**
 * \brief
 */
using sp_client_session = typename std::shared_ptr<class client_session>;

/**
 * \brief
 */
using sp_channel = typename std::shared_ptr<class channel>;

/**
 * \brief
 */
//using sp_event = typename std::shared_ptr<class event>;

/**
 * \brief
 */
//using sp_srv_session = typename std::shared_ptr<class srv_session>;

/**
 * \brief
 */
//using sp_sshkey = typename std::shared_ptr<class sshkey>;

/**
 * \brief
 */
//using up_sshbind = typename std::unique_ptr<class sshbind>;

/**
 * \brief
 */
//using sp_sshbind = typename std::shared_ptr<class sshbind>;

/**
 * \brief
 */
//using srv_incoming_connection = typename boost::signals2::signal<void ()>;

/**
 * \brief
 */
//using sp_tty = typename std::shared_ptr<class tty>;

/**
 * \brief
 */
//typedef std::map<std::string, std::string> tty_env;

/**
 * \brief
 */
//typedef std::unique_lock<std::mutex> u_lock;

enum class options {
	HOST                        = SSH_OPTIONS_HOST,
	PORT                        = SSH_OPTIONS_PORT,
	PORT_STR                    = SSH_OPTIONS_PORT_STR,
	FD                          = SSH_OPTIONS_FD,
	USER                        = SSH_OPTIONS_USER,
	SSH_DIR                     = SSH_OPTIONS_SSH_DIR,
	IDENTITY                    = SSH_OPTIONS_IDENTITY,
	ADD_IDENTITY                = SSH_OPTIONS_ADD_IDENTITY,
	KNOWNHOSTS                  = SSH_OPTIONS_KNOWNHOSTS,
	TIMEOUT                     = SSH_OPTIONS_TIMEOUT,
	TIMEOUT_USEC                = SSH_OPTIONS_TIMEOUT_USEC,
	SSH1                        = SSH_OPTIONS_SSH1,
	SSH2                        = SSH_OPTIONS_SSH2,
	LOG_VERBOSITY               = SSH_OPTIONS_LOG_VERBOSITY,
	LOG_VERBOSITY_STR           = SSH_OPTIONS_LOG_VERBOSITY_STR,
	CIPHERS_C_S                 = SSH_OPTIONS_CIPHERS_C_S,
	CIPHERS_S_C                 = SSH_OPTIONS_CIPHERS_S_C,
	COMPRESSION_C_S             = SSH_OPTIONS_COMPRESSION_C_S,
	COMPRESSION_S_C             = SSH_OPTIONS_COMPRESSION_S_C,
	PROXYCOMMAND                = SSH_OPTIONS_PROXYCOMMAND,
	BINDADDR                    = SSH_OPTIONS_BINDADDR,
	STRICTHOSTKEYCHECK          = SSH_OPTIONS_STRICTHOSTKEYCHECK,
	COMPRESSION                 = SSH_OPTIONS_COMPRESSION,
	COMPRESSION_LEVEL           = SSH_OPTIONS_COMPRESSION_LEVEL,
	KEY_EXCHANGE                = SSH_OPTIONS_KEY_EXCHANGE,
	HOSTKEYS                    = SSH_OPTIONS_HOSTKEYS,
	GSSAPI_SERVER_IDENTITY      = SSH_OPTIONS_GSSAPI_SERVER_IDENTITY,
	GSSAPI_CLIENT_IDENTITY      = SSH_OPTIONS_GSSAPI_CLIENT_IDENTITY,
	GSSAPI_DELEGATE_CREDENTIALS = SSH_OPTIONS_GSSAPI_DELEGATE_CREDENTIALS,
	HMAC_C_S                    = SSH_OPTIONS_HMAC_C_S,
	HMAC_S_C                    = SSH_OPTIONS_HMAC_S_C,
};

} // namespace ssh
