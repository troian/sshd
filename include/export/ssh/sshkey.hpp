//
// Created by Artur Troian on 10/12/16.
//
#pragma once

#include <memory>

#include <ssh/types.hpp>

namespace ssh {

/**
 * \brief
 */
enum class key_type {
	UNKNOWN = SSH_KEYTYPE_UNKNOWN,
	DSS     = SSH_KEYTYPE_DSS,
	RSA     = SSH_KEYTYPE_RSA,
	RSA1    = SSH_KEYTYPE_RSA1,
	ECDSA   = SSH_KEYTYPE_ECDSA
};

/**
 * \brief
 */
using sp_key_pair = typename std::shared_ptr<class key_pair>;

/**
 * \brief
 */
class key_pair final : public obj_management<class key_pair> {
public:
	/**
	 * \brief       Load key from file
	 *
	 * \param[in]   file
	 */
	explicit key_pair(const std::string &file);

	/**
	 * \brief       Generate new key
	 *
	 * \param[in]   type
	 */
	explicit key_pair(key_type type, int length);

	~key_pair();

public:
	/**
	 * \brief
	 *
	 * \return
	 */
	ssh_key priv();

	/**
	 * \brief
	 *
	 * \return
	 */
	ssh_key pub();

	/**
	 * \brief
	 *
	 * \param b64
	 */
	void pub_b64(std::string &b64);

	/**
	 * \brief
	 *
	 * \return
	 */
	key_type type();

private:
	ssh_key  priv_;
	ssh_key  pub_;
	key_type type_;
};

}
