#!/usr/bin/env python3
"""
Auto-discovery utilities for IoTempower testing.

This module provides functions to automatically discover:
- Available platforms from lib/node_types directory
- Available devices from devices.ini file  
- Test pin definitions from testdefs.h files with base inheritance support
- Generate appropriate test syntax for device/platform combinations

The system now uses the new testdefs.h files instead of extracting from pins.h,
and implements proper inheritance through the base directory symlink system
similar to the logic in bin/prepare_build_dir script.
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


def get_platform_testdefs_through_base(node_types_dir: str, platform: str) -> Dict[str, str]:
    """
    Extract test pin definitions from a platform's testdefs.h file, following base inheritance.
    Similar to the logic in bin/prepare_build_dir script.
    
    Args:
        node_types_dir: Path to lib/node_types directory
        platform: Platform name
        
    Returns:
        Dictionary mapping test pin names to their values
    """
    platform_path = os.path.join(node_types_dir, platform)
    
    def read_testdefs_recursive(current_path: str) -> Dict[str, str]:
        """Recursively read testdefs.h following base directory inheritance"""
        test_pins = {}
        
        # First, check if there's a base directory/link and recurse into it
        base_path = os.path.join(current_path, "base")
        if os.path.exists(base_path):
            if os.path.islink(base_path):
                # Follow the symlink
                target = os.readlink(base_path)
                if target.startswith('../'):
                    # Relative path, resolve it
                    base_platform_path = os.path.join(current_path, target)
                    base_platform_path = os.path.normpath(base_platform_path)
                else:
                    base_platform_path = target
                
                # Recursively get base platform test definitions
                base_test_pins = read_testdefs_recursive(base_platform_path)
                test_pins.update(base_test_pins)
            elif os.path.isdir(base_path):
                # If base is a directory, recurse into it  
                base_test_pins = read_testdefs_recursive(base_path)
                test_pins.update(base_test_pins)
        
        # Now read testdefs.h in current platform (overrides base definitions)
        testdefs_file = os.path.join(current_path, "src", "testdefs.h")
        if os.path.exists(testdefs_file):
            try:
                with open(testdefs_file, 'r') as f:
                    content = f.read()
                    
                # Look for IOT_TEST_* definitions
                pin_pattern = r'#define\s+(IOT_TEST_\w+)\s+(.+)'
                matches = re.findall(pin_pattern, content)
                
                for pin_name, pin_value in matches:
                    # Clean up the pin value (remove comments)
                    pin_value = pin_value.split('//')[0].split('/*')[0].strip()
                    test_pins[pin_name] = pin_value
                    
            except Exception as e:
                print(f"Warning: Could not read testdefs.h in {current_path}: {e}")
                
        return test_pins
    
    # Start the recursive search from the platform directory
    return read_testdefs_recursive(platform_path)


def can_device_be_tested_on_platform(device_name: str, device_config: Dict[str, str], 
                                   platform_testdefs: Dict[str, str], platform: str) -> bool:
    """
    Determine if a device can be tested on a platform based on available testdefs.
    
    Args:
        device_name: Name of the device
        device_config: Device configuration from devices.ini
        platform_testdefs: Available test pin definitions for the platform
        platform: Platform name
        
    Returns:
        True if device can be tested on this platform, False otherwise
    """
    # Skip devices that only provide base classes
    code = device_config.get('code', '').strip()
    if '/* only base class */' in code:
        return False
        
    # Skip devices that are primarily dependencies or utilities
    skip_devices = [
        'input_base', 'rgb_base', 'pwm', 'servo_switch', 'i2c_socket', 
        'rgb_matrix', 'animator', 'display44780'
    ]
    if device_name in skip_devices:
        return False
    
    # Check if platform has any test definitions at all
    if not platform_testdefs:
        return False
    
    # Device-specific requirements
    if device_name in ['input', 'button', 'contact']:
        return 'IOT_TEST_INPUT' in platform_testdefs or 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name in ['output', 'led', 'out', 'relais', 'relay']:
        return 'IOT_TEST_OUTPUT' in platform_testdefs or 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'analog':
        return True  # Analog typically doesn't need specific test pins
        
    elif device_name in ['dht', 'dht11', 'dht22']:
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name in ['ds18b20', 'dallas']:
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'servo':
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'edge_counter':
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name in ['hcsr04', 'ultra_sonic_distance']:
        return 'IOT_TEST_DIGITAL' in platform_testdefs and 'IOT_TEST_DIGITAL_2' in platform_testdefs
        
    elif device_name == 'hx711':
        return 'IOT_TEST_DIGITAL' in platform_testdefs and 'IOT_TEST_DIGITAL_2' in platform_testdefs
        
    elif device_name == 'rgb_strip':
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'rgb_strip_bus':
        # Prefer platform-specific neopixel pins, but fall back to digital
        return ('IOT_TEST_NEOPIXEL_CONTROL_METHOD' in platform_testdefs and 
                'IOT_TEST_NEOPIXEL_TESTPIN' in platform_testdefs) or 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'rgb_single':
        return ('IOT_TEST_DIGITAL' in platform_testdefs and 'IOT_TEST_DIGITAL_2' in platform_testdefs and 
                'IOT_TEST_OUTPUT' in platform_testdefs and 'IOT_TEST_INPUT' in platform_testdefs)
        
    # I2C devices - generally don't need specific pins but need platform support
    elif device_name in ['bmp180', 'bmp085', 'bmp280', 'gyro6050', 'gyro', 'gyro9250', 
                        'gesture_apds9960', 'sgp30', 'vl53l0x', 'tof_distance',
                        'bh1750', 'tsl2561', 'mpr121', 'ens16x', 'scd4x']:
        return True
        
    # RFID device  
    elif device_name in ['mfrc522', 'rfid']:
        return True
    
    # Platform-specific devices - only for their respective platforms
    elif device_name == 'm5stickc_imu':
        return 'm5stickc' in platform
    elif device_name == 'm5stickc_display':
        return 'm5stickc' in platform
    elif device_name == 'cyd_display':
        return 'cyd' in platform
        
    # Display devices (generic)
    elif device_name == 'display':
        return True
        
    # IR devices
    elif device_name in ['ir_receiver', 'ir_sender']:
        return 'IOT_TEST_DIGITAL' in platform_testdefs
    
    # Default: can test if platform has basic test pins
    return 'IOT_TEST_DIGITAL' in platform_testdefs or 'IOT_TEST_OUTPUT' in platform_testdefs


    # Default: can test if platform has basic test pins
    return 'IOT_TEST_DIGITAL' in platform_testdefs or 'IOT_TEST_OUTPUT' in platform_testdefs


def generate_device_test_syntax(device_name: str, device_config: Dict[str, str], 
                              platform_testdefs: Dict[str, str], platform: str = "") -> Optional[str]:
    """
    Determine if a device can be tested on a platform based on available testdefs.
    
    Args:
        device_name: Name of the device
        device_config: Device configuration from devices.ini
        platform_testdefs: Available test pin definitions for the platform
        platform: Platform name
        
    Returns:
        True if device can be tested on this platform, False otherwise
    """
    # Skip devices that only provide base classes
    code = device_config.get('code', '').strip()
    if '/* only base class */' in code:
        return False
        
    # Skip devices that are primarily dependencies or utilities
    skip_devices = [
        'input_base', 'rgb_base', 'pwm', 'servo_switch', 'i2c_socket', 
        'rgb_matrix', 'animator', 'display44780'
    ]
    if device_name in skip_devices:
        return False
    
    # Check if platform has any test definitions at all
    if not platform_testdefs:
        return False
    
    # Device-specific requirements
    if device_name in ['input', 'button', 'contact']:
        return 'IOT_TEST_INPUT' in platform_testdefs or 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name in ['output', 'led', 'out', 'relais', 'relay']:
        return 'IOT_TEST_OUTPUT' in platform_testdefs or 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'analog':
        return True  # Analog typically doesn't need specific test pins
        
    elif device_name in ['dht', 'dht11', 'dht22']:
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name in ['ds18b20', 'dallas']:
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'servo':
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'edge_counter':
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name in ['hcsr04', 'ultra_sonic_distance']:
        return 'IOT_TEST_DIGITAL' in platform_testdefs and 'IOT_TEST_DIGITAL_2' in platform_testdefs
        
    elif device_name == 'hx711':
        return 'IOT_TEST_DIGITAL' in platform_testdefs and 'IOT_TEST_DIGITAL_2' in platform_testdefs
        
    elif device_name == 'rgb_strip':
        return 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'rgb_strip_bus':
        # Prefer platform-specific neopixel pins, but fall back to digital
        return ('IOT_TEST_NEOPIXEL_CONTROL_METHOD' in platform_testdefs and 
                'IOT_TEST_NEOPIXEL_TESTPIN' in platform_testdefs) or 'IOT_TEST_DIGITAL' in platform_testdefs
        
    elif device_name == 'rgb_single':
        return ('IOT_TEST_DIGITAL' in platform_testdefs and 'IOT_TEST_DIGITAL_2' in platform_testdefs and 
                'IOT_TEST_OUTPUT' in platform_testdefs and 'IOT_TEST_INPUT' in platform_testdefs)
        
    # I2C devices - generally don't need specific pins but need platform support
    elif device_name in ['bmp180', 'bmp085', 'bmp280', 'gyro6050', 'gyro', 'gyro9250', 
                        'gesture_apds9960', 'sgp30', 'vl53l0x', 'tof_distance',
                        'bh1750', 'tsl2561', 'mpr121', 'ens16x', 'scd4x']:
        return True
        
    # RFID device  
    elif device_name in ['mfrc522', 'rfid']:
        return True
    
    # Platform-specific devices - only for their respective platforms
    elif device_name == 'm5stickc_imu':
        return 'm5stickc' in platform
    elif device_name == 'm5stickc_display':
        return 'm5stickc' in platform
    elif device_name == 'cyd_display':
        return 'cyd' in platform
        
    # Display devices (generic)
    elif device_name == 'display':
        return True
        
    # IR devices
    elif device_name in ['ir_receiver', 'ir_sender']:
        return 'IOT_TEST_DIGITAL' in platform_testdefs
    
    # Default: can test if platform has basic test pins
    return 'IOT_TEST_DIGITAL' in platform_testdefs or 'IOT_TEST_OUTPUT' in platform_testdefs


def generate_device_test_syntax(device_name: str, device_config: Dict[str, str], 
                              platform_testdefs: Dict[str, str], platform: str = "") -> Optional[str]:
    """
    Generate test syntax for a device based on its configuration and available test definitions.
    
    Args:
        device_name: Name of the device
        device_config: Device configuration from devices.ini
        platform_testdefs: Available test pin definitions for the platform
        
    Returns:
        Test syntax string or None if cannot be generated
    """
    # Use the new can_device_be_tested_on_platform function to check compatibility first
    if not can_device_be_tested_on_platform(device_name, device_config, platform_testdefs, platform):
        return None
    
    # Try to generate appropriate syntax based on device type and available test definitions
    example_name = "example_name"
    
    # Device-specific syntax generation
    if device_name in ['input', 'button', 'contact']:
        if 'IOT_TEST_INPUT' in platform_testdefs:
            return f'input({example_name}, {platform_testdefs["IOT_TEST_INPUT"]}, "released", "pressed");'
        elif 'IOT_TEST_DIGITAL' in platform_testdefs:
            return f'input({example_name}, {platform_testdefs["IOT_TEST_DIGITAL"]}, "released", "pressed");'
            
    elif device_name in ['output', 'led', 'out', 'relais', 'relay']:
        if 'IOT_TEST_OUTPUT' in platform_testdefs:
            return f'led({example_name}, {platform_testdefs["IOT_TEST_OUTPUT"]}, "turn on", "turn off");'
        elif 'IOT_TEST_DIGITAL' in platform_testdefs:
            return f'led({example_name}, {platform_testdefs["IOT_TEST_DIGITAL"]}, "turn on", "turn off");'
            
    elif device_name == 'analog':
        return f'analog({example_name});'
        
    elif device_name in ['dht', 'dht11', 'dht22']:
        if 'IOT_TEST_DIGITAL' in platform_testdefs:
            return f'dht({example_name}, {platform_testdefs["IOT_TEST_DIGITAL"]});'
            
    elif device_name in ['ds18b20', 'dallas']:
        if 'IOT_TEST_DIGITAL' in platform_testdefs:
            return f'ds18b20({example_name}, {platform_testdefs["IOT_TEST_DIGITAL"]});'
            
    elif device_name == 'servo':
        if 'IOT_TEST_DIGITAL' in platform_testdefs:
            return f'servo({example_name}, {platform_testdefs["IOT_TEST_DIGITAL"]}, 800);'
            
    elif device_name == 'edge_counter':
        if 'IOT_TEST_DIGITAL' in platform_testdefs:
            return f'edge_counter({example_name}, {platform_testdefs["IOT_TEST_DIGITAL"]});'
            
    elif device_name in ['hcsr04', 'ultra_sonic_distance']:
        if 'IOT_TEST_DIGITAL' in platform_testdefs and 'IOT_TEST_DIGITAL_2' in platform_testdefs:
            return f'hcsr04(distance, {platform_testdefs["IOT_TEST_DIGITAL"]}, {platform_testdefs["IOT_TEST_DIGITAL_2"]});'
            
    elif device_name == 'hx711':
        if 'IOT_TEST_DIGITAL' in platform_testdefs and 'IOT_TEST_DIGITAL_2' in platform_testdefs:
            return f'hx711({example_name}, {platform_testdefs["IOT_TEST_DIGITAL"]}, {platform_testdefs["IOT_TEST_DIGITAL_2"]}, 419.0, true);'
            
    elif device_name == 'rgb_strip':
        if 'IOT_TEST_DIGITAL' in platform_testdefs:
            return f'rgb_strip({example_name}, 7, WS2812B, {platform_testdefs["IOT_TEST_DIGITAL"]}, GRB);'
            
    elif device_name == 'rgb_strip_bus':
        if 'IOT_TEST_NEOPIXEL_CONTROL_METHOD' in platform_testdefs and 'IOT_TEST_NEOPIXEL_TESTPIN' in platform_testdefs:
            return f'rgb_strip_bus({example_name}, 7, F_GRB, {platform_testdefs["IOT_TEST_NEOPIXEL_CONTROL_METHOD"]}, {platform_testdefs["IOT_TEST_NEOPIXEL_TESTPIN"]});'
        elif 'IOT_TEST_DIGITAL' in platform_testdefs:
            # Fallback for platforms without specific neopixel pins
            return f'rgb_strip_bus({example_name}, 7, F_GRB, NeoEsp8266Uart1800KbpsMethod, {platform_testdefs["IOT_TEST_DIGITAL"]});'
            
    elif device_name == 'rgb_single':
        if ('IOT_TEST_DIGITAL' in platform_testdefs and 'IOT_TEST_DIGITAL_2' in platform_testdefs and 
            'IOT_TEST_OUTPUT' in platform_testdefs and 'IOT_TEST_INPUT' in platform_testdefs):
            return f'rgb_single(r0, {platform_testdefs["IOT_TEST_DIGITAL"]}, {platform_testdefs["IOT_TEST_DIGITAL_2"]}, {platform_testdefs["IOT_TEST_OUTPUT"]}, true);'
            
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
        if 'IOT_TEST_DIGITAL' in platform_testdefs:
            return f'ir_receiver({example_name}, {platform_testdefs["IOT_TEST_DIGITAL"]});'
    elif device_name == 'ir_sender':
        if 'IOT_TEST_DIGITAL' in platform_testdefs:
            return f'ir_sender({example_name}, {platform_testdefs["IOT_TEST_DIGITAL"]});'
    
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
    
    for platform in platforms:
        # Get test definitions for this platform using the new testdefs system
        platform_testdefs = get_platform_testdefs_through_base(node_types_dir, platform)
        
        # Skip platforms without any test definitions
        if not platform_testdefs:
            print(f"Warning: No test definitions found for platform {platform}, skipping")
            continue
            
        for device_name, device_config in devices.items():
            # Check if device can be tested on this platform
            if can_device_be_tested_on_platform(device_name, device_config, platform_testdefs, platform):
                # Generate test syntax
                syntax = generate_device_test_syntax(device_name, device_config, platform_testdefs, platform)
                
                if syntax:
                    combinations.append((platform, device_name, syntax))
                else:
                    # Check if device has aliases and try them
                    aliases = device_config.get('aliases', '').split()
                    for alias in aliases:
                        if can_device_be_tested_on_platform(alias, device_config, platform_testdefs, platform):
                            alias_syntax = generate_device_test_syntax(alias, device_config, platform_testdefs, platform)
                            if alias_syntax:
                                combinations.append((platform, alias, alias_syntax))
                                break
    
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
        testdefs = get_platform_testdefs_through_base(node_types_dir, platform)
        print(f"Platform {platform} test definitions: {testdefs}")
    
    # Test combination generation
    combinations = generate_auto_combinations(iotempower_root)
    print(f"Generated {len(combinations)} test combinations")
    
    # Show a few examples
    for i, (platform, device, syntax) in enumerate(combinations[:5]):
        print(f"  {platform} + {device}: {syntax}")