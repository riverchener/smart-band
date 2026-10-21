# Smart Band · 基于 STM32F103 的多功能智能手环

> 一个跑在 STM32F103C8T6 上的手表式小设备：128×64 OLED 图形界面、RTC 走时、秒表、手电筒、姿态仪、重力小游戏与动态表情。
> A hand‑held "smart band" firmware for STM32F103C8T6 — SSD1306 GUI, RTC clock, stopwatch, flashlight, IMU attitude/level, and a Chrome‑Dino game.

![MCU](https://img.shields.io/badge/MCU-STM32F103C8T6-blue)
![HAL](https://img.shields.io/badge/Driver-STM32_HAL-03234B)
![Build](https://img.shields.io/badge/Build-CMake_%2B_Ninja-green)
![Lang](https://img.shields.io/badge/Language-C11-orange)

---

## 目录

- [功能特性](#功能特性)
- [硬件组成](#硬件组成)
- [引脚分配](#引脚分配)
- [软件架构](#软件架构)
- [目录结构](#目录结构)
- [编译与环境](#编译与环境)
- [使用说明](#使用说明)
- [实现要点](#实现要点)
- [待办与改进方向](#待办与改进方向)
- [致谢与许可](#致谢与许可)

---

## 功能特性

| 功能 | 说明 |
| --- | --- |
| 🕐 **数字时钟** | 大字号 `时:分:秒` + `年/月/日`，时间由片上 RTC（LSE 32.768 kHz）维持，掉电由纽扣电池续走 |
| 🔋 **电量显示** | 每帧对电池分压采样 3000 次求均值，换算成 0–100% 并绘制电池图标 |
| ⚙️ **日期时间设置** | 年 / 月 / 日 / 时 / 分 / 秒逐项增减，直接写回 RTC 计数器 |
| ⏱️ **秒表** | 开始 / 停止 / 清零，支持 `时:分:秒` 累计 |
| 🔦 **手电筒** | 板载 LED 常亮 / 熄灭切换，当作应急小灯 |
| 📐 **姿态仪 (MPU6050)** | 读取加速度计 + 陀螺仪，一阶互补滤波（α = 0.9）解算 Roll / Pitch / Yaw |
| 🫧 **水平仪** | 把解算出的倾角映射成圆内气泡位置，直观显示是否放平 |
| 🦖 **谷歌小恐龙** | 无限跑酷：地面滚动、云朵漂移、三类随机障碍、正弦跳跃曲线与碰撞检测，实时计分 |
| 🙂 **动态表情** | 眨眼 / 睁眼逐帧动画，练习图形绘制与帧刷新 |
| 🎞️ **滑动菜单** | 图标横向滑动动画 + 选中项滑入过渡，界面切换有连贯动效 |

## 硬件组成

| 器件 | 型号 / 规格 | 接口 |
| --- | --- | --- |
| 主控 | STM32F103C8T6（Cortex‑M3，72 MHz，64 KB Flash / 20 KB RAM） | — |
| 显示屏 | SSD1306 OLED，128×64，I²C | 硬件 I2C2 |
| 六轴传感器 | MPU6050 | 软件 I²C（GPIO 模拟） |
| 按键 | 3 个轻触按键，内部上拉，低电平有效 | GPIO 输入 |
| 指示灯 | 板载 LED，低电平点亮 | GPIO 输出 |
| 电池检测 | 电阻分压后接 ADC | ADC1_IN0 |
| 时钟源 | 外部 32.768 kHz 晶振（LSE） | RTC |

> 主频由 HSE × 9 PLL 倍频到 72 MHz，ADC 采用 PCLK2/6 = 12 MHz，RTC 使用 LSE 自动 1 Hz 预分频。

## 引脚分配

| 引脚 | 功能 | 方向 / 模式 | 备注 |
| --- | --- | --- | --- |
| `PB1` | KEY1 | 输入上拉 | 上一项 / 加一 |
| `PA6` | KEY2 | 输入上拉 | 下一项 / 减一 |
| `PA4` | KEY3 | 输入上拉 | 确认 / 返回 |
| `PB15` | LED | 推挽输出 | 低电平点亮 |
| `PB8` | I2C1_SCL | 推挽输出 | 软件 I²C，驱动 MPU6050 |
| `PB9` | I2C1_SDA | 推挽输出 | 软件 I²C，驱动 MPU6050 |
| `PB10` | I2C2_SCL | 复用开漏 | 硬件 I²C，驱动 OLED（速率 400 kHz） |
| `PB11` | I2C2_SDA | 复用开漏 | 硬件 I²C，驱动 OLED |
| `PA0` | ADC1_IN0 | 模拟输入 | 电池分压采样 |
| `PA13/PA14` | SWDIO / SWCLK | 调试 | 下载与调试 |

I²C 从机地址：OLED `0x78`，MPU6050 `0xD0`。

## 软件架构

整体为**前后台结构**：一个 1 ms 硬件定时中断做时基，主循环负责界面逻辑与绘制。

```
                 ┌──────────────────────────────┐
                 │  TIM2 中断 @ 1 kHz           │
   time base ───►│  Key_Tick()      20 ms 消抖  │
                 │  StopWatch_Tick() 1 s 进位   │
                 │  Dino_Tick()     动画/计分   │
                 └──────────────┬───────────────┘
                                │ 共享标志位
                 ┌──────────────▼───────────────┐
                 │  main() 主循环（页面状态机） │
                 │  First_Page_Clock()          │
                 │   ├─ 确认"菜单" → Menu()     │
                 │   └─ 确认"设置" → SettingPage │
                 └──────────────┬───────────────┘
                                │
      ┌──────────┬──────────┬───┴─────┬──────────┬──────────┐
   StopWatch    LED     MPU6050     Game      Emoji    Gradienter
```

**关键设计**

- **非阻塞按键**：`Key_Tick()` 在中断里以 20 ms 周期采样引脚，检测**释放沿**后写入 `Key_Num`，主循环用 `Key_GetNum()` 读取并自动清零，天然实现"一次按下只触发一次"。
- **全局帧缓冲**：`OLED_NewFrame()` 清空显存 → 各 `OLED_DrawXXX()` / `OLED_PrintXXX()` 只写 GRAM → `OLED_ShowFrame()` 一次性把整屏刷到面板。避免逐元素写屏造成的撕裂与卡顿。
- **页面函数返回码**：每个页面函数（`Menu` / `StopWatch` / `LED` / `Game`…）内部是 `while(1)` 局部状态机，返回 `0` 表示"回到上一级"，主循环据此重新进入首页。
- **菜单滑动动画**：`pre_selection` / `target_selection` 双向逼近，每帧按 `x_step ± Speed` 步进，`x_step` 归位即完成一次切换；选定后 `MenuToFunction()` 做纵向滑出过渡。
- **中断不碰屏幕**：中断只更新计数与标志位，绘制留在主循环，避免长耗时 I²C 传输阻塞时基。

## 目录结构

```
smart_band/
├── CMakeLists.txt              # 顶层构建脚本（用户源码在此登记）
├── CMakePresets.json           # Debug / Release 预设，Ninja + arm-none-eabi
├── smart_band.ioc              # STM32CubeMX 工程配置
├── startup_stm32f103xb.s       # 启动文件（向量表 + Reset_Handler）
├── STM32F103XX_FLASH.ld        # 链接脚本（64 KB Flash / 20 KB RAM）
├── cmake/
│   ├── gcc-arm-none-eabi.cmake # 交叉编译工具链文件
│   ├── starm-clang.cmake       # Clang 工具链备选
│   └── stm32cubemx/            # CubeMX 生成的源文件清单
├── Core/
│   ├── Inc/                    # 全部头文件
│   └── Src/
│       ├── main.c              # 主循环、时钟配置、TIM2 回调
│       ├── gpio.c  tim.c  adc.c  i2c.c  rtc.c      # 外设初始化（CubeMX 生成）
│       ├── oled.c              # SSD1306 驱动 + 完整绘图原语
│       ├── font.c              # ASCII 字体、16×16 中文字库、位图素材
│       ├── menu.c              # 首页 / 设置 / 菜单 / 秒表 / 手电筒 / 姿态 / 游戏入口
│       ├── h_rtc.c             # 基于 Unix 计数器的 RTC 封装
│       ├── settime.c           # 日期时间逐项设置界面
│       ├── mpu6050.c / hi2c.c  # MPU6050 寄存器读写 + 软件 I²C 时序
│       ├── dino.c              # 小恐龙游戏逻辑
│       ├── key.c  led.c  delay.c  format.c         # 按键、LED、微秒延时、格式化
│       └── stm32f1xx_it.c  stm32f1xx_hal_msp.c  sysmem.c  syscalls.c
└── Drivers/                    # STM32F1 HAL + CMSIS（官方原版）
```

## 编译与环境

### 依赖

- [CMake](https://cmake.org/) ≥ 3.22
- [Ninja](https://ninja-build.org/)
- [GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm)（`arm-none-eabi-gcc`，需在 `PATH` 中）
- STM32CubeProgrammer 或 OpenOCD（下载用，可选）

### 构建

```bash
git clone https://github.com/<your-name>/smart_band.git
cd smart_band

cmake --preset Debug
cmake --build --preset Debug -j
```

Release 构建只需把 `Debug` 换成 `Release`：

```bash
cmake --preset Release
cmake --build --preset Release -j
```

产物位于 `build/<Preset>/smart_band.elf`，同时生成 `smart_band.map`（链接映射与内存占用，链接时已开启 `--print-memory-usage`）。如需 `.hex` / `.bin`：

```bash
arm-none-eabi-objcopy -O ihex   build/Release/smart_band.elf smart_band.hex
arm-none-eabi-objcopy -O binary build/Release/smart_band.elf smart_band.bin
```

### 下载

```bash
# OpenOCD（ST-Link）
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
        -c "program build/Debug/smart_band.elf verify reset exit"
```

或用 STM32CubeProgrammer 打开 `.hex` 直接烧录。

### 重新生成外设代码

外设初始化代码由 STM32CubeMX 生成。若修改了 `.ioc`，请在 CubeMX 中重新生成，注意保留 `/* USER CODE BEGIN */` 与 `/* USER CODE END */` 之间的内容。

### 编码注意事项

源码中含 UTF‑8 中文字符串（界面文字与注释）以及 16×16 中文点阵。**请确保编辑器与编译器均以 UTF‑8 读写源文件**，否则中文将显示为乱码。

## 使用说明

| 操作 | 按键 | 效果 |
| --- | --- | --- |
| 首页左右切换焦点 | KEY1 / KEY2 | 在「菜单」与「设置」之间高亮切换 |
| 进入 | KEY3 | 进入菜单页 / 设置页 |
| 菜单内左右选功能 | KEY1 / KEY2 | 图标横向滑动 |
| 功能内操作 | 见各页面 | 秒表：开始 / 停止 / 清零；手电筒：ON / OFF |
| 返回 | KEY3（把光标移到左上角「返回」图标后按确认） | 回到上一级页面 |

**菜单结构**

```
首页时钟 ─┬─ 菜单 ─┬─ ← 返回
          │        ├─ ⏱ 秒表
          │        ├─ 🔦 手电筒
          │        ├─ 📐 姿态仪 (MPU6050)
          │        ├─ 🦖 小恐龙
          │        ├─ 🙂 动态表情
          │        └─ 🫧 水平仪
          └─ 设置 ─┬─ ← 返回
                   └─ 日期时间设置 → 年/月/日/时/分/秒
```

**小恐龙游戏**：进入后按 KEY1 跳跃，撞到障碍物显示 `Game Over`；跳跃轨迹由 `28 · sin(π·t/1000)` 给出，速度随地面滚动位置推进。

## 实现要点

**1. RTC 掉电续走与一次性初始化**

`H_RTC_Init()` 先读备份寄存器 `BKP_DR1`，若等于魔数 `0x32F2` 则直接返回，避免每次上电把时间重置；首次上电才调用 `HAL_RTC_Init()` 并写入默认时间。时间统一用 `mktime()` / `gmtime()` 在 Unix 时间戳与 `struct tm` 之间转换：

```c
struct tm* H_HAL_RTC_GetTime(void) {
  time_t unixtime = RTC_ReadTimeCounter(&hrtc);
  return gmtime(&unixtime);
}
```

读计数器时采用 `CNTH → CNTL → CNTH` 的三次读取法，处理 16 位半字读取期间发生进位的高低位撕裂问题。

**2. 电量估算**

```c
for (int i = 0; i < 3000; i++) sum += HAL_ADC_GetValue(&hadc1);  // 均值滤波
ADValue = sum / 3000;
Battery_Capacity = (ADValue - 3275) * 100 / 819;                 // 线性映射到 0–100
```

采用连续转换 + 多次平均抑制抖动，并把结果限幅在 `[0, 100]`。

**3. 姿态解算：一阶互补滤波**

陀螺仪积分短期平滑但会漂移，加速度计长期稳定但噪声大，两者互补：

```c
roll_g  = roll  + gx * Δt;                          // 陀螺积分
roll_a  = atan2f(ay, az) * 180 / π;                 // 加速度解算
roll    = 0.9 * roll_g + 0.1 * roll_a;              // 互补融合（α = 0.9，Δt = 5 ms）
```

Yaw 无绝对参考，仅做陀螺积分。

**4. 微秒级延时**

`Delay_us()` 直接改写 `SysTick` 的 `LOAD / VAL / CTRL` 寄存器轮询 COUNTFLAG，为软件 I²C 提供精确时序：`SysTick->LOAD = 72 * xus`（72 MHz → 1 µs/xus）。

**5. 弹性地面滚动**

128 像素宽的屏幕要显示 256 像素长的地面图，按下标偏移取模实现无缝循环：

```c
if (Ground_Pos < 128)
  for (int i = 0; i < 128; i++) OLED_GRAM[7][i] = groundData[i + Ground_Pos];
else {  // 跨越端点时拆成两段拼接
  for (int i = 0; i < 255 - Ground_Pos; i++) OLED_GRAM[7][i] = groundData[i + Ground_Pos];
  for (int i = 255 - Ground_Pos; i < 128; i++) OLED_GRAM[7][i] = groundData[i + Ground_Pos - 255];
}
```

直接写 `OLED_GRAM` 而非逐点调 API，省掉大量函数调用开销。

## 待办与改进方向

- [ ] 页面普遍采用阻塞式 `while(1)` 局部状态机，后续可改为统一事件队列 + 状态机表，降低页面间耦合
- [ ] 按键增加长按 / 连击识别，设置时分秒可快速调整
- [ ] MPU6050 增加零偏校准与温度补偿，Yaw 引入磁力计或改用 Madgwick / Mahony
- [ ] 游戏难度随分数递增（障碍间隔、滚动速度自适应），加入最高分掉电保存
- [ ] 低功耗：空闲时关闭 OLED、降低主频、进入 STOP 模式，配合 RTC 闹钟唤醒
- [ ] 用 DMA 刷屏替代逐帧 I²C 轮询传输，解放 CPU

## 致谢与许可

- `Core/Src/oled.c`、`font.c` 的 OLED 驱动与字库来自 **波特律动（keysking）** 的 STM32 教程驱动，遵循 **MIT License**。
- `Drivers/` 下为 STMicroelectronics 官方 **STM32F1 HAL** 与 **CMSIS**，遵循各自 LICENSE。
- 本工程应用层代码（`menu.c` / `dino.c` / `settime.c` / `h_rtc.c` / `mpu6050.c` 等）可自由学习与参考使用。

---

如果这个项目对你有帮助，欢迎点个 ⭐ Star。
