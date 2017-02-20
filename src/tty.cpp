//#include <simply/types.hpp>
#include <signal.h>
#include <sys/wait.h>

#include <ssh/tty.hpp>

#ifdef __APPLE__
	#include <util.h>
#elif __linux__
	#include <pty.h>
	#include <utmp.h>
#else
	#error "Undefined platform"
#endif

namespace ssh {

//tty::tty(tty_env &env, const std::string &bash, int &fd) :
//	base_class("TTY")
//{
//	struct termios *term = NULL;
//	struct winsize *win = NULL;
//
//	tty_env *l_env = new tty_env(env);
//
//	pid_ = forkpty(&fd, NULL, term, win);
//	if (pid_ == 0) {
////		for (auto i : *l_env) {
////			setenv((char *)i.first.c_str(), i.second.c_str(), 1);
////		}
//		chdir("~/");
//		execl(bash.c_str(), bash.c_str(), 0, (char *)NULL);
//		abort();
//	} else if (pid_) {
//		delete l_env;
//		LOGI_CLASS() << "TTY created: PID: " << pid_;
//	} else {
//		delete l_env;
//		throw std::runtime_error("Couldn't create TTY");
//	}
//}

tty::~tty()
{
	close(fd_);
	kill(pid_, SIGTERM);
}

pty::pty(const char *term, struct winsize *win) :
	  base_class("SSH.PTY")
	, pid_(0)
{
	if (openpty(&pty_m_, &pty_s_, NULL, NULL, win) != 0) {
		LOGE_CLASS() << "Failed to open pty: " << strerror(errno);
		throw;
	}
}

pty::~pty()
{
	u_lock lock(lock_);

	close(pty_m_);
	if (pty_s_ > 0)
		close(pty_s_);

	int rc;

	if (pid_)
		waitpid(pid_, &rc, WNOHANG);
}

int pty::exec(const char *mode, const char *bash, const char *cmd)
{
	u_lock lock(lock_);
	switch (pid_ = fork()) {
	case -1: {
		LOGE_CLASS() << "Failed to fork: " << strerror(errno);
		return pid_;
	}
	case 0: {
		close(pty_m_);
		if (login_tty(pty_s_) != 0) {
			exit(1);
		}
		execl(bash, "sh", NULL, NULL, NULL);
		exit(0);
	}
	default:
		LOGI_CLASS() << "pid: " << pid_;
		close(pty_s_);
		pty_s_ = -1;
		return pty_m_;
	}
}

void pty::add_env()
{
	char *param = const_cast<char *>("TERM");
	char *var = const_cast<char *>("xterm-256color");

	char *newvar = NULL;
	int plen, vlen;
	plen = strlen(param);
	vlen = strlen(var);

	newvar = static_cast<char *>(malloc(plen + vlen + 2)); /* 2 is for '=' and '\0' */
	memcpy(newvar, param, plen);
	newvar[plen] = '=';
	memcpy(&newvar[plen+1], var, vlen);
	newvar[plen+vlen+1] = '\0';
	/* newvar is leaked here, but that's part of putenv()'s semantics */
	if (putenv(newvar) < 0) {

	}
}

//void pty::exec_nopty(const char *bash, const char *cmd)
//{
//	int in[2];
//	int out[2];
//	int err[2];
//
//	/* Do the plumbing to be able to talk with the child process. */
//	if (pipe(in) != 0) {
//		goto stdin_failed;
//	}
//	if (pipe(out) != 0) {
//		goto stdout_failed;
//	}
//	if (pipe(err) != 0) {
//		goto stderr_failed;
//	}
//
//	switch(pid_ = fork()) {
//	case -1:
//		goto fork_failed;
//	case 0:
//		/* Finish the plumbing in the child process. */
//		close(in[1]);
//		close(out[0]);
//		close(err[0]);
//		dup2(in[0], STDIN_FILENO);
//		dup2(out[1], STDOUT_FILENO);
//		dup2(err[1], STDERR_FILENO);
//		close(in[0]);
//		close(out[1]);
//		close(err[1]);
//		/* exec the requested command. */
//		execl(bash, "sh", "-c", cmd, NULL);
//		exit(0);
//	}
//
//	close(in[0]);
//	close(out[1]);
//	close(err[1]);
//
////	cdata->child_stdin = in[1];
////	cdata->child_stdout = out[0];
////	cdata->child_stderr = err[0];
//
////	return SSH_OK;
//
//fork_failed:
//	close(err[0]);
//	close(err[1]);
//stderr_failed:
//	close(out[0]);
//	close(out[1]);
//stdout_failed:
//	close(in[0]);
//	close(in[1]);
//stdin_failed:
////	return SSH_ERROR;
//}

int pty::resize(struct winsize *win)
{
	if (ioctl(pty_m_, TIOCSWINSZ, win) != 0) {
		LOGE_CLASS() << "Failed to resize pty: " << strerror(errno);
		return -1;
	}

	return 0;
}

} // namespace ssh
