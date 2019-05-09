//
// Created by Artur Troian on 10/10/16.
//

#include <ssh/auth.hh>
#include <openssl/sha.h>

namespace ssh {

// --------------------------------------------------------------
// Implementation of class server_auth
// --------------------------------------------------------------
server_auth::server_auth()
{}

server_auth::~server_auth()
{}

bool server_auth::authorize(const std::string &user, const char *pass, size_t size) {
	u_lock lock(lock_);

	try {
		std::string digest;

		if (!sha2_digest((const uint8_t *)pass, size, digest)) {
			return false;
		}

		auto &p = auth_.at(user).pass;

		auto s = p.find(digest);

		if (s == p.end()) {
			return false;
		}

	} catch (...) {
		return false;
	}
	return true;
}

bool server_auth::sha2_digest(const uint8_t *data, size_t size, std::string &digest)
{
	SHA256_CTX context;

	if(!SHA256_Init(&context))
		return false;

	if(!SHA256_Update(&context, data, size))
		return false;

	uint8_t md[SHA256_DIGEST_LENGTH]; // 32 bytes

	if(!SHA256_Final(md, &context))
		return false;

	digest.clear();
	tools::base64::encode(digest, md, SHA256_DIGEST_LENGTH);

	return true;
}

// --------------------------------------------------------------
// Implementation of class server_auth_add
// --------------------------------------------------------------
server_auth_add::server_auth_add()
{}

server_auth_add::~server_auth_add()
{}

void server_auth_add::add_auth(const std::string &user, const char *pass, size_t size)
{
	std::string digest;

	if (!sha2_digest((const uint8_t *)pass, size, digest)) {
		throw std::runtime_error("Couldn't generate SHA2 digest");
	}

	u_lock lock(lock_);

	try {
		auth_.at(user);
	} catch (const std::out_of_range &e) {
		auth_.insert(std::pair<std::string, server_auth::srv_types>(user, server_auth::srv_types {}));
	}

	auto &u = auth_.at(user);

	auto i = u.pass.find(digest);

	if (i == u.pass.end()) {
		u.pass.insert(digest);
	} else {
		throw std::runtime_error("Already exists");
	}
}

} // namespace ssh
