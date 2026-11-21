/*
 * QUIET OS — core/event_manager.c
 *
 * Event queue implementation.
 * See event_manager.h for documentation.
 */

#include "event_manager.h"
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Queue storage                                                       */
/* ------------------------------------------------------------------ */

#define EVENT_QUEUE_SIZE 64

static Event s_queue[EVENT_QUEUE_SIZE];
static int   s_head = 0;   /* Next position to read  */
static int   s_tail = 0;   /* Next position to write */

void event_manager_init(void)
{
    s_head = 0;
    s_tail = 0;
    memset(s_queue, 0, sizeof(s_queue));
    printf("[EVENT] Event manager initialized. Queue size: %d\n", EVENT_QUEUE_SIZE);
}

void event_manager_deinit(void)
{
    s_head = 0;
    s_tail = 0;
}

void event_push(Event event)
{
    int next_tail = (s_tail + 1) % EVENT_QUEUE_SIZE;

    if (next_tail == s_head) {
        /* Queue full — this is a bug if it happens frequently */
        printf("[EVENT] WARNING: Event queue full! Dropping event type=%d\n",
               (int)event.type);
        return;
    }

    s_queue[s_tail] = event;
    s_tail = next_tail;
}

void event_push_key(EventType type)
{
    Event e;
    e.type      = type;
    e.data.value = 0;
    event_push(e);
}

bool event_poll(Event* out_event)
{
    if (s_head == s_tail) {
        return false;  /* Queue is empty */
    }
    *out_event = s_queue[s_head];
    s_head = (s_head + 1) % EVENT_QUEUE_SIZE;
    return true;
}

void event_clear(void)
{
    s_head = 0;
    s_tail = 0;
}
