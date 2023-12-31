/*
* This file was generated by the CommonAPI Generators.
* Used org.genivi.commonapi.core 3.2.0.v202012010850.
* Used org.franca.core 0.13.1.201807231814.
*
* This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
* If a copy of the MPL was not distributed with this file, You can obtain one at
* http://mozilla.org/MPL/2.0/.
*/
#ifndef V1_COMMONAPI_EXAMPLESB_CAPI_Test_B_PROXY_HPP_
#define V1_COMMONAPI_EXAMPLESB_CAPI_Test_B_PROXY_HPP_

#include <v1/commonapi/examplesB/CAPITestBProxyBase.hpp>


#if !defined (COMMONAPI_INTERNAL_COMPILATION)
#define COMMONAPI_INTERNAL_COMPILATION
#define HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif


#if defined (HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE)
#undef COMMONAPI_INTERNAL_COMPILATION
#undef HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif

namespace v1 {
namespace commonapi {
namespace examplesB {

template <typename ... _AttributeExtensions>
class CAPITestBProxy
    : virtual public CAPITestB,
      virtual public CAPITestBProxyBase,
      virtual public _AttributeExtensions... {
public:
    CAPITestBProxy(std::shared_ptr<CommonAPI::Proxy> delegate);
    ~CAPITestBProxy();

    typedef CAPITestB InterfaceType;


    /**
     * Returns the CommonAPI address of the remote partner this proxy communicates with.
     */
    virtual const CommonAPI::Address &getAddress() const;

    /**
     * Returns true if the remote partner for this proxy is currently known to be available.
     */
    virtual bool isAvailable() const;

    /**
     * Returns true if the remote partner for this proxy is available.
     */
    virtual bool isAvailableBlocking() const;

    /**
     * Returns the wrapper class that is used to (de-)register for notifications about
     * the availability of the remote partner of this proxy.
     */
    virtual CommonAPI::ProxyStatusEvent& getProxyStatusEvent();

    /**
     * Returns the wrapper class that is used to access version information of the remote
     * partner of this proxy.
     */
    virtual CommonAPI::InterfaceVersionAttribute& getInterfaceVersionAttribute();

    virtual std::future<void> getCompletionFuture();

    /**
     * Calls foo with synchronous semantics.
     *
     * All const parameters are input parameters to this method.
     * All non-const parameters will be filled with the returned values.
     * The CallStatus will be filled when the method returns and indicate either
     * "SUCCESS" or which type of error has occurred. In case of an error, ONLY the CallStatus
     * will be set.
     */
    virtual void foo(int32_t _x1, CommonAPI::CallStatus &_internalCallStatus, CAPITestB::stdErrorTypeEnum &_error, int32_t &_y1, const CommonAPI::CallInfo *_info = nullptr);
    /**
     * Calls foo with asynchronous semantics.
     *
     * The provided callback will be called when the reply to this call arrives or
     * an error occurs during the call. The CallStatus will indicate either "SUCCESS"
     * or which type of error has occurred. In case of any error, ONLY the CallStatus
     * will have a defined value.
     * The std::future returned by this method will be fulfilled at arrival of the reply.
     * It will provide the same value for CallStatus as will be handed to the callback.
     */
    virtual std::future<CommonAPI::CallStatus> fooAsync(const int32_t &_x1, FooAsyncCallback _callback = nullptr, const CommonAPI::CallInfo *_info = nullptr);



 private:
    std::shared_ptr< CAPITestBProxyBase> delegate_;
};

typedef CAPITestBProxy<> CAPITestBProxyDefault;


//
// CAPITestBProxy Implementation
//
template <typename ... _AttributeExtensions>
CAPITestBProxy<_AttributeExtensions...>::CAPITestBProxy(std::shared_ptr<CommonAPI::Proxy> delegate):
        _AttributeExtensions(*(std::dynamic_pointer_cast< CAPITestBProxyBase>(delegate)))...,
        delegate_(std::dynamic_pointer_cast< CAPITestBProxyBase>(delegate)) {
}

template <typename ... _AttributeExtensions>
CAPITestBProxy<_AttributeExtensions...>::~CAPITestBProxy() {
}

template <typename ... _AttributeExtensions>
void CAPITestBProxy<_AttributeExtensions...>::foo(int32_t _x1, CommonAPI::CallStatus &_internalCallStatus, CAPITestB::stdErrorTypeEnum &_error, int32_t &_y1, const CommonAPI::CallInfo *_info) {
    delegate_->foo(_x1, _internalCallStatus, _error, _y1, _info);
}

template <typename ... _AttributeExtensions>
std::future<CommonAPI::CallStatus> CAPITestBProxy<_AttributeExtensions...>::fooAsync(const int32_t &_x1, FooAsyncCallback _callback, const CommonAPI::CallInfo *_info) {
    return delegate_->fooAsync(_x1, _callback, _info);
}

template <typename ... _AttributeExtensions>
const CommonAPI::Address &CAPITestBProxy<_AttributeExtensions...>::getAddress() const {
    return delegate_->getAddress();
}

template <typename ... _AttributeExtensions>
bool CAPITestBProxy<_AttributeExtensions...>::isAvailable() const {
    return delegate_->isAvailable();
}

template <typename ... _AttributeExtensions>
bool CAPITestBProxy<_AttributeExtensions...>::isAvailableBlocking() const {
    return delegate_->isAvailableBlocking();
}

template <typename ... _AttributeExtensions>
CommonAPI::ProxyStatusEvent& CAPITestBProxy<_AttributeExtensions...>::getProxyStatusEvent() {
    return delegate_->getProxyStatusEvent();
}

template <typename ... _AttributeExtensions>
CommonAPI::InterfaceVersionAttribute& CAPITestBProxy<_AttributeExtensions...>::getInterfaceVersionAttribute() {
    return delegate_->getInterfaceVersionAttribute();
}


template <typename ... _AttributeExtensions>
std::future<void> CAPITestBProxy<_AttributeExtensions...>::getCompletionFuture() {
    return delegate_->getCompletionFuture();
}

} // namespace examplesB
} // namespace commonapi
} // namespace v1



// Compatibility
namespace v1_2 = v1;

#endif // V1_COMMONAPI_EXAMPLESB_CAPI_Test_B_PROXY_HPP_
