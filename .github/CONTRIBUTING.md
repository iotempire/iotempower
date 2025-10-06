# Contributing to IoTempower

Thank you for your interest in contributing to IoTempower! 

This guide aims to make your contribution process frictionless and fun.

## Getting Started


### 1. **Have IoTempower Installed**:
  
  - follow the [IoTempower installation instructions](https://github.com/iotempire/iotempower/blob/master/doc/installation.rst), for example, using the default `curl` command:
    
  ```bash
    curl -L https://now.iotempower.us | bash -s -- --default
  ```
  - verify the installation was successful by running the `iot` command in a (new) terminal
    - use `bash run` inside the `/iot` dir to fix any install issues

### 2. **Backup the IoTempower install directory**: 
  
  - The **default** installation directory location = `/iot`
    - the following steps assume you installed IoTempower in the default location
    - refer to the **manual installation** steps to set a custom install dir location
      - adjust these instructions according to your configuration
  
  - use the `mv` command to **move** the contents of `iot` to a new **backup** called `iot.bak`:
  
  ```bash
    mv iot iot.bak    
  ```
  
### 3. **Fork IoTempower**: 
  
  - open the IoTempower Repository on GitHub in a browser: https://github.com/iotempire/iotempower
  - find the `Fork` button just below the page header, between `Star` and `Watch` buttons
    - click `Fork` to create a new `Fork` of the `main` branch and follow prompts
      - please consider supporting IoTempower with a `Star`
      - use `Watch` to receive dev updates by email

#### 3.1 **Sync & Clone Your Fork**: 
  
  - After forking, clone your fork to your local machine:
  
  ```bash
  git clone git@github.com:<your-GitHub-username>/iotempower.git
  ```

  - Make sure you have an `SSH Key` on your local device linked to your GitHub account at [GitHub Keys](https://github.com/settings/keys). 
    - Use `ssh-keygen` to generate one if needed.
  - make sure your local git is configured; learn more here: [Setting your username in Git (link)](https://docs.github.com/en/get-started/getting-started-with-git/setting-your-username-in-git)
    - use `git config --list` to check if your git `user.name` and `user.email` are set 
    - if  `git config user.name` and `git config user.email`
      - use the `--global` flag if you'd like this configuration to apply system-wide
  
  - Keep your fork in **sync** with the main IoTempower repository by pulling changes regularly
      - open your forked repo in a browser and select `Sync fork` 
        - then run a `pull` in your local repo
        - or use `iot upgrade` to update your `iot` install automagically

### 4. **Move `.local` To Your Fork**: 
  
  - finally, copy the `iot.bak/.local` dir to your newly cloned fork:
  
  ```bash
    cp -a iot.bak/.local iot/
  ```

  - verify the installation was successful by running the `iot` command in a (new) terminal
    - use `bash run` inside the `/iot` dir to fix any install issues
  - Your own forked IoTempower is ready for your contributions!  


## Making Contributions

### Basic Steps:
  1. **Get Ready**: Open the forked `/iot` directory in your IDE of choice
    
  - the IoTempower team recommends [`VS-Codium`](https://vscodium.com/) or [`VS-Code`](https://code.visualstudio.com/)
  - use `Pull` to keep your (re)`synced` fork updated with the main IoTempower repo on your local fork clone
  
  2. **Make changes**:
    
  - `stage changes` & `commit` your `changes` to your fork on GitHub
  
  3. **Submit a Pull Request**: 
  
  - when ready, click `Contribute` and select `Open pull request`
    - this takes you to a page with a summary of your `commits`
    - fill in relevant information about your `Pull Request` (PR)
      - consider using one of the provided templates
        - **Describe Your Changes**: make sure to provide a description of what your changes do and mention any relevant issue numbers
    - you can continue pushing new `commits` and they will automatically be part of the current PR
    
  4. **Get Reviewed**: 
  
  - Once submitted, your pull request will be reviewed by the core team and possibly other contributors.
  - After the dev team reviews and accepts your changes your `Pull Request` will be `merged` into the `main` IoTempower branch directly.

### Additional Notes:
  - **Follow the Style Guides**: Please adhere to the code style guidelines outlined below.
  - **Write Meaningful Commit Messages**: Describe what your changes do and why.
  - **Test**: Please test new features to ensure they work as expected (see Testing section below).
  - **Update the Docs**: If you're changing anything that requires a change in documentation, please update it.

## Code Style Guidelines

For consistency and readability, we enforce a specific coding style for our **`C++`** files. 

Our project adheres to a modified Google style guide, with specific modifications for indent width.

### Configure Your Development Environment

- **Visual Studio Code Users**: If you are using Visual Studio Code or VSCodium, you can easily adjust your code formatting settings to match our style. 
  - Look for `Clang_format_style` in the settings (you can use `Ctrl` + `,` to open settings) and set it to:
  ```json
   { "BasedOnStyle": "Google", "IndentWidth": 4 }
  ```
  - Use the formatting shortcut **`Ctrl+Shift+I`** on *Linux* to automatically *format* your code.
  - Learn more about configuring this feature here: https://code.visualstudio.com/docs/cpp/cpp-ide

- **Other Environments**: If you use a different IDE or text editor, please configure it to use the Google C++ style guide as a base but set the indentation width to 4 spaces.

- **Before Submitting a Pull Request**:
  - Ensure your contributions are properly formatted according to these guidelines. 
  - Please review your changes for any unintended formatting alterations before committing. 
    - This helps maintain the quality and consistency of the codebase.


## Testing Your Changes

Testing is an important part of ensuring your contributions work correctly. IoTempower includes comprehensive testing capabilities.

### General Testing Recommendations

- **Test Compilation**: Run compilation tests to verify your changes don't break device configurations
- **Test Before Submitting**: Always test your changes before submitting a pull request
- **Run Relevant Tests**: Focus on tests that are related to your changes

### Running Tests Locally

The easiest way to run tests is using the `iot test` command:

```bash
iot test
```

To run only compilation tests:

```bash
iot test compile
```

To test specific boards and devices:

```bash
iot test compile --boards=wemos_d1_mini,esp32 --devices=laser_distance
```

### Alternative: Using pytest Directly

If you prefer to use pytest directly:

1. Navigate to the `tests` folder
2. Activate the IoT environment by typing `iot` (if not already activated)
3. Run tests:

```bash
# Run all tests
pytest -s -v

# Run a specific test file
pytest -s -v test_compilation.py

# Run with specific parameters
pytest -s -v test_compilation.py --boards=wemos_d1_mini,esp32 --devices=laser_distance
```

### Testing with Docker/Podman

You can test your local repository changes inside a Docker or Podman container. This is particularly useful for ensuring consistency across different environments.

**Docker Testing:**

Assuming your working repository is in your home folder under `iot`, test inside Docker with:

```bash
cat iot/bin/curl_install_docker | IOTEMPOWER_REPO=~/iot bash -s -- test compilation
```

**Podman Testing:**

Similarly, for Podman:

```bash
cat iot/bin/curl_install_podman | IOTEMPOWER_REPO=~/iot bash -s -- test compilation
```

This one-line command will:
- Install IoTempower in a container
- Use your local repository as the source
- Run the compilation tests

For more detailed information about testing, including deployment and hardware tests, see the [testing documentation](https://github.com/iotempire/iotempower/blob/master/doc/testing.rst).


## Alternative Installation Methods

### Docker/Podman Installation

If you prefer to work in a containerized environment, IoTempower supports both Docker and Podman installations:

**Docker Installation:**

```bash
curl -L docker.iotempower.us | bash -
```

**Podman Installation:**

```bash
curl -L podman.iotempower.us | bash -
```

**Important Notes:**
- Container environments don't have direct access to serial ports (especially on Windows)
- You can only flash via network using rfc2217
- On Windows with Docker Desktop and WSL 2 Ubuntu integration, run the command in your WSL 2 Ubuntu environment
- The install script creates an `iot` starter script; if it doesn't work, check `examples/scripts/iot-docker` or `iot-podman` for manual setup

After installation, use the container environment just like a native installation by running the `iot` command.


## Reporting Bugs or Requesting Features

Here's how to submit a **`Bug Report`**: 
- Navigate to [GitHub Issues (link)](https://github.com/iotempire/iotempower/issues),
- Select `New Issue` and apply the `bug` label
- Please use the provided template and include detailed information such as:
  - the **problem** you are experiencing, 
  - any relevant **logs** & **error** messages, 
  - **steps to reproduce**, and 
  - **system specs**. 

---
Here's how to submit a **`Feature Request`**: 
- Navigate to [GitHub Issues (link)](https://github.com/iotempire/iotempower/issues),
- Select `New Issue` and apply the `enhancement` label
- **describe** the feature, its **benefits** and potential **implementation** to start a discussion
---

## Support

If you have any questions, please view our [SUPPORT.md](SUPPORT.md) for ways to find help.

We appreciate your contributions to IoTempower and look forward to your active participation.