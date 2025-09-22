#include "ws_l2_client.hpp"

int main() {
    std::vector<std::string> products = {
        //just a basic data for now. We will be looking at a dynamic potfolio of liquid assets later. 
        "BTC-USD", "ETH-USD", "SOL-USD", "ADA-USD"
    };

    WsL2Client client(0, products, "data", "l2_data.csv");
    client.run();
    return 0;
}
