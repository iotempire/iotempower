IotMenu 2 (Textual TUI)
=======================

Overview
--------
`iot_menu_2` is the project's main Textual-based terminal UI. It exposes project management tasks (compile/deploy/adopt/etc.), embeds the AP configurator UI, and provides consoles for device/system actions.

Purpose
-------
- Provide a compact, keyboard-driven interface to common IoTempower workflows.
- Host the AP/network configurator as an embeddable screen so users can configure networking from within the TUI.
- Stream long-running script output into the TUI (via `web_starter`/serve) for real-time feedback.

Requirements
------------
- Python 3.10+ (development uses 3.12)
- `textual` package installed into the Python interpreter used to run or debug the app
- For full functionality, have the IoTempower environment initialized (the `iot` management commands available)


-------------------------
`iot_menu_2` integrates the AP configurator (`lib/ap_configurator`) via an adapter screen named `APConfiguratorScreen`.

Key behaviours:
- The adapter temporarily `chdir()`s into `lib/ap_configurator` before mounting the AP configurator screens so existing scripts that rely on relative paths continue to work.
- The adapter installs AP configurator sub-screens into the host app only when necessary and guards against duplicate screen installation.
- The main `BasicMenu` includes an `AP Configurator` button wired to open the embedded screen.
- While the AP configurator screen is active, global "go up" actions are intercepted so they do not conflict with the configurator UI.

Files of interest
-----------------
- `lib/iot_menu_2/app.py` — entry point and app-wide initialization (adds `lib/` shim to `sys.path`).
- `lib/iot_menu_2/menus/basic_menu.py` — main menu definition (button to launch AP configurator).
- `lib/iot_menu_2/screens/ap_configurator_screen.py` — the adapter screen that mounts `ap_configurator` inside the app.
- `lib/iot_menu_2/serve.py` — streaming helper for script output to the TUI.
- `lib/ap_configurator/` — the AP configurator package (runnable standalone or embeddable).

Developer notes
---------------
- Prefer non-invasive changes to the app routing; avoid global modifications unless necessary.
- If a ScreenError occurs due to duplicate registrations during development, restart the app or guard `install_screen` calls behind an existence check.

Troubleshooting
---------------
- `ModuleNotFoundError` for `textual`: ensure the interpreter used by your debugger has `textual` installed (create `.venv` and point the debugger to it).
- Non-interactive debug session: run the debugger in an integrated terminal or run the app outside the debugger with `PYTHONPATH=lib python -m iot_menu_2.app`.

Next steps
----------
- Generate a `requirements.txt` or `pyproject.toml` for deterministic installs.
- Add a sample VS Code `launch.json` that sets `PYTHONPATH=lib` and uses the project venv, if desired.
- Consider adding MQTT controls to the TUI for realtime device interaction.
