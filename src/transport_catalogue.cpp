#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>
#include <functional>
#include <numeric>
#include <map>
#include <unordered_set>
#include <utility>

using namespace std::literals;

const int64_t HASH_COUNT = 37;
const int64_t s = 2654435769;
const int64_t p = 8;
constexpr int64_t BIG_INT = 4294967296;

namespace transport_catalogue {

Stop::Stop(std::string name, geo::Coordinates coordinates)
: name(std::move(name)), coordinates(coordinates) {}

namespace detail {

std::size_t StopPairHasher::operator()(const std::pair<const Stop*, const Stop*>& stop_pair) const {
    return ((std::hash<const void*>{}(stop_pair.first) + HASH_COUNT * std::hash<const void*>{}(stop_pair.second)) * s % BIG_INT) >> (32 - p);
}

} // namespace detail

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

std::optional<std::vector<const Bus*>> TransportCatalogue::GetBusesPassingThroughStop(std::string_view name) const {
    const Stop* stop = FindStop(name);

    if (stop == nullptr) {
        return std::nullopt;
    }

    if (stop_to_buses_.count(stop) > 0) {
        return stop_to_buses_.at(stop);
    }
    return std::nullopt;
}

void TransportCatalogue::SetDistanceBetweenStops(const Stop* stop1, const Stop* stop2, double distance) {
    real_distance_between_stops_.emplace(std::make_pair(std::make_pair(stop1, stop2), distance));
}

double TransportCatalogue::GetDistanceBetweenStops(const Stop* stop1, const Stop* stop2) const {
    if (real_distance_between_stops_.count(std::make_pair(stop1, stop2)) > 0) {
        return real_distance_between_stops_.at(std::make_pair(stop1, stop2));
    }
    if (real_distance_between_stops_.count(std::make_pair(stop2, stop1)) > 0) {
        return real_distance_between_stops_.at(std::make_pair(stop2, stop1));
    }
    return geo::ComputeDistance(stop2->coordinates, stop1->coordinates);
}

void TransportCatalogue::AddBus(const std::string& name, const std::vector<std::string>& stop_names) {
    Bus bus;
    bus.name = name;

    for (const std::string& name : stop_names) {
        const Stop* stop = FindStop(name);
        bus.route.push_back(stop);
    }

    buses_.push_back(std::move(bus));
    buses_lookup_[buses_.back().name] = &buses_.back();

    for (const auto& stop : buses_.back().route) {
        stop_to_buses_[stop].push_back(&buses_.back());
    }
}

const Bus* TransportCatalogue::FindBus(std::string_view name) const {
    if (buses_lookup_.count(name) > 0) {
        return buses_lookup_.at(name);
    }
    return nullptr;
}

std::optional<RouteInfo> TransportCatalogue::GetBusInfo(std::string_view name) const {
    const auto* bus = FindBus(name);

    if (bus == nullptr) {
        return std::nullopt;
    }

    int stop_count = static_cast<int>(bus->route.size());
    std::unordered_set<const Stop*> unique_stops(bus->route.begin(), bus->route.end());
    int unique_stop_count = static_cast<int>(unique_stops.size());

    auto geo_distance = std::transform_reduce(
        bus->route.begin() + 1,
        bus->route.end(),
        bus->route.begin(), // Необязательный (используется для указания последовательности из какой берётся)
        0.,
        std::plus<>(),
        [this](auto next, auto prev) { //next - текущая; prev - предыдущая
            return transport_catalogue::geo::ComputeDistance(prev->coordinates, next->coordinates);
        });

    auto real_distance = std::transform_reduce(
        bus->route.begin() + 1,
        bus->route.end(),
        bus->route.begin(),
        0.,
        std::plus<>(),
        [this](auto next, auto prev) {
            return GetDistanceBetweenStops(prev, next);
        }
        );

    auto curvature = real_distance / geo_distance;

    return RouteInfo{stop_count, unique_stop_count, real_distance, curvature};
}

} // namespace transport_catalogue
