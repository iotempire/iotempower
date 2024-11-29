Testing IoTempower
====================

Overview
--------

The **Testing IoTempower** project focuses on ensuring the robustness and reliability of IoTempower applications. This documentation provides details on the structure and contents of the testing folder, along with instructions on running the tests.

Contents
--------

The testing folder for the IoTempower project is located at `iot/tests`. The files that start with the `test` prefix are actual test files. Other files contain shared functionalities and configuration for tests.

File Descriptions
-----------------

1. **conf_data.py**: Contains configurations and test cases used by the test files. This file includes several variables:
    - `packages`: A list of dictionaries which hold values for dependency names and nature of the dependency. Important for `test_installations`.
    - `boards`: A list of devices to test compilation. All boards and iot devices paired individually to test compilation of all combinations.
    - `devices`: A list of sensor names and syntaxes. All boards and iot devices paired individually to test compilation of all combinations.
    - `isolated_combinations_to_test`: Combinations of sensors and boards that can only run on specific boards.
    - `gateway_host`: Hostname for the Raspberry Pi in the local network, typically `iotgateway`. Important for `test_deploy` and `test_hardware` since these test should be run on actual Raspberry Pi device.
    - `default_username`: Default username for ssh connection into Raspberry Pi.
    - `mqtt_listen_period`: Number of seconds to listen MQTT channel to verify that expected message is published under expected topic.
    - `local_bind_mqtt_port`: Since we are connecting Raspberry Pi as device in a local network, it doesn't expose it's MQTT port(1883 by default). That's why we are ssh tunneling(binding) that port into one of our local ports.
    - `private_key_file_path`: Raspberry Pi should be configured before running tests to know our private key for making smooth ssh connection experience. Change this value in case you have configured another key with Pi.
    - `nodes_folder_path`: Full path of the folder for running deployment and hardware testing.
    - `tested_node_name`: Node that's been tested folder name.
    - `tester_node_name`: Node that's testing  folder name.
    - `cases_for_deployment`: A list of tuples where each tuple represents a separate test. Each tuple contains:
        - The board name for the `node.conf` file.
        - A list of lines for the `setup.cpp` file.
        - A list of tuples, each containing an MQTT topic and the expected payload from that topic.
    - `cases_for_hardware`: A list of tuples where each tuple represents a separate test. Each tuple contains:
        - Pair of board names for the tested and tester node which goes `node.conf` file of each node respectively.
        - Pair of list of syntax lines ofr the tested and tester which goes to `setup.cpp` file of each node respectively.
        - Tuple which holds three lists of tuples:
            - Tested node topic and initial status messages emitted by tested node.
            - Set topic and according set commands to trigger the tester node.
            - Tested node topic and the expected status messages after tester trigger.
    - `deploy_device_address`, `tester_device_address`, `tested_device_address`: Explained in `Hardware configuration`_ part below.

2. **test_installations.py**: Checks if your installations are complete. This test ensures that all the selected dependencies are correctly installed and configured so that IoTempower works properly. The input file for this test is `installation.json`, which is generated during the installation period and saves your selection of dependencies. Some dependencies are not crucial and are only installed for specific features. If you are using a Docker image or Raspberry Pi image, there is no need to run these tests as they have already been checked.
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

4. **test_deployment.py**: Requires a Raspberry Pi device to be connected to the network. Normally, the Pi registers itself in the local network under the name `iotgateway`. If this is not the case for you, change the `gateway_host` variable in `conf_data.py`. You should configure SSH connection before running tests (see details at "https://www.ssh.com/academy/ssh/copy-id"). The actual testing parameters are assigned to the `cases_for_deployment` variable, which is a list of tuples. Each tuple represents a separate test and holds three values:
   - The board name for `node.conf` file.
   - A list of lines for `setup.cpp` file.
   - A list of tuples where each tuple contains an MQTT topic and the expected payload from that topic.

    - **Purpose**: To verify that the deployment process works correctly on a Raspberry Pi.
    - **Details**:
      - Change `gateway_host` in `data.py` if `iotgateway` is not used.
      - Configure SSH as described at "https://www.ssh.com/academy/ssh/copy-id".
      - `cases_for_deployment` lists test cases, each with a board name, setup lines, and MQTT topic-payload expectations.

5. **test_hardware.py**: The statements for `test_deployment` is also applicable for this test. And you should have at least to dongles connected PI for tester and tested roles.
    - **Purpose**: To verify that after the deployment process the physical changes also takes effect rather than seeing those changes in serial or MQTT channel.
    - **Details**: `cases_for_hardware` lists test cases and new tests cases should be added to that list.

Running Tests
-------------

To run the tests:

1. Navigate to the `tests` folder.
2. Activate the IoT environment by typing `iot` (if it hasn't been activated already).
3. Use the `pytest` testing library to run the tests. The command to run all tests is:

   .. code-block:: shell

       pytest -s -v

4. To run a specific test, append the test file name to the command. For example, to run the deployment tests:

   .. code-block:: shell

       pytest -s -v test_deployment.py

5. To run a specific test with board and device parameters, use the following command:

   .. code-block:: shell

       pytest -s -v test_deployment.py --boards=<board1>,<board2> --devices=<device1>,<device2>

   - `<board1>`: The name of the first board to test. For example, "wemos_d1_mini".
   - `<board2>`: The name of the second board to test (optional).
   - `<device1>`: The name of the first device to test. For example, "rfid".
   - `<device2>`: The name of the second device to test (optional).

   Note that you can specify as many `<board>` and `<device>` pairs as needed. If neither `<boards>` nor `<devices>` is specified, all combinations will be tested.

6. Example Command

   To run tests for specific boards and devices, you can use:

   .. code-block:: shell

       pytest -s -v --boards=wemos_d1_mini,esp32 --devices=laser_distance test_compilation.py

   This command will execute tests for both specified boards (`wemos_d1_mini` and `esp32`) using the specified device (`laser_distance`).


Hardware configuration
----------


To set up deployment and hardware testing you should define address of the node device in conf file.

You can easily list the connected devices by executing command in Raspberry PI::

    ls -l /dev/serial/by-path/

Device Layout
=============

This is device layout of Raspberry PI:

::

     1.3:1.0    1.2:1.0
    +-------+  +-------+
    |  USB  |  |  USB  |  +---------+
    +-------+  +-------+  |         |
     1.4:1.0    1.1:1.0   |   LAN   |
    +-------+  +-------+  |         |
    |  USB  |  |  USB  |  +---------+
    +-------+  +-------+


Conclusion
----------

The **Testing IoTempower** project ensures thorough testing of the IoTempower application, covering installations, compilation, and deployment. This documentation serves as a guide to understanding the structure, content, and usage of the testing folder.
