#include <ssh/channel.hpp>

namespace ssh {

proxy_channel::proxy_channel(
			  sp_boost_io io
			, session *s
			, const std::string &host
			, int port
			, ssh_channel c
			, ssh_channel_callbacks cb
			, chan_conn_signal conn_sig) :
	  channel(io, "SSH.CH.PROXY", s, c, cb, conn_sig)
	, sock_(DEREF_IO(io))
	, resolver_(DEREF_IO(io))
	, read_buf_(10240)
{
	try {
		start(host, port);
	} catch (...) {
		throw;
	}
}

proxy_channel::~proxy_channel()
{
	sock_.close();
	resolver_.cancel();
}

void proxy_channel::start(const std::string &host, int port)
{
	boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
	boost::asio::ip::tcp::resolver::iterator iterator = resolver_.resolve(query);

	try {
		boost::asio::connect(sock_, iterator);
	} catch (...) {
		throw;
	}

	sock_.async_read_some(boost::asio::buffer(read_buf_.data(), read_buf_.capacity()),
	                        boost::bind(
		                          &proxy_channel::handle_read
		                        , this
		                        , boost::asio::placeholders::error
		                        , boost::asio::placeholders::bytes_transferred));
}

void proxy_channel::handle_read(const boost::system::error_code &ec, size_t bytes_received)
{
	if (!ec) {
		int len = bytes_received;
		uint8_t *ptr = read_buf_.data();

		for (int lenw = 0; len > 0; len -= lenw, ptr += lenw) {
			try {
				lenw = channel::write(ptr, len);
			} catch (ssh::ssh_exception &e) {
				LOGE_CLASS() << "Could'nt write FWD -> SSH: " << e.what();
				break;
			}
		}

		sock_.async_read_some(boost::asio::buffer(read_buf_.data(), read_buf_.capacity()),
			boost::bind(
				&proxy_channel::handle_read
				, this
				, boost::asio::placeholders::error
				, boost::asio::placeholders::bytes_transferred));
	}
}

size_t proxy_channel::on_data(void *data, uint32_t len, int is_stderr)
{
	(void)is_stderr;

	return boost::asio::write(sock_, boost::asio::buffer(data, len));
}

} // namespace ssh
