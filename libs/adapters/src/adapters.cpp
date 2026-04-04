#include "adapters.hpp"
#include <chrono>
#include <cstring>

/**      BinanceSocket       **/

BinanceSocket::BinanceSocket(const std::vector<std::string>& symbols, const std::string& uri) {
    if (symbols.size() == 0)
        return;

    Dict asks_dict, bids_dict;
    asks_dict["asks"] = std::vector<double>{0.0,0.0};
    bids_dict["bids"] = std::vector<double>{0.0,0.0};
    std::vector<Dict> temp_vec{asks_dict, bids_dict};

    for(auto symbol : symbols) {
        std::string new_symbol{symbol + this->cur_postfix};
        this->symbols.push_back(new_symbol);
        this->data[new_symbol] = temp_vec;
    }

    // create link for all given symbols, example link:
    // this->uri = "wss://stream.binance.com:9443/ws/btcusdt@bookTicker";
    std::string mutable_uri = uri;
    createUri(mutable_uri);
    setOnMessage();
}

void BinanceSocket::createUri(std::string& uri) {
    this->uri = uri;
    for(auto symbol : this->symbols) {
        transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
        this->uri += "/" + symbol + "@bookTicker";
    }
}

std::vector<BinanceSocket::Dict> BinanceSocket::getData(const std::string& symbol) {
    auto it = this->data.find(symbol);
    if (it == this->data.end()) {
        std::cout << symbol << " is not found.\n";
        return {};
    }
    return it->second;
}

void BinanceSocket::setOnMessage() {
    auto message_cb = [&](std::string j) {
        // wait-free atomic ring buffer
        buffer.push(j);
    };

    ws.setOnMessage_cb(message_cb);
}

void BinanceSocket::run() {
    running = true;
    consumer_thread = std::thread([this]() {
        json emptyJSON;
        std::string value;
        while (running) {
            if (!buffer.pop(value)) {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                continue;
            }
            auto tick = json::parse(value, nullptr, false);
            if (tick.is_discarded()) {
                continue;
            }
            auto data = tick.value("data", emptyJSON);
            std::string symbol = data.value("s", "");

            if (symbol.empty() || data.find("a") == data.end() || data.find("b") == data.end() || data.find("A") == data.end() || data.find("B") == data.end()) {
                continue;
            }

            this->data[symbol][0]["asks"][0] = data["a"].get<double>();
            this->data[symbol][0]["asks"][1] = data["A"].get<double>();
            this->data[symbol][1]["bids"][0] = data["b"].get<double>();
            this->data[symbol][1]["bids"][1] = data["B"].get<double>();
        }
    });

    ws.connect(this->uri);
}

BinanceSocket::~BinanceSocket() {
    running = false;
    if (consumer_thread.joinable()) {
        consumer_thread.join();
    }
}

/**      GateioCoinWs       **/

GateioCoinWs::GateioCoinWs(const std::vector<std::string>& symbols, const std::string& uri) {
    if (symbols.size() == 0)
        return;

    Dict asks_dict, bids_dict;
    asks_dict["asks"] = std::vector<double>{0.0,0.0};
    bids_dict["bids"] = std::vector<double>{0.0,0.0};
    std::vector<Dict> temp_vec{asks_dict, bids_dict};

    for(auto symbol : symbols) {
        std::string new_symbol{symbol + this->cur_postfix};
        this->symbols.push_back(new_symbol);
        this->data[new_symbol] = temp_vec;
    }

    this->uri = uri;
    // create request message for given symbols
    this->message = createSendMessage();
    setOnMessage();
}

std::string GateioCoinWs::createSendMessage() {
    std::string symbol_list_str = "[";
    for (const auto& symbol : symbols) {
        symbol_list_str += "\"" + symbol + "\",";
    }
    if (!symbols.empty()) {
        symbol_list_str.pop_back(); // Remove the trailing comma
    }
    symbol_list_str += "]";

    json json_message = {
        {"channel", "spot.book_ticker"},
        {"event", "subscribe"},
        {"payload", symbol_list_str}
    };

    return json_message.dump();
}

std::vector<GateioCoinWs::Dict> GateioCoinWs::getData(const std::string& symbol) {
    auto it = this->data.find(symbol);
    if (it == this->data.end()) {
        std::cout << symbol << " is not found.\n";
        return {};
    }
    return it->second;
}

void GateioCoinWs::setOnMessage() {
    auto message_cb = [&](std::string j) {
        // wait-free atomic ring buffer
        buffer.push(j);
    };

    ws.setOnMessage_cb(message_cb);
}

void GateioCoinWs::run() {
    running = true;
    consumer_thread = std::thread([this]() {
        json emptyJSON;
        std::string value;
        while (running) {
            if (!buffer.pop(value)) {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                continue;
            }
            auto tick = json::parse(value, nullptr, false);
            if (tick.is_discarded()) {
                continue;
            }
            auto data = tick.value("data", emptyJSON);
            std::string symbol = data.value("s", "");

            if (symbol.empty() || data.find("a") == data.end() || data.find("b") == data.end() || data.find("A") == data.end() || data.find("B") == data.end()) {
                continue;
            }

            this->data[symbol][0]["asks"][0] = data["a"].get<double>();
            this->data[symbol][0]["asks"][1] = data["A"].get<double>();
            this->data[symbol][1]["bids"][0] = data["b"].get<double>();
            this->data[symbol][1]["bids"][1] = data["B"].get<double>();
        }
    });

    ws.connect(this->uri);
    ws.send_request_loop(this->message);
}

GateioCoinWs::~GateioCoinWs() {
    running = false;
    if (consumer_thread.joinable()) {
        consumer_thread.join();
    }
}
