Pipeline:
Must be sent by the server and recv by the client.

Request/Reply:
It must be preferentially sent by the server and recv by the client. Then the client can send.
nn_setsockopt special parameter:
NN_REQ_RESEND_IVL

Pair:
You must specify a server and a client. The server uses 'nn_bind' and the client uses 'nn_connect'.

Pub/Sub:
You must set nn_setsockopt:
nn_setsockopt(sock, NN_SUB, NN_SUB_SUBSCRIBE, topicName.c_str(), topicName.length())
when topicName is empty, you can recv all topic.
nn_setsockopt special parameter:
NN_SUB_SUBSCRIBE
NN_SUB_UNSUBSCRIBE

Survey:
when NN_SURVEYOR call nn_recv(), it has a wait time. you can set this timeout by calling nn_setsockopt(), the parameter is NN_SURVEYOR_DEADLINE.
nn_setsockopt special parameter:
NN_SURVEYOR_DEADLINE


Bus:
Unlike other patterns, which share a url, this pattern is one url for each node, and you need connect other url to recv msg.
when you call nn_send(), msg will send to all node who had connect your url.
when you call nn_recv(), you will recv all msg which url you conncet.
In addtion, if you nodeA connect nodeB.url, and nodeB connect nodeA.url. Then when either party sends a msg, the other party will receive two msg.
In general, you can use it to connect to other nodes, and other nodes don't need to connect to it.

nn_setsockopt standard parameter:
NN_LINGER
NN_SNDBUF
NN_RCVBUF
NN_SNDTIMEO
NN_RCVTIMEO
NN_RECONNECT_IVL
NN_RECONNECT_IVL_MAX
NN_SNDPRIO
NN_RCVPRIO
NN_SNDFD
NN_RCVFD
NN_DOMAIN
NN_PROTOCOL
NN_IPV4ONLY
NN_SOCKET_NAME
NN_RCVMAXSIZE
NN_MAXTTL

tcp:
In general, bind ip use own ip or *, and conncet ip use binder ip.