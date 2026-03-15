import os
import shutil
import subprocess
import pytest

local_dir = os.getenv("IOTEMPOWER_LOCAL", "")
test_dir = os.path.join(local_dir, ".tests", "test_compilation")
cache_dir = os.path.join(local_dir, "compile_cache", "platformio")

# Cleaning the cache whenever the tests are run
shutil.rmtree(cache_dir, ignore_errors=True)

os.makedirs(test_dir, exist_ok=True)
open(f"{test_dir}/system.conf", "w").close()
remove_cache_flag = os.getenv("IOTEMPOWER_TEST_REMOVE_CACHE", "")

@pytest.fixture(scope="module")
def parametrize_board_device(request):
    return request.param

def test_compilation_isolated(parametrize_board_device):
    board_name, device_name, example_syntax = parametrize_board_device
    print(f"\nCompiling device={device_name} for board={board_name}")
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
