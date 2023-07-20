#include "msgNodeFactory.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include <nanomsg/reqrep.h>
#include <nanomsg/pair.h>
#include <nanomsg/pubsub.h>
#include <nanomsg/survey.h>
#include <nanomsg/bus.h>

using namespace std;

MsgNode::~MsgNode()
{
    nn_shutdown(sock, 0);
}

void MsgNode::Init(const MsgNodeAttribute &attr)
{
    url = attr.url;
}

bool MsgNode::SendMsg(const string &msg)
{
    int bytes = nn_send(sock, msg.c_str(), msg.length(), 0);
    if (bytes < 0)
    {
        return false;
    }
    return true;
}

string MsgNode::GetMsg()
{
    string msg;

    char *buf = NULL;
    int bytes = nn_recv(sock, &buf, NN_MSG, 0);
    if (bytes >= 0)
    {
        msg = string(buf, bytes);
        nn_freemsg(buf);
        cout << "recv data[" << msg << "], bytes[" << bytes << "]" << endl;
    }
    else
    {
        if (nn_errno() == ETIMEDOUT)
        {
            cout << "timeout" << endl;
        }
    }
    return msg;
}

void PipelineServerNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_PUSH)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_bind(sock, url.c_str()) < 0)
    {
        cout << "nn_bind failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    cout << "creat PipelineServerNode succeed." << endl;
}

bool PipelineServerNode::SendMsg(const string &msg)
{
    return MsgNode::SendMsg(msg);
}

void PipelineClientNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_PULL)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_connect(sock, url.c_str()) < 0)
    {
        cout << "nn_connect failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    cout << "creat PipelineClientNode succeed." << endl;
}

string PipelineClientNode::GetMsg()
{
    return MsgNode::GetMsg();
}

void ReqRepServerNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_REQ)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_bind(sock, url.c_str()) < 0)
    {
        cout << "nn_bind failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    cout << "creat ReqRepServerNode succeed." << endl;
}

bool ReqRepServerNode::SendMsg(const string &msg)
{
    return MsgNode::SendMsg(msg);
}

string ReqRepServerNode::GetMsg()
{
    return MsgNode::GetMsg();
}

void ReqRepClientNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_REP)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_connect(sock, url.c_str()) < 0)
    {
        cout << "nn_connect failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    cout << "creat ReqRepClientNode succeed." << endl;
}

bool ReqRepClientNode::SendMsg(const string &msg)
{
    return MsgNode::SendMsg(msg);
}

string ReqRepClientNode::GetMsg()
{
    return MsgNode::GetMsg();
}

void PairNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_PAIR)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_bind(sock, url.c_str()) < 0)
    {
        if (nn_connect(sock, url.c_str()) < 0)
        {
            cout << "nn_bind and nn_connect failed"
                 << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
            return;
        }
        else
        {
            cout << "creat PairNode client succeed." << endl;
        }
    }
    else
    {
        cout << "creat PairNode server succeed." << endl;
    }
}

bool PairNode::SendMsg(const string &msg)
{
    return MsgNode::SendMsg(msg);
}

string PairNode::GetMsg()
{
    return MsgNode::GetMsg();
}

void PubSubServerNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_PUB)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_bind(sock, url.c_str()) < 0)
    {
        cout << "nn_bind failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    cout << "creat PubSubServerNode succeed." << endl;
}

bool PubSubServerNode::SendMsg(const string &msg)
{
    return MsgNode::SendMsg(msg);
}

void PubSubClientNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_SUB)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_setsockopt(sock, NN_SUB, NN_SUB_SUBSCRIBE, attr.topic.c_str(), attr.topic.length()) < 0)
    {
        cout << "nn_setsockopt failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_connect(sock, url.c_str()) < 0)
    {
        cout << "nn_connect failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    cout << "creat PubSubClientNode succeed." << endl;
}

string PubSubClientNode::GetMsg()
{
    return MsgNode::GetMsg();
}

void SurveyServerNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_SURVEYOR)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    int timeout = 1000;
    if (nn_setsockopt(sock, NN_SURVEYOR, NN_SURVEYOR_DEADLINE, &timeout, sizeof(timeout)) < 0)
    {
        cout << "nn_setsockopt failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_bind(sock, url.c_str()) < 0)
    {
        cout << "nn_bind failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    cout << "creat SurveyServerNode succeed." << endl;
}

bool SurveyServerNode::SendMsg(const string &msg)
{
    return MsgNode::SendMsg(msg);
}

string SurveyServerNode::GetMsg()
{
    return MsgNode::GetMsg();
}

void SurveyClientNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_RESPONDENT)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_connect(sock, url.c_str()) < 0)
    {
        cout << "nn_connect failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    cout << "creat SurveyClientNode succeed." << endl;
}

bool SurveyClientNode::SendMsg(const string &msg)
{
    return MsgNode::SendMsg(msg);
}

string SurveyClientNode::GetMsg()
{
    return MsgNode::GetMsg();
}

void BusNode::Init(const MsgNodeAttribute &attr)
{
    MsgNode::Init(attr);

    if ((sock = nn_socket(AF_SP, NN_BUS)) < 0)
    {
        cout << "nn_socket failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    if (nn_bind(sock, url.c_str()) < 0)
    {
        cout << "nn_bind failed"
             << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
        return;
    }

    for (auto url : attr.connectUrl)
    {
        if (nn_connect(sock, url.c_str()) < 0)
        {
            cout << "nn_connect failed"
                 << "error: " << strerror(errno) << ", nn_error:" << nn_errno() << endl;
            return;
        }
    }
    cout << "creat BusNode succeed." << endl;
}

bool BusNode::SendMsg(const string &msg)
{
    return MsgNode::SendMsg(msg);
}

string BusNode::GetMsg()
{
    return MsgNode::GetMsg();
}

MsgNodeFactory::MsgNodeFactory()
{
}

MsgNodeFactory::~MsgNodeFactory()
{
}

MsgNode *MsgNodeFactory::CreateMsgNode(const MsgNodeAttribute &_attr)
{
    MsgNode *node = nullptr;
    if (_attr.role == "PUSH")
    {
        node = new PipelineServerNode();
    }
    else if (_attr.role == "REQ")
    {
        node = new ReqRepServerNode();
    }
    else if (_attr.role == "PUB")
    {
        node = new PubSubServerNode();
    }
    else if (_attr.role == "SURVEYOR")
    {
        node = new SurveyServerNode();
    }
    else if (_attr.role == "PULL")
    {
        node = new PipelineClientNode();
    }
    else if (_attr.role == "REP")
    {
        node = new ReqRepClientNode();
    }
    else if (_attr.role == "SUB")
    {
        node = new PubSubClientNode();
    }
    else if (_attr.role == "RESPONDENT")
    {
        node = new SurveyClientNode();
    }
    else if (_attr.role == "PAIR")
    {
        node = new PairNode();
    }
    else if (_attr.role == "BUS")
    {
        node = new BusNode();
    }

    if (node != nullptr)
    {
        node->Init(_attr);
    }
    return node;
}