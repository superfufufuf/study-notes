#include <thread>
#include <iostream>
#include <algorithm>
#include "ModelA.h"
#include "ModelB.h"
#include "CAPIConst.h"

std::string CurrentModel;

using namespace std;

int main(int argc, char const *argv[])
{
    std::list<std::string> listAllModel;
    listAllModel.push_back(RunModel_All);
    listAllModel.push_back(RunModel_Method);
    listAllModel.push_back(RunModel_Broadcast);
    listAllModel.push_back(RunModel_Notifier);
    listAllModel.push_back(RunModel_Getter);
    listAllModel.push_back(RunModel_Setter);

    cout << "Please selecte test project:" << endl;
    for (auto model : listAllModel)
    {
        cout << "    " << model << endl;
    }

    while (CurrentModel.empty())
    {
        cin >> CurrentModel;
        if (find(listAllModel.begin(), listAllModel.end(), CurrentModel) == listAllModel.end())
        {
            cout << "Cannot find this model ,Please re-select" << endl;
            CurrentModel.clear();
        }
    }

    thread(ServerM).detach();
    if (CurrentModel == RunModel_All)
    {
        thread(ClientN1).detach();
        thread(ClientN2).detach();
    }

    cout << "all thread create complete" << endl;
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}