#include <poll.h>

#include <ssh/ssh.hpp>
#include <ssh/session.hpp>
#include <ssh/server_session.hpp>
#include <ssh/channel.hpp>
#include <ssh/sshd.hpp>
#include <ssh/bind.hpp>

namespace ssh {

sshd::sshd(boost::asio::io_service &io, int port) :
	base_class("SSHD")
{
	try {
		evt_ = std::make_shared<event>();

		auth_ = std::make_shared<server_auth_add>();
		auth_->add_auth("amr", "12345", strlen("12345"));

		bind_ = std::make_unique<sshbind>(io, std::dynamic_pointer_cast<server_auth>(auth_));
		bind_->option_set(SSH_BIND_OPTIONS_BINDADDR, "127.0.0.1");
		bind_->option_set(SSH_BIND_OPTIONS_BINDPORT, &port);
		bind_->option_set(SSH_BIND_OPTIONS_RSAKEY, "/Users/amr/.ssh/simply-servers.pem");

		bind_->set_blocking(false);
		bind_->listen(boost::bind(&sshd::incoming_connection, this));

		acceptor_alive_.store(true);
		acceptor_ = std::thread(std::bind(&sshd::acceptor_thread, this));

		LOGI_CLASS() << "Started";
	} catch (...) {
		throw;
	}
}

sshd::~sshd()
{
	acceptor_alive_.store(false);
	bind_.reset();
	acceptor_.join();
	LOGI_CLASS() << "Stopped";
}

void sshd::incoming_connection()
{
	LOGI_CLASS() << "Handling incoming connection";
}

void sshd::acceptor_thread()
{
	sp_event evt = std::make_shared<ssh::event>();

	while (acceptor_alive_.load()) {
		try {
			bool ses_configured = false;

			sp_srv_session ses = bind_->accept();

			int verbosity = SSH_LOG_FUNCTIONS;
			sp_channel chan;

			ses->set_option(SSH_OPTIONS_SSH_DIR, "~/");
			ses->set_option(SSH_OPTIONS_LOG_VERBOSITY, &verbosity);

			ses->key_exchange();

			ses->add_to_event(evt_);

			while (1);

			// Fall through authorization
//			while (!ses_configured) {
//				message = ssh_message_get(ses->c_session());
//				if (message == nullptr) {
//					ses_configured = false;
//					LOGE_CLASS() << "Session received empty message. Cleaning up session";
//					break;
//				} else {
//					ses->handle_msg_auth(message);
//					if (ses->is_authorized() == true) {
//						ses_configured = true;
//						break;
//					}
//				}
//			}
//
//			if (!ses_configured) {
//				continue;
//			} else {
//				// Falling through open channel
//				ses_configured = false;
//
//				message = ssh_message_get(ses->c_session());
//				if (message != nullptr) {
//					if (ses->handle_msg_open_channel(message, ses, chan) == 0) {
//						ses_configured = true;
//					}
//				}
//			}
//
//			tty_env env;
//			if (!ses_configured) {
//				continue;
//			} else {
//				ses_configured = false;
//
//				// Handle open shell/pty
//				message = ssh_message_get(ses->c_session());
//				if (message != nullptr) {
//					if (ses->handle_msg_channel_req(message, env) != 0) {
//						continue;
//					}
//				}
//
////				// Handle env request
////				message = ssh_message_get(ses->c_session());
////				if (message != nullptr) {
////					if (ses->handle_msg_channel_req(message, env) != 0) {
////						continue;
////					}
////				}
//
//				ses_configured = true;
//			}
//
//			if (!ses_configured) {
//				continue;
//			}

//			int fd;
//
//			sp_tty tty_ses;
//
////			env.insert(std::pair<std::string, std::string>("HOME", "~/"));
//			try {
//				tty_ses = std::make_shared<ssh::tty>(env, "/bin/bash", fd);
//			} catch (...) {
//
//			}
//
//			sessions_.insert(std::pair<void *, sp_session>(ses->get_session(), ses));
//
//			cb_s.userdata = &fd;
////			ssh_callbacks_init(&cb_s);
//			chan->set_sb(&cb_s);
//
//
//			evt->fd_add(fd, copy_fd_to_chan, chan->c_channel());
//
//			ses->add_to_event(evt);
//
//			while (1);
//
//
//			int auth  = 0;
//			int shell = 0;
//			ssh_message message;
//
//			do {
//				message = ssh_message_get(ses->c_session());
//				if (!message)
//					break;
//				switch (ssh_message_type(message)) {
//				case SSH_REQUEST_AUTH:
//					switch (ssh_message_subtype(message)) {
//					case SSH_AUTH_METHOD_PASSWORD:
//						printf("User %s wants to auth with pass %s\n", ssh_message_auth_user(message),
//						       ssh_message_auth_password(message));
//						ssh_message_auth_reply_success(message, 0);
//						auth = 1;
//						break;
//					case SSH_AUTH_METHOD_NONE:
//					default:
//						printf("User %s wants to auth with unknown auth %d\n",
//						       ssh_message_auth_user(message),
//						       ssh_message_subtype(message));
//						ssh_message_auth_set_methods(message,
//						                             SSH_AUTH_METHOD_PASSWORD |
//						                             SSH_AUTH_METHOD_INTERACTIVE);
//						ssh_message_reply_default(message);
//						break;
//					}
//					break;
//				default:
//					ssh_message_auth_set_methods(message,
//					                             SSH_AUTH_METHOD_PASSWORD |
//					                             SSH_AUTH_METHOD_INTERACTIVE);
//					ssh_message_reply_default(message);
//				}
//				ssh_message_free(message);
//			} while (auth == 0);
//
//			/* wait for a channel session */
//			LOGI_CLASS() << "waiting for channel";
//
//			ssh_channel chan = NULL;
//
//			do {
//				message = ssh_message_get(ses->c_session());
//				if (message != NULL) {
//					LOGI_CLASS() << "MSG type: " << ssh_message_subtype(message);
//					if (ssh_message_type(message) == SSH_REQUEST_CHANNEL_OPEN &&
//					    ssh_message_subtype(message) == SSH_CHANNEL_SESSION) {
//						chan = ssh_message_channel_request_open_reply_accept(message);
//						break;
//					} else {
//						ssh_message_reply_default(message);
//					}
//					ssh_message_free(message);
//				} else {
//					LOGI_CLASS() << "Empty message";
//					break;
//				}
//			} while (!chan);
//
//			LOGI_CLASS() << "accepted tunnel channel";
//			do {
//				message = ssh_message_get(ses->c_session());
//				if (message != NULL) {
//					if (ssh_message_type(message) == SSH_REQUEST_CHANNEL) {
//						if (ssh_message_subtype(message) == SSH_CHANNEL_REQUEST_SHELL) {
//							shell = 1;
//							LOGI_CLASS() << "shell";
//							ssh_message_channel_request_reply_success(message);
//							ssh_message_free(message);
//							break;
//						} else if (ssh_message_subtype(message) == SSH_CHANNEL_REQUEST_PTY) {
//							ssh_message_channel_request_reply_success(message);
//							ssh_message_free(message);
//							continue;
//						}
//					}
//					ssh_message_reply_default(message);
//					ssh_message_free(message);
//				} else {
//					break;
//				}
//			} while (!shell);
//
//			socket_t fd;
//			struct termios *term = NULL;
//			struct winsize *win = NULL;
//			pid_t childpid;
//			ssh_event event;
//			short events;
//
//			childpid = forkpty(&fd, NULL, term, win);
//			if (childpid == 0) {
//				execl("/bin/bash", "/bin/bash", ( char * ) NULL);
//				abort();
//			} else if (childpid) {
//				LOGI_CLASS() << "TTY created: PID: " << childpid;
//			} else {
//				throw std::runtime_error("Couldn't create TTY");
//			}
//
//			cb_s.userdata = &fd;
//			ssh_callbacks_init(&cb_s);
//			ssh_set_channel_callbacks(chan, &cb_s);
//
//			events = POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL;
//
//			event = ssh_event_new();
//			if (event == NULL) {
//				throw std::runtime_error("Couldn't get a event");
//			}
//
//			if (ssh_event_add_fd(event, fd, events, copy_fd_to_chan, chan) != SSH_OK) {
//				throw std::runtime_error("Couldn't add an fd to the event");
//			}
//
//			if (ssh_event_add_session(event, ses->c_session()) != SSH_OK) {
//				throw std::runtime_error("Couldn't add the session to the event");
//			}
//
//			do {
//				ssh_event_dopoll(event, 1000);
//			} while (!ssh_channel_is_closed(chan));
//
//			ssh_event_remove_fd(event, fd);
//
//			ssh_event_remove_session(event, ses->c_session());
//
//			ssh_event_free(event);
//			while (1);
		} catch (const std::exception &e) {
			LOGE_CLASS() << e.what();
		} catch (ssh_exception &e) {
			LOGE_CLASS() << e.what();
		}
	}
}

} // namespace ssh

