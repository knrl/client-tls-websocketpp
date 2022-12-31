#include "adapters.hpp"

int main() {
    std::vector<std::string> symbols{
                                    "ANT" , "XRP" , "MATIC", "WAVES", "PEOPLE", "FIL" , "ALGO", 
                                    "CHZ" , "ATOM", "INJ"  , "CRV"  , "SRM"   , "TRX" , "DOGE", 
                                    "LDO" , "AR"  , "IMX"  , "ETC"  , "GALA"  , "AVAX", "HNT" , 
                                    "OP"  , "LINK", "UNI"  , "GMT"  , "BNB"   , "NEAR", "SOL" , 
                                    "LIT" , "ETH" , "REEF" , "DOT"  , "XLM"   , "RSR" , "APE" , 
                                    "IOTA", "BTC" , "LTC"  , "JASMY", "BCH"   , "EOS" , "AAVE", 
                                    "FLOW", "DYDX", "ICP"  , "ENS"  , "OCEAN" , "SAND", "ADA" , 
                                    "HBAR", "SUSHI"};

    const std::string uri_bs{"wss://stream.binance.com:9443/ws"};
    BinanceSocket bs(symbols, uri_bs);
    bs.run();
    auto data_bs = bs.getData("BTCUSDT");

    const std::string uri_gc{"wss://fx-ws.gateio.ws/v4/ws/usdt"};
    GateioCoinWs gc(symbols, uri_gc);
    gc.run();
    auto data_gc = gc.getData("BTC_USDT");

    return EXIT_SUCCESS;
}