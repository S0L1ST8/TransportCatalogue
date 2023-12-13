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

namespace detail {

struct StopPairHasher {
    std::size_t operator()(const std::pair<const Stop*, const Stop*>& stop_pair) const;
};

} // namespace detail

struct RouteInfo {
    int stop_count{};
    int unique_stop_count{};
    double distance{};
    double curvature{};
};

class TransportCatalogue {
  public:
    TransportCatalogue() = default;

    void AddStop(const std::string& name, geo::Coordinates coordinates);
    [[nodiscard]] const Stop* FindStop(std::string_view name) const;
    std::optional<std::vector<const Bus*>> GetBusesPassingThroughStop(std::string_view name) const;

    void SetDistanceBetweenStops(const Stop* stop1, const Stop* stop2, double distance);
    double GetDistanceBetweenStops(const Stop* stop1, const Stop* stop2) const;

    void AddBus(const std::string& name, const std::vector<std::string>& stop_names);
    [[nodiscard]] const Bus* FindBus(std::string_view name) const;
    [[nodiscard]] std::optional<RouteInfo> GetBusInfo(std::string_view name) const;

  private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, const Stop*> stops_lookup_;
    std::unordered_map<std::string_view, const Bus*> buses_lookup_;
    std::unordered_map<const Stop*, std::vector<const Bus*>> stop_to_buses_;
    std::unordered_map<const std::pair<const Stop*, const Stop*>, double, detail::StopPairHasher> real_distance_between_stops_;
};

} // namespace transport_catalogue
