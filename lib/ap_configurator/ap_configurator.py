#!/usr/bin/python3


# Textual
from textual import log
from textual.app import App, ComposeResult
from textual.containers import VerticalScroll, Grid
from textual.widgets import Button, Header, Footer, Static, OptionList, Input, RadioButton, RadioSet, Label, Log
from textual.widgets.option_list import Option, Separator
from textual.reactive import reactive
from textual.message import Message
from textual.widgets import Markdown
from textual.screen import Screen
from textual import events

# Python libraries
import asyncio
import platform
import sys

# Modules
import config
from utils import update_static, validate_config_params, run_cmd_async
from screens import ConnectedClients, LocalConfiguration, OpenWRTConfiguration, APSettings, WiFiChipInfo, QuitScreen, RestartScreen



class APConfigurator(App):
    """
    Access point configuration app
    
    This is the main UI class, containing the main menu screen 
    and references to all other screens and functionality.
    """

    CSS_PATH = "style.tcss"
    BINDINGS = [("q", "quit", "Quit")]
    #dark = False

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
                Separator(),
                Option("View Connected Clients", id="vcc"),
                Option("View AP Settings", id="vas"),
                Option("View Wi-Fi Chip Information", id="vci"),
                Separator(),
                Option("Quit", id="vq"),
                id="menu"
            )
        yield Footer()
        

    def on_mount(self) -> None:
        self.install_screen(ConnectedClients(), name="concli")
        self.install_screen(LocalConfiguration(), name="localconf")
        self.install_screen(OpenWRTConfiguration(), name="wrtconf")
        self.install_screen(APSettings(), name="apsettings")
        self.install_screen(WiFiChipInfo(), name="wifichipinfo")

        asyncio.create_task(self.update_detected_chip())
        asyncio.create_task(self.check_iotempower())
        asyncio.create_task(self.check_running_ap())


    def confirm_chip(self, detected) -> str:
        """Detect chip manufacturer and active AP software from long info string, 
        as returned by scripts/detect_wifi_chip.sh"""

        config.WIFI_CHIP_FULL = detected
        detected = detected.lower()

        if 'netman_available' in detected:
            config.SOFTAP = 'networkmanager'
        if 'hostapd_available' in detected:
            config.SOFTAP = 'hostapd'

        if any(x in detected for x in ['brcm', 'broadcom', 'bcm']):
            return 'Broadcom'
        if any(x in detected for x in ['iwlwifi', 'intel']):
            return 'Intel'
        if any(x in detected for x in ['rtl', 'realtek']):
            return 'Realtek'
        if any(x in detected for x in ['ath9k', 'ath10k', 'ath11k']):
            return 'Atheros'
        if any(x in detected for x in ['mediatek', 'mt79', 'rt53', 'rt35', 'mt76']):
            return 'MediaTek'

        return ''


    async def on_option_list_option_selected(self, message: OptionList.OptionSelected) -> None:
        await self.handle_option(message.option.id)


    async def handle_option(self, option_id: str) -> None:
        # Don't allow accessing any of the screens if IoTempower is not activated,
        # most of the scripts will not work
        if not config.IOTEMPOWER and not option_id == "vq":
            self.push_screen(QuitScreen())
            return

        if option_id == "cap":
            self.push_screen('localconf')
        elif option_id == "cor":
            self.push_screen('wrtconf')
        elif option_id == "vcc":
            self.push_screen('concli')
        elif option_id == "vas":
            asyncio.create_task(self.check_running_ap())
            self.push_screen('apsettings')
        elif option_id == "vci":
            self.push_screen('wifichipinfo')
        elif option_id == "dap":
            await run_cmd_async(f'bash ./scripts/turn_off_ap.sh {config.AP_SSID}', bg=True)
            self.push_screen(RestartScreen())
            pass
        elif option_id == "vq":
            self.action_quit()


    async def update_detected_chip(self) -> None:
        # Run the main chip info detection script
        out,err = await run_cmd_async("bash ./scripts/detect_wifi_chip.sh")

        # Gather separate information on the system
        plfrm = platform.machine() + '-' + platform.platform(aliased=True, terse=True) + '-' + platform.system() + '-' + platform.processor()
        config.SYSTEM = plfrm

        if out and (chip := self.confirm_chip(out)):
            config.WIFI_CHIP = chip
            result = '\t[+] Your Wi-Fi chip: ' + chip
        else:
            result = '\t[!] Unable to detect your Wi-Fi chip.'

        update_static(self, 'detected-chip', result)

        # Also detect and set name of output device
        out,err = await run_cmd_async("bash ./scripts/detect_dev_name.sh")
        if out:
            config.WDEVICE = out.strpip()
            update_static(self, 'detected-chip', f', output device: {out}', append=True)


    async def check_running_ap(self) -> None:
        # First, check if any saved credentials already exist
        out1,err1 = await run_cmd_async("bash ./scripts/read_wifi_creds.sh")
        creds = out1.strip()
        if out1 and len(creds) > 2 and ',' in creds:
            cl = creds.split(',')
            config.AP_SSID = cl[0].strip()
            config.AP_IP = cl[2].strip()


        # Check whether hostapd has been activated already
        out,err = await run_cmd_async("ps -a | grep 'hostapd\\|create_ap'")

        if 'hostapd' in out:
            config.AP_RUNNING = 'hostapd'
        else:
            # Check NM service status separately
            out,err = await run_cmd_async("nmcli con show --active")
            if not err and out and config.AP_SSID and config.AP_SSID in out:
                config.AP_RUNNING = 'NetworkManager'

        if config.AP_RUNNING:
            update_static(self, 'softap-status', '\t[+] An active AP has been detected: ' + config.AP_RUNNING)
            self.query_one('#menu').add_option(Option("Turn Off Active AP", id="dap"))


    async def check_iotempower(self) -> None:
        # Check whether IoTempower environment is activated
        out,err = await run_cmd_async('bash ./scripts/detect_iotempower.sh')

        config.IOTEMPOWER = out.strip() == 'yes'

        status = "\t[+] IoTempower is installed correctly and activated" if config.IOTEMPOWER else "\t[-] IoTempower is not activated! Functionality is not available!"

        update_static(self, 'iotemp-status', status)

        if not config.IOTEMPOWER:
            self.push_screen(QuitScreen())


    def action_quit(self) -> None:
        self.exit()


if __name__ == "__main__":
    app = APConfigurator()  
    app.run()
