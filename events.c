#include "events.h"

#define MAX_EVENTS 1024

static event_t eventv[MAX_EVENTS];
static int eventc;

int create_event(event_t event)
{
	if (eventc == MAX_EVENTS)
		return 1;

	eventv[eventc] = event;
	eventc++;
	return 0;
}

size_t next_events(event_t *dest, size_t count)
{
	int time = eventv[0].time;
	for (int i = 1; i < eventc; ++i)
		if (eventv[i].time < time)
			time = eventv[i].time;

	int num = 0;
	for (int i = 0; i < eventc; ++i) {
		if (eventv[i].time == time) {
			dest[num++] = eventv[i];
			eventc--;
			eventv[i] = eventv[eventc];
			i--;
		}

		if (num == count)
			break;
	}
		

	return num;
}

/* Unit test */

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	#define BUFFER_COUNT 16
	event_t buffer[BUFFER_COUNT];

	srand(time(NULL));
	for (int i = 0; i < 100; ++i)
		create_event((event_t) { EV_PROCESS_BEGIN, rand() % 20, 0 });

	for (;;) {
		size_t num = next_events(buffer, BUFFER_COUNT);
		if (num == 0)
			break;
		printf("time = %d, num = %zu\n", buffer[0].time, num);
	}
		
	return 0;
}
#endif
