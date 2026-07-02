pragma Singleton

import QtQuick

QtObject {
    readonly property string currentLocale: LocaleManager ? LocaleManager.currentLanguage : "en"

    readonly property bool isChinese: currentLocale === "zh"

    // Search
    readonly property string searchPlaceholder: isChinese ? "搜索令牌..." : "Search tokens..."

    // Token list page
    readonly property string addToken: isChinese ? "添加令牌" : "Add Token"
    readonly property string backupAndRestore: isChinese ? "备份与还原" : "Backup & Restore"
    readonly property string clearAllTokens: isChinese ? "清空全部令牌" : "Clear All Tokens"
    readonly property string about: isChinese ? "关于" : "About"

    // Clear dialog
    readonly property string clearAllTokensTitle: isChinese ? "清空全部令牌" : "Clear All Tokens"
    readonly property string clearAllTokensConfirm: isChinese ? "确定要删除全部令牌吗？此操作不可撤销。" : "Are you sure you want to delete all tokens? This action cannot be undone."
    readonly property string cancel: isChinese ? "取消" : "Cancel"
    readonly property string clear: isChinese ? "清空" : "Clear"

    // About dialog
    readonly property string aboutTitle: isChinese ? "关于" : "About"
    readonly property string version: isChinese ? "版本 " + appVersion : "Version " + appVersion
    readonly property string aboutDescription: isChinese
        ? "基于 Qt 6.8 + QML 的桌面双因素认证令牌管理器。\n支持 TOTP/HOTP，从 2FAS 备份导入导出，\n二维码识别导入。"
        : "Desktop two-factor authentication token manager based on Qt 6.8 + QML.\nSupports TOTP/HOTP, import/export from 2FAS backup,\nQR code scanning."
    readonly property string ok: isChinese ? "确定" : "OK"

    // Token delegate
    readonly property string copied: isChinese ? "已复制！" : "Copied!"
    readonly property string edit: isChinese ? "编辑" : "Edit"
    readonly property string copy: isChinese ? "复制" : "Copy"
    readonly property string deleteToken: isChinese ? "删除" : "Delete"

    // Add token page
    readonly property string addTokenTitle: isChinese ? "添加令牌" : "Add Token"
    readonly property string back: isChinese ? "< 返回" : "< Back"
    readonly property string manualInput: isChinese ? "手动输入" : "Manual Input"
    readonly property string qrImport: isChinese ? "二维码导入" : "QR Code Import"
    readonly property string selectImage: isChinese ? "选择图片..." : "Select Image..."
    readonly property string screenCapture: isChinese ? "截屏识别" : "Screen Capture"
    readonly property string selectingQrRegion: isChinese ? "请选择二维码区域..." : "Select QR code region..."
    readonly property string decoding: isChinese ? "解码中..." : "Decoding..."
    readonly property string decodeSuccess: isChinese ? "识别成功" : "Recognition Successful"
    readonly property string name: isChinese ? "名称" : "Name"
    readonly property string namePlaceholder: isChinese ? "服务名称" : "Service Name"
    readonly property string secret: isChinese ? "密钥" : "Secret"
    readonly property string secretPlaceholder: isChinese ? "密钥 (Base32)" : "Secret (Base32)"
    readonly property string account: isChinese ? "账号" : "Account"
    readonly property string accountPlaceholder: isChinese ? "账号 (可选)" : "Account (Optional)"
    readonly property string issuer: isChinese ? "发行者" : "Issuer"
    readonly property string issuerPlaceholder: isChinese ? "发行者 (可选)" : "Issuer (Optional)"
    readonly property string advancedSettings: isChinese ? "高级设置" : "Advanced Settings"
    readonly property string algorithm: isChinese ? "算法" : "Algorithm"
    readonly property string period: isChinese ? "周期 (秒)" : "Period (seconds)"
    readonly property string digits: isChinese ? "位数" : "Digits"
    readonly property string addTokenButton: isChinese ? "添加令牌" : "Add Token"
    readonly property string nameAndSecretRequired: isChinese ? "名称和密钥不能为空" : "Name and secret cannot be empty"
    readonly property string addTokenFailed: isChinese ? "添加令牌失败" : "Failed to add token"

    // Edit token page
    readonly property string editTokenTitle: isChinese ? "编辑令牌" : "Edit Token"
    readonly property string saveChanges: isChinese ? "保存修改" : "Save Changes"
    readonly property string saveFailed: isChinese ? "保存修改失败" : "Failed to save changes"

    // Import page
    readonly property string importExportTitle: isChinese ? "备份与还原" : "Backup & Restore"
    readonly property string importTab: isChinese ? "导入" : "Import"
    readonly property string exportTab: isChinese ? "导出" : "Export"
    readonly property string importFrom2fas: isChinese ? "从 2FAS 备份文件导入令牌" : "Import tokens from 2FAS backup file"
    readonly property string select2fasFile: isChinese ? "选择 .2fas 文件..." : "Select .2fas file..."
    readonly property string passwordIfEncrypted: isChinese ? "密码 (如已加密)" : "Password (if encrypted)"
    readonly property string startImport: isChinese ? "开始导入" : "Start Import"
    readonly property string importSuccess: isChinese ? "导入成功！" : "Import successful!"
    readonly property string importFailed: isChinese ? "导入失败，请检查文件和密码。" : "Import failed, please check the file and password."

    readonly property string exportTo2fas: isChinese ? "导出令牌到 2FAS 备份文件" : "Export tokens to 2FAS backup file"
    readonly property string currentTokenCount: isChinese ? "当前共 %1 个令牌" : "Currently %1 token(s)"
    readonly property string selectSaveLocation: isChinese ? "选择保存位置..." : "Select save location..."
    readonly property string exportButton: isChinese ? "导出" : "Export"
    readonly property string exportSuccess: isChinese ? "导出成功！" : "Export successful!"
    readonly property string exportFailed: isChinese ? "导出失败" : "Export failed"

    // Screen capture
    readonly property string dragToSelectRegion: isChinese ? "拖动鼠标选择二维码区域" : "Drag to select QR code region"
    readonly property string pressEscToCancel: isChinese ? "按 Esc 取消" : "Press Esc to cancel"

    // QR Decoder errors (from C++)
    readonly property string cannotGetScreen: isChinese ? "无法获取屏幕" : "Cannot get screen"
    readonly property string tempFileCreateFailed: isChinese ? "临时文件创建失败" : "Failed to create temp file"
    readonly property string regionCropFailed: isChinese ? "区域裁剪失败" : "Failed to crop region"
    readonly property string portalNotAvailable: isChinese ? "无法请求截屏 (Portal 不可用)" : "Cannot capture screen (Portal not available)"
    readonly property string captureCancelled: isChinese ? "截屏被取消或拒绝" : "Screen capture cancelled or denied"
    readonly property string captureDataEmpty: isChinese ? "截屏返回数据为空" : "Screen capture returned empty data"
    readonly property string captureFileNotExist: isChinese ? "截屏文件不存在" : "Screen capture file does not exist"
    readonly property string cannotReadCaptureImage: isChinese ? "无法读取截屏图片" : "Cannot read screen capture image"
    readonly property string captureCropFailed: isChinese ? "截屏裁剪失败" : "Screen capture crop failed"
    readonly property string failedToDecodeQr: isChinese ? "Failed to decode QR code" : "Failed to decode QR code"
    readonly property string noQrCodeFound: isChinese ? "No QR code found in image" : "No QR code found in image"
    readonly property string notValidOtpQr: isChinese ? "Not a valid OTP auth QR code" : "Not a valid OTP auth QR code"
    readonly property string failedToParseOtp: isChinese ? "Failed to parse OTP auth URI" : "Failed to parse OTP auth URI"

    // File dialogs
    readonly property string selectQrScreenshot: isChinese ? "选择二维码截图" : "Select QR code screenshot"
    readonly property string select2fasBackup: isChinese ? "选择 2FAS 备份文件" : "Select 2FAS backup file"
    readonly property string save2fasBackup: isChinese ? "保存 2FAS 备份文件" : "Save 2FAS backup file"
    readonly property string imageFiles: isChinese ? "图片文件 (*.png *.jpg *.jpeg *.bmp)" : "Image files (*.png *.jpg *.jpeg *.bmp)"
    readonly property string allFiles: isChinese ? "所有文件 (*)" : "All files (*)"
    readonly property string twofasBackup: isChinese ? "2FAS 备份 (*.2fas)" : "2FAS backup (*.2fas)"
}
