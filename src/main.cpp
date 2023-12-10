#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

using namespace std::literals;

int main() {
    using namespace transport_catalogue;

    TransportCatalogue transport_catalogue;

    input::FillTransportCatalogue(transport_catalogue);
    output::Requests(transport_catalogue);
}