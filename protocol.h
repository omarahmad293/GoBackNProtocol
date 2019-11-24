#ifndef PROJECT_PROTOCOL_H
#define PROJECT_PROTOCOL_H

#define TIMEOUT 10 /* Timeout time in seconds*/
#define NO_TIMEOUT '0'

#include <time.h>
#include <Queue>
using namespace std;

typedef unsigned int seq_nr; /* sequence or ack numbers */

typedef struct
{
	unsigned char data;
} packet; /* packet definition */

typedef enum
{
	data_frame,
	ack,
	nak
} frame_kind; /* frame kind definition */

typedef struct
{                    /* frames are transported in this layer */
	frame_kind kind; /* what kind of frame is it? */
	seq_nr seq;      /* sequence number */
	seq_nr ack;      /* acknowledgement number */
	packet info;     /* the network layer packet */
} frame;

typedef struct
{
	vector<packet> info;
	bool enabled;
} network_layer;


void Init_Network_Layer(unsigned char data);

frame send_data(int time, seq_nr sq);
void decrement_timeout();
unsigned char check_timeout();
bool check_frame(frame f, seq_nr frame_expected);
frame send_ack(frame f);
frame resend_buffer(vector<frame> *buffer);

/* Macro inc is expanded in-line: increment k circularly. */

#define inc(k)       \
    if (k < MAX_SEQ) \
        k = k + 1;   \
    else             \
        k = 0


#endif //PROJECT_PROTOCOL_H
