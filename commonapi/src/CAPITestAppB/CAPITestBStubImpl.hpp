// Copyright (C) 2014-2019 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef CAPITestBSTUBIMPL_HPP_
#define CAPITestBSTUBIMPL_HPP_

#include <CommonAPI/CommonAPI.hpp>
#include <v1/commonapi/examplesB/CAPITestBStubDefault.hpp>

class CAPITestBStubImpl : public v1_2::commonapi::examplesB::CAPITestBStubDefault
{

public:
    CAPITestBStubImpl();
    virtual ~CAPITestBStubImpl();
    virtual void foo(const std::shared_ptr<CommonAPI::ClientId> _client,
                     int32_t _x1,
                     fooReply_t _reply);

private:
    int cnt;
};

#endif // CAPITestBSTUBIMPL_HPP_
