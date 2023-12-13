#pragma once

#include "transport_catalogue.h"

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace transport_catalogue {

namespace detail {

int ReadLineWithNumber();
std::string Strip(const std::string& str);

struct StringPairHasher {
    std::size_t operator()(const std::pair<std::string, std::string>& string_pair) const;
};

} // namespace detail

namespace input {

std::pair<geo::Coordinates, std::unordered_map<std::string, double>> ParseStopData(const std::string &raw_data);

std::vector<std::string> ParseBusData(const std::string &raw_data);

void FillTransportCatalogue(TransportCatalogue& transport_catalogue);

} // namespace input

} // namespace transport_catalogue
