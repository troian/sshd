//
// Created by Artur Troian on 10/9/16.
//
#pragma once

namespace ssh {

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>

/**
 * \brief
 */
class ssh_exception {
public:
	explicit ssh_exception(ssh_session s)
	{
		code_ = ssh_get_error_code(s);
		description_ = std::string(ssh_get_error(s));
	}

	explicit ssh_exception(ssh_bind b)
	{
		code_ = ssh_get_error_code(b);
		description_ = std::string(ssh_get_error(b));
	}

	ssh_exception(const ssh_exception &e) :
		code_(e.code_)
		, description_(e.description_)
	{
	}

	/** @brief returns the Error code
	 * @returns SSH_FATAL Fatal error happened (not recoverable)
	 * @returns SSH_REQUEST_DENIED Request was denied by remote host
	 * @see ssh_get_error_code
	 */
	int code()
	{
		return code_;
	}

	/**
	 * \brief returns the error message of the last exception
	 * \returns pointer to a c string containing the description of error
	 * \see ssh_get_error
	 */
	std::string what()
	{
		return description_;
	}
private:
	int         code_;
	std::string description_;
};

} // namespace ssh
