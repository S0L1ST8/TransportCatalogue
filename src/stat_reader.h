#pragma once

#include "transport_catalogue.h"

#include <set>
#include <tuple>

namespace transport_catalogue {

namespace output {

void PrintRouteInfo(const std::tuple<int, int, double> &route_info);

void PrintBusesPassingThroughStop(
    const std::optional<std::vector<const Bus*>> &buses);

void Requests(const TransportCatalogue& transport_catalogue);

} // namesoace output

} // namespace transport_catalogue