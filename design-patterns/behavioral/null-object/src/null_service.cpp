// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/null_service.hpp"
#include "../../../../headers/project_utils.hpp"

namespace null_object_pattern {

//
// null object message processing implementation
// accepts any message but performs no actual processing
// always returns success to maintain interface contract
//
template<std::copyable MessageType>
std::expected<bool, std::string> 
null_service<MessageType>::process_message(const MessageType& message) {
    // null object behavior: accept message silently without processing
    // this eliminates the need for null checks in client code
    // the message parameter is intentionally unused (marked with void cast)
    static_cast<void>(message);
    
    // return success without any side effects
    // this allows normal program flow to continue
    return true;
}

//
// null object configuration implementation
// accepts any configuration but makes no state changes
// always returns success to indicate acceptance
//
template<std::copyable MessageType>
std::expected<bool, std::string> 
null_service<MessageType>::configure(const std::string& config) {
    // null object behavior: accept configuration silently
    // no validation or state changes are performed
    // this provides neutral behavior that doesn't disrupt program flow
    static_cast<void>(config);
    
    // return success to maintain consistent interface behavior
    return true;
}

//
// null object status reporting implementation
// returns consistent status message identifying null object state
// provides useful information for debugging and monitoring
//
template<std::copyable MessageType>
std::expected<std::string, std::string> 
null_service<MessageType>::get_status() const {
    // return standardized status message for null objects
    // this helps with debugging and system monitoring
    // the message clearly indicates the null object pattern is in use
    return std::string{"null service: no operations performed, neutral behavior active"};
}

//
// null object shutdown implementation
// performs no actual shutdown operations but maintains interface contract
// always returns success for consistent behavior
//
template<std::copyable MessageType>
std::expected<bool, std::string> 
null_service<MessageType>::shutdown() {
    // null object behavior: no resources to clean up
    // no state to reset or operations to finalize
    // simply return success to satisfy interface requirements
    return true;
}

//
// null object identification implementation
// clearly identifies this as a null object implementation
// allows clients to detect null object behavior when needed
//
template<std::copyable MessageType>
bool null_service<MessageType>::is_null_object() const {
    // return true to identify this as a null object
    // enables conditional logic when null object detection is required
    // useful for debugging, testing, and specialized handling
    return true;
}

//
// explicit template instantiations for common types
// ensures template code is compiled and available in the library
// prevents linker errors when using template classes across compilation units
//
template class null_service<std::string>;
template class null_service<int>;
template class null_service<double>;

} // namespace null_object_pattern