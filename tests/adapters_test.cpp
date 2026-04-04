#include <gtest/gtest.h>
#include "adapters.hpp"

// --- BinanceSocket Tests ---

TEST(BinanceSocketTest, ConstructorInitializesData) {
    std::vector<std::string> symbols{"BTC", "ETH"};
    const std::string uri = "wss://stream.binance.com:9443/ws";
    BinanceSocket bs(symbols, uri);

    auto btc = bs.getData("BTCUSDT");
    ASSERT_EQ(btc.size(), 2);
    EXPECT_EQ(btc[0].count("asks"), 1);
    EXPECT_EQ(btc[1].count("bids"), 1);
    EXPECT_DOUBLE_EQ(btc[0]["asks"][0], 0.0);
    EXPECT_DOUBLE_EQ(btc[0]["asks"][1], 0.0);
    EXPECT_DOUBLE_EQ(btc[1]["bids"][0], 0.0);
    EXPECT_DOUBLE_EQ(btc[1]["bids"][1], 0.0);

    auto eth = bs.getData("ETHUSDT");
    ASSERT_EQ(eth.size(), 2);
}

TEST(BinanceSocketTest, GetDataUnknownSymbolReturnsEmpty) {
    std::vector<std::string> symbols{"BTC"};
    const std::string uri = "wss://stream.binance.com:9443/ws";
    BinanceSocket bs(symbols, uri);

    auto result = bs.getData("UNKNOWN");
    EXPECT_TRUE(result.empty());
}

TEST(BinanceSocketTest, EmptySymbolsList) {
    std::vector<std::string> symbols{};
    const std::string uri = "wss://stream.binance.com:9443/ws";
    BinanceSocket bs(symbols, uri);

    auto result = bs.getData("BTCUSDT");
    EXPECT_TRUE(result.empty());
}

// --- GateioCoinWs Tests ---

TEST(GateioCoinWsTest, ConstructorInitializesData) {
    std::vector<std::string> symbols{"BTC", "ETH"};
    const std::string uri = "wss://fx-ws.gateio.ws/v4/ws/usdt";
    GateioCoinWs gc(symbols, uri);

    auto btc = gc.getData("BTC_USDT");
    ASSERT_EQ(btc.size(), 2);
    EXPECT_EQ(btc[0].count("asks"), 1);
    EXPECT_EQ(btc[1].count("bids"), 1);
}

TEST(GateioCoinWsTest, GetDataUnknownSymbolReturnsEmpty) {
    std::vector<std::string> symbols{"BTC"};
    const std::string uri = "wss://fx-ws.gateio.ws/v4/ws/usdt";
    GateioCoinWs gc(symbols, uri);

    auto result = gc.getData("UNKNOWN");
    EXPECT_TRUE(result.empty());
}

TEST(GateioCoinWsTest, EmptySymbolsList) {
    std::vector<std::string> symbols{};
    const std::string uri = "wss://fx-ws.gateio.ws/v4/ws/usdt";
    GateioCoinWs gc(symbols, uri);

    auto result = gc.getData("BTC_USDT");
    EXPECT_TRUE(result.empty());
}

TEST(GateioCoinWsTest, CreateSendMessageFormat) {
    std::vector<std::string> symbols{"BTC_USDT", "ETH_USDT"};
    const std::string uri = "wss://fx-ws.gateio.ws/v4/ws/usdt";
    GateioCoinWs gc(symbols, uri);

    // The constructor calls createSendMessage internally.
    // We can verify the data was initialized for the modified symbols.
    auto btc = gc.getData("BTC_USDT_USDT");
    ASSERT_EQ(btc.size(), 2);
}
