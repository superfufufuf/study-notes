/*
 * This file was generated by the CommonAPI Generators.
 * Used org.genivi.commonapi.someip 3.2.0.v202012010944.
 * Used org.franca.core 0.13.1.201807231814.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 */
#include <v1/commonapi/examplesA/CAPITestASomeIPProxy.hpp>

#if !defined (COMMONAPI_INTERNAL_COMPILATION)
#define COMMONAPI_INTERNAL_COMPILATION
#define HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif

#include <CommonAPI/SomeIP/AddressTranslator.hpp>

#if defined (HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE)
#undef COMMONAPI_INTERNAL_COMPILATION
#undef HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif

namespace v1 {
namespace commonapi {
namespace examplesA {

std::shared_ptr<CommonAPI::SomeIP::Proxy> createCAPITestASomeIPProxy(
    const CommonAPI::SomeIP::Address &_address,
    const std::shared_ptr<CommonAPI::SomeIP::ProxyConnection> &_connection) {
    return std::make_shared< CAPITestASomeIPProxy>(_address, _connection);
}

void initializeCAPITestASomeIPProxy() {
    CommonAPI::SomeIP::AddressTranslator::get()->insert(
        "local:commonapi.examplesA.CAPITestA:v1_2:commonapi.examplesA.Methods",
        0x1234, 0x5678, 1, 2);
    CommonAPI::SomeIP::Factory::get()->registerProxyCreateMethod(
        "commonapi.examplesA.CAPITestA:v1_2",
        &createCAPITestASomeIPProxy);
}

INITIALIZER(registerCAPITestASomeIPProxy) {
    CommonAPI::SomeIP::Factory::get()->registerInterface(initializeCAPITestASomeIPProxy);
}

CAPITestASomeIPProxy::CAPITestASomeIPProxy(
    const CommonAPI::SomeIP::Address &_address,
    const std::shared_ptr<CommonAPI::SomeIP::ProxyConnection> &_connection)
        : CommonAPI::SomeIP::Proxy(_address, _connection),
          myStatus_(*this, 0x80f2, CommonAPI::SomeIP::event_id_t(0x80f2), CommonAPI::SomeIP::event_type_e::ET_EVENT , CommonAPI::SomeIP::reliability_type_e::RT_UNRELIABLE, false, std::make_tuple(static_cast< CommonAPI::SomeIP::IntegerDeployment<int32_t>* >(nullptr)))
{
}

CAPITestASomeIPProxy::~CAPITestASomeIPProxy() {
    completed_.set_value();
}


CAPITestASomeIPProxy::MyStatusEvent& CAPITestASomeIPProxy::getMyStatusEvent() {
    return myStatus_;
}

void CAPITestASomeIPProxy::foo(int32_t _x1, std::string _x2, CommonAPI::CallStatus &_internalCallStatus, CAPITestA::stdErrorTypeEnum &_error, int32_t &_y1, std::string &_y2, const CommonAPI::CallInfo *_info) {
    CommonAPI::Deployable< CAPITestA::stdErrorTypeEnum, ::v1::commonapi::examplesA::CAPITestA_::stdErrorTypeEnumDeployment_t> deploy_error(static_cast< ::v1::commonapi::examplesA::CAPITestA_::stdErrorTypeEnumDeployment_t * >(nullptr));
    CommonAPI::Deployable< int32_t, CommonAPI::SomeIP::IntegerDeployment<int32_t>> deploy_x1(_x1, static_cast< CommonAPI::SomeIP::IntegerDeployment<int32_t>* >(nullptr));
    CommonAPI::Deployable< std::string, CommonAPI::SomeIP::StringDeployment> deploy_x2(_x2, &::v1::commonapi::examplesA::CAPITestA_::foo_x2Deployment);
    CommonAPI::Deployable< int32_t, CommonAPI::SomeIP::IntegerDeployment<int32_t>> deploy_y1(static_cast< CommonAPI::SomeIP::IntegerDeployment<int32_t>* >(nullptr));
    CommonAPI::Deployable< std::string, CommonAPI::SomeIP::StringDeployment> deploy_y2(&::v1::commonapi::examplesA::CAPITestA_::foo_y2Deployment);
    CommonAPI::SomeIP::ProxyHelper<
        CommonAPI::SomeIP::SerializableArguments<
            CommonAPI::Deployable<
                int32_t,
                CommonAPI::SomeIP::IntegerDeployment<int32_t>
            >,
            CommonAPI::Deployable<
                std::string,
                CommonAPI::SomeIP::StringDeployment
            >
        >,
        CommonAPI::SomeIP::SerializableArguments<
            CommonAPI::Deployable<
                CAPITestA::stdErrorTypeEnum,
                ::v1::commonapi::examplesA::CAPITestA_::stdErrorTypeEnumDeployment_t
            >,
            CommonAPI::Deployable<
                int32_t,
                CommonAPI::SomeIP::IntegerDeployment<int32_t>
            >,
            CommonAPI::Deployable<
                std::string,
                CommonAPI::SomeIP::StringDeployment
            >
        >
    >::callMethodWithReply(
        *this,
        CommonAPI::SomeIP::method_id_t(0x7530),
        false,
        false,
        (_info ? _info : &CommonAPI::SomeIP::defaultCallInfo),
        deploy_x1, deploy_x2,
        _internalCallStatus,
        deploy_error,
        deploy_y1, deploy_y2);
    _error = deploy_error.getValue();
    _y1 = deploy_y1.getValue();
    _y2 = deploy_y2.getValue();
}

std::future<CommonAPI::CallStatus> CAPITestASomeIPProxy::fooAsync(const int32_t &_x1, const std::string &_x2, FooAsyncCallback _callback, const CommonAPI::CallInfo *_info) {
    CommonAPI::Deployable< CAPITestA::stdErrorTypeEnum, ::v1::commonapi::examplesA::CAPITestA_::stdErrorTypeEnumDeployment_t> deploy_error(static_cast< ::v1::commonapi::examplesA::CAPITestA_::stdErrorTypeEnumDeployment_t * >(nullptr));
    CommonAPI::Deployable< int32_t, CommonAPI::SomeIP::IntegerDeployment<int32_t>> deploy_x1(_x1, static_cast< CommonAPI::SomeIP::IntegerDeployment<int32_t>* >(nullptr));
    CommonAPI::Deployable< std::string, CommonAPI::SomeIP::StringDeployment> deploy_x2(_x2, &::v1::commonapi::examplesA::CAPITestA_::foo_x2Deployment);
    CommonAPI::Deployable< int32_t, CommonAPI::SomeIP::IntegerDeployment<int32_t>> deploy_y1(static_cast< CommonAPI::SomeIP::IntegerDeployment<int32_t>* >(nullptr));
    CommonAPI::Deployable< std::string, CommonAPI::SomeIP::StringDeployment> deploy_y2(&::v1::commonapi::examplesA::CAPITestA_::foo_y2Deployment);
    return CommonAPI::SomeIP::ProxyHelper<
        CommonAPI::SomeIP::SerializableArguments<
            CommonAPI::Deployable<
                int32_t,
                CommonAPI::SomeIP::IntegerDeployment<int32_t>
            >,
            CommonAPI::Deployable<
                std::string,
                CommonAPI::SomeIP::StringDeployment
            >
        >,
        CommonAPI::SomeIP::SerializableArguments<
            CommonAPI::Deployable<
                CAPITestA::stdErrorTypeEnum,
                ::v1::commonapi::examplesA::CAPITestA_::stdErrorTypeEnumDeployment_t
            >,
            CommonAPI::Deployable<
                int32_t,
                CommonAPI::SomeIP::IntegerDeployment<int32_t>
            >,
            CommonAPI::Deployable<
                std::string,
                CommonAPI::SomeIP::StringDeployment
            >
        >
    >::callMethodAsync(
        *this,
        CommonAPI::SomeIP::method_id_t(0x7530),
        false,
        false,
        (_info ? _info : &CommonAPI::SomeIP::defaultCallInfo),
        deploy_x1, deploy_x2,
        [_callback] (CommonAPI::CallStatus _internalCallStatus, CommonAPI::Deployable< CAPITestA::stdErrorTypeEnum, ::v1::commonapi::examplesA::CAPITestA_::stdErrorTypeEnumDeployment_t > _deploy_error, CommonAPI::Deployable< int32_t, CommonAPI::SomeIP::IntegerDeployment<int32_t> > _y1, CommonAPI::Deployable< std::string, CommonAPI::SomeIP::StringDeployment > _y2) {
            if (_callback)
                _callback(_internalCallStatus, _deploy_error.getValue(), _y1.getValue(), _y2.getValue());
        },
        std::make_tuple(deploy_error, deploy_y1, deploy_y2));
}

void CAPITestASomeIPProxy::getOwnVersion(uint16_t& ownVersionMajor, uint16_t& ownVersionMinor) const {
    ownVersionMajor = 1;
    ownVersionMinor = 2;
}

std::future<void> CAPITestASomeIPProxy::getCompletionFuture() {
    return completed_.get_future();
}

} // namespace examplesA
} // namespace commonapi
} // namespace v1
