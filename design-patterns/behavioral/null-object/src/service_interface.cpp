// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/service_interface.hpp"
#include "../headers/real_service.hpp"
#include "../headers/null_service.hpp"
#include "../../../../headers/project_utils.hpp"

namespace null_object_pattern {

//
// factory method implementation for creating appropriate service instances
// encapsulates the decision logic for real vs null object creation
// provides a clean interface for service instantiation
//
template<std::copyable MessageType>
std::unique_ptr<service_interface<MessageType>> 
service_interface<MessageType>::create_service(bool enable_real_service) {
    if (enable_real_service) {
        LOG_INFO_PRINT("creating real service instance for message processing");
        return std::make_unique<real_service<MessageType>>();
    } else {
        LOG_INFO_PRINT("creating null service instance for disabled/testing mode");
        return std::make_unique<null_service<MessageType>>();
    }
}

//
// convenience factory function for string-based services
// provides simpler interface for the most common use case
// reduces template instantiation complexity for clients
//
std::unique_ptr<string_service> create_string_service(bool enable_real_service) {
    return service_interface<std::string>::create_service(enable_real_service);
}

//
// explicit template instantiations for common types
// ensures template code is compiled and available in the library
// prevents linker errors when using template classes across compilation units
//
template class service_interface<std::string>;
template class service_interface<int>;
template class service_interface<double>;

} // namespace null_object_pattern
