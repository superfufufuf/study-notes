#include "msgNodeFactory.h"
#include <string>
#include <iostream>
#include <list>
#include <algorithm>
#include <thread>

using namespace std;

string userinfo =
    R"(
server:
    PUSH
    REQ
    PUB
    SURVEYOR
client:
    PULL
    REP
    SUB
    RESPONDENT
point:
    PAIR
    BUS
)";

static list<string> userList = {"PULL", "REP", "PUB", "SURVEYOR", "PUSH", "REQ", "SUB", "RESPONDENT", "PAIR", "BUS"};

int main(int argc, char const *argv[])
{
    MsgNodeFactory factory;
    list<string>::iterator iter;
    MsgNodeAttribute attr;

    do
    {
        cout << "please input user:" << userinfo << endl;
        cin >> attr.role;
        iter = find(userList.begin(), userList.end(), attr.role);
    } while (iter == userList.end());

    cout << "please input url(E.g. inproc://test; ipc:///tmp/test.ipc; tcp://*:5555; ws://*:5555, entering \"def\" will use the default url):" << endl;
    cin >> attr.url;
    if (attr.url == "def")
    {
        attr.url = "ipc:///tmp/test.ipc";
    }

    int RelationshipOfSendMustRecv = 0;
    if (attr.role == "REQ" || attr.role == "SURVEYOR")
    {
        RelationshipOfSendMustRecv = -1;
    }
    else if (attr.role == "REP" || attr.role == "RESPONDENT")
    {
        RelationshipOfSendMustRecv = 1;
    }

    if (attr.role == "SUB")
    {
        cout << "please input topic(\"all\" indicates that all topics are received):" << endl;
        cin >> attr.topic;
        if (attr.topic == "all")
        {
            attr.topic.clear();
        }
    }
    else if (attr.role == "BUS")
    {
        while (true)
        {
            string url;
            cout << "please input all connect url(If you want to exit, input \"exit\"):" << endl;
            cin >> url;
            if (url == "exit")
            {
                break;
            }
            attr.connectUrl.push_back(url);
        }
    }

    auto node = factory.CreateMsgNode(attr);
    if (RelationshipOfSendMustRecv == 0)
    {
        thread([&]()
               {
            while (true)
            {
                string msg = node->GetMsg(); 
                this_thread::sleep_for(chrono::milliseconds(1000));
            } })
            .detach();
    }

    while (true)
    {
        if (RelationshipOfSendMustRecv > 0)
        {
            string msg = node->GetMsg();
            cout << "recv data[" << msg << "]" << endl;
        }
        string data;
        cin >> data;
        if (node->SendMsg(data))
        {
            cout << "send data[" << data << "] succeed." << endl;
        }
        else
        {
            cout << "send data[" << data << "] failed." << endl;
        }
        if (RelationshipOfSendMustRecv < 0)
        {
            string msg = node->GetMsg();
            cout << "recv data[" << msg << "]" << endl;
        }
    }

    return 0;
}