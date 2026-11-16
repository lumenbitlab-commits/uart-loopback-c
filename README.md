# UART Loopback in C

This is the first milestone in my Embedded Linux Driver and BSP learning project. A macOS user-space C program uses `termios` to configure a CP2102 USB-to-UART adapter at 115200 baud, 8N1. In the recorded hardware loopback, it sent five bytes (`HELLO`), received all five, and verified the payload byte for byte.

[LumenBit Lab 技術部落格](docs/README.md)

| Source | Purpose |
| --- | --- |
| [`uart_client.c`](uart_client.c) | UART configuration, partial read and write handling, and payload verification |
| [`ring_buffer.c`](ring_buffer.c) | Standalone eight-byte FIFO exercise with push, pop, and a full-buffer demo |

## Test setup

| Item | Setting |
| --- | --- |
| Host | macOS |
| Adapter | CP2102 USB-to-UART |
| Baud rate | 115200 |
| Frame format | 8 data bits, no parity, 1 stop bit (8N1) |
| Loopback | Connect TX to RX on the same USB-to-UART adapter |
| Test payload | `HELLO` (5 bytes) |

## Recorded hardware result

```text
Write result = 5
Received 5 Bytes
Received = HELLO
UART Loopback PASS
```

This is the output excerpt from the earlier CP2102 run. The current version reports the same count as `Total written = 5 Bytes`.

## Build and run

Connect the adapter's TX and RX pins to each other, then plug it into the Mac. Run `ls /dev/cu.*` to identify the adapter's device path. Substitute that path for `/dev/cu.usbserial-XXXX` below. The program configures the port and sends `HELLO` automatically.

```sh
clang -std=gnu11 -Wall -Wextra -pedantic uart_client.c -o uart_client
./uart_client /dev/cu.usbserial-XXXX
```

The program prints `UART Loopback PASS` and exits with status `0` when the five bytes match. Receiving fewer bytes or a different payload gives a nonzero exit status. Each read waits up to approximately one second for input.

The standalone ring buffer demo builds and runs independently.

```sh
clang -std=c11 -Wall -Wextra -pedantic ring_buffer.c -o ring_buffer
./ring_buffer
```

The demo accepts `A` through `H`, rejects `I` when the buffer is full, and reports `Count = 8`.

## Local verification

Both programs compiled with Apple Clang with warnings treated as errors. On a macOS pseudo-terminal, the UART program passed a matching echo delivered in two chunks and correctly failed on a mismatched echo, a short echo, and no response. The ring buffer passed separate checks for full and empty states, FIFO order, and index wraparound. The current UART revision was exercised on the pseudo-terminal. The CP2102 result above is from the earlier hardware run.

## Roadmap

| Milestone | Status |
| --- | --- |
| macOS user-space UART loopback | Hardware pass recorded |
| Ring buffer | Standalone exercise complete |
| Integration of the ring buffer with UART reception | Planned |
| Linux kernel module | Planned |
| Linux character device driver | Planned |
| Device Tree and board support package (BSP) | Planned |
