#include "protocol.h"
#include <iostream>
#include <map>
using namespace std;

extern network_layer Network;
extern map<unsigned char, int> timeout_list;

void Init_Network_Layer(unsigned char data)
{
	/* Initialize network layer with the input data */
	packet p = { data };
	Network.info.push_back(p);
}

frame send_data(int time, seq_nr sq)
{
	/* Send frame and start timer and add frame to buffer and return the frame */
	cout << "Sender sent " << Network.info.front().data << sq << endl;
	packet p = Network.info.front();
	timeout_list[p.data] = time + TIMEOUT;
	Network.info.erase(Network.info.begin());

	frame f;
	f.info = p;
	f.seq = sq;

	return f;
}

void decrement_timeout()
{
	/* Decrement the timeout list */
	for (map<unsigned char, int>::iterator i = timeout_list.begin(); i != timeout_list.end(); i++)
	{
		(i->second)--;
	}
}

unsigned char check_timeout()
{
	/* Check if a frame has timedout and return its packet */
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
	/* Check the expected frame */
	return (f.seq == frame_expected);
}

frame send_ack(frame f)
{
	cout << "Receiver sent ACK on " << f.info.data << f.seq << endl;

	return f;
}

frame resend_buffer(vector<frame> *buffer)
{
	/* Move all buffered frames to the network layers to be resent */
	frame f = buffer->front();
	cout << "\033[1;33mRESEND\033[1;0m" << endl;
	while (!buffer->empty())
	{
		Network.info.push_back(buffer->begin()->info);
		buffer->erase(buffer->begin());
	}
	return f;
}