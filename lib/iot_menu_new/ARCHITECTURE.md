# IotMenu 2 — System Architecture

## Overview

This document describes the complete architecture of the IotMenu 2 Textual-based UI, including how `serve.py` connects to all screens, deployment scripts, libraries, and bash scripts.

## High-Level Flow

```
serve.py (textual-serve HTTP wrapper)
    ↓
IotMenu App (app.py) — main Textual application
    ↓
BasicMenu — navigation menu
    ↓
Screens (DeployScreen, AdoptScreen, WebStarterScreen, etc.)
    ↓
Script Activation Logic (Python wrappers around bash scripts)
    ├→ Bash Scripts (bin/compile, bin/deploy, bin/adopt, etc.)
    └→ External Services (CloudCmd, Node-RED, Caddy, MQTT)
```

## Component Breakdown

### 1. serve.py (Entry Point)

- **File**: `lib/iot_menu_2/serve.py`
- **Role**: HTTP server wrapper using `textual-serve` library
- **Responsibility**: Spawns the Textual app and provides HTTP access to the TUI
- **Command**: `python -m iot_menu_2.serve` or via `textual-serve` directly

```python
from textual_serve.server import Server
server = Server("python -m app")
server.serve()
```

---

### 2. IotMenu App (app.py)

- **File**: `lib/iot_menu_2/app.py`
- **Class**: `IotMenu(App)`
- **Key Responsibilities**:
  - Maintains the application state (current path, screen stack)
  - Manages web starter subprocess and log streaming
  - Registers screen factories
  - Routes user input to appropriate screens

**Key Methods**:
- `start_web_starter()` — launches the `web_starter` bash script and streams output
- `stop_backend()` — stops the web starter process cleanly
- `_append_log_line()` — adds lines to `web_log_buffer` and both broadcasts via `WebOutput` message and persists to file
- `compose()` — renders the main menu

**State**:
```python
self.web_starter: asyncio.subprocess.Process      # The running web_starter process
self.web_stream_task: asyncio.Task                # The streaming coroutine
self.web_log_buffer: deque[str]                   # In-memory log buffer (last 5000 lines)
self.SCREENS: dict                                # Screen factory registry
```

---

### 3. BasicMenu (Navigation)

- **File**: `lib/iot_menu_2/menus/basic_menu.py`
- **Role**: Main menu widget; displays buttons to navigate to all major screens
- **Buttons**:
  - Deploy
  - Adopt
  - New Node
  - New Folder
  - WiFi Setup
  - OpenWrt Setup
  - System Template
  - Upgrade IoT
  - Web Starter
  - AP Configurator

---

### 4. Screens (Script Execution Layer)

Each screen corresponds to a major workflow and invokes script logic:

| Screen | File | Bash Script | Python Logic |
|--------|------|-------------|--------------|
| DeployScreen | `screens/deploy_screen.py` | `bin/deploy` | `script_activation_logic/deploy_script.py` |
| AdoptScreen | `screens/adopt_screen.py` | `bin/adopt` | `script_activation_logic/adopt_script.py` |
| WifiSetupSystemconf | `screens/wifi_setup_system_conf_screen.py` | `bin/setup_systemconf` | `script_activation_logic/wifi_setup_systemconf_script.py` |
| OpenwrtSetup | `screens/open_wrt_setup_screen.py` | `bin/wifi_openwrt_setup` | `script_activation_logic/open_wrt_setup_script.py` |
| SystemTemplate | `screens/system_template_screen.py` | `bin/create_system_template` | `script_activation_logic/create_system_logic.py` |
| WebStarter | `screens/web_starter_screen.py` | `bin/web_starter` | Managed by app.py |
| APConfigurator | `screens/ap_configurator_screen.py` | N/A | `lib/ap_configurator/*` |

---

### 5. Script Activation Logic (Python Wrappers)

Located in `lib/iot_menu_2/script_activation_logic/`, these modules wrap bash scripts and provide a Python interface to screens.

**Example Structure**:
```python
def deploy_script(path: Path, board: str, port: str):
    script = Path(__file__).parent.parent.parent.parent / "bin/deploy"
    result = subprocess.run(
        ["bash", str(script), str(board), str(port)],
        capture_output=True,
        text=True,
        cwd=path
    )
    return [result.stdout, result.stderr, result.returncode]
```

---

### 6. Bash Scripts (bin/)

Core management commands:

- **web_starter** — Launches all web services (CloudCmd, Node-RED, Caddy) in background jobs
- **deploy** — Compiles and uploads firmware to a node
- **adopt** — Initializes an existing node (reads config, sets up WiFi)
- **compile** — Compiles firmware without uploading
- **create_node_template** — Scaffolds a new node folder
- **create_system_template** — Scaffolds a new system folder
- **cloudcmd_starter** — Starts the CloudCmd file browser service
- **nodered_starter** — Starts Node-RED flow editor
- **caddy_starter** — Starts the Caddy reverse proxy

---

### 7. Web Logging Flow

The app maintains a real-time log of `web_starter` output:

1. **Subprocess Output**:
   - `web_starter` bash script spawned as subprocess
   - stdout/stderr combined and read line-by-line via async stream

2. **In-Memory Buffer**:
   - Lines appended to `self.web_log_buffer` (deque, max 5000 lines)
   - Emits `WebOutput` message to notify listening screens

3. **Persistent Log**:
   - Lines also written to `web_starter.log` file
   - Re-loaded on app startup

4. **Display**:
   - WebStarterScreen subscribes to `WebOutput` messages
   - Displays log in a scrollable text widget

---

### 8. Web Services (Runtime)

Launched by `web_starter`:

- **CloudCmd** — File browser and editor; runs on port 8000
- **Node-RED** — Flow editor for home automation; runs on port 1880
- **Caddy** — Reverse proxy/SSL terminator; exposes services on 80/443

All three communicate with the MQTT broker configured in `system.conf`.

---

### 9. AP Configurator Integration

The AP configurator (`lib/ap_configurator`) is embedded in the main app via `APConfiguratorScreen`:

- **Adapter Screen**: `lib/iot_menu_2/screens/ap_configurator_screen.py`
- **Behaviour**:
  - Temporarily `chdir()` into `lib/ap_configurator` to preserve script relative paths
  - Installs AP configurator sub-screens into the host app (guarded against duplicates)
  - Intercepts Backspace/Up key presses to prevent global "go up" from interfering
- **Launch**: Button in BasicMenu labeled "AP Configurator"

---

### 10. External Libraries

- **textual** — TUI framework; provides App, Screen, Widget, Message classes
- **textual-serve** — HTTP wrapper for Textual; allows web-based access to the TUI
- **asyncio** — Async process management and task scheduling
- **subprocess** — Spawning and managing bash scripts

---

## Data Flow Diagram (Simplified)

```
┌──────────────┐
│   serve.py   │
│ (HTTP Server)│
└──────┬───────┘
       │
       ↓
┌─────────────────────┐
│  IotMenu App        │
│  (app.py)           │
│  ┌─────────────┐    │
│  │ BasicMenu   │    │
│  └──────┬──────┘    │
│  ┌──────┴─────────────────────────┬──────────────┐
│  ↓                                ↓              ↓
│ Screen Screens              Scripts        External
│ Factory  (Deploy,           (Python       Services
│          Adopt, WiFi)       wrappers)     (CloudCmd,
│  ┌─────┐                      │           Node-RED)
│  │ web_log_buffer          Bash Scripts    │
│  │ (deque[str])            (bin/*)         │
│  │  │                         │            │
│  │  └─→ web_starter.log       ↓            │
│  │                         Processes       │
│  └─────────────┬───────────────────────────┘
│       emit WebOutput messages
└────────────────────────────────────────────────┘
```

---

## Key Integration Points

### From Menu Button to Bash Script

1. User clicks button in BasicMenu
2. Button press triggers screen switch
3. Screen loads and displays form or progress indicator
4. User submits form or confirms action
5. Screen calls Python wrapper in `script_activation_logic/`
6. Python wrapper calls `subprocess.run()` on bash script in `bin/`
7. Bash script runs (may spawn multiple child processes)
8. Output collected and displayed on screen or in log

### From Web Starter to UI

1. BasicMenu "Web Starter" button clicked
2. App calls `start_web_starter()` which spawns `bin/web_starter` subprocess
3. Subprocess output read asynchronously by `web_stream_task`
4. Each line appended to `web_log_buffer` via `_append_log_line()`
5. `_append_log_line()` emits `WebOutput` message
6. `WebStarterScreen` receives message and updates display

---

## File Structure Summary

```
lib/iot_menu_2/
├── app.py                          # Main app class and entry point
├── serve.py                        # HTTP server wrapper
├── screens/
│   ├── deploy_screen.py            # Deploy workflow
│   ├── adopt_screen.py             # Adopt workflow
│   ├── web_starter_screen.py       # Web services log viewer
│   ├── ap_configurator_screen.py   # AP config adapter
│   └── ... (other screens)
├── menus/
│   ├── basic_menu.py               # Main navigation menu
│   └── ... (other menus)
├── script_activation_logic/
│   ├── deploy_script.py            # Calls bin/deploy
│   ├── adopt_script.py             # Calls bin/adopt
│   └── ... (other script wrappers)
├── messages/
│   ├── web_output.py               # WebOutput message for log updates
│   └── ... (other messages)
└── tcss/
    └── iot_menu.tcss               # Textual CSS styling

bin/
├── web_starter                     # Main web service launcher
├── deploy                          # Firmware deploy script
├── adopt                           # Node adoption script
├── compile                         # Firmware compile script
├── cloudcmd_starter                # CloudCmd service launcher
├── nodered_starter                 # Node-RED service launcher
├── caddy_starter                   # Caddy service launcher
└── ... (other management scripts)
```

---

## Troubleshooting

### App starts but no TextualUI shows

- Check that Python has `textual` installed
- Verify the debugger uses a TTY (integrated terminal in VS Code)

### Web starter doesn't start

- Check that `IoTempower` is active: `iotempower` environment must be initialized
- Verify bash scripts in `bin/` are executable
- Check logs with `tail -f web_starter.log`

### AP configurator button doesn't work

- Check that `lib/ap_configurator/` exists and has an `__init__.py`
- Verify `sys.path` shim in `app.py` adds `lib/` to path
- Ensure `APConfiguratorScreen` can import `ap_configurator_screen.py` and the imports are done under the folder as .import

---

## Further Reading

- See `README.md` in this folder for quick start and requirements
- See `doc/architecture.rst` for IoTempower system-level architecture
- See `doc/device-architecture.rst` for device driver implementation details
