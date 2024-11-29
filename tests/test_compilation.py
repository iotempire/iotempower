import os
import shutil
import subprocess
import pytest

from tests.conf_data import isolated_combinations_to_test

local_dir = os.getenv("IOTEMPOWER_LOCAL", "")
test_dir = os.path.join(local_dir, ".tests", "test_compilation")
cache_dir = os.path.join(local_dir, "compile_cache", "platformio")

# Cleaning the cache whenever the tests are run
shutil.rmtree(cache_dir, ignore_errors=True)

os.makedirs(test_dir, exist_ok=True)
open(f"{test_dir}/system.conf", "w").close()
remove_cache_flag = os.getenv("IOTEMPOWER_TEST_REMOVE_CACHE", "")

@pytest.fixture(scope="module", params=isolated_combinations_to_test)
def parametrize_board_device(request):
    boards = request.config.getoption("--boards")
    devices = request.config.getoption("--devices")
    board, device, example_syntax = request.param
    
    if (boards and board not in boards.split(",")) or (devices and device not in devices.split(",")):
        pytest.skip("Skipping due to board or device not selected.")
    
    return board, device, example_syntax

def test_compilation_isolated(parametrize_board_device):
    board_name, device_name, example_syntax = parametrize_board_device
    node_directory = f"{test_dir}/node"
    os.makedirs(node_directory, exist_ok=True)
    
    with open(f"{node_directory}/node.conf", "w") as f:
        f.write(f'board="{board_name}"')
    
    with open(f"{node_directory}/setup.cpp", "w") as f:
        f.write(example_syntax)
    
    try:
        subprocess.check_call(["compile"], cwd=node_directory)
    except subprocess.CalledProcessError:
        # assert False, f"Failed to compile {device_name} for {board_name}"
        pytest.fail(f"Failed to compile {device_name} for {board_name}")
    finally:
        shutil.rmtree(node_directory, ignore_errors=True)
        if remove_cache_flag.lower() == "true":
            shutil.rmtree(cache_dir, ignore_errors=True)
