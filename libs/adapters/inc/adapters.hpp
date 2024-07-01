#ifndef ADAPTERS__HPP
#define ADAPTERS__HPP

#include "ws.hpp"
#include "ringbuffer.hpp"

class BinanceSocket {
public:
    explicit BinanceSocket(const std::vector<std::string>& symbols, const std::string& uri);
    using Dict = std::map<std::string, std::vector<double>>; // "asks" : {1,1}
    std::vector<Dict> getData(const std::string&);           // returns {"asks" : {1,1}, "bids" : {1,1}}
    void createUri(std::string& uri);
    void setOnMessage();
    void run();

private:
    const std::string cur_postfix{"USDT"};
    std::vector<std::string> symbols;
    std::map<std::string, std::vector<Dict>> data;

    websocket::websocket_endpoint::OnMessageCB message_cb;
    websocket::websocket_endpoint ws;
    std::string uri{};

    static const uint32_t BUFFER_SIZE = 64;
    ringbuffer<std::string, BUFFER_SIZE> buffer;
};

class GateioCoinWs {
public:
    explicit GateioCoinWs(const std::vector<std::string>& symbols, const std::string& uri);
    using Dict = std::map<std::string, std::vector<double>>; // "asks" : {1,1}
    std::vector<Dict> getData(const std::string&);                  // returns {"asks" : {1,1}, "bids" : {1,1}}
    std::string createSendMessage();
    void setOnMessage();
    void run();

private:
    const std::string cur_postfix{"_USDT"};
    std::vector<std::string> symbols;
    std::map<std::string, std::vector<Dict>> data;

    websocket::websocket_endpoint::OnMessageCB message_cb;
    websocket::websocket_endpoint ws;
    std::string uri{};

    std::string message{};
    static const uint32_t BUFFER_SIZE = 64;
    ringbuffer<std::string, BUFFER_SIZE> buffer;
};

#endif // ADAPTERS_HPP