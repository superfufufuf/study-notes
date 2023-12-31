/*
* This file was generated by the CommonAPI Generators.
* Used org.genivi.commonapi.core 3.2.0.v202012010850.
* Used org.franca.core 0.13.1.201807231814.
*
* This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
* If a copy of the MPL was not distributed with this file, You can obtain one at
* http://mozilla.org/MPL/2.0/.
*/
#ifndef V1_COMMONAPI_EXAMPLESA_CAPI_Test_A_STUB_HPP_
#define V1_COMMONAPI_EXAMPLESA_CAPI_Test_A_STUB_HPP_

#include <functional>
#include <sstream>



#include <v1/commonapi/examplesA/CommonTypes.hpp>

#include <v1/commonapi/examplesA/CAPITestA.hpp>

#if !defined (COMMONAPI_INTERNAL_COMPILATION)
#define COMMONAPI_INTERNAL_COMPILATION
#define HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif

#include <CommonAPI/Deployment.hpp>
#include <CommonAPI/InputStream.hpp>
#include <CommonAPI/OutputStream.hpp>
#include <CommonAPI/Struct.hpp>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

#include <mutex>

#include <CommonAPI/Stub.hpp>

#if defined (HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE)
#undef COMMONAPI_INTERNAL_COMPILATION
#undef HAS_DEFINED_COMMONAPI_INTERNAL_COMPILATION_HERE
#endif

namespace v1 {
namespace commonapi {
namespace examplesA {

/**
 * Receives messages from remote and handles all dispatching of deserialized calls
 * to a stub for the service CAPITestA. Also provides means to send broadcasts
 * and attribute-changed-notifications of observable attributes as defined by this service.
 * An application developer should not need to bother with this class.
 */
class CAPITestAStubAdapter
    : public virtual CommonAPI::StubAdapter,
      public virtual CAPITestA {
 public:
    /**
    * Sends a broadcast event for myStatus. Should not be called directly.
    * Instead, the "fire<broadcastName>Event" methods of the stub should be used.
    */
    virtual void fireMyStatusEvent(const int32_t &_myCurrentValue, const uint8_t &_myState, const bool &_bValid, const std::string &_notes, const CommonAPI::ByteBuffer &_bigData) = 0;
    ///Notifies all remote listeners about a change of value of the attribute x.
    virtual void fireXAttributeChanged(const int32_t &x) = 0;
    ///Notifies all remote listeners about a change of value of the attribute a1.
    virtual void fireA1AttributeChanged(const ::v1::commonapi::examplesA::CommonTypes::a1Struct &a1) = 0;


    virtual void deactivateManagedInstances() = 0;

    void lockXAttribute(bool _lockAccess) {
        if (_lockAccess) {
            xMutex_.lock();
        } else {
            xMutex_.unlock();
        }
    }
    void lockA1Attribute(bool _lockAccess) {
        if (_lockAccess) {
            a1Mutex_.lock();
        } else {
            a1Mutex_.unlock();
        }
    }

protected:
    /**
     * Defines properties for storing the ClientIds of clients / proxies that have
     * subscribed to the selective broadcasts
     */
    std::recursive_mutex xMutex_;
    std::recursive_mutex a1Mutex_;

};

/**
 * Defines the necessary callbacks to handle remote set events related to the attributes
 * defined in the IDL description for CAPITestA.
 * For each attribute two callbacks are defined:
 * - a verification callback that allows to verify the requested value and to prevent setting
 *   e.g. an invalid value ("onRemoteSet<AttributeName>").
 * - an action callback to do local work after the attribute value has been changed
 *   ("onRemote<AttributeName>Changed").
 *
 * This class and the one below are the ones an application developer needs to have
 * a look at if he wants to implement a service.
 */
class CAPITestAStubRemoteEvent
{
public:
    virtual ~CAPITestAStubRemoteEvent() { }

    /// Verification callback for remote set requests on the attribute x
    virtual bool onRemoteSetXAttribute(const std::shared_ptr<CommonAPI::ClientId> _client, int32_t _value) = 0;
    /// Action callback for remote set requests on the attribute x
    virtual void onRemoteXAttributeChanged() = 0;
    /// Verification callback for remote set requests on the attribute a1
    virtual bool onRemoteSetA1Attribute(const std::shared_ptr<CommonAPI::ClientId> _client, ::v1::commonapi::examplesA::CommonTypes::a1Struct _value) = 0;
    /// Action callback for remote set requests on the attribute a1
    virtual void onRemoteA1AttributeChanged() = 0;
};

/**
 * Defines the interface that must be implemented by any class that should provide
 * the service CAPITestA to remote clients.
 * This class and the one above are the ones an application developer needs to have
 * a look at if he wants to implement a service.
 */
class CAPITestAStub
    : public virtual CommonAPI::Stub<CAPITestAStubAdapter, CAPITestAStubRemoteEvent>
{
public:
    typedef std::function<void (CAPITestA::stdErrorTypeEnum _error, int32_t _y1, std::string _y2)> fooReply_t;

    virtual ~CAPITestAStub() {}
    void lockInterfaceVersionAttribute(bool _lockAccess) { static_cast<void>(_lockAccess); }
    bool hasElement(const uint32_t _id) const {
        return (_id < 4);
    }
    virtual const CommonAPI::Version& getInterfaceVersion(std::shared_ptr<CommonAPI::ClientId> _client) = 0;

    /// This is the method that will be called on remote calls on the method foo.
    virtual void foo(const std::shared_ptr<CommonAPI::ClientId> _client, int32_t _x1, std::string _x2, fooReply_t _reply) = 0;
    /// Sends a broadcast event for myStatus.
    virtual void fireMyStatusEvent(const int32_t &_myCurrentValue, const uint8_t &_myState, const bool &_bValid, const std::string &_notes, const CommonAPI::ByteBuffer &_bigData) {
        auto stubAdapter = CommonAPI::Stub<CAPITestAStubAdapter, CAPITestAStubRemoteEvent>::stubAdapter_.lock();
        if (stubAdapter)
            stubAdapter->fireMyStatusEvent(_myCurrentValue, _myState, _bValid, _notes, _bigData);
    }
    /// Provides getter access to the attribute x
    virtual const int32_t &getXAttribute(const std::shared_ptr<CommonAPI::ClientId> _client) = 0;
    /// sets attribute with the given value and propagates it to the adapter
    virtual void fireXAttributeChanged(int32_t _value) {
    auto stubAdapter = CommonAPI::Stub<CAPITestAStubAdapter, CAPITestAStubRemoteEvent>::stubAdapter_.lock();
    if (stubAdapter)
        stubAdapter->fireXAttributeChanged(_value);
    }
    void lockXAttribute(bool _lockAccess) {
        auto stubAdapter = CommonAPI::Stub<CAPITestAStubAdapter, CAPITestAStubRemoteEvent>::stubAdapter_.lock();
        if (stubAdapter)
            stubAdapter->lockXAttribute(_lockAccess);
    }
    /// Provides getter access to the attribute a1
    virtual const ::v1::commonapi::examplesA::CommonTypes::a1Struct &getA1Attribute(const std::shared_ptr<CommonAPI::ClientId> _client) = 0;
    /// sets attribute with the given value and propagates it to the adapter
    virtual void fireA1AttributeChanged(::v1::commonapi::examplesA::CommonTypes::a1Struct _value) {
    auto stubAdapter = CommonAPI::Stub<CAPITestAStubAdapter, CAPITestAStubRemoteEvent>::stubAdapter_.lock();
    if (stubAdapter)
        stubAdapter->fireA1AttributeChanged(_value);
    }
    void lockA1Attribute(bool _lockAccess) {
        auto stubAdapter = CommonAPI::Stub<CAPITestAStubAdapter, CAPITestAStubRemoteEvent>::stubAdapter_.lock();
        if (stubAdapter)
            stubAdapter->lockA1Attribute(_lockAccess);
    }


    using CommonAPI::Stub<CAPITestAStubAdapter, CAPITestAStubRemoteEvent>::initStubAdapter;
    typedef CommonAPI::Stub<CAPITestAStubAdapter, CAPITestAStubRemoteEvent>::StubAdapterType StubAdapterType;
    typedef CommonAPI::Stub<CAPITestAStubAdapter, CAPITestAStubRemoteEvent>::RemoteEventHandlerType RemoteEventHandlerType;
    typedef CAPITestAStubRemoteEvent RemoteEventType;
    typedef CAPITestA StubInterface;
};

} // namespace examplesA
} // namespace commonapi
} // namespace v1


// Compatibility
namespace v1_2 = v1;

#endif // V1_COMMONAPI_EXAMPLESA_CAPI_Test_A_STUB_HPP_
