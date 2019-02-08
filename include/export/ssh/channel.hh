//
// Created by Artur Troian on 10/9/16.
//
#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <ssh/types.hh>
#include <ssh/session.hh>

namespace ssh {

/**
 * \brief
 */
class channel : public base_class, public obj_management<class channel>, public boost::enable_shared_from_this<channel> {
private:
	class methods_base {
	private:
		friend class channel;
	protected:
		ssh_channel _channel;
	};

	class get : public methods_base {
	public:
		ssh_channel channel();
	};

	class status : public methods_base {
	public:
		bool is_open();
		bool is_eof();
	};

public:
	/**
	 * \brief
	 *
	 * \param[in]   io
	 * \param[in]   log_name
	 * \param[in]   s
	 * \param[in]   c
	 * \param[in]   cb
	 * \param[in]   conn_sig
	 */
	explicit channel(
				  boost_io::sp io
				, const std::string &log_name
				, boost::shared_ptr<session> s
				, ssh_channel c = nullptr
				, ssh_channel_callbacks cb = nullptr
				, chan_conn_signal conn_sig = nullptr);

	/**
	 * \brief   Create SSH channel from pure object
	 *
	 * \param[in]  s
	 * \param[in]  c
	 */
	explicit channel(
				  boost_io::sp io
				, boost::shared_ptr<session> s
				, ssh_channel c = nullptr
				, ssh_channel_callbacks cb = nullptr
				, chan_conn_signal conn_sig = nullptr);

	~channel() override;

public:
	channel(const channel &) = delete;
	channel &operator=(const channel &) = delete;

public:
	class get &get() {
		return _get;
	}

	class status &status() {
		return _status;
	}

	int write(const void *data, size_t len, bool is_stderr = false);

	virtual size_t on_data(void *data, uint32_t len, int is_stderr);

protected:
	boost::shared_ptr<session> _session;
	ssh_channel                _channel;
	bool                       _foreign_chan;
	boost_io::sp               _io;

private:
	chan_conn_signal  _conn_sig;
	class get         _get;
	class status      _status;
};

/**
 * \brief
 */
class proxy_channel : public channel {
private:
	using tcp = boost::asio::ip::tcp;

public:
	/**
	 * \brief
	 *
	 * \param[in]   io;
	 * \param[in]   session
	 * \param[in]   host
	 * \param[in]   port
	 * \param[in]   c
	 * \param[in]   cb
	 * \param[in]   conn_sig
	 */
	proxy_channel(
			  boost_io::sp io
			, boost::shared_ptr<session> s
			, ssh_channel c = nullptr
			, ssh_channel_callbacks cb = nullptr
			, chan_conn_signal conn_sig = nullptr);

	~proxy_channel() override;

public:
	/**
	 * \brief
	 *
	 * \param[in]   host
	 * \param[in]   port
	 */
	void start(const std::string &host, int port);

public:
	auto shared_from_this() {
		return shared_from(this);
	}

private:
	/**
	 * \brief
	 *
	 * \param[in]   ec
	 * \param[in]   bytes_received
	 */
	void handle_read(const boost::system::error_code &ec, size_t bytes_received);

	/**
	 * \brief
	 *
	 * \param[in]   data
	 * \param[in]   len
	 * \param[in]   is_stderr
	 *
	 * \return
	 */
	size_t on_data(void *data, uint32_t len, int is_stderr) override;

private:
	tcp::socket           _sock;
	std::vector<uint8_t>  _read_buf;
};

} // namespace ssh
