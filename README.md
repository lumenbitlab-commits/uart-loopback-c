# UART Loopback in C

**November 2026 · Hardware loopback test passed**

The first milestone in my Embedded Linux Driver and BSP learning project is a UART loopback test. I used C and POSIX `termios` on macOS to configure a serial port and check that the bytes sent through a USB-to-UART adapter were returned correctly.

## Test setup

| Item | Setting |
| --- | --- |
| Host | macOS |
| Serial interface | POSIX `termios` in user space |
| Baud rate | 115200 |
| Frame format | 8 data bits, no parity, 1 stop bit (8N1) |
| Loopback | Connect TX to RX on the same USB-to-UART adapter |
| Test payload | `HELLO` (5 bytes) |

## Recorded result

```text
Write result = 5
Received 5 Bytes
Received = HELLO
UART Loopback PASS
```

This run verified that the five bytes sent matched the five bytes read back in the macOS user-space loopback setup.

## Test procedure

1. Connect TX to RX on a USB-to-UART adapter and plug the adapter into the Mac.
2. Identify the adapter’s serial device and select it in the test program.
3. Set the serial port to 115200 baud, 8N1, then run the loopback with `HELLO`.
4. Check that exactly five bytes are received and that they match the transmitted bytes.

This repository documents the test setup and recorded output. The C source and its build command can be added once the original test file is located.

## Next milestone

Integrate the standalone ring buffer with UART reads and add tests for wraparound and buffer overflow. Linux character device driver and BSP work are planned for later milestones.
