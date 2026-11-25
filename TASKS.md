# Phân chia công việc – Đồ án CO3009

**Hạn nộp báo cáo và code: 04/12/2026** (nên nộp trước 1 ngày, tức 03/12).
Cách dùng: mỗi người đánh dấu `[x]` khi xong việc của mình rồi commit file này cùng code.

---

## 1. Nguyên tắc để 3 người làm độc lập

1. **Mỗi file chỉ có một chủ.** Chỉ chủ file được sửa file đó. Cần đổi gì ở file người khác thì nhắn họ hoặc mở Pull Request.
2. **`global.h` là hợp đồng.** Tên hàm, tham số và giá trị trả về trong `global.h` đã chốt, không tự đổi. Muốn đổi phải báo cả nhóm.
3. **Không chờ nhau.** Phần của người khác chưa xong thì dùng bản tạm (xem mục 3) để vẫn test được phần mình.
4. **Mỗi người một nhánh** (`feature/...`), mở Pull Request vào `dev`, người được phân công review rồi mới merge.
5. **`main.c` chia vùng riêng.** A tạo sẵn trong các khối `USER CODE` ba vùng chú thích `/* ---- A ---- */`, `/* ---- B ---- */`, `/* ---- C ---- */`. Mỗi người chỉ viết vào vùng của mình (khai báo, init, `SCH_Add_Task`), nhờ vậy khi merge không bị xung đột.
6. **Không gọi HAL trong `fsm_*.c` và `scheduler.c`.** HAL chỉ được gọi trong `main.c` và các module phần cứng (`button`, `traffic_light`, `dht20`, `lcd`).

## 2. Ai sở hữu file nào

| File (.h và .c) | Chủ | Vai trò |
|---|---|---|
| `global` | A | Hằng số, enum, biến dùng chung |
| `scheduler` | A | Bộ lập lịch O(1) |
| `software_timer` | A | Timer phần mềm cho các FSM |
| `fsm_automatic` | A | Chế độ tự động |
| `button` | B | Đọc nút, chống nhiễu, nhấn giữ |
| `fsm_manual` | B | Chế độ thủ công |
| `traffic_light` | C | Điều khiển đèn hai đường |
| `dht20` | C | Đọc nhiệt độ, độ ẩm |
| `lcd` | C | Hiển thị LCD qua I2C |
| `fsm_tuning` | C | Chế độ chỉnh thời gian |
| `main.c` | A tạo khung, mỗi người điền vùng riêng | Khởi tạo và đăng ký task |

## 3. Phụ thuộc giữa các phần và cách làm tạm

| Phần của bạn cần | Do ai làm | Làm tạm trong lúc chờ |
|---|---|---|
| Chạy định kỳ (task mỗi 10 ms) | A (scheduler) | Trong ngắt TIM2 đặt `flag_10ms = 1`; trong `while(1)` thấy cờ thì gọi hàm của bạn rồi xóa cờ |
| `setTimer`, `timer_flag[]` | A (software_timer) | Dùng một biến đếm tạm trong file test của bạn |
| `isButtonPressed(i)` | B (button) | Hàm tạm trả về 1 theo ý bạn (ví dụ mỗi 3 giây), hoặc đặt biến rồi sửa bằng debugger |
| `traffic_set(road, color)` | C (traffic_light) | Hàm tạm ghi vào mảng `last_color[road]`, xem bằng **Live Expressions** trong CubeIDE |
| `lcd_print`, `lcd_set_cursor`... | C (lcd) | Hàm rỗng, hoặc ghi chuỗi ra biến toàn cục để xem bằng debugger |

File tạm chỉ dùng để test, **không commit lên `dev`**. Khi phần thật của người kia đã được merge thì xóa file tạm và chạy lại test với bản thật.

## 4. Lịch tổng quan

| Thời gian | Việc | Kết quả cần có |
|---|---|---|
| 18–19/11 | Dựng repo và project nền (A), điền bảng chân (B), kiểm tra chân I2C (C) | Project build được, đã commit lên `dev` |
| 19–21/11 | Test phần cứng, mỗi người một phần | Bảng tổ hợp đèn, mức logic nút, địa chỉ I2C |
| 21–25/11 | Viết module của mình, test độc lập | Mỗi module có demo nhỏ |
| 22–24/11 | Vẽ sơ đồ FSM của mình | Sơ đồ trong `docs/` |
| 25–30/11 | Code FSM của mình, rồi tích hợp | Chạy đủ 3 chế độ |
| 30/11–02/12 | Test hệ thống, viết báo cáo | Danh sách edge case, báo cáo PDF |
| 03–04/12 | Merge, tag, nộp | Đã nộp |

---

# THÀNH VIÊN A – Nhóm trưởng (lõi hệ thống)

**Mục tiêu:** hệ thống chạy được trên scheduler, chế độ tự động hoạt động, repo và bản nộp gọn gàng.
**Nhánh:** `feature/scheduler`, `feature/fsm-auto`

## A.1 Repo và project nền (18–19/11)
- [ ] Tạo repo GitHub, thêm B và C làm collaborator, tạo nhánh `dev`
- [ ] Thêm `.gitignore` (`Debug/`, `Release/`, `*.o`, `*.elf`, `*.map`) và đặt `README.md`, `TASKS.md` vào repo
- [ ] Tạo project từ Board Selector **NUCLEO-F103RB**, đặt **SYS → Debug = Serial Wire**
- [ ] Cấu hình GPIO (theo bảng chân B điền), I2C1, **TIM2 ngắt 10 ms** (tính PSC/ARR theo f_TIM thực tế trong tab Clock Configuration)
- [ ] Trong `main.c` tạo ba vùng chú thích A, B, C như nguyên tắc số 5
- [ ] Test: nháy LED PA5, nạp lại nhiều lần để chắc chắn không bị khóa board
- [ ] Commit project nền lên `dev`, báo B và C kéo về

## A.2 `scheduler.h / scheduler.c` (21–25/11)

**Làm gì:** bộ lập lịch chạy các task định kỳ hoặc một lần, thay cho `HAL_Delay`.

| Hàm | Việc cần làm |
|---|---|
| `SCH_Init()` | Xóa danh sách task |
| `SCH_Add_Task(pFunc, DELAY, PERIOD)` | Chèn task vào danh sách theo thứ tự thời gian chạy, `DELAY` của mỗi task tính **tương đối** so với task đứng trước. `PERIOD = 0` nghĩa là chạy một lần. Trả về `TaskID` |
| `SCH_Delete_Task(taskID)` | Xóa task bất kỳ, cộng phần `Delay` còn lại cho task đứng sau |
| `SCH_Update()` | Gọi trong ngắt TIM2. **Phải O(1)**: chỉ giảm `Delay` của task đầu danh sách, về 0 thì đặt `RunMe = 1`. Không duyệt cả danh sách |
| `SCH_Dispatch_Tasks()` | Gọi trong `while(1)`. Nếu task đầu sẵn sàng: chạy, lấy khỏi danh sách, nếu có `PERIOD` thì chèn lại với `Delay = PERIOD` |

**Lưu ý:**
- Đơn vị `DELAY`, `PERIOD` là **tick** (1 tick = 10 ms).
- Chọn mảng hay danh sách liên kết và ghi lý do vào báo cáo.
- Việc sắp xếp nằm ở `SCH_Add_Task`, không nằm ở `SCH_Update`.

**Test độc lập (chỉ cần board và LED):**
- [ ] Hai LED nháy 0,5 s và 1 s cùng lúc, đúng nhịp
- [ ] Một task one-shot chạy đúng một lần
- [ ] Chèn task ở đầu, giữa, cuối danh sách đều đúng thứ tự
- [ ] Xóa một task đang chờ, các task còn lại vẫn đúng giờ
- [ ] Nhiều task có cùng thời điểm chạy không bị bỏ sót

**Xong khi:** các test trên đều đạt và đã merge vào `dev`.

## A.3 `software_timer.h / software_timer.c` (sau scheduler, trước 26/11)

**Làm gì:** các bộ đếm thời gian cho FSM, đếm theo tick.

| Hàm | Việc cần làm |
|---|---|
| `setTimer(index, duration)` | Đặt timer `index` đếm `duration` tick, xóa cờ |
| `clearTimer(index)` | Dừng và xóa cờ của timer |
| `timer_run()` | Task chạy mỗi tick: giảm các bộ đếm, hết giờ thì bật `timer_flag[index] = 1` |

**Test độc lập:** đặt hai timer 1 s và 3 s, bật LED khi cờ lên, kiểm tra đúng thời gian và có thể đặt lại.
**Xong khi:** `setTimer` được gọi lại nhiều lần vẫn đúng, và timer đã hết giờ không bật cờ lần nữa.

## A.4 `global.h / global.c`
- [ ] Giữ `global.h` đúng với hợp đồng cả nhóm đã chốt
- [ ] Khi nhóm quyết định đổi (ví dụ ràng buộc thời gian, gán nút), chỉ A sửa rồi báo B và C

## A.5 `fsm_automatic.h / fsm_automatic.c` (22–30/11)

**Làm gì:** chế độ tự động, hai đường luân phiên đỏ, xanh, vàng theo `time_red`, `time_green`, `time_amber`.

| Trạng thái | Việc cần làm |
|---|---|
| `AUTO_INIT` | Đặt hai đường về trạng thái an toàn, đặt timer, vẽ màn hình LCD chế độ tự động, chuyển sang trạng thái đầu của chu kỳ |
| `AUTO_RED1_GREEN2` | Đường 1 đỏ, đường 2 xanh. Hết thời gian xanh thì sang `AUTO_RED1_AMBER2` |
| `AUTO_RED1_AMBER2` | Đường 2 vàng. Hết thời gian vàng thì sang `AUTO_GREEN1_RED2` |
| `AUTO_GREEN1_RED2` | Đường 1 xanh, đường 2 đỏ. Hết thời gian thì sang `AUTO_AMBER1_RED2` |
| `AUTO_AMBER1_RED2` | Đường 1 vàng. Hết thời gian thì quay lại `AUTO_RED1_GREEN2` |

**Chuyển chế độ:** nhấn `BTN_MODE` thì đặt `mode = MODE_MANUAL`, `status = MAN_INIT`.
**Quy tắc bắt buộc:**
- Không gọi HAL, chỉ dùng `traffic_set`, `setTimer`, `isButtonPressed`, `lcd_*`.
- Hai đường **không bao giờ cùng xanh**, kể cả lúc vừa chuyển chế độ.
- Đổi `time_*` từ giây sang tick: `MS_TO_TICKS(time_green * 1000)`.

**Test độc lập (dùng bản tạm của `traffic_set`, `isButtonPressed`, `lcd_*`):**
- [ ] Xem `last_color[1]`, `last_color[2]` trong Live Expressions: chu kỳ đúng thứ tự và đúng thời gian
- [ ] Đổi `time_*` bằng debugger, chu kỳ đổi theo
- [ ] Ép `isButtonPressed(BTN_MODE)` trả về 1, `mode` và `status` chuyển đúng

**Xong khi:** chu kỳ chạy liên tục đúng, không có thời điểm hai đường cùng xanh.

## A.6 Tích hợp và nộp bài (30/11–04/12)
- [ ] Gom các task vào `main.c`: `button_read`, `timer_run`, `dht20_task`, ba hàm `fsm_*_run`, hiển thị LCD
- [ ] Review Pull Request của B
- [ ] Viết báo cáo: bìa, giới thiệu, ánh xạ chân (mục 3.1), kiến trúc, scheduler (chọn array hay linked list và lý do), kết luận
- [ ] Merge `dev` vào `main`, tạo tag `v1.0-final`, ghi commit hash vào bìa báo cáo
- [ ] Nộp PDF và link GitHub lên LMS (nộp sớm 1 ngày)

---

# THÀNH VIÊN B – Đầu vào và chế độ thủ công

**Mục tiêu:** đọc nút nhấn ổn định và chế độ thủ công hoạt động.
**Nhánh:** `feature/button`, `feature/fsm-manual`

## B.1 Chuẩn bị (18–21/11)
- [ ] Điền bảng ánh xạ chân D3–D10 từ schematic của board vào `README.md` (A dùng bảng này để cấu hình `.ioc`)
- [ ] Test phần cứng: đọc 4 nút và nút USER, ghi lại **mức logic khi nhấn** (0 hay 1) vào `README.md`

## B.2 `button.h / button.c` (21–25/11)

**Làm gì:** đọc 4 nút, chống nhiễu, phát hiện nhấn thường và nhấn giữ. Được phép gọi HAL (`HAL_GPIO_ReadPin`) trong file này.

| Hàm | Việc cần làm |
|---|---|
| `button_init()` | Xóa các cờ và bộ đếm |
| `button_read()` | Task chạy mỗi 10 ms. Với mỗi nút: đọc chân, chống nhiễu (cần vài lần đọc giống nhau liên tiếp), đặt cờ nhấn, đếm thời gian giữ, đặt cờ nhấn giữ sau khoảng 1 s |
| `isButtonPressed(i)` | Trả về 1 **một lần** nếu nút `i` vừa được nhấn, rồi xóa cờ |
| `isButtonLongPressed(i)` | Trả về 1 một lần nếu nút `i` được giữ đủ lâu, rồi xóa cờ |

**Lưu ý:**
- Dùng mức logic đã đo ở B.1, không đoán.
- Một lần nhấn giữ không được tính thêm thành nhiều lần nhấn thường.
- Đọc nút bằng lần gọi định kỳ, không dùng `HAL_Delay`.

**Test độc lập (chỉ cần board, dùng cờ 10 ms tạm):**
- [ ] Nhấn nhanh một lần, LED đổi trạng thái đúng một lần
- [ ] Giữ nút, nhận được cờ nhấn giữ và không bị lặp liên tục
- [ ] Nhấn dồn dập, không đếm sai
- [ ] Cả 4 nút hoạt động độc lập nhau

**Xong khi:** các test trên đạt và đã merge vào `dev`.

## B.3 `fsm_manual.h / fsm_manual.c` (22–30/11)

**Làm gì:** chế độ thủ công, người dùng tự đổi trạng thái đèn bằng nút.

| Trạng thái | Việc cần làm |
|---|---|
| `MAN_INIT` | Đặt hai đường về trạng thái an toàn, vẽ màn hình LCD chế độ thủ công, sang trạng thái đầu |
| `MAN_RED1_GREEN2` … `MAN_AMBER1_RED2` | Giữ nguyên đèn của trạng thái đó. Nhấn `BTN_NEXT` thì sang trạng thái kế tiếp theo cùng thứ tự với chế độ tự động |

**Chuyển chế độ:** nhấn `BTN_MODE` thì đặt `mode = MODE_TUNING`, `status = TUNE_INIT`.
**Việc cần quyết định và ghi vào báo cáo:**
- Chuyển chế độ khi đèn đang vàng thì đèn kế tiếp là gì?
- Có cần ép về trạng thái an toàn trước khi sang trạng thái kế tiếp không?

**Test độc lập (dùng bản tạm của `traffic_set`, `lcd_*`):**
- [ ] Nhấn `BTN_NEXT` liên tục, `last_color` đi đúng chu kỳ
- [ ] Không có tổ hợp nào hai đường cùng xanh
- [ ] Nhấn `BTN_MODE`, `mode` và `status` chuyển đúng

**Xong khi:** đi hết vòng trạng thái đúng và không gọi HAL trong file này.

## B.4 Cuối kỳ (30/11–04/12)
- [ ] Review Pull Request của C
- [ ] Test edge case: nhấn nút liên tục, chuyển chế độ khi đèn đang vàng, nhấn nhiều nút cùng lúc
- [ ] Viết báo cáo: module button, FSM manual, kịch bản test
- [ ] Cập nhật `README.md`: bảng nút nhấn, cách vận hành thực tế

---

# THÀNH VIÊN C – Ngoại vi và chế độ chỉnh thời gian

**Mục tiêu:** đèn, cảm biến, LCD hoạt động và chế độ chỉnh thời gian đúng ràng buộc.
**Nhánh:** `feature/traffic`, `feature/lcd-dht20`, `feature/fsm-tuning`

## C.1 Chuẩn bị (18–21/11)
- [ ] Xác nhận cặp chân SCL/SDA của slot I2C trên schematic, báo A để cấu hình I2C1
- [ ] Test đèn: thử 4 tổ hợp tín hiệu (0,0) (0,1) (1,0) (1,1) cho cả hai module đèn, điền bảng vào `README.md`
- [ ] I2C scan: ghi lại địa chỉ DHT20 và LCD (nhớ dịch trái 1 bit: `addr << 1` khi gọi HAL)

## C.2 `traffic_light.h / traffic_light.c` (21–23/11)

**Làm gì:** điều khiển đèn của hai đường theo màu. Được phép gọi HAL (`HAL_GPIO_WritePin`).

| Hàm | Việc cần làm |
|---|---|
| `traffic_init()` | Tắt cả hai đường |
| `traffic_set(road, color)` | `road` là 1 hoặc 2; `color` là `LIGHT_OFF`, `LIGHT_RED`, `LIGHT_GREEN`, `LIGHT_AMBER`. Đổi màu thành cặp tín hiệu (A, B) theo bảng đã đo ở C.1 rồi ghi ra chân tương ứng |

**Lưu ý:** dùng đúng bảng thực đo, không chép bảng mẫu.
**Test độc lập:** gọi `traffic_set` lần lượt cả 4 màu cho từng đường, màu hiển thị đúng; đường 1 và đường 2 không ảnh hưởng nhau.
**Xong khi:** các test trên đạt. Hãy merge sớm vì A và B dùng hàm này khi tích hợp.

## C.3 `lcd.h / lcd.c` (21–25/11)

**Làm gì:** điều khiển LCD qua I2C. Được phép gọi HAL (`HAL_I2C_Master_Transmit`).

| Hàm | Việc cần làm |
|---|---|
| `lcd_init()` | Khởi tạo LCD theo trình tự của chip điều khiển |
| `lcd_clear()` | Xóa màn hình |
| `lcd_set_cursor(row, col)` | Đặt vị trí con trỏ |
| `lcd_print(str)` | In chuỗi |
| `lcd_print_int(value)` | In số nguyên |

**Test độc lập:** in một chuỗi, một số, in ở các vị trí khác nhau, xóa rồi in lại.
**Xong khi:** hiển thị ổn định, không lỗi ký tự, không làm treo chương trình khi LCD chưa cắm.

## C.4 `dht20.h / dht20.c` (21–25/11)

**Làm gì:** đọc nhiệt độ và độ ẩm từ cảm biến DHT20 qua I2C. Được phép gọi HAL.

| Hàm | Việc cần làm |
|---|---|
| `dht20_init()` | Khởi tạo cảm biến theo datasheet |
| `dht20_read()` | Gửi lệnh đo, chờ, đọc dữ liệu, kiểm tra CRC, ghi kết quả vào `dht20_temperature` và `dht20_humidity`. Trả về 1 nếu thành công |
| `dht20_task()` | Task chạy định kỳ khoảng 2 giây, gọi `dht20_read()` |

**Lưu ý:** không đọc liên tục, và không dùng `HAL_Delay` trong task.
**Test độc lập:** mỗi 2 giây đọc và in nhiệt độ, độ ẩm lên LCD; đặt tay lên cảm biến thì giá trị đổi.
**Xong khi:** giá trị hợp lý và ổn định, rút cảm biến thì chương trình không treo.

## C.5 `fsm_tuning.h / fsm_tuning.c` (22–30/11)

**Làm gì:** chế độ chỉnh thời gian đèn đỏ, xanh, vàng, hiển thị trên LCD.

| Trạng thái | Việc cần làm |
|---|---|
| `TUNE_INIT` | Đặt hai đường về trạng thái an toàn, hiển thị giá trị đầu tiên trên LCD, sang `TUNE_RED` |
| `TUNE_RED` | Chỉnh thời gian đèn đỏ. `BTN_INC` tăng giá trị, `BTN_CONFIRM` xác nhận, `BTN_NEXT` sang `TUNE_GREEN` |
| `TUNE_GREEN` | Tương tự cho đèn xanh, `BTN_NEXT` sang `TUNE_AMBER` |
| `TUNE_AMBER` | Tương tự cho đèn vàng, `BTN_NEXT` về `TUNE_RED` |

**Chuyển chế độ:** nhấn `BTN_MODE` thì đặt `mode = MODE_AUTO`, `status = AUTO_INIT`.
**Việc cần quyết định và ghi vào báo cáo:**
- Ràng buộc thời gian: ví dụ `Red = Green + Amber`, giá trị trong khoảng `MIN_TIME`..`MAX_TIME`.
- Khi giá trị không hợp lệ thì xử lý thế nào (giữ giá trị cũ, hiện lỗi trên LCD, hay tự điều chỉnh)?
- Chỉ ghi vào `time_red`, `time_green`, `time_amber` khi đã xác nhận và hợp lệ.

**Test độc lập (dùng bản tạm của `isButtonPressed`):**
- [ ] Ép từng nút trả về 1, giá trị trên LCD đổi đúng
- [ ] Nhập giá trị sai (quá lớn, quá nhỏ, vi phạm ràng buộc), hệ thống xử lý đúng như đã quyết định
- [ ] Giá trị đã xác nhận thật sự đổi `time_*`; giá trị chưa xác nhận thì không
- [ ] Nhấn `BTN_MODE`, `mode` và `status` chuyển đúng

**Xong khi:** các test trên đạt, không gọi HAL trong file này.

## C.6 Cuối kỳ (30/11–04/12)
- [ ] Review Pull Request của A
- [ ] Test edge case: nhập thời gian không hợp lệ, rút LCD hoặc cảm biến khi đang chạy
- [ ] Viết báo cáo: phần cứng, bảng tổ hợp đèn, DHT20, LCD, FSM tuning, ràng buộc thời gian, kết quả test
- [ ] Chuẩn bị kịch bản demo

---

# TÍCH HỢP (cả nhóm, 25–30/11)

Chỉ làm khi từng người đã merge phần của mình vào `dev`.

| Thứ tự | Việc | Ai làm |
|---|---|---|
| 1 | Xóa toàn bộ file và hàm tạm, build lại | Mỗi người, phần của mình |
| 2 | Đăng ký task vào `main.c` (vùng riêng mỗi người) | Mỗi người |
| 3 | Chạy chế độ tự động đủ chu kỳ | A |
| 4 | Chuyển Auto → Manual → Tuning → Auto, kiểm tra đèn, timer, LCD sau mỗi lần chuyển | Cả nhóm |
| 5 | Chạy liên tục để kiểm tra độ ổn định | Cả nhóm |

Gợi ý bảng task trong `main.c` (đơn vị tick, 1 tick = 10 ms):

| Task | Chu kỳ | Vùng |
|---|---|---|
| `button_read` | 1 | B |
| `timer_run` | 1 | A |
| `fsm_automatic_run`, `fsm_manual_run`, `fsm_tuning_run` | 1 | A, B, C |
| `dht20_task` | 200 (2 s) | C |
| Cập nhật LCD | theo thiết kế | C |

**Câu hỏi cần trả lời được ở mỗi lần chuyển chế độ** (phần chiếm 30% điểm): đèn hai đường ở trạng thái nào ngay sau chuyển? Timer và task được khởi tạo lại thế nào? LCD hiển thị gì? Hai đường có bao giờ cùng xanh không?

---

## Checklist chất lượng trước khi nộp

- [ ] `SCH_Update` là O(1) và hỗ trợ one-shot
- [ ] Không còn `HAL_Delay` trong chương trình chính
- [ ] Không có lệnh HAL trong `fsm_*.c` và `scheduler.c`
- [ ] Hai đường không bao giờ cùng xanh ở mọi lần chuyển trạng thái và chuyển chế độ
- [ ] Mỗi thành viên có commit bằng tài khoản của mình
- [ ] Hệ thống chạy liên tục ổn định
- [ ] `README.md` cập nhật cách build, nạp và vận hành, đã điền hết các chỗ `[...]`
- [ ] Báo cáo đã xóa các đoạn hướng dẫn màu xám, xuất PDF đúng tên file
- [ ] Repo public hoặc đã thêm giảng viên làm collaborator