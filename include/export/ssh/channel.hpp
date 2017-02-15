//
// Created by Artur Troian on 10/9/16.
//
#pragma once

#include <ssh/types.hpp>
#include <cstdint>

namespace ssh {

/**
 * \brief
 */
class channel : public base_class, public obj_management<class channel> {
private:
	class methods_base {
	private:
		friend class channel;
	protected:
		ssh_channel channel_;
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
				  sp_boost_io io
				, const std::string &log_name
				, session *s
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
				  sp_boost_io io
				, session *s
				, ssh_channel c = nullptr
				, ssh_channel_callbacks cb = nullptr
				, chan_conn_signal conn_sig = nullptr);

	virtual ~channel();

public:
	class get &get() {
		return get_;
	}

	class status &status() {
		return status_;
	}

	int write(const void *data, size_t len, bool is_stderr = false);

	virtual int on_data(void *data, uint32_t len, int is_stderr);

protected:
	session          *session_;
	ssh_channel       channel_;
	bool              foreign_chan_;
	sp_boost_io       io_;

private:
	chan_conn_signal  conn_sig_;
	class get         get_;
	class status      status_;
};

/**
 * \brief
 */
class proxy_channel : public channel {
public:
	/**
	 * \brief
	 *
	 * \param[in]   io
	 * \param[in]   session
	 * \param[in]   host
	 * \param[in]   port
	 * \param[in]   c
	 * \param[in]   cb
	 * \param[in]   conn_sig
	 */
	proxy_channel(
			  sp_boost_io io
			, session *s
			, const std::string &host
			, int port
			, ssh_channel c = nullptr
			, ssh_channel_callbacks cb = nullptr
			, chan_conn_signal conn_sig = nullptr);

	virtual ~proxy_channel();

public:
	template <typename... _Args>
	static std::shared_ptr<class proxy_channel> shared(_Args&&... __args) {
		return std::make_shared<class proxy_channel>(__args...);
	}

private:
	/**
	 * \brief
	 *
	 * \param[in]   host
	 * \param[in]   port
	 */
	void start(const std::string &host, int port);

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
	virtual int on_data(void *data, uint32_t len, int is_stderr);

private:
	boost::asio::ip::tcp::socket   sock_;
	boost::asio::ip::tcp::resolver resolver_;
	std::vector<uint8_t>           read_buf_;
};

} // namespace ssh
