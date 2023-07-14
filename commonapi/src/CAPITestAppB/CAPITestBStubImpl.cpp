// Copyright (C) 2014-2019 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CAPITestBStubImpl.hpp"

using namespace v1_2::commonapi::examplesB;

CAPITestBStubImpl::CAPITestBStubImpl()
{
    cnt = 0;
}

CAPITestBStubImpl::~CAPITestBStubImpl()
{
}

void CAPITestBStubImpl::foo(const std::shared_ptr<CommonAPI::ClientId> _client,
                            int32_t _x1,
                            fooReply_t _reply)
{

    std::cout << "[ServerN]foo called, setting new values." << std::endl;

    CAPITestB::stdErrorTypeEnum methodError = CAPITestB::stdErrorTypeEnum::MY_FAULT;
    int32_t y1 = _x1 * _x1;
    _reply(methodError, y1);
}
