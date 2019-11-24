#include <iostream>
#include <list>
#include <map>
#include "protocol.h"
using namespace std;

#define TIME_UNIT 500
#define MAX_SEQ 7

network_layer Network[2];
physical_layer Physical[2];
vector<char> Ignores;
map<unsigned char, int> timeout_list;

int main()
{
	int Network_Packets;
	unsigned char DataString;

	cout << "Enter Number of Packets" << endl;
	cin >> Network_Packets;

	cout << "Enter Packets of PC 0" << endl;
	for (int i = 0; i < Network_Packets; i++)
	{
		cin >> DataString;
		Init_Network_Layer(0, DataString);
	}
	/*
	cout << "Enter Packets of PC 1" << endl;
	for (int i = 0; i < Network_Packets; i++)
	{
		cin >> DataString;
		Init_Network_Layer(1, DataString);
	}
	*/
	cout << "Enter Number of Ignores" << endl;
	cin >> Network_Packets;
	cout << "Enter Ignores sequentially" << endl;
	for (int i = 0; i < Network_Packets; i++)
	{
		char dummy;
		cin >> dummy;
		Ignores.push_back(dummy);
	}


	seq_nr frame_expected = 0;
	seq_nr sq = 0;
	seq_nr ack_expected = 0;    /*last sent item*/
	frame ack = {};
	vector<frame> buffer;
	frame f;

	for (int clock = 0; clock < TIME_UNIT; clock++)
	{
		if ((Network[0].info.empty()) && (buffer.empty()))
		{
			cout << "\033[1;32mAll packets are sent succesfully\033[0m" << endl;
			return 0;
		}

		cout << "\033[1;31m" << clock + 1 << "\033[0m" << endl;
		decrement_timeout();
		unsigned char timedout = check_timeout();

		if (timedout != NO_TIMEOUT && clock != 0) //timeout
		{
			f = resend_buffer(&buffer);
			sq = f.seq;
			timedout = NO_TIMEOUT;
		}

		if (ack_expected == ack.seq && clock != 0)
		{
			cout << "ACK " << ack.info.data << ack.seq << " was received" << endl;
			timeout_list.erase(ack.info.data);

			for (auto it = buffer.begin(); it != buffer.end(); it++)
			{
				if (it->seq == ack.seq)
				{
					buffer.erase(it);
					break;
				}
			}

			inc(ack_expected);
		}

		if (!Network[0].info.empty())
		{

			f = send_data(clock, sq);
			buffer.push_back(f);
			inc(sq);

			/*======================================================*/

			if (!Ignores.empty() && f.info.data == Ignores[0])
			{
				cout << f.info.data << " is ignored" << endl;
				Ignores.erase(Ignores.begin());
			}
			else
			{
				if (check_frame(f, frame_expected))
				{
					cout << f.info.data << " is received" << endl;
					cout << "Sending ACK " << frame_expected << endl;
					inc(frame_expected);
					ack = send_ack(f);
				}
				else
				{
					cout << f.info.data << " is discarded" << endl;
				}

			}
		}
	}

	return 0;

}


//TODO: bidirectional