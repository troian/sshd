//
// Created by Artur Troian on 10/12/16.
//

#include <ssh/sshkey.hh>

namespace ssh {

key_pair::key_pair(const std::string &file)
	: _priv(nullptr)
	, _pub(nullptr)
	, _type(type::UNKNOWN)
{
	ssh_key _pr;
	ssh_key _pu;

	if (ssh_pki_import_privkey_file(file.c_str(), nullptr, nullptr, nullptr, &_pr) != SSH_OK) {
		throw std::runtime_error("Couldn't open key file");
	}

	sp_key pr = sp_key(_pr, ::ssh_key_free);

	_type = static_cast<type>(ssh_key_type(_pr));

	if (ssh_pki_export_privkey_to_pubkey(_pr, &_pu) != SSH_OK ) {
		ssh_key_free(_pu);
		throw std::runtime_error("Couldn't export to pub key");
	}

	_priv = pr;
	_pub  = sp_key(_pu, ::ssh_key_free);
}

key_pair::key_pair(type t, int length)
	: _priv(nullptr)
	, _pub(nullptr)
	, _type(t)
{
	ssh_key _pr;
	ssh_key _pu;

	if (ssh_pki_generate(static_cast<enum ssh_keytypes_e>(t), length, &_pr) != SSH_OK) {
		throw std::runtime_error("Couldn't generate key");
	}

	sp_key pr = sp_key(_pr, ::ssh_key_free);

	if (ssh_pki_export_privkey_to_pubkey(_pr, &_pu) != SSH_OK ) {
		throw std::runtime_error("Couldn't export to pub key");
	}

	_priv = pr;
	_pub  = sp_key(_pu, ::ssh_key_free);
}

key_pair::sp_key key_pair::priv() {
	return _priv;
}

key_pair::sp_key key_pair::pub() {
	return _pub;
}

std::string key_pair::pub_base64() const {
	char *buf;

	if (ssh_pki_export_pubkey_base64(_pub.get(), &buf) != SSH_OK) {
		throw std::runtime_error("Couldn't export pub key to base64");
	}

	std::string b64(buf);

	ssh_string_free_char(buf);

	if (b64.rfind("ssh-rsa ", 0) != 0) {
		b64 = "ssh-rsa " + b64;
	}

	return b64;
}

key_pair::type key_pair::get_type() {
	return _type;
}

} // namespace ssh
