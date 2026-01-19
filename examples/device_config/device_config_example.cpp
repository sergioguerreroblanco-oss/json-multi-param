#include <iostream>

#include "device_config_protocol.h"

using namespace jsonmp;

int main()
{
    DeviceConfig defaults;
    defaults.speed                = 50;
    defaults.temperature_limit    = 60.0;
    defaults.mode                 = "AUTO";
    defaults.enabled              = true;
    defaults.network.dhcp_enabled = true;
    defaults.network.ip_address   = "192.168.1.100";
    defaults.network.netmask      = "255.255.255.0";

    ParamSet tx;
    define_device_config_schema(tx, defaults);

    std::string error;

    DeviceConfig outgoing         = defaults;
    outgoing.speed                = 120;
    outgoing.mode                 = "MANUAL";
    outgoing.network.dhcp_enabled = false;
    outgoing.network.ip_address   = "10.0.0.42";

    if (!write_device_config_to_params(tx, outgoing, error))
    {
        std::cerr << "TX error: " << error << "\n";
        return 1;
    }

    auto j = tx.to_json();
    std::cout << "JSON to send:\n" << j.dump(4) << "\n\n";

    std::string compact = tx.to_compact_string();
    std::cout << "Compact string:\n" << compact << "\n\n";

    ParamSet rx;
    define_device_config_schema(rx, defaults);

    if (!rx.from_compact_string(compact, error))
    {
        std::cerr << "RX parse error: " << error << "\n";
        return 1;
    }

    DeviceConfig received;
    if (!load_device_config_from_params(rx, received, error))
    {
        std::cerr << "RX load error: " << error << "\n";
        return 1;
    }

    std::cout << "Received speed=" << received.speed << "\n";
    return 0;
}
