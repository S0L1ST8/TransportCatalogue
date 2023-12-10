#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>
#include <functional>
#include <numeric>
#include <map>
#include <unordered_set>
#include <utility>

using namespace std::literals;

namespace transport_catalogue {

Stop::Stop(std::string name, geo::Coordinates coordinates)
    : name(name), coordinates(coordinates) {}

void TransportCatalogue::AddStop(const std::string& name, geo::Coordinates coordinates) {
    stops_.emplace_back(name, coordinates);
    stops_lookup_[stops_.back().name] = &stops_.back();
    const Stop* new_stop = &stops_.back();
    stop_to_buses_[new_stop];
}

const Stop* TransportCatalogue::FindStop(std::string_view name) const {
    if (stops_lookup_.count(name) > 0) {
        return stops_lookup_.at(name);
    }
    return nullptr;
}

const std::optional<std::vector<const Bus*>> TransportCatalogue::GetBusesPassingThroughStop(std::string_view name) const {
    auto stop = FindStop(name);

    if (!stop) {
        return std::nullopt;
    }

    if (stop_to_buses_.count(stop) > 0) {
        return stop_to_buses_.at(stop);
    }
    return std::nullopt;
}

void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stop_names) {
    Bus bus;
    bus.name = name;

    for (const std::string& name : stop_names) {
        auto stop = FindStop(name);
        bus.route.push_back(stop);
    }

    buses_.push_back(std::move(bus));
    buses_lookup_[buses_.back().name] = &buses_.back();

    for (const auto& stop : buses_.back().route) {
        stop_to_buses_[stop].push_back(&buses_.back());
    }
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    if (buses_lookup_.count(name)) {
        return buses_lookup_.at(name);
    }
    return nullptr;
}

const std::optional<std::tuple<int, int, double>> TransportCatalogue::GetBusInfo(std::string_view name) const {
    auto bus = FindBus(name);

    if (!bus) {
        return std::nullopt;
    }

    int stop_count = bus->route.size();
    std::unordered_set<const Stop*> unique_stops(bus->route.begin(), bus->route.end());
    int unique_stop_count = unique_stops.size();

    auto real_distance = std::transform_reduce(
        bus->route.begin() + 1,
        bus->route.end(),
        bus->route.begin(), // Необязательный (используется для указания последовательности из какой берётся)
        0.,
        std::plus<>(),
        [this](auto next, auto prev) { //next - текущая; prev - предыдущая
            return transport_catalogue::geo::ComputeDistance(prev->coordinates, next->coordinates);
        });

    return std::tie(stop_count, unique_stop_count, real_distance);

}

} // namespace transport_catalogue