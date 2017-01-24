//
// Created by Artur Troian on 10/9/16.
//
#pragma once

#include <ssh/types.hpp>

namespace ssh {

/**
 * \brief
 */
class event : public base_class {
public:
	event();
	virtual ~event();

public:
	void fd_add(int fd, ssh_event_callback cb, void *userdata);

	void fd_add(short events, int fd, ssh_event_callback cb, void *userdata);

	void fd_del(int fd);

	void session_add(ssh_session s);

	void session_del(ssh_session s);

private:
	void worker();

private:
	ssh_event        event_;
	short            events_;

	std::mutex       lock_;
	std::atomic_int  amount_;

	std::thread      worker_;
//	std::atomic_bool worker_alive_;
};

} // namespace ssh
