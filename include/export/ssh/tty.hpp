//
// Created by Artur Troian on 10/11/16.
//
#pragma once

#include <memory>
#include <string>

#include <ssh/types.hpp>

namespace ssh {

/**
 * \brief
 */
class tty : public base_class {
public:
	tty(tty_env &env, const std::string &bash, int &fd);
	~tty();

private:
	pid_t pid_;
	int   fd_;
};

using up_pty = typename std::unique_ptr<class pty>;

class pty final : public base_class {
public:
	pty(const char *term, struct winsize *win);

	~pty();

public:
	int exec(const char *mode, const char *bash, const char *cmd);

//	void exec_nopty(const char *bash, const char *cmd);

	int resize(struct winsize *win);

	socket_t fd() {
		return pty_m_;
	}

private:
	void add_env();

private:
	pid_t      pid_;
	/* For PTY allocation */
	socket_t   pty_m_;
	socket_t   pty_s_;

	std::mutex lock_;
};

class nopty final : public base_class {
public:
	nopty();
	~nopty();
};

} // namespace ssh
