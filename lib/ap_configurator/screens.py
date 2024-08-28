#!/usr/bin/python3


# Textual
from textual.app import ComposeResult
from textual.screen import Screen
from textual.containers import Grid, VerticalScroll
from textual.widgets import Button, Header, Footer, Static, Input, RadioButton, RadioSet, Log, Markdown, Label

# Python libraries
import asyncio

# Modules
import config
from utils import update_static, validate_config_params, run_cmd_async, extract_keywords



class ConnectedClients(Screen):
    """Show the currently connected clients"""

    BINDINGS = [("m", "app.pop_screen", "Back to Menu")]

    def compose(self) -> ComposeResult:
        yield Header()
        with VerticalScroll():
            yield Markdown("""
# Connected Clients

This tool will detect all clients connected to an active AP and discoverable over MQTT.
                """)
            yield Markdown(f"""
                """, id="scanres")
            yield Button("Scan", id="scan-btn", classes="buttons")
        yield Footer()


    async def run_arp_scan(self, dev):
        out,err = await run_cmd_async(f"sudo arp-scan --localnet --interface={dev}")

        lines = out.split('\n')
        start_index = -1
        end_index = -1

        for index, line in enumerate(lines):
            if line.startswith('Starting arp-scan'):
                start_index = index
            elif line.endswith('packets dropped by kernel'):
                end_index = index
                break

        macs = lines[start_index+1:end_index-1] if start_index != -1 and end_index != -1 else []
        out = {}
        for m in macs:
            row = m.split('\t')
            out[row[0]] = row[1] # IP, MAC

        return out


    async def run_get_ips(self, dev):
        out,err = await run_cmd_async('get_ips')
        lines = out.split('\n')

        macs = await self.run_arp_scan(dev)
        out = {}
        for line in lines[1:]:
            if not line: 
                continue
            line = line.split(': ')
            ip = line[1]
            name = line[0]
            if ip in macs.keys():
                out[name] = [ip, macs[ip]]

        return out


    async def check_connected_clients(self) -> None:
        out = await self.run_get_ips(config.WDEVICE)
        out_formatted = ''
        for k,v in out.items():
            out_formatted += f'| {k} | {v[0]} | {v[1]} |\n'

        self.query_one('#scanres', Markdown).update(f"""
| Name      | IP | MAC |
| ----- | ----- | ----- | 
{out_formatted}


Total clients: {len(out.keys())}
            """)


    def on_mount(self):
        pass


    def on_button_pressed(self, event: Button.Pressed) -> None:
        self.query_one('#scanres', Markdown).update("""
Running scan...
""")
        asyncio.create_task(self.check_connected_clients())




class LocalConfiguration(Screen):
    """
    Local AP configuration using hostapd or NetworkManager

    User input is collected for the SSID and password, 
    and a live log is shown to document the process.

    Using IoTempower scripts: accesspoint and accesspoint-nm
    """

    BINDINGS = [("m", "app.pop_screen", "Back to Menu")]

    def compose(self) -> ComposeResult:
        yield Header()

        with VerticalScroll():
            yield Markdown("""
# Configure Access Point
                """)

            yield Static('\tSoftware:')
            with RadioSet(id='ap_backend'):
                yield RadioButton("hostapd", id="hostapd", value=True)
                yield RadioButton("NetworkManager", value=False, id='networkmanager')

            yield Static('\tThe name for the network:')
            yield Input(placeholder="Network Name", id='ssid')
            yield Static('\tPassword for the network:')
            yield Input(placeholder="Password", password=True, id='netpass')
            yield Static('\tPassword confirmation:')
            yield Input(placeholder="Password", password=True, id='netpass2')
            yield Static(f'\n\tThe default IP address of {config.BASEIP} will be used for the network.')
            yield Button("Configure", id="config-btn", classes="buttons")
            yield Log('\t\n', id="status", auto_scroll=True)
        yield Footer()


    def on_mount(self):
        # Disable creating a new AP if one is running already
        if config.AP_RUNNING:
            log = self.query_one(Log)
            log.clear()
            log.write_line('Access point is already running on this system!')
            self.query_one('#config-btn', Button).disabled = True


    async def configure_local(self) -> None:
        # Collect user input
        backend = self.query_one('#ap_backend').pressed_button.id
        nname = self.query_one('#ssid').value
        npass = self.query_one('#netpass').value
        npass2 = self.query_one('#netpass2').value

        log = self.query_one(Log)

        # Validate
        if not validate_config_params(log, backend, nname, npass, npass2):
            return

        log.clear()
        log.write_line('Starting configuration...')
        self.query_one('#config-btn', Button).disabled = True
        
        # Apply broadcom minimal firmware patch
        if config.WIFI_CHIP == 'Broadcom' or True:
            log.write_line('Attempting to activate minimal firmware for your Wi-Fi chip...')

            out,err = await run_cmd_async("bash ./scripts/activate_brcm_minimal.sh")
            log.write_line(out)
            log.write_line(err)
        

        # Launch MQTT right away
        log.write_line('Starting MQTT server...')
        await run_cmd_async(f"bash ./scripts/iot_mqtt_start.sh", bg=True)

        # Start AP configuration
        if backend == 'hostapd':
            log.write_line('Running hostapd setup...')
            log.write_line('Please wait up to 30 seconds for the AP to start')
            await run_cmd_async(f"bash ./scripts/iot_hp_setup.sh {nname} {npass} {config.BASEIP} {config.WDEVICE}", bg=True)
        
        elif backend == 'networkmanager':
            log.write_line('Running NetworkManager setup...')
            out,err = await run_cmd_async(f"bash ./scripts/iot_nm_setup.sh {nname} {npass} {config.BASEIP} '/24' {config.WDEVICE}")
            log.write_line(out)
            if err:
                log.write_line('\nNB! Please restart your device to activate the AP!')


    def on_button_pressed(self, event: Button.Pressed) -> None:
        asyncio.create_task(self.configure_local())




class OpenWRTConfiguration(Screen):
    """
    OpenWRT router automatic configuration

    User input is collected for the SSID and password, 
    and a live log is shown to document the process.

    Using IoTempower scripts: wifi_openwrt_uci, setup_systemconf
    """

    BINDINGS = [("m", "app.pop_screen", "Back to Menu")]

    def compose(self) -> ComposeResult:
        yield Header()

        with VerticalScroll():
            yield Markdown("""
# Configure OpenWRT Router

Make sure your router has the following requirements met:

- OpenWRT version 21 and newer supported
- At least 16MB of memory available
- An SSH connection can be made to the router

Before starting, please do the following steps:

- Connect the router to Internet
- Connect this device to the router over Ethernet

Then, follow the instructions as they appear in the screen below.
            """)

            yield Static('\tThe name for the network:')
            yield Input(placeholder="Network Name", id='ssid')
            yield Static('\tPassword for the network:')
            yield Input(placeholder="Password", password=True, id='netpass')
            yield Static('\tPassword confirmation:')
            yield Input(placeholder="Password", password=True, id='netpass2')
            yield Button("Configure", id="config-btn", classes="buttons")
            yield Log('\t\n', id="status")
        yield Footer()


    async def configure_openwrt(self) -> None:
        # Collect user input
        nname = self.query_one('#ssid').value
        npass = self.query_one('#netpass').value
        npass2 = self.query_one('#netpass2').value
        

        log = self.query_one(Log)

        if not validate_config_params(log, True, nname, npass, npass2):
            return

        log.clear()
        log.write_line('Starting configuration...')
        self.query_one('#config-btn', Button).disabled = True

        out,err = await run_cmd_async(f"bash ./scripts/iot_openwrt_setup.sh {nname} {npass} {config.BASEIP}")
        log.write_line(out)
        log.write_line(err)


    def on_button_pressed(self, event: Button.Pressed) -> None:
        asyncio.create_task(self.configure_openwrt())




class APSettings(Screen):
    """
    Display AP configuration

    Show the pre-configured credentials of an AP and more general
    network settings on this system  
    """

    BINDINGS = [("m", "app.pop_screen", "Back to Menu")]

    def compose(self) -> ComposeResult:
        yield Header()
        with VerticalScroll():
            yield Markdown(f"""
# Access Point Settings
            """)
        yield Footer()


    async def read_credentials(self) -> None:
        ssid = '--'
        if config.AP_SSID:
            ssid = config.AP_SSID

        ip = '--'
        if config.AP_IP:
            ip = config.AP_IP

        ap_run = 'False'
        if config.AP_RUNNING:
            ap_run = config.AP_RUNNING
        
        self.query_one(Markdown).update(f"""
# Access Point Settings

The following Wi-Fi AP credentials can be found on your system:

| Setting      | Value |
| ----------- | ----------- |
| Network name (SSID) | {ssid} |
| IP address          | {ip}  |
| Default IP for new APs | {config.BASEIP} |
| Output wireless device | {config.WDEVICE}  |
| IoTempower activated | {config.IOTEMPOWER} |
| Access point running | {ap_run} |
| Other AP software present on system | {config.SOFTAP} |

            """)


    def on_mount(self) -> None:
        asyncio.create_task(self.read_credentials())




class WiFiChipInfo(Screen):
    """
    Display Wi-Fi hardware and system information

    Show any collected information about the current system,
    and any Wi-Fi capable devices present on it  
    """

    BINDINGS = [("m", "app.pop_screen", "Back to Menu")]

    def compose(self) -> ComposeResult:
        yield Header()
        with VerticalScroll():
            yield Markdown(f"""
# Wi-Fi Chip Information
            """)

        yield Footer()

    async def load_info(self) -> None:
        kwords = extract_keywords(config.WIFI_CHIP_FULL)
        kword_filtered = ''
        for kw in kwords:
            key = kw[0].strip()
            value = kw[1].strip()
            if value:
                if key == 'controller_subsystem' or key == 'pci_controller':
                    value = '| Full device name: |' + value + '|'

                elif key == 'kernel_driver_in_use':
                    value = '| Driver in use: |' + value + '|'

                elif key == 'cpuver':
                    value = '| System model information: |' + value + '|'

                else:
                    continue

                kword_filtered += value + '\n'

        # Display the warning about limited clients on some Broadcom and Intel chips
        chip_compat_warning = ''

        if config.WIFI_CHIP == 'Broadcom':
            chip_compat_warning = '## Notice\n\nIt appears that you have a Broadcom Wi-Fi chip with a potential limit of only **8** active clients, which means that any clients you attempt to connect after the 8th one will silently fail!\n\nA minimal firmare version is available, which might increase the client limit to **20**. The minimal firmware is activated automatically when creating an AP using this app.'
        elif config.WIFI_CHIP == 'Intel':
            chip_compat_warning = '## Notice\n\nIt appears that you have an Intel Wi-Fi chip on your system, some of which have a known upper client limit of **11** devices, which means that no more than 11 clients can be connected to your computer at the same time! Unfortunately, there is no simple workaround to this issue, so just keep this in mind.\n\nUsing an external OpenWRT-based Wi-Fi router is recommended. '


        self.query_one(Markdown).update(f"""
# Wi-Fi Chip Information

| Setting      | Value |
| ----------- | ----------- |
| Chip manufacturer | {config.WIFI_CHIP} |
{kword_filtered}| System info | {config.SYSTEM} |


{chip_compat_warning}

""")


    def on_mount(self) -> None:
        asyncio.create_task(self.load_info())




class QuitScreen(Screen):
    """
    Screen with a dialog to quit
    Used to block the app if IoTempower is not activated
    """

    def compose(self) -> ComposeResult:
        yield Grid(
            Label("Please install and activate IoTempower before using this app!", id="question"),
            Button("Quit", variant="error", id="quit"),
            Button("Cancel", variant="primary", id="cancel"),
            id="dialog",
        )

    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "quit":
            self.app.exit()
        self.app.pop_screen()


class RestartScreen(Screen):
    """
    Information dialog screen with request to restart app
    Used after disabling the AP
    """

    def compose(self) -> ComposeResult:
        yield Grid(
            Label("AP has been turned off. Please restart this app to continue!", id="question"),
            Button("Quit", variant="primary", id="quit"),
            Button("Cancel", id="cancel"),
            id="dialog",
        )

    def on_button_pressed(self, event: Button.Pressed) -> None:
        if event.button.id == "quit":
            self.app.exit()
        self.app.pop_screen()
