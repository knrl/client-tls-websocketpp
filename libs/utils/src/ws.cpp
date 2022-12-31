#include "ws.hpp"

namespace websocket{
/**     connection_metadata      **/

connection_metadata::connection_metadata(int id, websocketpp::connection_hdl hdl, std::string uri)
    : m_id(id)
    , m_hdl(hdl)
    , m_status("Connecting")
    , m_uri(uri)
    , m_server("N/A")
{}

void connection_metadata::on_open(client * c, websocketpp::connection_hdl hdl) {
    m_status = "Open";

    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
}

void connection_metadata::on_fail(client * c, websocketpp::connection_hdl hdl) {
    m_status = "Failed";

    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    m_error_reason = con->get_ec().message();
}

void connection_metadata::on_close(client * c, websocketpp::connection_hdl hdl) {
    m_status = "Closed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    std::stringstream s;
    s << "close code: " << con->get_remote_close_code() << " (" 
      << websocketpp::close::status::get_string(con->get_remote_close_code()) 
      << "), close reason: " << con->get_remote_close_reason();
    m_error_reason = s.str();
}

std::ostream & operator<< (std::ostream & out, connection_metadata const & data) {
    out << "> URI: " << data.m_uri << "\n"
        << "> Status: " << data.m_status << "\n"
        << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
        << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);
    return out;
}

int connection_metadata::get_id(void) {
    return this->m_id;
}

websocketpp::connection_hdl connection_metadata::get_hdl(void) {
    return this->m_hdl;
}

std::string connection_metadata::get_status(void) {
    return this->m_status;
}

std::string connection_metadata::get_uri(void) {
    return this->m_uri;
}

std::string connection_metadata::get_server(void) {
    return this->m_server;
}

std::string connection_metadata::get_error_reason(void) {
    return this->m_error_reason;
}

/**     TLS INIT      **/

static context_ptr on_tls_init() {
    // establishes a SSL connection
    context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::single_dh_use);
    } catch (std::exception &e) {
        std::cout << "Error in context pointer: " << e.what() << std::endl;
    }
    return ctx;
}

/**     websocket_endpoint      **/

websocket_endpoint::websocket_endpoint() 
    : m_next_id(0) {
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

    m_endpoint.set_access_channels(websocketpp::log::alevel::none);
    m_endpoint.set_error_channels(websocketpp::log::elevel::fatal);

    m_endpoint.init_asio();
    m_endpoint.set_tls_init_handler(websocketpp::lib::bind(&on_tls_init));
    m_endpoint.start_perpetual();
}

int websocket_endpoint::connect(std::string const & uri) {
    websocketpp::lib::error_code ec;

    std::cout << uri << "\n";

    client::connection_ptr con = m_endpoint.get_connection(uri, ec);

    if (ec) {
        std::cout << "> Connect initialization error: " << ec.message() << std::endl;
        return -1;
    }

    int new_id = m_next_id++;
    connection_metadata::ptr metadata_ptr(new connection_metadata(new_id, con->get_handle(), uri));
    m_connection_list[new_id] = metadata_ptr;

    con->set_open_handler(websocketpp::lib::bind(
        &connection_metadata::on_open,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));
    con->set_fail_handler(websocketpp::lib::bind(
        &connection_metadata::on_fail,
        metadata_ptr,
        &m_endpoint,
        websocketpp::lib::placeholders::_1
    ));
    con->set_message_handler(
        [this](websocketpp::connection_hdl, client::message_ptr msg) {
          json j = msg->get_raw_payload().c_str();
          on_message_cb(j);
    });

    m_endpoint.connect(con);
    m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));

    return new_id;
}

void websocket_endpoint::send_request_loop(std::string message) {
    websocketpp::lib::error_code ec;
    const uint32_t id{0}; 

    con_list::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end())
    {
        std::cout << "> No connection found with id " << id << std::endl;
        return;
    }

    std::thread t1([&]() {
        while (true) {
            m_endpoint.send(metadata_it->second->get_hdl(), message, websocketpp::frame::opcode::text, ec);
            if(ec){
                std::cout << "> Error sending message: " << ec.message() << std::endl;
                return;
            }
        }
    });
}

void websocket_endpoint::set_on_message_cb(OnMessageCB message_cb) {
    on_message_cb = message_cb;
}

connection_metadata::ptr websocket_endpoint::get_metadata(int id) const {
    con_list::const_iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        return connection_metadata::ptr();
    } else {
        return metadata_it->second;
    }
}

void websocket_endpoint::close(int id, websocketpp::close::status::value code) {
    websocketpp::lib::error_code ec;
    
    con_list::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        std::cout << "> No connection found with id " << id << std::endl;
        return;
    }
    
    m_endpoint.close(metadata_it->second->get_hdl(), code, "", ec);
    if (ec) {
        std::cout << "> Error initiating close: " << ec.message() << std::endl;
    }
}

websocket_endpoint::~websocket_endpoint() {
    m_endpoint.stop_perpetual();
    
    for (con_list::const_iterator it = m_connection_list.begin(); it != m_connection_list.end(); ++it) {
        if (it->second->get_status() != "Open") {
            // Only close open connections
            continue;
        }
        
        std::cout << "> Closing connection " << it->second->get_id() << std::endl;
        
        websocketpp::lib::error_code ec;
        m_endpoint.close(it->second->get_hdl(), websocketpp::close::status::going_away, "", ec);
        if (ec) {
            std::cout << "> Error closing connection " << it->second->get_id() << ": "  
                      << ec.message() << std::endl;
        }
    }
    
    m_thread->join();
}
} // namespace websocket