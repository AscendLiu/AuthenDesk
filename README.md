# AuthenDesk — 桌面双因素认证令牌管理器

基于 Qt 6.8 + QML 的跨平台桌面 2FA 令牌管理器，参考 [2FAS Android](https://github.com/twofas/2fas-android) 实现。

![License](https://img.shields.io/badge/license-MIT-blue)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey)
![Qt](https://img.shields.io/badge/Qt-6.8-green)

## 功能

- **TOTP / HOTP 令牌生成** — 支持 SHA1 / SHA256 / SHA512 算法，6 位或 8 位数字
- **实时倒计时** — 每个令牌独立圆形倒计时，剩余 5 秒红色高亮
- **2FAS 备份导入** — 支持明文和 AES-256-GCM 加密 `.2fas` 文件
- **备份导出** — 导出为 2FAS 兼容 JSON 格式
- **二维码导入** — 支持图片文件识别和屏幕截图区域选择
- **品牌图标自动匹配** — 内置 1,933 个服务品牌，2,494 个 PNG 图标
- **SQLite 本地存储** — 启动自动初始化数据库
- **多显示器截图** — 每屏独立截图窗口
- **搜索过滤** — 按名称、账号实时过滤
- **编辑令牌** — 修改已有令牌的全部属性
- **深色 / 浅色主题**

## 截图

```
┌──────────────────────────┐
│  🔍 搜索令牌...        ⋮ │   ← 搜索框 + 菜单
├──────────────────────────┤
│ [G] Google             30│   ← 令牌列表（图标 + 名称 + 倒计时）
│    user@gmail.com        │
│    123 456     next: ... │
├──────────────────────────┤
│ [Gi] GitHub            25│
│    dev@github.com        │
│    789 012     next: ... │
└──────────────────────────┘
```

## 编译

### 依赖

- **Qt 6.5+** (Core, Gui, Quick, QuickControls2, Network, Sql, DBus)
- **OpenSSL** (libssl, libcrypto)
- **CMake 3.16+**
- **GCC 或 Clang** (C++17)
- **zbar** (二维码解码，`zbarimg` 命令行工具)

### 构建

```bash
git clone git@gitee.com:AscendLiu/AuthenDesk.git
cd AuthenDesk
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
./build/AuthenDesk
```

## 安装

### Arch Linux (AUR)

```bash
yay -S authendesk
# 或手动
git clone https://aur.archlinux.org/authendesk.git
cd authendesk
makepkg -si
```

### 手动安装

```bash
sudo cmake --install build --prefix /usr
sudo install -Dm644 assets/authendesk.desktop /usr/share/applications/authendesk.desktop
sudo install -Dm644 assets/app_icon.png /usr/share/icons/hicolor/256x256/apps/authendesk.png
```

## 使用

| 操作 | 方式 |
|------|------|
| 复制令牌码 | 点击令牌项 |
| 查看详情 / 删除 | 右键 / 长按 → 弹出抽屉面板 |
| 编辑令牌 | 抽屉面板 → "编辑" |
| 添加令牌 | 菜单 → "添加令牌" |
| 二维码导入 | 添加页 → "二维码导入" → 选择图片或截屏识别 |
| 备份导入 | 菜单 → "备份与还原" → "导入" |
| 备份导出 | 菜单 → "备份与还原" → "导出" |
| 搜索过滤 | 顶部搜索框输入关键词 |
| 清空全部 | 菜单 → "清空全部令牌" |

## 项目结构

```
AuthenDesk/
├── assets/
│   ├── icons/                  # 2,494 个品牌 PNG 图标
│   ├── services.json           # 1,933 个服务定义（匹配规则 + 图标映射）
│   ├── app_icon.png            # 应用程序图标
│   └── authendesk.desktop      # Linux 桌面入口
├── qml/
│   ├── main.qml                # 主窗口 + StackView 导航
│   ├── TokenListPage.qml       # 令牌列表页（搜索 + 菜单）
│   ├── TokenDelegate.qml       # 单个令牌列表项（图标 + 码 + 倒计时）
│   ├── AddTokenPage.qml        # 添加令牌（手动 / 二维码）
│   ├── EditTokenPage.qml       # 编辑令牌
│   ├── ImportPage.qml          # 备份导入 / 导出
│   └── ScreenCapture.qml       # 截屏选区窗口
├── src/
│   ├── main.cpp                # 入口
│   ├── totp.cpp/h              # TOTP/HOTP 生成（RFC 6238 / RFC 4226）
│   ├── tokenmanager.cpp/h      # SQLite 数据持久化
│   ├── tokendata.cpp/h         # 令牌数据结构 + JSON 序列化
│   ├── serviceiconprovider.cpp/h # 品牌图标匹配
│   ├── importparser.cpp/h      # 2FAS 备份导入（含 AES-256-GCM 解密）
│   ├── qrdecoder.cpp/h         # 二维码解码 + 截屏
│   └── models/
│       └── tokenlistmodel.cpp/h # QAbstractListModel（搜索过滤 + 定时刷新）
├── CMakeLists.txt
├── PKGBUILD
└── qml.qrc
```

## 存储

令牌数据存储在 SQLite 数据库：

**位置**: `~/.local/share/AuthenDesk/AuthenDesk/tokens.db`

导出格式兼容 [2FAS 备份格式](https://docs.2fas.com/features/backup) (schema v4)。

## 技术栈

| 技术 | 说明 |
|------|------|
| Qt 6.8 | C++ 框架 + QML 声明式 UI |
| SQLite | 本地数据持久化 |
| OpenSSL | AES-256-GCM 加密备份解密 |
| zbar | 二维码图像解码 |
| D-Bus Desktop Portal | Wayland 截屏 |
| RFC 6238 | TOTP 生成标准 |
| RFC 4226 | HOTP 生成标准 |

## 许可

MIT License

## 致谢

- [2FAS Android](https://github.com/twofas/2fas-android) — 参考原型，图标资源和 services.json 来源于此项目
- 2FAS 品牌图标版权归各自所有者
