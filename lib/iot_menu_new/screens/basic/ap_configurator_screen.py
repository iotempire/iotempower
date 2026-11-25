from __future__ import annotations

from textual.screen import Screen
from textual.app import ComposeResult
from textual.containers import VerticalScroll
from textual.widgets import Header, Footer, Static, OptionList
from textual.widgets.option_list import Option
from textual.widgets import Markdown
from textual import events
import asyncio
import os
from pathlib import Path

# ap_configurator package
from ap_configurator import config
from ap_configurator.utils import update_static, run_cmd_async
from ap_configurator.screens import (
    ConnectedClients,
    LocalConfiguration,
    OpenWRTConfiguration,
    APSettings,
    WiFiChipInfo,
    QuitScreen,
    RestartScreen,
)


class APConfiguratorScreen(Screen):
    BINDINGS = [("m", "app.pop_screen", "Back")]

    def on_key(self, event: events.Key) -> None:
        # Prevent the global "up" action (bound to Backspace) from
        # being executed while this screen is active.
        if event.key in ("backspace", "up"):
            
            event.stop()

    def compose(self) -> ComposeResult:
        yield Header()
        with VerticalScroll():
            yield Markdown("""
# APConfigurator

This application will help you automatically configure
your Access Point and network settings.
            """)

            yield Static("", id="detected-chip")
            yield Static("", id="iotemp-status")
            yield Static("", id="softap-status")
            yield OptionList(
                Option("Configure Access Point", id="cap"),
                Option("Configure OpenWRT Router", id="cor"),
                Option("──────────", disabled=True),
                Option("View Connected Clients", id="vcc"),
                Option("View AP Settings", id="vas"),
                Option("View Wi-Fi Chip Information", id="vci"),
                Option("──────────", disabled=True),
                id="menu",
            )
        yield Footer()

    def on_mount(self) -> None:
        # Ensure scripts run from ap_configurator package dir
        pkg_dir = Path(__import__("ap_configurator").__file__).resolve().parent
        try:
            os.chdir(pkg_dir)
        except Exception:
            pass

        # Install sub-screens into the host App so they can be pushed by name
        app = self.app

        def _maybe_install(name: str, screen) -> None:
            if name in getattr(app, "_installed_screens", {}):
                return
            try:
                app.install_screen(screen, name=name)
            except Exception:
                # If install fails for any reason, ignore — the app may already
                # have an equivalent screen installed by another code path.
                return

        _maybe_install("ap_concli", ConnectedClients())
        _maybe_install("ap_localconf", LocalConfiguration())
        _maybe_install("ap_wrtconf", OpenWRTConfiguration())
        _maybe_install("ap_settings", APSettings())
        _maybe_install("ap_wifichip", WiFiChipInfo())
        _maybe_install("ap_quit", QuitScreen())
        _maybe_install("ap_restart", RestartScreen())

        # Start background detection tasks
        asyncio.create_task(self.update_detected_chip())
        asyncio.create_task(self.check_iotempower())
        asyncio.create_task(self.check_running_ap())

    async def update_detected_chip(self) -> None:
        out, err = await run_cmd_async("bash ./scripts/detect_wifi_chip.sh")
        pkg = config
        pkg.SYSTEM = ' '.join([pkg.SYSTEM or '', 'detected']) if hasattr(pkg, 'SYSTEM') else ''
        if out and (chip := self.confirm_chip(out)):
            pkg.WIFI_CHIP = chip
            result = '\t[+] Your Wi-Fi chip: ' + chip
        else:
            result = '\t[!] Unable to detect your Wi-Fi chip.'
        update_static(self, 'detected-chip', result)

    def confirm_chip(self, detected) -> str:
        config.WIFI_CHIP_FULL = detected
        detected_l = detected.lower()
        if 'netman_available' in detected_l:
            config.SOFTAP = 'networkmanager'
        if 'hostapd_available' in detected_l:
            config.SOFTAP = 'hostapd'
        if any(x in detected_l for x in ['brcm', 'broadcom', 'bcm']):
            return 'Broadcom'
        if any(x in detected_l for x in ['iwlwifi', 'intel']):
            return 'Intel'
        if any(x in detected_l for x in ['rtl', 'realtek']):
            return 'Realtek'
        if any(x in detected_l for x in ['ath9k', 'ath10k', 'ath11k']):
            return 'Atheros'
        if any(x in detected_l for x in ['mediatek', 'mt79', 'rt53', 'rt35', 'mt76']):
            return 'MediaTek'
        return ''

    async def check_running_ap(self) -> None:
        out1, err1 = await run_cmd_async("bash ./scripts/read_wifi_creds.sh")
        creds = out1.strip() if out1 else ''
        if out1 and len(creds) > 2 and ',' in creds:
            cl = creds.split(',')
            config.AP_SSID = cl[0].strip()
            config.AP_IP = cl[2].strip()

        out, err = await run_cmd_async("ps -a | grep 'hostapd\\|create_ap'")
        if 'hostapd' in out:
            config.AP_RUNNING = 'hostapd'
        else:
            out, err = await run_cmd_async("nmcli con show --active")
            if not err and out and config.AP_SSID and config.AP_SSID in out:
                config.AP_RUNNING = 'NetworkManager'

        if config.AP_RUNNING:
            update_static(self, 'softap-status', '\t[+] An active AP has been detected: ' + config.AP_RUNNING)

    async def check_iotempower(self) -> None:
        out, err = await run_cmd_async('bash ./scripts/detect_iotempower.sh')
        config.IOTEMPOWER = out.strip() == 'yes' if out else False
        status = "\t[+] IoTempower is installed correctly and activated" if config.IOTEMPOWER else "\t[-] IoTempower is not activated! Functionality is not available!"
        update_static(self, 'iotemp-status', status)
        if not config.IOTEMPOWER:
            self.app.push_screen('ap_quit')

    async def on_option_list_option_selected(self, message: OptionList.OptionSelected) -> None:
        mapping = {
            'cap': 'ap_localconf',
            'cor': 'ap_wrtconf',
            'vcc': 'ap_concli',
            'vas': 'ap_settings',
            'vci': 'ap_wifichip',
            'vq': 'ap_quit',
        }
        target = mapping.get(message.option.id)
        if target:
            self.app.push_screen(target)

