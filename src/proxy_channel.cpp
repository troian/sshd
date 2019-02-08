#include <ssh/channel.hh>

namespace ssh {

proxy_channel::proxy_channel(
			  boost_io::sp io
			, boost::shared_ptr<session> s
			, ssh_channel c
			, ssh_channel_callbacks cb
			, chan_conn_signal conn_sig)
	: channel(io, "SSH.CH.PROXY", s, c, cb, conn_sig)
	, _sock(DEREF_IO(io))
	, _read_buf(10240)
{
}

proxy_channel::~proxy_channel() {
	_sock.close();
}

void proxy_channel::start(const std::string &host, int port) {
	LOGI_CLASS() << "forwarding connection to " << host << ":" << std::to_string(port);
	tcp::resolver::query query(host, std::to_string(port));
	tcp::resolver _resolver(DEREF_IO(_io));
	tcp::resolver::iterator iterator = _resolver.resolve(query);

	try {
		boost::asio::connect(_sock, iterator);
	} catch (...) {
		throw;
	}

	LOGI_CLASS() << "connection forwarding started";

	_sock.async_read_some(boost::asio::buffer(_read_buf.data(), _read_buf.capacity()),
	                        boost::bind(
	                          &proxy_channel::handle_read
	                        , shared_from_this()
	                        , boost::asio::placeholders::error
	                        , boost::asio::placeholders::bytes_transferred));
}

void proxy_channel::handle_read(const boost::system::error_code &ec, size_t bytes_received) {
	if (!ec) {
		size_t len = bytes_received;
		uint8_t *ptr = _read_buf.data();

		for (int lenw = 0; len > 0; len -= lenw, ptr += lenw) {
			try {
				lenw = channel::write(ptr, len);
			} catch (ssh::ssh_exception &e) {
				LOGE_CLASS() << "Could'nt write FWD -> SSH: " << e.what();
				break;
			}
		}

		_sock.async_read_some(boost::asio::buffer(_read_buf.data(), _read_buf.capacity()),
			boost::bind(
				  &proxy_channel::handle_read
				, shared_from_this()
				, boost::asio::placeholders::error
				, boost::asio::placeholders::bytes_transferred));
	}
}

size_t proxy_channel::on_data(void *data, uint32_t len, int is_stderr) {
	(void) is_stderr;

	return boost::asio::write(_sock, boost::asio::buffer(data, len));
}

} // namespace ssh
