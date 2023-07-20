#include <string>
#include <atomic>
#include <list>

using std::string;

struct MsgNodeAttribute
{
    string role;                  //{NN_PUSH, NN_PULL, NN_REQ, NN_REP, NN_PAIR, NN_PUB, NN_SUB, NN_SURVEYOR, NN_RESPONDENT, NN_BUS}
    string url;                   // E.g. inproc://test; ipc:///tmp/test.ipc; tcp://*:5555; ws://*:5555
    string topic;                 // only PubSub use.
    std::list<string> connectUrl; // only Bus use.
};

class MsgNode
{
public:
    virtual ~MsgNode();

    virtual void Init(const MsgNodeAttribute &attr);

    virtual bool SendMsg(const string &msg);

    virtual string GetMsg();

protected:
    string url;
    int sock;
};

class SercerNode : public MsgNode
{
};

class ClientNode : public MsgNode
{
};

class PipelineServerNode : public SercerNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual bool SendMsg(const string &msg) override;
};

class PipelineClientNode : public ClientNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual string GetMsg() override;
};

class ReqRepServerNode : public SercerNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual bool SendMsg(const string &msg) override;
    virtual string GetMsg() override;
};

class ReqRepClientNode : public ClientNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual bool SendMsg(const string &msg) override;
    virtual string GetMsg() override;
};

class PairNode : public MsgNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual bool SendMsg(const string &msg) override;
    virtual string GetMsg() override;
};

class PubSubServerNode : public SercerNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual bool SendMsg(const string &msg) override;
};

class PubSubClientNode : public ClientNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual string GetMsg() override;
};

class SurveyServerNode : public SercerNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual bool SendMsg(const string &msg) override;
    virtual string GetMsg() override;
};

class SurveyClientNode : public ClientNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual bool SendMsg(const string &msg) override;
    virtual string GetMsg() override;
};

class BusNode : public MsgNode
{
    virtual void Init(const MsgNodeAttribute &attr) override;
    virtual bool SendMsg(const string &msg) override;
    virtual string GetMsg() override;
};

class MsgNodeFactory
{
public:
    MsgNodeFactory();
    ~MsgNodeFactory();

    MsgNode *CreateMsgNode(const MsgNodeAttribute &_attr);

private:
};