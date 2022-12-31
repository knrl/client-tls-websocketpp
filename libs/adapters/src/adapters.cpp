#include "adapters.hpp"
#include <cstring>

/**      BinanceSocket       **/

BinanceSocket::BinanceSocket(std::vector<std::string> symbols, const std::string uri) {
    if (symbols.size() == 0)
        return;

    dict asks_dict, bids_dict;
    asks_dict["asks"] = std::vector<double>{0.0,0.0};
    bids_dict["bids"] = std::vector<double>{0.0,0.0};
    std::vector<dict> temp_vec{asks_dict, bids_dict};
    for(auto symbol : symbols) {
        std::string new_symbol{symbol + this->cur_postfix};
        this->symbols.push_back(new_symbol);
        this->data[new_symbol] = temp_vec;
    }

    // create link for all given symbols
    // this->uri = "wss://stream.binance.com:9443/ws/btcusdt@bookTicker";
    this->uri = create_uri(uri);
    set_on_message();
}

std::string BinanceSocket::create_uri(std::string uri) {
    for(auto symbol : this->symbols) {
        transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
        uri += "/" + symbol + "@bookTicker";
    }
    return uri;
}

std::vector<BinanceSocket::dict> BinanceSocket::getData(std::string symbol) {
    if (this->data[symbol].size() == 0)
        std::cout << symbol << " is not found.\n";
    return this->data[symbol]; // example: symbol = "BTCUSDT"
}

void BinanceSocket::set_on_message() {
    auto f = [&](std::string j) {
        // wait-free atomic ring buffer
        buffer.push(j);
    };

    ws.set_on_message_cb(f);
}

void BinanceSocket::run() {
    std::thread t1([&]() {
        json emptyJSON;
        std::string value;
        buffer.pop(value);

        json tick = json::parse(value);
        json data = tick.value("data", emptyJSON);
        std::string symbol = data.at("symbol");
    
        data[symbol][0]["asks"][0] = data.at("a");
        data[symbol][0]["asks"][1] = data.at("A");
        data[symbol][0]["bids"][0] = data.at("b");
        data[symbol][0]["bids"][1] = data.at("B");
    });

    ws.connect(this->uri);
}

/**      GateioCoinWs       **/

GateioCoinWs::GateioCoinWs(std::vector<std::string> symbols, const std::string uri) {
    if (symbols.size() == 0)
        return;

    dict asks_dict, bids_dict;
    asks_dict["asks"] = std::vector<double>{0.0,0.0};
    bids_dict["bids"] = std::vector<double>{0.0,0.0};
    std::vector<dict> temp_vec{asks_dict, bids_dict};
    for(auto symbol : symbols) {
        std::string new_symbol{symbol + this->cur_postfix};
        this->symbols.push_back(new_symbol);
        this->data[new_symbol] = temp_vec;
    }

    this->uri = uri;
    // create request message for given symbols
    this->message = create_send_message();
    set_on_message();
}

std::string GateioCoinWs::create_send_message() {
    std::string symbol_list_str{"["};
    for(auto symbol : symbols) {
        symbol_list_str += "," + symbol; 
    }
    symbol_list_str += "]";

    json json_message = {
        {"channel", "spot.book_ticker"},
        {"event",   "subscribe"},
        {"payload", symbol_list_str}
    };

    return json_message;
}

std::vector<BinanceSocket::dict> GateioCoinWs::getData(std::string symbol) {
    if (this->data[symbol].size() == 0)
        std::cout << symbol << " is not found.\n";
    return this->data[symbol]; // example: symbol = "BTC_USDT"
}

void GateioCoinWs::set_on_message() {
    auto f = [&](std::string j) {
        // wait-free atomic ring buffer
        buffer.push(j);
    };

    ws.set_on_message_cb(f);
}

void GateioCoinWs::run() {
    std::thread t1([&]() {
        json emptyJSON;
        std::string value;
        buffer.pop(value);

        json tick = json::parse(value);
        json data = tick.value("data", emptyJSON);
        std::string symbol = data.at("symbol");
    
        data[symbol][0]["asks"][0] = data.at("a");
        data[symbol][0]["asks"][1] = data.at("A");
        data[symbol][0]["bids"][0] = data.at("b");
        data[symbol][0]["bids"][1] = data.at("B");
    });

    ws.connect(this->uri);
    ws.send_request_loop(this->message);
}