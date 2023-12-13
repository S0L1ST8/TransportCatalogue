#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>

using namespace std::literals;

namespace transport_catalogue::output {

void PrintRouteInfo(const std::optional<RouteInfo>& route_info) {
    std::cout << route_info.value().stop_count << " stops on route, "s
              << route_info.value().unique_stop_count << " unique stops, "s
              << std::setprecision(6) << route_info.value().distance << " route length, "s
              << route_info.value().curvature << " curvature"s << std::endl;
}

void PrintBusesPassingThroughStop(const std::optional<std::vector<const Bus*>>& buses) {
    if (!buses || buses->empty()) {
        std::cout << "no buses"s;
    }
    else {
        std::set<std::string_view> unique_buses;
        for (const auto* bus : *buses) {
            unique_buses.insert(bus->name);
        }
        std::cout << "buses "s;
        for (const auto& bus : unique_buses) {
            std::cout << bus << ' ';
        }
    }
}

void Requests(const TransportCatalogue& transport_catalogue) {
    int input_query_count = detail::ReadLineWithNumber();

    for (int i = 0; i < input_query_count; ++i) {
        std::string request_type;
        std::string name;
        std::cin >> request_type;
        std::cin >> std::ws;
        std::getline(std::cin, name);

        if (request_type == "Bus"s) {
            const auto route_info = transport_catalogue.GetBusInfo(name);
            std::cout << "Bus "s << name << ": "s;
            if (route_info) {
                PrintRouteInfo(route_info.value());
            }
            else {
                std::cout << "not found"s << std::endl;
            }
        }
        else if (request_type == "Stop"s) {
            std::cout << "Stop "s << name << ": "s;
            const auto buses = transport_catalogue.GetBusesPassingThroughStop(name);
            if (buses) {
                PrintBusesPassingThroughStop(buses.value());
            }
            else {
                std::cout << "not found"s;
            }
            std::cout << std::endl;
        }
    }
}

} // namespace transport_catalogue::output
