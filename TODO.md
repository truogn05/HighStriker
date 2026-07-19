# TODO — Phần cứng & Code (Load Cell + HX711 → dữ liệu điểm cho TouchGFX)

> Phạm vi: chỉ lắp mạch + firmware đọc lực, tính % và mức LED bar (0-100%, 11 mức). **Không đụng vào GUI** — chỉ chuẩn bị sẵn dữ liệu (interface) để agent khác đọc và hiển thị lên TouchGFX.

---

## 0. Ghi chú tham khảo từ `High_Striker_Lavie`

Repo gốc dùng: **SCK → PD12**, **DOUT → PB12** (xem `HX711_Init(GPIOD, GPIO_PIN_12, GPIOB, GPIO_PIN_12)` trong `main.c`). Comment trong `HX711.c` ghi PB12/PB14 là **sai/lệch so với code thật** — đừng tin theo comment, tin theo lời gọi hàm thực tế. Board mục tiêu của bạn cũng là STM32F429ZIT6 nên có thể giữ nguyên 2 chân này nếu chưa bị LCD/TouchGFX chiếm dụng — **cần xác nhận lại bằng CubeMX Pinout trước khi hàn/breadboard**.

---

## 1. Lắp mạch (Hardware)

### 1.1 Danh sách linh kiện
- [ ] Board STM32F429ZIT6
- [ ] Module HX711 (breakout board)
- [ ] Load cell (xác định tải trọng tối đa phù hợp lực đập tay, ví dụ 50kg/100kg tùy loại)
- [ ] Dây nối, breadboard hoặc mạch in thử nghiệm
- [ ] Nguồn 5V ổn định cho HX711 (không dùng chung đường nguồn nhiễu với motor/LED công suất lớn nếu có)

### 1.2 Sơ đồ đấu nối

**Load cell → HX711** (theo màu dây chuẩn load cell 4 dây):
| Load cell | HX711 |
|---|---|
| Đỏ (E+) | E+ |
| Đen (E-) | E- |
| Trắng (A-) | A- |
| Xanh lá (A+) | A+ |

**HX711 → STM32F429ZIT6**:
| HX711 | STM32 (đề xuất, xác nhận lại trên CubeMX) |
|---|---|
| VCC | 5V (hoặc 3.3V tùy module — kiểm tra datasheet module cụ thể) |
| GND | GND |
| SCK | PD12 (Output) |
| DOUT | PB12 (Input, no pull) |
| RATE (nếu có chân này) | Kéo lên HIGH nếu muốn 80 SPS thay vì 10 SPS mặc định |

### 1.3 Checklist lắp mạch
- [ ] Mở CubeMX, kiểm tra PD12/PB12 có đang bị LCD (TouchGFX/FMC/SPI) hoặc LED bar chiếm dụng không → nếu trùng, đổi sang cặp chân GPIO trống khác và cập nhật lại bảng trên
- [ ] Đấu load cell → HX711 đúng màu dây (đo bằng đồng hồ VOM nếu load cell không ghi màu rõ)
- [ ] Đấu HX711 → STM32, cấp nguồn, đo điện áp VCC/GND bằng VOM trước khi cấp cho STM32 (tránh chết module do đấu ngược)
- [ ] Kiểm tra chân RATE (10 SPS vs 80 SPS) — nên chọn **80 SPS** vì cần bắt đỉnh lực va đập nhanh (đã ghi trong rủi ro ở kế hoạch tổng)
- [ ] Cố định cơ khí load cell chắc chắn (nếu load cell bị xê dịch, số liệu sẽ sai/nhiễu nặng)

---

## 2. Firmware — Driver HX711

### 2.1 Việc cần làm
- [ ] Port lại `HX711.c/.h` (có thể giữ gần như nguyên bản logic bit-banging từ repo tham khảo — logic đọc 24-bit, sign-extend, xung gain đã đúng chuẩn HX711)
- [ ] Đổi `delay_us(1)` sang hàm delay chính xác (dùng DWT cycle counter thay vì delay loop thô, để timing SCK ổn định trên STM32F429 xung nhịp cao — bản gốc dùng delay loop có thể không chính xác)
- [ ] Thêm timeout an toàn khi chờ DOUT xuống LOW (đã có trong bản gốc, giữ lại — nhưng xử lý rõ ràng giá trị lỗi `0xFFFFFFFF` ở lớp gọi, không để lọt vào tính điểm)
- [ ] Test độc lập: đọc raw data qua UART (dùng `snprintf`, KHÔNG dùng `sprintf(buf, sizeof(buf), ...)` như bug đã phát hiện ở bản gốc)

---

## 3. Firmware — Lớp xử lý lực (`load_cell.c/.h`)

### 3.1 Hiệu chỉnh (calibration)
- [ ] **Tare/offset:** lúc khởi động, lấy trung bình N mẫu (ví dụ N=10) khi không có lực tác động → lưu làm `offset`
- [ ] **Scale factor:** đặt một vật có khối lượng đã biết lên load cell, đo raw, tính `scale = (raw - offset) / khoi_luong_biet`. Ghi giá trị này cố định vào code (hoặc lưu Flash để không phải hiệu chỉnh lại mỗi lần nạp)
- [ ] Công thức quy đổi: `force = (raw_average - offset) / scale`

### 3.2 Lọc nhiễu & bắt đỉnh lực
- [ ] Lấy trung bình trượt (moving average) nhỏ (3-5 mẫu) để giảm nhiễu tức thời, nhưng **không lọc quá mạnh** vì sẽ làm mất đỉnh lực thật (lực va đập chỉ tồn tại vài trăm ms)
- [ ] Thuật toán bắt đỉnh (peak detection):
  1. Khi `force` vượt `TRIGGER_THRESHOLD` → vào trạng thái đang đo, liên tục cập nhật `peak_force = max(peak_force, force)`
  2. Kết thúc đo khi `force` giảm xuống dưới `TRIGGER_THRESHOLD` **hoặc** quá `PEAK_TIMEOUT_MS` (ví dụ 500-800ms) kể từ lúc bắt đầu
  3. `peak_force` thu được là giá trị dùng để tính % và mức LED bar

### 3.3 Định nghĩa hằng số cần hiệu chỉnh thực tế
```c
#define FORCE_TRIGGER_THRESHOLD   ...   // lực tối thiểu để tính là "một cú đập" (kg hoặc đơn vị lực đã chọn)
#define FORCE_MAX_THRESHOLD       ...   // lực từ mức này trở lên = 100%
#define PEAK_TIMEOUT_MS           600   // thời gian tối đa gom mẫu để tìm đỉnh
```
Hai giá trị `FORCE_TRIGGER_THRESHOLD` và `FORCE_MAX_THRESHOLD` **phải đo thực nghiệm** bằng cách cho vài người đập thử và ghi log raw force qua UART, không đoán suông.

---

## 4. Quy đổi ra % và 11 mức LED bar (logic High Striker chuẩn)

**Quy tắc:** lực đạt `FORCE_MAX_THRESHOLD` trở lên → 100%. Dưới ngưỡng đó → tính theo tỉ lệ, **làm tròn lên**. Có 11 mức hiển thị: 0%, 10%, 20%, ..., 100% (mức 0 → 10).

```c
uint8_t force_to_percent(int32_t peak_force)
{
    if (peak_force <= 0) return 0;
    if (peak_force >= FORCE_MAX_THRESHOLD) return 100;

    // làm tròn lên (ceil) bằng phép chia nguyên
    uint32_t percent = ((uint32_t)peak_force * 100 + FORCE_MAX_THRESHOLD - 1) / FORCE_MAX_THRESHOLD;
    return (uint8_t)percent; // 1..99
}

uint8_t percent_to_level(uint8_t percent) // trả về 0..10 (11 mức)
{
    uint8_t level = (percent + 9) / 10; // ceil(percent / 10)
    if (level > 10) level = 10;
    return level;
}
```

- [ ] Đo thực nghiệm để chọn `FORCE_MAX_THRESHOLD` sao cho một cú đập "rất mạnh" mới chạm 100% (tránh ai đập cũng full ngay, mất vui)
- [ ] Test bảng giá trị biên: peak_force = 0 → level 0; peak_force = FORCE_MAX_THRESHOLD-1 → level phải làm tròn lên đúng (ví dụ 91% → level 10, không phải level 9)
- [ ] Viết unit test nhỏ (test trên PC hoặc log qua UART) cho 2 hàm trên trước khi tích hợp phần cứng thật, vì đây là phần logic dễ sai lệch 1 đơn vị (off-by-one)

---

## 5. Interface bàn giao cho agent GUI (TouchGFX)

Vì giao diện đã có sẵn, chỉ cần expose dữ liệu ở dạng đơn giản, ổn định để agent đọc và bind vào `Model::tick()`. Đề xuất:

```c
// score_interface.h — file duy nhất mà bên GUI cần include

typedef struct {
    uint8_t  level;        // 0-10 (11 mức LED bar, dùng để tô sáng bao nhiêu vạch)
    uint8_t  percent;      // 0-100, để hiển thị số % nếu UI cần
    uint8_t  isNewPeak;    // 1 = vừa có một cú đập mới chốt xong (dùng để GUI trigger hiệu ứng), tự reset về 0 sau khi GUI đọc
    uint16_t highScorePercent; // % của highScore đã lưu, để hiển thị "Best"
} ScoreDisplayData_t;

extern volatile ScoreDisplayData_t g_scoreDisplay;

// Hàm để bên GUI gọi trong Model::tick(), trả bản sao an toàn (tránh đọc dữ liệu đang bị ghi dở)
void Score_GetDisplayData(ScoreDisplayData_t *out);
```

- [ ] Viết `Score_GetDisplayData()` có bảo vệ bằng `taskENTER_CRITICAL()`/mutex khi copy struct (tránh race condition giữa `GameLogicTask` ghi và `GUI_Task` đọc)
- [ ] `GameLogicTask` là nơi duy nhất ghi vào `g_scoreDisplay`, không để `SensorTask` hay ISR ghi trực tiếp
- [ ] Ghi rõ trong file header: đơn vị, khoảng giá trị, ý nghĩa từng field — để agent GUI không cần đọc thêm code khác
- [ ] Sau khi có interface này chạy ổn định (log qua UART thấy `level`/`percent` đúng theo lực tác động thật), mới bàn giao cho bước tích hợp TouchGFX

---

## 6. Thứ tự thực hiện đề xuất

1. Lắp mạch (mục 1) → đo được raw HX711 qua UART, ổn định
2. Hiệu chỉnh offset/scale (mục 3.1) → raw → lực có đơn vị thật, đúng
3. Peak detection (mục 3.2) → bắt đúng đỉnh lực một cú đập
4. Hàm quy đổi % và level (mục 4) → test kỹ biên off-by-one
5. Dựng `ScoreDisplayData_t` + `Score_GetDisplayData()` (mục 5), log ra UART để tự kiểm tra bằng mắt trước khi giao cho agent GUI
6. Bàn giao: gửi agent GUI file `score_interface.h` + mô tả ngắn cách gọi hàm trong `Model::tick()`
