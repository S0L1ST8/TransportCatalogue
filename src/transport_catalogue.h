#pragma once

#include "geo.h"

#include <deque>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace transport_catalogue {

struct Stop {
    std::string name;
    geo::Coordinates coordinates;

    Stop(std::string name, geo::Coordinates coordinates);
};

struct Bus {
    std::string name;
    std::vector<const Stop*> route;
};

class TransportCatalogue {
  public:
    TransportCatalogue() = default;

    void AddStop(const std::string& name, geo::Coordinates coordinates);
    const Stop* FindStop(std::string_view name) const;
    const std::optional<std::vector<const Bus*>> GetBusesPassingThroughStop(std::string_view name) const;

    void AddBus(const std::string& name, const std::vector<std::string>& stop_names);
    const Bus* FindBus(std::string_view name) const;
    const std::optional<std::tuple<int, int, double>> GetBusInfo(std::string_view name) const;

  private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*> stops_lookup_;
    std::unordered_map<std::string_view, const Bus*> buses_lookup_;
    std::unordered_map<const Stop*, std::vector<const Bus*>> stop_to_buses_;
};

} // namespace transport_catalogue