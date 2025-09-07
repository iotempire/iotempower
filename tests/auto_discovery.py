#!/usr/bin/env python3
"""
Auto-discovery utilities for IoTempower testing.

This module provides functions to automatically discover:
- Available platforms from lib/node_types directory
- Available devices from devices.ini file
- Generate appropriate test syntax for device/platform combinations
"""

import os
import re
import configparser
from typing import List, Dict, Tuple, Set, Optional


def discover_platforms(node_types_dir: str) -> List[str]:
    """
    Discover all available platforms from lib/node_types directory.
    
    Args:
        node_types_dir: Path to lib/node_types directory
        
    Returns:
        List of platform names that have actual implementations (not just symlinks)
    """
    platforms = []
    
    if not os.path.exists(node_types_dir):
        return platforms
        
    for item in os.listdir(node_types_dir):
        platform_path = os.path.join(node_types_dir, item)
        
        # Skip non-directories
        if not os.path.isdir(platform_path):
            continue
            
        # Check if this is a real platform or just a symlink
        # We want to include both real platforms and symlinked ones for testing
        platforms.append(item)
    
    # Sort for consistent ordering
    platforms.sort()
    return platforms


def discover_real_platforms(node_types_dir: str) -> List[str]:
    """
    Discover platforms that have actual src directories (not just symlinks).
    
    Args:
        node_types_dir: Path to lib/node_types directory
        
    Returns:
        List of platform names that have actual src implementations
    """
    platforms = []
    
    if not os.path.exists(node_types_dir):
        return platforms
        
    for item in os.listdir(node_types_dir):
        platform_path = os.path.join(node_types_dir, item)
        
        # Skip non-directories
        if not os.path.isdir(platform_path):
            continue
            
        # Check if it has a src directory (indicating real implementation)
        src_path = os.path.join(platform_path, "src")
        if os.path.exists(src_path):
            platforms.append(item)
    
    # Sort for consistent ordering
    platforms.sort()
    return platforms


def get_platform_base(node_types_dir: str, platform: str) -> str:
    """
    Get the base platform for a given platform (following symlinks).
    
    Args:
        node_types_dir: Path to lib/node_types directory
        platform: Platform name
        
    Returns:
        Base platform name (the one with actual src directory)
    """
    platform_path = os.path.join(node_types_dir, platform)
    
    # First check if the platform itself has a src directory
    src_path = os.path.join(platform_path, "src")
    if os.path.exists(src_path):
        return platform
    
    # If not, check if it's a symlink with a base
    base_file = os.path.join(platform_path, "base")
    if os.path.exists(base_file):
        if os.path.islink(base_file):
            # Follow the symlink
            target = os.readlink(base_file)
            # Extract platform name from target (e.g., "../esp32" -> "esp32")
            base_platform = os.path.basename(target)
            # Recursively check the base platform
            return get_platform_base(node_types_dir, base_platform)
        
    return platform  # Fallback to original name


def parse_devices_ini(devices_ini_path: str) -> Dict[str, Dict[str, str]]:
    """
    Parse devices.ini file to extract device definitions.
    
    Args:
        devices_ini_path: Path to devices.ini file
        
    Returns:
        Dictionary mapping device names to their configuration
    """
    if not os.path.exists(devices_ini_path):
        return {}
        
    config = configparser.ConfigParser(comment_prefixes=(';',))
    config.read(devices_ini_path)
    
    devices = {}
    for section_name in config.sections():
        devices[section_name] = dict(config[section_name])
        
    return devices


def get_platform_test_pins(node_types_dir: str, platform: str) -> Dict[str, str]:
    """
    Extract test pin definitions from a platform's pins.h file.
    
    Args:
        node_types_dir: Path to lib/node_types directory
        platform: Platform name
        
    Returns:
        Dictionary mapping test pin names to their values
    """
    # Get the base platform that has the actual src directory
    base_platform = get_platform_base(node_types_dir, platform)
    pins_file = os.path.join(node_types_dir, base_platform, "src", "pins.h")
    
    test_pins = {}
    
    if not os.path.exists(pins_file):
        return test_pins
        
    try:
        with open(pins_file, 'r') as f:
            content = f.read()
            
        # Look for IOT_TEST_* definitions
        pin_pattern = r'#define\s+(IOT_TEST_\w+)\s+(.+)'
        matches = re.findall(pin_pattern, content)
        
        for pin_name, pin_value in matches:
            # Clean up the pin value (remove comments)
            pin_value = pin_value.split('//')[0].split('/*')[0].strip()
            test_pins[pin_name] = pin_value
            
    except Exception as e:
        print(f"Warning: Could not read pins.h for {platform}: {e}")
        
    return test_pins


def generate_device_test_syntax(device_name: str, device_config: Dict[str, str], 
                              platform_pins: Dict[str, str], platform: str = "") -> Optional[str]:
    """
    Generate test syntax for a device based on its configuration and available pins.
    
    Args:
        device_name: Name of the device
        device_config: Device configuration from devices.ini
        platform_pins: Available test pins for the platform
        
    Returns:
        Test syntax string or None if cannot be generated
    """
    # Skip devices that only provide base classes
    code = device_config.get('code', '').strip()
    if '/* only base class */' in code:
        return None
        
    # Skip devices that are primarily dependencies or utilities
    skip_devices = [
        'input_base', 'rgb_base', 'pwm', 'servo_switch', 'i2c_socket', 
        'rgb_matrix', 'animator', 'display44780'
    ]
    if device_name in skip_devices:
        return None
    
    # Try to generate appropriate syntax based on device type and available pins
    example_name = "example_name"
    
    # Device-specific syntax generation
    if device_name in ['input', 'button', 'contact']:
        if 'IOT_TEST_INPUT' in platform_pins:
            return f'input({example_name}, {platform_pins["IOT_TEST_INPUT"]}, "released", "pressed");'
        elif 'IOT_TEST_DIGITAL' in platform_pins:
            return f'input({example_name}, {platform_pins["IOT_TEST_DIGITAL"]}, "released", "pressed");'
            
    elif device_name in ['output', 'led', 'out', 'relais', 'relay']:
        if 'IOT_TEST_OUTPUT' in platform_pins:
            return f'led({example_name}, {platform_pins["IOT_TEST_OUTPUT"]}, "turn on", "turn off");'
        elif 'IOT_TEST_DIGITAL' in platform_pins:
            return f'led({example_name}, {platform_pins["IOT_TEST_DIGITAL"]}, "turn on", "turn off");'
            
    elif device_name == 'analog':
        return f'analog({example_name});'
        
    elif device_name in ['dht', 'dht11', 'dht22']:
        if 'IOT_TEST_DIGITAL' in platform_pins:
            return f'dht({example_name}, {platform_pins["IOT_TEST_DIGITAL"]});'
            
    elif device_name in ['ds18b20', 'dallas']:
        if 'IOT_TEST_DIGITAL' in platform_pins:
            return f'ds18b20({example_name}, {platform_pins["IOT_TEST_DIGITAL"]});'
            
    elif device_name == 'servo':
        if 'IOT_TEST_DIGITAL' in platform_pins:
            return f'servo({example_name}, {platform_pins["IOT_TEST_DIGITAL"]}, 800);'
            
    elif device_name == 'edge_counter':
        if 'IOT_TEST_DIGITAL' in platform_pins:
            return f'edge_counter({example_name}, {platform_pins["IOT_TEST_DIGITAL"]});'
            
    elif device_name in ['hcsr04', 'ultra_sonic_distance']:
        if 'IOT_TEST_DIGITAL' in platform_pins and 'IOT_TEST_DIGITAL_2' in platform_pins:
            return f'hcsr04(distance, {platform_pins["IOT_TEST_DIGITAL"]}, {platform_pins["IOT_TEST_DIGITAL_2"]});'
            
    elif device_name == 'hx711':
        if 'IOT_TEST_DIGITAL' in platform_pins and 'IOT_TEST_DIGITAL_2' in platform_pins:
            return f'hx711({example_name}, {platform_pins["IOT_TEST_DIGITAL"]}, {platform_pins["IOT_TEST_DIGITAL_2"]}, 419.0, true);'
            
    elif device_name == 'rgb_strip':
        if 'IOT_TEST_DIGITAL' in platform_pins:
            return f'rgb_strip({example_name}, 7, WS2812B, {platform_pins["IOT_TEST_DIGITAL"]}, GRB);'
            
    elif device_name == 'rgb_strip_bus':
        if 'IOT_TEST_NEOPIXEL_CONTROL_METHOD' in platform_pins and 'IOT_TEST_NEOPIXEL_TESTPIN' in platform_pins:
            return f'rgb_strip_bus({example_name}, 7, F_GRB, {platform_pins["IOT_TEST_NEOPIXEL_CONTROL_METHOD"]}, {platform_pins["IOT_TEST_NEOPIXEL_TESTPIN"]});'
        elif 'IOT_TEST_DIGITAL' in platform_pins:
            # Fallback for platforms without specific neopixel pins
            return f'rgb_strip_bus({example_name}, 7, F_GRB, NeoEsp8266Uart1800KbpsMethod, {platform_pins["IOT_TEST_DIGITAL"]});'
            
    elif device_name == 'rgb_single':
        if ('IOT_TEST_DIGITAL' in platform_pins and 'IOT_TEST_DIGITAL_2' in platform_pins and 
            'IOT_TEST_OUTPUT' in platform_pins and 'IOT_TEST_INPUT' in platform_pins):
            return f'rgb_single(r0, {platform_pins["IOT_TEST_DIGITAL"]}, {platform_pins["IOT_TEST_DIGITAL_2"]}, {platform_pins["IOT_TEST_OUTPUT"]}, true);'
            
    # I2C devices - no pins needed
    elif device_name in ['bmp180', 'bmp085', 'bmp280', 'gyro6050', 'gyro', 'gyro9250', 
                        'gesture_apds9960', 'sgp30', 'vl53l0x', 'tof_distance',
                        'bh1750', 'tsl2561', 'mpr121', 'ens16x', 'scd4x']:
        return f'{device_name}({example_name});'
        
    # RFID device  
    elif device_name in ['mfrc522', 'rfid']:
        return f'mfrc522({example_name});'
    
    # Platform-specific devices - only for their respective platforms
    elif device_name == 'm5stickc_imu' and 'm5stickc' in platform:
        return f'm5stickc_imu({example_name});'
    elif device_name == 'm5stickc_display' and 'm5stickc' in platform:
        return f'm5stickc_display({example_name});'
    elif device_name == 'cyd_display' and 'cyd' in platform:
        return f'cyd_display({example_name});'
        
    # Display devices (generic)
    elif device_name == 'display':
        return f'display({example_name});'
        
    # IR devices
    elif device_name == 'ir_receiver':
        if 'IOT_TEST_DIGITAL' in platform_pins:
            return f'ir_receiver({example_name}, {platform_pins["IOT_TEST_DIGITAL"]});'
    elif device_name == 'ir_sender':
        if 'IOT_TEST_DIGITAL' in platform_pins:
            return f'ir_sender({example_name}, {platform_pins["IOT_TEST_DIGITAL"]});'
    
    # If we couldn't generate syntax, return None
    return None


def generate_auto_combinations(iotempower_root: str) -> List[Tuple[str, str, str]]:
    """
    Generate all valid board/device/syntax combinations automatically.
    
    Args:
        iotempower_root: Path to IoTempower root directory
        
    Returns:
        List of tuples (board, device_name, example_syntax)
    """
    node_types_dir = os.path.join(iotempower_root, "lib", "node_types")
    devices_ini_path = os.path.join(node_types_dir, "esp", "src", "devices.ini")
    
    # Discover platforms and devices
    platforms = discover_platforms(node_types_dir)
    devices = parse_devices_ini(devices_ini_path)
    
    combinations = []
    
    # Skip devices that are primarily dependencies or utilities
    skip_devices = [
        'input_base', 'rgb_base', 'pwm', 'servo_switch', 'i2c_socket', 
        'rgb_matrix', 'animator', 'display44780'
    ]
    
    for platform in platforms:
        # Get test pins for this platform
        platform_pins = get_platform_test_pins(node_types_dir, platform)
        
        # Skip platforms without any test pins defined
        if not platform_pins:
            print(f"Warning: No test pins defined for platform {platform}, skipping")
            continue
            
        for device_name, device_config in devices.items():
            # Generate test syntax
            syntax = generate_device_test_syntax(device_name, device_config, platform_pins, platform)
            
            if syntax:
                combinations.append((platform, device_name, syntax))
            else:
                # Check if device has aliases and try them
                aliases = device_config.get('aliases', '').split()
                found_syntax = False
                for alias in aliases:
                    alias_syntax = generate_device_test_syntax(alias, device_config, platform_pins, platform)
                    if alias_syntax:
                        combinations.append((platform, alias, alias_syntax))
                        found_syntax = True
                        break
                        
                if not found_syntax:
                    if device_name not in skip_devices:  # Only warn for devices we expect to support
                        print(f"Warning: Could not generate syntax for device {device_name} on platform {platform}")
    
    return combinations


if __name__ == "__main__":
    # Test the auto-discovery functions
    import sys
    
    if len(sys.argv) > 1:
        iotempower_root = sys.argv[1]
    else:
        iotempower_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    
    print(f"IoTempower root: {iotempower_root}")
    
    # Test platform discovery
    node_types_dir = os.path.join(iotempower_root, "lib", "node_types")
    platforms = discover_platforms(node_types_dir)
    print(f"Discovered {len(platforms)} platforms: {platforms}")
    
    # Test device discovery
    devices_ini_path = os.path.join(node_types_dir, "esp", "src", "devices.ini")
    devices = parse_devices_ini(devices_ini_path)
    print(f"Discovered {len(devices)} devices: {list(devices.keys())}")
    
    # Test pin discovery for a few platforms
    for platform in platforms[:3]:
        pins = get_platform_test_pins(node_types_dir, platform)
        print(f"Platform {platform} test pins: {pins}")
    
    # Test combination generation
    combinations = generate_auto_combinations(iotempower_root)
    print(f"Generated {len(combinations)} test combinations")
    
    # Show a few examples
    for i, (platform, device, syntax) in enumerate(combinations[:5]):
        print(f"  {platform} + {device}: {syntax}")