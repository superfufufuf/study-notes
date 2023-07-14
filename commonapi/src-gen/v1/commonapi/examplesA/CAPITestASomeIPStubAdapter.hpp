/*
 * This file was generated by the CommonAPI Generators.
 * Used org.genivi.commonapi.someip 3.2.0.v202012010944.
 * Used org.franca.core 0.13.1.201807231814.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 */
#ifndef V1_COMMONAPI_EXAMPLESA_CAPI_TEST_A_SOMEIP_STUB_ADAPTER_HPP_
#define V1_COMMONAPI_EXAMPLESA_CAPI_TEST_A_SOMEIP_STUB_ADAPTER_HPP_

#include <v1/commonapi/examplesA/CAPITestAStub.hpp>
#include <v1/commonapi/examplesA/CAPITestASomeIPDeployment.hpp>

#if !defined (COMMONAPI_INTERNAL_COMPILATION)
#define COMMONAPI_INTERNAL_COMPILATION
#define HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif

#include <CommonAPI/SomeIP/AddressTranslator.hpp>
#include <CommonAPI/SomeIP/StubAdapterHelper.hpp>
#include <CommonAPI/SomeIP/StubAdapter.hpp>
#include <CommonAPI/SomeIP/Factory.hpp>
#include <CommonAPI/SomeIP/Types.hpp>
#include <CommonAPI/SomeIP/Constants.hpp>

#if defined (HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE)
#undef COMMONAPI_INTERNAL_COMPILATION
#undef HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif

namespace v1 {
namespace commonapi {
namespace examplesA {

template <typename _Stub = ::v1::commonapi::examplesA::CAPITestAStub, typename... _Stubs>
class CAPITestASomeIPStubAdapterInternal
    : public virtual CAPITestAStubAdapter,
      public CommonAPI::SomeIP::StubAdapterHelper< _Stub, _Stubs...>,
      public std::enable_shared_from_this< CAPITestASomeIPStubAdapterInternal<_Stub, _Stubs...>>
{
public:
    typedef CommonAPI::SomeIP::StubAdapterHelper< _Stub, _Stubs...> CAPITestASomeIPStubAdapterHelper;

    ~CAPITestASomeIPStubAdapterInternal() {
        deactivateManagedInstances();
        CAPITestASomeIPStubAdapterHelper::deinit();
    }

    void fireMyStatusEvent(const int32_t &_myCurrentValue);

    void deactivateManagedInstances() {}
    
    CommonAPI::SomeIP::GetAttributeStubDispatcher<
        ::v1::commonapi::examplesA::CAPITestAStub,
        CommonAPI::Version
    > getCAPITestAInterfaceVersionStubDispatcher;

    CommonAPI::SomeIP::MethodWithReplyStubDispatcher<
        ::v1::commonapi::examplesA::CAPITestAStub,
        std::tuple< int32_t, std::string>,
        std::tuple< CAPITestA::stdErrorTypeEnum, int32_t, std::string>,
        std::tuple< CommonAPI::SomeIP::IntegerDeployment<int32_t>, CommonAPI::SomeIP::StringDeployment>,
        std::tuple< ::v1::commonapi::examplesA::CAPITestA_::stdErrorTypeEnumDeployment_t, CommonAPI::SomeIP::IntegerDeployment<int32_t>, CommonAPI::SomeIP::StringDeployment>
    > fooStubDispatcher;
    
    CAPITestASomeIPStubAdapterInternal(
        const CommonAPI::SomeIP::Address &_address,
        const std::shared_ptr<CommonAPI::SomeIP::ProxyConnection> &_connection,
        const std::shared_ptr<CommonAPI::StubBase> &_stub):
        CommonAPI::SomeIP::StubAdapter(_address, _connection),
        CAPITestASomeIPStubAdapterHelper(
            _address,
            _connection,
            std::dynamic_pointer_cast< CAPITestAStub>(_stub)),
        getCAPITestAInterfaceVersionStubDispatcher(&CAPITestAStub::lockInterfaceVersionAttribute, &CAPITestAStub::getInterfaceVersion, false, true),
        fooStubDispatcher(
            &CAPITestAStub::foo,
            false,
            _stub->hasElement(0),
            std::make_tuple(static_cast< CommonAPI::SomeIP::IntegerDeployment<int32_t>* >(nullptr), &::v1::commonapi::examplesA::CAPITestA_::foo_x2Deployment),
            std::make_tuple(static_cast< ::v1::commonapi::examplesA::CAPITestA_::stdErrorTypeEnumDeployment_t * >(nullptr), static_cast< CommonAPI::SomeIP::IntegerDeployment<int32_t>* >(nullptr), &::v1::commonapi::examplesA::CAPITestA_::foo_y2Deployment))
        
    {
        CAPITestASomeIPStubAdapterHelper::addStubDispatcher( { CommonAPI::SomeIP::method_id_t(0x7530) }, &fooStubDispatcher );
        // Provided events/fields
        {
            std::set<CommonAPI::SomeIP::eventgroup_id_t> itsEventGroups;
            itsEventGroups.insert(CommonAPI::SomeIP::eventgroup_id_t(0x80f2));
            CommonAPI::SomeIP::StubAdapter::registerEvent(CommonAPI::SomeIP::event_id_t(0x80f2), itsEventGroups, CommonAPI::SomeIP::event_type_e::ET_EVENT, CommonAPI::SomeIP::reliability_type_e::RT_UNRELIABLE);
        }
    }

    // Register/Unregister event handlers for selective broadcasts
    void registerSelectiveEventHandlers();
    void unregisterSelectiveEventHandlers();

};

template <typename _Stub, typename... _Stubs>
void CAPITestASomeIPStubAdapterInternal<_Stub, _Stubs...>::fireMyStatusEvent(const int32_t &_myCurrentValue) {
    CommonAPI::Deployable< int32_t, CommonAPI::SomeIP::IntegerDeployment<int32_t>> deployed_myCurrentValue(_myCurrentValue, static_cast< CommonAPI::SomeIP::IntegerDeployment<int32_t>* >(nullptr));
    CommonAPI::SomeIP::StubEventHelper<CommonAPI::SomeIP::SerializableArguments<  CommonAPI::Deployable< int32_t, CommonAPI::SomeIP::IntegerDeployment<int32_t> > 
    >>
        ::sendEvent(
            *this,
            CommonAPI::SomeIP::event_id_t(0x80f2),
            false,
             deployed_myCurrentValue 
    );
}


template <typename _Stub, typename... _Stubs>
void CAPITestASomeIPStubAdapterInternal<_Stub, _Stubs...>::registerSelectiveEventHandlers() {
}

template <typename _Stub, typename... _Stubs>
void CAPITestASomeIPStubAdapterInternal<_Stub, _Stubs...>::unregisterSelectiveEventHandlers() {
}

template <typename _Stub = ::v1::commonapi::examplesA::CAPITestAStub, typename... _Stubs>
class CAPITestASomeIPStubAdapter
    : public CAPITestASomeIPStubAdapterInternal<_Stub, _Stubs...> {
public:
    CAPITestASomeIPStubAdapter(const CommonAPI::SomeIP::Address &_address,
                                            const std::shared_ptr<CommonAPI::SomeIP::ProxyConnection> &_connection,
                                            const std::shared_ptr<CommonAPI::StubBase> &_stub)
        : CommonAPI::SomeIP::StubAdapter(_address, _connection),
          CAPITestASomeIPStubAdapterInternal<_Stub, _Stubs...>(_address, _connection, _stub) {
    }
};

} // namespace examplesA
} // namespace commonapi
} // namespace v1

#endif // V1_COMMONAPI_EXAMPLESA_CAPI_Test_A_SOMEIP_STUB_ADAPTER_HPP_
