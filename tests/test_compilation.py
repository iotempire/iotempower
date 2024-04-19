import os
import shutil
import subprocess

import pytest

from tests.data import isolated_combinations_to_test

# TODO find platformio installation and move it to the requirements.txt -> Platformio is already in requirement.txt
local_dir = os.getenv("IOTEMPOWER_LOCAL", "")
test_dir = os.path.join(local_dir, ".tests", "test_compilation")
cache_dir = os.path.join(local_dir, "compile_cache", "platformio")

# Cleaning the cache whenever the tests are run
shutil.rmtree(cache_dir, ignore_errors=True)

os.makedirs(test_dir, exist_ok=True)
open(f"{test_dir}/system.conf", "w").close()
remove_cache_flag = os.getenv("IOTEMPOWER_TEST_REMOVE_CACHE", "")


# TODO
# Node name should be randomized hash
# Run with multiprocessing https://stackoverflow.com/questions/29560212/parallely-running-parameterized-tests-in-pytest
@pytest.mark.parametrize("board_name, device_name, example_syntax", isolated_combinations_to_test)
def test_compilation_isolated(board_name: str, device_name: str, example_syntax: str):
    node_directory = f"{test_dir}/node"
    os.makedirs(node_directory, exist_ok=True)
    with open(f"{node_directory}/node.conf", "w") as f:
        f.write(f'board="{board_name}"')
    with open(f"{node_directory}/setup.cpp", "w") as f:
        f.write(example_syntax)
    try:
        subprocess.check_call(["compile"], cwd=node_directory)
    except subprocess.CalledProcessError:
        assert False, f"Failed to compile {device_name} for {board_name}"
    finally:
        shutil.rmtree(node_directory, ignore_errors=True)
        if remove_cache_flag.lower() == "true":
            shutil.rmtree(cache_dir, ignore_errors=True)


# def test_compilation_integrated():
#     cluster_folder = f"{test_dir}/cluster"
#     os.makedirs(cluster_folder, exist_ok=True)
