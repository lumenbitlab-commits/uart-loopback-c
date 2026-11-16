# 為什麼 UART 的 `read()` 可能一次收不完 `HELLO`？

2026-11-16 · C、UART、macOS

先前在 macOS 用 CP2102 做 UART Loopback，程式成功送出並收回 `HELLO` 五個位元組。如果回傳資料先到 `HE`，過一會兒才到 `LLO`，程式要怎麼知道還得繼續讀？

## 讀取上限與實際回傳值

`read(fd, buf, 5)` 的 `5` 是這次最多能讀取的位元組數。成功讀取時，回傳值表示實際放進緩衝區的位元組數。本專題設定 `VMIN = 0`、`VTIME = 10`，有資料可讀就可能提早回傳，沒有資料時這次呼叫最多等待約一秒。 [Apple macOS `read(2)` 手冊](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/read.2.html)

程式用 `total` 記已收到多少位元組。下面這行把新資料放到 `received + total`，並把這次的讀取上限設為還缺的長度。

```c
ssize_t bytes_read = read(fd, received + total, length - total);
```

如果第一次收到 `HE`，`total` 就加到 `2`。下一次從索引 `2` 開始寫入，最多再收 `3` 個位元組。收到預期長度後，以 `memcmp()` 比對完整內容。等待逾時時，程式會停止累積並依已收到的內容判定結果。 [UART 原始碼](../uart_client.c)

## 用虛擬串列埠驗證

在 macOS 建立一對虛擬終端（PTY），讓測試程式扮演回傳端。分段案例先送 `HE`，等加上記錄輸出的測試版讀到 `2`，才送 `LLO`。這樣可以直接觀察每次 `read()` 的回傳值。

| 回傳內容 | `read()` 回傳值 | 程式結果 |
| --- | --- | --- |
| 先送 `HE`，再送 `LLO` | `2`、`3` | `PASS` |
| `HEXXO` | `5` | `FAIL`，內容不符 |
| 只送 `HEL` | `3`、`0` | `FAIL`，等不到剩餘資料 |
| 持續沒有輸入 | `0` | `FAIL`，等待逾時 |

上表來自目前 C 程式的 macOS 虛擬串列埠測試。[專題 README](../README.md)另有先前版本在 CP2102 上的實機回傳紀錄。下一步是用最新版重跑實機測試，並把 UART 接收接上 Ring Buffer，檢查回繞與緩衝區滿時的行為。

[回技術部落格](README.md)
