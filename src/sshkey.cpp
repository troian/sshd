//
// Created by Artur Troian on 10/12/16.
//

#include <ssh/sshkey.hpp>

namespace ssh {

key_pair::key_pair(const std::string &file)
{
	if (ssh_pki_import_privkey_file(file.c_str(), NULL, NULL, NULL, &priv_) != SSH_OK) {
		throw std::runtime_error("Couldn't open key file");
	}

	type_ = (key_type)ssh_key_type(priv_);

	if (ssh_pki_export_privkey_to_pubkey(priv_, &pub_) != SSH_OK ) {
		ssh_key_free(priv_);
		throw std::runtime_error("Couldn't export to pub key");
	}
}

key_pair::key_pair(key_type type, int length) :
	type_(type)
{
	if (ssh_pki_generate((enum ssh_keytypes_e)type, length, &priv_) != SSH_OK) {
		throw std::runtime_error("Couldn't generate key");
	}

	if (ssh_pki_export_privkey_to_pubkey(priv_, &pub_) != SSH_OK ) {
		ssh_key_free(priv_);
		throw std::runtime_error("Couldn't export to pub key");
	}
}

key_pair::~key_pair()
{
	ssh_key_free(priv_);
	ssh_key_free(pub_);
}

ssh_key key_pair::priv()
{
	return priv_;
}

ssh_key key_pair::pub()
{
	return pub_;
}

void key_pair::pub_b64(std::string &b64)
{
	char *buf;

	if (ssh_pki_export_pubkey_base64(pub_, &buf) != SSH_OK) {
		throw std::runtime_error("Couldn't export pub key to base64");
	}

	b64 = buf;

	free(buf);
}

key_type key_pair::type()
{
	return type_;
}

} // namespace ssh
