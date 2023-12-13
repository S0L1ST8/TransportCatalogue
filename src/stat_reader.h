#pragma once

#include "transport_catalogue.h"

#include <set>
#include <tuple>

namespace transport_catalogue::output {

void PrintRouteInfo(const std::optional<RouteInfo>& route_info);

void PrintBusesPassingThroughStop(std::optional<std::vector<const Bus*>>& buses);

void Requests(const TransportCatalogue& transport_catalogue);

} // namespace transport_catalogue::output
