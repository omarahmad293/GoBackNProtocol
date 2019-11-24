#include "protocol.h"
#include <iostream>
#include <map>
using namespace std;

extern network_layer Network[2];
extern map<unsigned char, int> timeout_list;

void Init_Network_Layer(int PC_Number, unsigned char data)
{
	packet p = { data };
	Network[PC_Number].info.push_back(p);
}

frame send_data(int time, seq_nr sq)
{
	cout << "Sender sent " << Network[0].info.front().data << sq << endl;
	packet p = Network[0].info.front();
	timeout_list[p.data] = time + TIMEOUT;
	Network[0].info.erase(Network[0].info.begin());

	frame f;
	f.info = p;
	f.seq = sq;

	return f;
}

void decrement_timeout()
{
	for (map<unsigned char, int>::iterator i = timeout_list.begin(); i != timeout_list.end(); i++)
	{
		(i->second)--;
	}
}

unsigned char check_timeout()
{
	for (map<unsigned char, int>::iterator i = timeout_list.begin(); i != timeout_list.end(); i++)
	{
		if (i->second == 0)
		{
			cout << "\033[1;33m" << i->first << " timedout\033[1;0m" << endl;
			return i->first;
		}
	}
	return NO_TIMEOUT;
}

bool check_frame(frame f, seq_nr frame_expected)
{
	return (f.seq == frame_expected);
}

frame send_ack(frame f)
{
	cout << "Receiver sent ACK on " << f.info.data << f.seq << endl;

	return f;
}

frame resend_buffer(vector<frame> *buffer)
{
	frame f = buffer->front();
	cout << "\033[1;33mRESEND\033[1;0m" << endl;
	while (!buffer->empty())
	{
		Network[0].info.push_back(buffer->begin()->info);
		buffer->erase(buffer->begin());
	}
	return f;
}