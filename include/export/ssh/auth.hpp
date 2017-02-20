//
// Created by Artur Troian on 10/10/16.
//
#pragma once

#include <map>
#include <unordered_set>
#include <memory>
#include <string>

#include <ssh/types.hpp>
#include <tools/base64.hpp>

namespace ssh {

using sp_server_auth = typename std::shared_ptr<class server_auth>;

using sp_server_auth_add = typename std::shared_ptr<class server_auth_add>;

/**
 * \brief
 */
class server_auth {
protected:
	typedef struct {
		std::unordered_set<std::string> pass;
		std::unordered_set<std::string> pubkeys;
	} srv_types;

public:
	server_auth();

	virtual ~server_auth();

public:
	bool authorize(const std::string &user, const char *pass, size_t size);
	bool authorize(const std::string &user, ssh_key key);
	bool authorize(const std::string &user, sp_sshkey key);

public:
	static bool sha2_digest(const uint8_t *data, size_t size, std::string &digest);

protected:
	std::mutex                               lock_;
	std::map<std::string, srv_types>  auth_;
};

class server_auth_add final : public server_auth {
public:
	server_auth_add();
	virtual ~server_auth_add();

public:
	void add_auth(const std::string &user, const char *pass, size_t size);
	void add_auth(const std::string &user, sp_sshkey key);
};

} // namespace ssh
