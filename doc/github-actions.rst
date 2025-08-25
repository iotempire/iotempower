# GitHub Actions CI/CD for IoTempower

This document describes the GitHub Actions workflows available for continuous integration and deployment (CI/CD) in the IoTempower project.

## Available Workflows

### 1. Compilation Tests (`compilation-tests.yml`)

**Purpose**: Validates compilation of IoTempower code for key hardware boards and devices.

**Triggers**:
- Pull requests to `master` or `main` branches
- Manual dispatch via GitHub Actions UI

**Default Test Scope**:
- **Boards**: `wemos_d1_mini`, `esp32minikit`
- **Devices**: `input`, `output`, `analog`

**Features**:
- Parallel execution for different boards
- Customizable board and device selection for manual runs
- Artifacts collection for debugging

**Usage**:
- Automatically runs on pull requests
- For manual testing: Go to Actions tab → "Compilation Tests" → "Run workflow"

### 2. Quick Smoke Tests (`smoke-tests.yml`)

**Purpose**: Fast validation of core IoTempower functionality.

**Triggers**:
- Push to `master` or `main` branches
- Daily at 2:00 AM UTC (scheduled)
- Manual dispatch

**Test Scope**:
- **Boards**: `wemos_d1_mini`
- **Devices**: `input`, `output`

**Usage**:
- Runs automatically on pushes to main branches
- Provides quick feedback on basic functionality

### 3. Extended Compilation Tests (`extended-compilation-tests.yml`)

**Purpose**: Comprehensive testing with configurable scope for thorough validation.

**Triggers**:
- Manual dispatch only

**Test Scopes**:
- **Basic**: `wemos_d1_mini`, `esp32minikit` + `input`, `output`, `analog`
- **Extended**: Adds `esp8266`, `esp32` + `bmp180`, `dht`, `dallas`
- **Full**: Adds `lolin_s2_mini` + `gyro6050`, `acoustic_distance`, `laser_distance`

**Features**:
- Three predefined test scopes
- Custom board and device selection
- Detailed test summaries

**Usage**:
- Go to Actions tab → "Extended Compilation Tests" → "Run workflow"
- Select test scope or provide custom board/device lists

## Technical Details

### Docker-based Testing
All workflows use the pre-built `ulno/iotempower` Docker image for consistent testing environments.

### Test Infrastructure
- Based on existing `bin/iot_test` script
- Uses pytest with `--boards` and `--devices` parameters
- Leverages configuration from `tests/conf_data.py`

### Parallelization
- Compilation tests run in parallel for different boards
- Reduces overall testing time

### Artifacts
- Test results are stored as artifacts for 7 days
- Available for download from the Actions tab

## For Developers

### Running Tests Locally
```bash
# Using Docker (recommended)
docker pull ulno/iotempower
mkdir -p /tmp/iot-container
cp -r . /tmp/iot-container/repo
docker run --rm -v /tmp/iot-container:/iot/data ulno/iotempower iot test compile --boards=wemos_d1_mini --devices=input,output

# Using local environment (requires IoTempower setup)
iot test compile --boards=wemos_d1_mini --devices=input,output
```

### Adding New Boards or Devices
1. Update `tests/conf_data.py` with new board/device definitions
2. Test locally first
3. Add to workflow configurations if needed

### Customizing Workflows
- Modify workflow files in `.github/workflows/`
- Adjust board/device lists in workflow defaults
- Update triggers as needed

## Integration with Pull Requests

The compilation tests automatically run on pull requests, providing:
- ✅ Green check for passing tests
- ❌ Red X for failing tests
- 📋 Detailed logs and artifacts for debugging

This ensures code quality and prevents regressions in the compilation process.

## For GitHub Copilot

The Extended Compilation Tests workflow is particularly useful for Copilot when:
- Testing new features across multiple boards
- Validating large code changes
- Running comprehensive validation before merging

Simply trigger the workflow with the appropriate test scope for your needs.