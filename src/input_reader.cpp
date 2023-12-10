#include "input_reader.h"
#include "geo.h"
#include "transport_catalogue.h"

#include <iostream>
#include <sstream>
#include <utility>

const int64_t HASH_COUNT = 37;
const int64_t s = 2654435769;
const int64_t p = 8;
constexpr int64_t BIG_INT = 4294967296;


using namespace std::literals;

namespace transport_catalogue {

namespace detail {

int ReadLineWithNumber() {
    int result;
    std::cin >> result;
    std::cin >> std::ws; // whitespace удаляет пробелы и табуляцию из потока
    return result;
}

std::string Strip(std::string str) {
    std::size_t pos_begin = str.find_first_not_of(' ');
    std::size_t pos_end = str.find_last_not_of(' ');
    return str.substr(pos_begin, pos_end - pos_begin + 1);
}

std::size_t StringPairHasher::operator()(const std::pair<std::string, std::string>& string_pair) const {
    return ((std::hash<std::string>{}(string_pair.first) + HASH_COUNT * std::hash<std::string>{}(string_pair.second)) * s % BIG_INT) >> (32 - p);
}

} // namespace detail

namespace input {

geo::Coordinates ParseStopData(const std::string& raw_data) {
    geo::Coordinates coordinates;

    std::size_t begin_pos = 0;
    std::size_t comma_pos = raw_data.find_first_of(","s);
    coordinates.lat = std::stod(detail::Strip(raw_data.substr(begin_pos, comma_pos - begin_pos)));

    begin_pos = comma_pos + 1;
    comma_pos = raw_data.find_first_of(","s, begin_pos);
    comma_pos = comma_pos == std::string::npos ? raw_data.size() : comma_pos;
    coordinates.lng = std::stod(detail::Strip(raw_data.substr(begin_pos, comma_pos - begin_pos)));
    return {coordinates.lat, coordinates.lng};
}

std::vector<std::string> ParseBusData(const std::string& raw_data) {
    std::size_t delim_pos = raw_data.find_first_of("->"s);
    char delim = raw_data[delim_pos];
    bool is_circular = delim == '>';

    std::size_t pos_begin = 0;
    std::size_t pos_end = delim_pos;
    std::vector<std::string> stop_names;
    while (pos_end != std::string::npos) {
        std::string stop_name = raw_data.substr(pos_begin, pos_end - pos_begin);
        stop_names.push_back(detail::Strip(stop_name));

        pos_begin = pos_end + 1;
        pos_end = raw_data.find_first_of(delim, pos_begin);
    }
    stop_names.push_back(detail::Strip(raw_data.substr(pos_begin, raw_data.size() - pos_begin)));
    if (!is_circular) {
        stop_names.reserve(stop_names.size() * 2);
        for (auto it = stop_names.rbegin() + 1; it != stop_names.rend(); ++it) {
            stop_names.push_back(*it);
        }
    }
    return stop_names;
}

void FillTransportCatalogue(TransportCatalogue& transport_catalogue) {
    int input_query_count = detail::ReadLineWithNumber();

    std::unordered_map<std::string, std::string> bus_raw_data;
    std::unordered_map<std::string, std::string> stop_raw_data;

    for (int i = 0; i != input_query_count; ++i) {
        std::string query_type, name;
        std::cin >> query_type;
        std::getline(std::cin, name, ':');
        name = detail::Strip(name);

        std::string raw_data;
        std::getline(std::cin, raw_data);
        if (query_type == "Stop"s) {
            stop_raw_data[name] = std::move(raw_data);
        }
        else if (query_type == "Bus"s) {
            bus_raw_data[name] = std::move(raw_data);
        }
    }

    for (const auto& [name, data] : stop_raw_data) {
        geo::Coordinates coordinates = ParseStopData(data);
        transport_catalogue.AddStop(name, coordinates);
    }

    for (const auto& [name, data] : bus_raw_data) {
        auto stop_names = ParseBusData(data);
        transport_catalogue.AddBus(name, stop_names);
    }
}

} // namespace input

} // namespace transport_catalogue