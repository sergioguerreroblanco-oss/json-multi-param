#pragma once

#include <string>

#include "constraints.h"
#include "param_set.h"

namespace jsonmp
{

struct NetworkSettings
{
    bool        dhcp_enabled;
    std::string ip_address;
    std::string netmask;
};

struct DeviceConfig
{
    int             speed;
    double          temperature_limit;
    std::string     mode;
    bool            enabled;
    NetworkSettings network;
};

/**
 * @brief Defines the ParamSet schema for DeviceConfig.
 *
 * @details
 * This function must be used on both TX and RX sides to guarantee
 * the same parameter names, types and constraints.
 */
inline void define_device_config_schema(ParamSet& params, const DeviceConfig& defaults)
{
    Constraints<int> c_speed;
    c_speed.has_min = true;
    c_speed.min     = 0;
    c_speed.has_max = true;
    c_speed.max     = 200;

    Constraints<double> c_temp;
    c_temp.has_min = true;
    c_temp.min     = 0.0;
    c_temp.has_max = true;
    c_temp.max     = 100.0;

    Constraints<std::string> c_mode;
    c_mode.has_allowed    = true;
    c_mode.allowed_values = {"AUTO", "MANUAL"};

    Constraints<std::string> c_ip;
    c_ip.has_min_length = true;
    c_ip.min_length     = 7;
    c_ip.has_max_length = true;
    c_ip.max_length     = 15;

    Constraints<std::string> c_netmask = c_ip;

    params.addInt("speed", defaults.speed, c_speed);
    params.addDouble("temperature_limit", defaults.temperature_limit, c_temp);
    params.addString("mode", defaults.mode, c_mode);
    params.addBool("enabled", defaults.enabled, {});

    params.addBool("network.dhcp_enabled", defaults.network.dhcp_enabled, {});
    params.addString("network.ip_address", defaults.network.ip_address, c_ip);
    params.addString("network.netmask", defaults.network.netmask, c_netmask);
}

/**
 * @brief Loads a DeviceConfig from an already-filled ParamSet.
 */
inline bool load_device_config_from_params(const ParamSet& params, DeviceConfig& cfg,
                                           std::string& error)
{
    try
    {
        cfg.speed             = params.get<int>("speed");
        cfg.temperature_limit = params.get<double>("temperature_limit");
        cfg.mode              = params.get<std::string>("mode");
        cfg.enabled           = params.get<bool>("enabled");

        cfg.network.dhcp_enabled = params.get<bool>("network.dhcp_enabled");
        cfg.network.ip_address   = params.get<std::string>("network.ip_address");
        cfg.network.netmask      = params.get<std::string>("network.netmask");
    }
    catch (const std::exception& ex)
    {
        error = ex.what();
        return false;
    }

    error.clear();
    return true;
}

/**
 * @brief Writes a DeviceConfig into an existing ParamSet using typed set().
 */
inline bool write_device_config_to_params(ParamSet& params, const DeviceConfig& cfg,
                                          std::string& error)
{
    if (!params.set<int>("speed", cfg.speed, error))
        return false;
    if (!params.set<double>("temperature_limit", cfg.temperature_limit, error))
        return false;
    if (!params.set<std::string>("mode", cfg.mode, error))
        return false;
    if (!params.set<bool>("enabled", cfg.enabled, error))
        return false;

    if (!params.set<bool>("network.dhcp_enabled", cfg.network.dhcp_enabled, error))
        return false;
    if (!params.set<std::string>("network.ip_address", cfg.network.ip_address, error))
        return false;
    if (!params.set<std::string>("network.netmask", cfg.network.netmask, error))
        return false;

    return true;
}

}  // namespace jsonmp
