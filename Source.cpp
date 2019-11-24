#include <iostream>
#include <list>
#include <map>
#include "protocol.h"
using namespace std;

#define TIME_UNIT 500					/* Maximum runtime of the program */
#define MAX_SEQ 7						/* Window size */

network_layer Network;					/* Simulation of the network layer*/
vector<char> Ignores;					/* Simulation of the corrupted frames*/
map<unsigned char, int> timeout_list;	/* Simulation of multiple timers in software */

int main()
{
	/* Variables used for data input */
	int Network_Packets;				
	unsigned char DataString;

	cout << "Enter Number of Packets" << endl;
	cin >> Network_Packets;

	cout << "Enter Packets of PC 0" << endl;
	for (int i = 0; i < Network_Packets; i++)
	{
		cin >> DataString;
		Init_Network_Layer(DataString);
	}

	cout << "Enter Number of Ignores" << endl;
	cin >> Network_Packets;
	cout << "Enter Ignores sequentially" << endl;
	for (int i = 0; i < Network_Packets; i++)
	{
		char dummy;
		cin >> dummy;
		Ignores.push_back(dummy);
	}

	seq_nr frame_expected = 0;	/* Next frame expected on inbound stream */
	seq_nr sq = 0;				/* Used for outbound stream */
	seq_nr ack_expected = 0;    /* Oldest frame as yet unacknowledged */
	frame ack = {};				/* Ack frame sent to the sender by the receiver*/
	vector<frame> buffer;		/* Buffers for the outbound stream */
	frame f;					/* Scratch variable */

	/* Simulating a real clock */
	for (int clock = 0; clock < TIME_UNIT; clock++)
	{
		/* Terminate the program */
		if ((Network.info.empty()) && (buffer.empty()))
		{
			cout << "\033[1;32mAll packets are sent succesfully\033[0m" << endl;
			return 0;
		}

		cout << "\033[1;31m" << clock + 1 << "\033[0m" << endl;
		decrement_timeout();
		unsigned char timedout = check_timeout();  /* Timedout packet flag*/

		/* Resend data if timeout happened */
		if (timedout != NO_TIMEOUT && clock != 0)
		{
			f = resend_buffer(&buffer);
			sq = f.seq;
			timedout = NO_TIMEOUT;
		}

		/* Correct ACK received */
		if (ack_expected == ack.seq && clock != 0)
		{
			cout << "ACK " << ack.info.data << ack.seq << " was received" << endl;
			/* Stop timer */
			timeout_list.erase(ack.info.data);

			/* Remove ACKed frame from the buffer */
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

		/* Send data if network layers isn't empty */
		if (!Network.info.empty())
		{
			f = send_data(clock, sq);
			buffer.push_back(f);
			inc(sq);

			/*======================================================*/

			/* Simulate corrupted frames */
			if (!Ignores.empty() && f.info.data == Ignores[0])
			{
				cout << f.info.data << " is ignored" << endl;
				Ignores.erase(Ignores.begin());
			}
			else
			{
				/* Receive frame and send ACK */
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