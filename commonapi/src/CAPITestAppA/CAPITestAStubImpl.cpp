// Copyright (C) 2014-2019 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "CAPITestAStubImpl.hpp"
#include "CAPIConst.h"

using namespace v1_2::commonapi::examplesA;

CAPITestAStubImpl::CAPITestAStubImpl()
{
    cnt = 0;
}

CAPITestAStubImpl::~CAPITestAStubImpl()
{
}

void CAPITestAStubImpl::incCounter()
{
    cnt++;
    if (CurrentModel == RunModel_All || CurrentModel == RunModel_Broadcast)
    {
        CommonAPI::ByteBuffer bigData;
        uint8_t chArr[] = {'I', ' ', 'i', 's', ' ', 'a', 'n', ' ', 'f', 'i', 's', 'h'};
        for (auto ch : chArr)
        {
            bigData.push_back(ch);
        }
        fireMyStatusEvent((int32_t)cnt, '8', true, "CAPITest", bigData);
        std::cout << "[ServerM]Send boardcast value = " << cnt << "!" << std::endl;
    }
    if (CurrentModel == RunModel_All || CurrentModel == RunModel_Notifier)
    {
        setXAttribute((int32_t)cnt);
        std::cout << "[ServerM]Send notify value = " << cnt << "!" << std::endl;
    }
};

void CAPITestAStubImpl::foo(const std::shared_ptr<CommonAPI::ClientId> _client,
                            int32_t _x1, std::string _x2,
                            fooReply_t _reply)
{

    std::cout << "[ServerM]foo called, setting new values." << std::endl;

    CAPITestA::stdErrorTypeEnum methodError = CAPITestA::stdErrorTypeEnum::MY_FAULT;
    int32_t y1 = -_x1;
    std::string y2 = _x2 + "xyz";
    _reply(methodError, y1, y2);
}
