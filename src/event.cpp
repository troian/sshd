#include <ssh/event.hpp>

#include <poll.h>

namespace ssh {

event::event() :
	  base_class("SSH.EVENT")
	, events_(POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL)
	, amount_(0)
{
	event_ = ssh_event_new();

	if (event_ == nullptr) {
		throw std::runtime_error("Couldn't allocate event object");
	}
}

event::~event()
{
	if (worker_.joinable()) {
		u_lock lock(lock_);
		amount_.store(0, std::memory_order_release);
		worker_.join();
	}

	ssh_event_free(event_);

	LOGI_CLASS() << "Exit events";
}

void event::fd_add(int fd, ssh_event_callback cb, void *userdata)
{
	if (ssh_event_add_fd(event_, fd, events_, cb, userdata) != SSH_OK) {
		throw std::runtime_error("Couldn't add an fd to the event");
	}
}

void event::fd_add(int16_t events, int fd, ssh_event_callback cb, void *userdata)
{
	if (ssh_event_add_fd(event_, fd, events, cb, userdata) != SSH_OK) {
		throw std::runtime_error("Couldn't add an fd to the event");
	}
}

void event::fd_del(int fd)
{
	if (ssh_event_remove_fd(event_, fd) != SSH_OK) {
		throw std::runtime_error("Couldn't remove fd from the event");
	}
}

void event::session_add(ssh_session s)
{
	if (ssh_event_add_session(event_, s) != SSH_OK) {
		throw std::runtime_error("Couldn't add the session to the event");
	}

	u_lock lock(lock_);

	if (amount_++ == 0) {
		worker_ = std::thread(&event::worker, this);
	}
}

void event::session_del(ssh_session s)
{
	if (ssh_event_remove_session(event_, s) != SSH_OK) {
		u_lock lock(lock_);
		amount_--;
		throw std::runtime_error("Couldn't remove the session from the event");
	}
}

void event::worker()
{
	while (amount_.load(std::memory_order_acquire) > 0) {
		if (ssh_event_dopoll(event_, 1000) == SSH_ERROR) {
			continue;
		}
	}

	LOGI_CLASS() << "Worker stopped";
}

} // namespace ssh
