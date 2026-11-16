#include <stdio.h>

#define BUFFER_SIZE 8

unsigned char buffer[BUFFER_SIZE];

int head = 0;
int tail = 0;
int count = 0;

int ring_buffer_push(unsigned char data)
{
    if (count == BUFFER_SIZE)
    {
        return -1;
    }

    buffer[head] = data;

    head = (head + 1) % BUFFER_SIZE;

    count++;

    return 0;
}

int ring_buffer_pop(unsigned char *data)
{
    if (count == 0)
    {
        return -1;
    }

    *data = buffer[tail];

    tail = (tail + 1) % BUFFER_SIZE;

    count--;

    return 0;
}

int main(void)
{
    for (int i = 0; i < 9; i++)
    {
        unsigned char data = 'A' + i;

        if (ring_buffer_push(data) == 0)
        {
            printf("Push %c: Success\n", data);
        }
        else
        {
            printf("Push %c: Buffer Full!\n", data);
        }
    }

    printf("Head = %d\n", head);
    printf("Tail = %d\n", tail);
    printf("Count = %d\n", count);

    return 0;
}
