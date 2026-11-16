#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd;
    struct termios tty;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <serial-device>\n", argv[0]);
        return 1;
    }

    // Step 1：開啟 CP2102
    fd = open(argv[1], O_RDWR | O_NOCTTY);

    // 檢查 open() 是否成功
    if (fd == -1)
    {
        perror("UART open failed");
        return 1;
    }

    printf("UART opened successfully\n");

    // Step 2：讀取目前 UART 設定
    if (tcgetattr(fd, &tty) == -1)
    {
        perror("tcgetattr failed");
        close(fd);
        return 1;
    }

    printf("UART settings read successfully\n");

    // 設定 Raw Mode，避免終端機處理 UART 資料
    cfmakeraw(&tty);

    // Step 3：設定接收速度為 115200 baud
    if (cfsetispeed(&tty, B115200) == -1)
    {
        perror("cfsetispeed failed");
        close(fd);
        return 1;
    }

    // Step 4：設定傳送速度為 115200 baud
    if (cfsetospeed(&tty, B115200) == -1)
    {
        perror("cfsetospeed failed");
        close(fd);
        return 1;
    }

    printf("UART speed values set successfully\n");

    // 設定 UART 8N1

    // 8：設定 8 個資料位元
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    // N：關閉同位檢查
    tty.c_cflag &= ~PARENB;

    // 1：設定 1 個停止位元
    tty.c_cflag &= ~CSTOPB;

    // 啟用 UART 接收功能，忽略 Modem 控制線狀態
    tty.c_cflag |= (CREAD | CLOCAL);

    // 關閉 RTS/CTS 硬體流量控制 RTS：Request To Send / CTS：Clear To Send
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // 設定 read() 最多等待約 1 秒
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;

    // Step 5：將 UART 設定套用到 CP2102
    if (tcsetattr(fd, TCSANOW, &tty) == -1)
    {
        perror("tcsetattr failed");
        close(fd);
        return 1;
    }

    printf("UART speed applied successfully\n");

    // Step 6：傳送 HELLO，處理 Partial Write
    const char message[] = "HELLO";

    const size_t length = sizeof(message) - 1;
    size_t total_written = 0;

    while (total_written < length)
    {
        ssize_t result = write(
            fd,
            message + total_written,
            length - total_written
        );

        if (result == -1)
        {
            perror("UART write failed");
            close(fd);
            return 1;
        }

        if (result == 0)
        {
            fprintf(stderr, "UART write made no progress\n");
            close(fd);
            return 1;
        }

        total_written += (size_t)result;
    }

    printf("Total written = %zu Bytes\n", total_written);

    // Step 7：接收 HELLO
    char received[sizeof(message)] = {0};

    size_t total = 0;

    while (total < length)
    {
        ssize_t bytes_read = read(fd, received + total, length - total);

        if (bytes_read == -1)
        {
            perror("UART read failed");
            close(fd);
            return 1;
        }

        if (bytes_read == 0)
        {
            printf("UART read timeout\n");
            break;
        }

        total += (size_t)bytes_read;
    }

    printf("Received %zu Bytes\n", total);
    printf("Received = %s\n", received);

    // Step 8：檢查傳送與接收的資料是否相同
    int passed = total == length && memcmp(message, received, length) == 0;
    printf("UART Loopback %s\n", passed ? "PASS" : "FAIL");
    close(fd);

    return passed ? 0 : 1;
}
