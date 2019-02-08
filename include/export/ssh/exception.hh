//
// Created by Artur Troian on 10/9/16.
//
#pragma once

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>

#include <string>

namespace ssh {

/**
 * \brief
 */
class ssh_exception : public std::runtime_error {
public:
	explicit ssh_exception(ssh_session s)
		: runtime_error(std::string(ssh_get_error(s)))
		, _code(ssh_get_error_code(s))
	{}

	explicit ssh_exception(ssh_bind b)
		: runtime_error(std::string(ssh_get_error(b)))
		, _code(ssh_get_error_code(b))
	{}

	/** @brief returns the Error code
	 * @returns SSH_FATAL Fatal error happened (not recoverable)
	 * @returns SSH_REQUEST_DENIED Request was denied by remote host
	 * @see ssh_get_error_code
	 */
	int code() {
		return _code;
	}

private:
	int _code;
};

} // namespace ssh
