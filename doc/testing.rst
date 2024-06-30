Testing IOT empower
====================

Overview
--------

The **Testing IOT empower** project focuses on ensuring the robustness and reliability of IOT empower applications. This documentation provides details on the structure and contents of the testing folder, along with instructions on running the tests.

Contents
--------

The testing folder for the IOT empower project is located at `iot/tests`. It consists of four files. The files that start with the `test` prefix are actual test files, and there is one additional file named `data.py` which contains all configurations and test cases for these tests.

File Descriptions
-----------------

1. **data.py**: Contains configurations and test cases used by the test files. This file includes several variables:
    - `boards`: A list of devices.
    - `devices`: A list of sensor names and syntaxes that can be compiled with all boards.
    - `isolated_combinations_to_test`: Combinations of sensors and boards that can only run on specific boards.
    - `gateway_host`: Hostname for the Raspberry Pi in the local network, typically `iotgateway`.
    - `cases_for_deployment`: A list of tuples where each tuple represents a separate test. Each tuple contains:
        - The board name for the `node.conf` file.
        - A list of lines for the `setup.cpp` file.
        - A list of tuples, each containing an MQTT topic and the expected payload from that topic.

2. **test_installations.py**: Checks if your installations are complete. This test ensures that all the selected dependencies are correctly installed and configured so that IOT empower works properly. The input file for this test is `installation.json`, which is generated during the installation period and saves your selection of dependencies. Some dependencies are not crucial and are only installed for specific features. If you are using a Docker image or Raspberry Pi image, there is no need to run these tests as they have already been checked.

    - **Purpose**: To verify that all dependencies are installed and configured correctly.
    - **Input**: `installation.json`, generated during installation.
    - **Note**: Not necessary if using Docker or Raspberry Pi images.

3. **test_compile.py**: Tests the compilation process. The compilation is a process where the framework takes node configurations and generates binary files to upload to the actual device. This process can be done in a fully isolated manner. In `data.py`, there is an example where:
   - `boards` variable lists all devices.
   - `devices` variable lists sensor names and syntaxes that can be compiled with all boards.
   - `isolated_combinations_to_test` variable contains combinations of sensors and boards that can only run on specific boards.

    - **Purpose**: To verify that node configurations can be compiled into binary files for devices.
    - **Details**:
      - Uses `boards` and `devices` lists to create all possible combinations.
      - `isolated_combinations_to_test` contains specific sensor-board combinations.
    - **Example**: Refer to `data.py` for how devices and sensors are combined.

4. **test_deployment.py**: Requires a Raspberry Pi device to be connected to the network. Normally, the Pi registers itself in the local network under the name `iotgateway`. If this is not the case for you, change the `gateway_host` variable in `data.py`. You should configure SSH connection before running tests (see details at "https://www.ssh.com/academy/ssh/copy-id"). The actual testing parameters are assigned to the `cases_for_deployment` variable, which is a list of tuples. Each tuple represents a separate test and holds three values:
   - The board name for `node.conf` file.
   - A list of lines for `setup.cpp` file.
   - A list of tuples where each tuple contains an MQTT topic and the expected payload from that topic.

    - **Purpose**: To verify that the deployment process works correctly on a Raspberry Pi.
    - **Details**:
      - Change `gateway_host` in `data.py` if `iotgateway` is not used.
      - Configure SSH as described at "https://www.ssh.com/academy/ssh/copy-id".
      - `cases_for_deployment` lists test cases, each with a board name, setup lines, and MQTT topic-payload expectations.

Running Tests
-------------

To run the tests:

1. Navigate to the `tests` folder.
2. Activate the IOT environment by typing `iot` (if it hasn't been activated already).
3. Use the `pytest` testing library to run the tests. The command to run all tests is:

   .. code-block:: shell

       pytest -s -v

4. To run a specific test, append the test file name to the command. For example, to run the deployment tests:

   .. code-block:: shell

       pytest -s -v test_deployment.py

Conclusion
----------

The **Testing IOT empower** project ensures thorough testing of the IOT empower application, covering installations, compilation, and deployment. This documentation serves as a guide to understanding the structure, content, and usage of the testing folder.
