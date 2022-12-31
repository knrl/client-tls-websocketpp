#include "ws.hpp"
#include "ringbuffer.hpp"

class BinanceSocket {
public:
    BinanceSocket(std::vector<std::string> symbols, const std::string uri);
    using dict = std::map<std::string, std::vector<double>>; // "asks" : {1,1}
    std::vector<dict> getData(std::string);                  // returns {"asks" : {1,1}, "bids" : {1,1}}
    std::string create_uri(std::string uri);
    void set_on_message();
    void run();

private:
    const std::string cur_postfix{"USDT"};
    std::vector<std::string> symbols;
    std::map<std::string, std::vector<dict>> data;

    websocket::websocket_endpoint::OnMessageCB message_cb;
    websocket::websocket_endpoint ws;
    std::string uri{};

    static const uint32_t BUFFER_SIZE = 64;
    ringbuffer<std::string, BUFFER_SIZE> buffer;
};

class GateioCoinWs {
public:
    GateioCoinWs(std::vector<std::string> symbols, const std::string uri);
    using dict = std::map<std::string, std::vector<double>>; // "asks" : {1,1}
    std::vector<dict> getData(std::string);                  // returns {"asks" : {1,1}, "bids" : {1,1}}
    std::string create_send_message();
    void set_on_message();
    void run();

private:
    const std::string cur_postfix{"_USDT"};
    std::vector<std::string> symbols;
    std::map<std::string, std::vector<dict>> data;

    websocket::websocket_endpoint::OnMessageCB message_cb;
    websocket::websocket_endpoint ws;
    std::string uri{};

    std::string message{};
    static const uint32_t BUFFER_SIZE = 64;
    ringbuffer<std::string, BUFFER_SIZE> buffer;
};