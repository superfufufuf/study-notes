/*
 * This file was generated by the CommonAPI Generators.
 * Used org.genivi.commonapi.someip 3.2.0.v202012010944.
 * Used org.franca.core 0.13.1.201807231814.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 */

#ifndef V1_COMMONAPI_EXAMPLESA_COMMON_TYPES_SOMEIP_DEPLOYMENT_HPP_
#define V1_COMMONAPI_EXAMPLESA_COMMON_TYPES_SOMEIP_DEPLOYMENT_HPP_


#if !defined (COMMONAPI_INTERNAL_COMPILATION)
#define COMMONAPI_INTERNAL_COMPILATION
#define HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif
#include <CommonAPI/SomeIP/Deployment.hpp>
#if defined (HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE)
#undef COMMONAPI_INTERNAL_COMPILATION
#undef HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif

namespace v1 {
namespace commonapi {
namespace examplesA {
namespace CommonTypes_ {

// typecollection-specific deployment types
typedef CommonAPI::SomeIP::StructDeployment<
    CommonAPI::SomeIP::StringDeployment,
    CommonAPI::SomeIP::StructDeployment<
        CommonAPI::SomeIP::IntegerDeployment<int32_t>,
        CommonAPI::EmptyDeployment,
        CommonAPI::EmptyDeployment
    >
> a1StructDeployment_t;

typedef CommonAPI::SomeIP::StructDeployment<
    CommonAPI::SomeIP::IntegerDeployment<int32_t>,
    CommonAPI::EmptyDeployment,
    CommonAPI::EmptyDeployment
> a2StructDeployment_t;


// typecollection-specific deployments

} // namespace CommonTypes_
} // namespace examplesA
} // namespace commonapi
} // namespace v1

#endif // V1_COMMONAPI_EXAMPLESA_COMMON_TYPES_SOMEIP_DEPLOYMENT_HPP_
