# VLESS to Clash 订阅转换器 — 设计文档

## 概述

一个基于 Web 的 VLESS 地址到 Clash 订阅配置转换工具。用户在网页中输入 VLESS 地址（或导入已有订阅链接），选择代理规则等配置选项后，即时生成 Clash 订阅地址和 YAML 配置文件。

## 需求

- 以网页形式提供，界面包含：VLESS 地址输入、Clash 订阅地址输出、YAML 配置下载
- 提供多种配置选项：客户端类型、TUN 模式、国内/国外流量策略、特殊网站规则等
- 生成的订阅地址外网可正常访问和下载
- 兼容 Clash Meta (mihomo) 和 Clash Premium 两种客户端

## 部署环境

- VPS 服务器部署
- Node.js + Express 后端
- 纯 HTML + Tailwind CSS 前端（无前端框架）
- 无数据库，即时生成

## 架构

采用 **URL Token 方案**：用户提交的 VLESS 地址和配置选项经压缩编码后嵌入订阅 URL，Clash 客户端请求时服务端解码并即时生成 YAML 返回。

```
用户浏览器 (HTML + Tailwind)
    │
    ├─ POST /api/generate       → 提交 VLESS 地址 + 配置选项
    │   └─ 返回: 订阅 URL + YAML 内容
    │
    ├─ GET /sub/:token           → Clash 客户端订阅拉取（返回 YAML）
    │
    ├─ GET /sub/:token?download=1 → 浏览器下载 YAML 文件
    │
    └─ POST /api/parse-sub       → 解析已有 base64 订阅链接
        └─ 返回: 解析出的 VLESS 地址列表

Express 服务端（无数据库）
    ├─ VLESS 解析器：vless:// → 结构化节点数据
    ├─ 订阅链接解析器：base64 订阅 → 提取 vless 地址
    ├─ Token 编解码器：配置数据 ↔ zlib 压缩 + base64url token
    └─ Clash 配置生成器：节点 + 规则模板 → YAML
```

## 前端界面

单页应用，从上到下三个区域：

### 区域 1：输入区

- Tab 切换「粘贴地址」和「导入订阅」两种模式
- 粘贴模式：多行文本框，每行一个 `vless://` 地址，实时显示识别的有效节点数
- 导入模式：输入已有的 base64 订阅链接 URL，服务端解析后填充节点列表

### 区域 2：配置选项区

网格布局（2 列），每个选项为一个卡片：

| 选项 | 控件类型 | 可选值 |
|------|----------|--------|
| 客户端类型 | 下拉框 | Clash Meta (mihomo)、Clash Premium |
| TUN 模式 | 开关 | 启用、禁用 |
| 默认出站 | 按钮切换 | 代理、直连 |
| 国内流量 | 按钮切换 | 直连、代理 |
| 国外流量 | 按钮切换 | 代理、直连 |
| 特殊网站规则 | 标签多选 | Google、YouTube、Telegram、Netflix、OpenAI、GitHub 等 |

底部居中「生成配置」按钮。

### 区域 3：结果区

生成后显示：
- **订阅地址** — URL 文本框 + 复制按钮
- **下载 YAML 文件** — 直接下载按钮
- **预览配置** — 展开/收起查看完整 YAML 内容

## VLESS 地址解析

将 `vless://` URI 解析为结构化对象：

```
vless://uuid@server:port?type=tcp&security=reality&pbk=xxx&fp=chrome&sni=xxx&sid=xxx&flow=xtls-rprx-vision#节点名
```

解析结果：

```js
{
  uuid: "xxx",
  server: "server",
  port: 443,
  type: "tcp",          // tcp, ws, grpc, h2
  security: "reality",  // none, tls, reality
  sni: "xxx",
  pbk: "xxx",           // Reality 公钥
  fp: "chrome",         // fingerprint
  sid: "xxx",           // Reality shortId
  flow: "xtls-rprx-vision",
  name: "节点名"
}
```

需要支持的 VLESS 传输类型：tcp、ws（WebSocket）、grpc、h2（HTTP/2）。

## Token 编解码

**编码流程：**
1. 将节点列表 + 配置选项序列化为 JSON
2. 使用 zlib（deflate）压缩
3. 使用 base64url 编码（URL 安全字符集）

**解码流程：** 反向操作。

```js
// 编码
const json = JSON.stringify({ nodes, options });
const compressed = zlib.deflateSync(Buffer.from(json));
const token = compressed.toString('base64url');

// 解码
const compressed = Buffer.from(token, 'base64url');
const json = zlib.inflateSync(compressed).toString();
const { nodes, options } = JSON.parse(json);
```

订阅 URL 格式：`https://your-domain.com/sub/{token}`

## Clash 配置生成

### 客户端兼容性

- **Clash Meta (mihomo)**：完整支持 VLESS 协议，支持 Reality、gRPC、WebSocket 等传输
- **Clash Premium**：不支持 VLESS，当用户选择 Premium 时，生成配置需明确提示不兼容

### 配置结构

生成的 YAML 配置包含以下主要段：

```yaml
# 基础设置
mixed-port: 7890
allow-lan: false
mode: rule
log-level: info

# DNS 设置
dns:
  enable: true
  enhanced-mode: fake-ip
  nameserver:
    - https://dns.alidns.com/dns-query
    - https://doh.pub/dns-query
  fallback:
    - https://1.1.1.1/dns-query
    - https://dns.google/dns-query

# TUN 设置（仅当 TUN 模式启用时）
tun:
  enable: true
  stack: system
  dns-hijack:
    - any:53

# 代理节点
proxies:
  - name: "节点1"
    type: vless
    server: xxx
    port: 443
    uuid: xxx
    # ... 其他参数

# 代理组
proxy-groups:
  - name: "PROXY"
    type: select
    proxies: [节点1, 节点2, AUTO]
  - name: "AUTO"
    type: url-test
    proxies: [节点1, 节点2]
    url: http://www.gstatic.com/generate_204
    interval: 300

# 规则
rules:
  # 特殊网站规则（根据用户选择）
  - DOMAIN-SUFFIX,google.com,PROXY
  - DOMAIN-SUFFIX,youtube.com,PROXY
  - DOMAIN-SUFFIX,telegram.org,PROXY
  # 国内/国外流量策略
  - GEOIP,CN,DIRECT        # 国内流量
  - MATCH,PROXY             # 兜底规则
```

### 规则集模板

内置规则集包含：
- 国内直连 IP 段（GEOIP,CN）
- 国内直连域名列表
- 常见代理域名列表（按网站分组：Google、YouTube、Telegram、Netflix、OpenAI、GitHub 等）
- 兜底规则（根据默认出站选项决定 MATCH 走代理还是直连）

用户选择的「国内流量」「国外流量」选项决定规则的默认方向。特殊网站标签可以覆盖默认行为。

## 错误处理

| 场景 | 处理方式 |
|------|----------|
| 无效 VLESS 地址 | 前端实时提示具体哪一行格式错误，高亮错误行 |
| 订阅链接无法访问 | 提示用户检查链接是否可达 |
| 订阅链接 base64 解码失败 | 提示链接格式不正确 |
| Token 解码失败（URL 被截断） | 返回 HTTP 400 + 错误说明 |
| Premium 客户端 + VLESS 协议 | 生成时弹窗提示不兼容，建议切换到 Clash Meta |

## 项目目录结构

```
vless2clash/
├── server.js              # Express 入口，启动服务
├── routes/
│   ├── api.js             # POST /api/generate, POST /api/parse-sub
│   └── subscription.js    # GET /sub/:token
├── lib/
│   ├── vless-parser.js    # VLESS 地址解析
│   ├── sub-parser.js      # base64 订阅链接解析
│   ├── token.js           # Token 编解码（zlib + base64url）
│   └── clash-generator.js # YAML 配置生成
├── templates/
│   └── rules.js           # 内置规则集定义
├── public/
│   ├── index.html         # 主页面 HTML
│   ├── style.css          # Tailwind 构建产物
│   └── app.js             # 前端交互逻辑
├── package.json
└── tailwind.config.js
```

## API 接口

### POST /api/generate

请求体：
```json
{
  "nodes": ["vless://...", "vless://..."],
  "options": {
    "client": "meta",
    "tun": true,
    "defaultOutbound": "proxy",
    "chinaTraffic": "direct",
    "foreignTraffic": "proxy",
    "specialSites": ["google", "youtube", "telegram"]
  }
}
```

响应：
```json
{
  "subscriptionUrl": "https://your-domain.com/sub/xxx",
  "yamlContent": "...",
  "nodeCount": 2
}
```

### POST /api/parse-sub

请求体：
```json
{
  "url": "https://example.com/sub/xxx"
}
```

响应：
```json
{
  "nodes": ["vless://...", "vless://..."],
  "count": 2
}
```

### GET /sub/:token

返回 `content-type: text/yaml` 的 Clash 配置文件。

带 `?download=1` 参数时返回 `Content-Disposition: attachment; filename=clash-config.yaml`。

## 技术依赖

- **express** — Web 框架
- **js-yaml** — YAML 生成
- **node 内置 zlib** — Token 压缩
- **tailwindcss** — 前端样式（构建时生成）
