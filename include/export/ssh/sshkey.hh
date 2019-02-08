//
// Created by Artur Troian on 10/12/16.
//
#pragma once

#include <memory>
#include <string>

#include <ssh/types.hh>

namespace ssh {

/**
 * \brief
 */
class key_pair final : public obj_management<class key_pair> {
public:
	/**
	 * \brief
	 */
	enum class type {
		UNKNOWN = SSH_KEYTYPE_UNKNOWN,
		DSS     = SSH_KEYTYPE_DSS,
		RSA     = SSH_KEYTYPE_RSA,
		RSA1    = SSH_KEYTYPE_RSA1,
		ECDSA   = SSH_KEYTYPE_ECDSA
	};

	using sp_key = typename std::shared_ptr<ssh_key_struct>;

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
	explicit key_pair(type t, int length);

	~key_pair() = default;

	key_pair(const type &) = delete;
	key_pair &operator=(const key_pair &) = delete;

public:
	/**
	 * \brief
	 *
	 * \return
	 */
	key_pair::sp_key priv();

	/**
	 * \brief
	 *
	 * \return
	 */
	key_pair::sp_key pub();

	/**
	 * \brief
	 *
	 * \param b64
	 */
	std::string pub_base64() const;

	/**
	 * \brief
	 *
	 * \return
	 */
	type get_type();

private:
	sp_key  _priv;
	sp_key  _pub;
	type    _type;
};

} // namespace ssh
