#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread

namespace bible {

using namespace std;

class BibleServer;
class BibleSocket;

struct BibleS


class BibleSocket{

private:
vector<> _pool;
vector<int> _client_socket_pool; 
size_t _max_client_socket_count = 10;
public:
BibleSocket();
~BibleSocket();

void Receive();
void Send();
void Accept();


};

class BibleServer{
private:

public:
	BibleServer(){}


};


} // end namespace bible.

#endif