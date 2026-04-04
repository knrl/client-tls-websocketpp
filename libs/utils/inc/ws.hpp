#ifndef WS__HPP
#define WS__HPP

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <functional>

#include "json.hpp"

using json = nlohmann::json;

namespace websocket{
typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;

class connection_metadata {
public:
    typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

    connection_metadata(int id, websocketpp::connection_hdl hdl, std::string uri);

    void on_open (client * c, websocketpp::connection_hdl hdl);
    void on_fail (client * c, websocketpp::connection_hdl hdl);
    void on_close(client * c, websocketpp::connection_hdl hdl);

    friend std::ostream & operator<< (std::ostream & out, connection_metadata const & data);

    int get_id(void);
    websocketpp::connection_hdl get_hdl(void);
    std::string get_status(void);
    std::string get_uri(void);
    std::string get_server(void);
    std::string get_error_reason(void);

private:
    int m_id;
    websocketpp::connection_hdl m_hdl;  // handle id, an optional id
    std::string m_status;               // connection status
    std::string m_uri;                  // address
    std::string m_server;
    std::string m_error_reason;
};

class websocket_endpoint {
public:
    using OnMessageCB = std::function<void(std::string j)>;

    websocket_endpoint ();
    ~websocket_endpoint();

    int connect(std::string const & uri);
    connection_metadata::ptr get_metadata(int id) const;
    void close(int id, websocketpp::close::status::value code);
    void setOnMessage_cb(OnMessageCB cb);
    void send_request_loop(std::string message);

private:
    typedef std::map<int,connection_metadata::ptr> con_list;

    client m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    std::thread m_send_thread;
    std::atomic<bool> m_sending{false};
    OnMessageCB on_message_cb;
    con_list m_connection_list;
    int m_next_id;
};
} // namespace websocket

#endif // WS__HPP