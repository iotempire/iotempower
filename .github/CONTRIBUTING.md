# Contributing to IoTempower

Thanks for your interest in IoTempower! We're excited to have you here.

## Getting Started

1. **Read the documentation** at [IoTempower Installation Guide](https://github.com/iotempire/iotempower/blob/master/doc/installation.rst)
2. **Check existing issues** to see if your idea is already being discussed
3. **Open an issue** describing what you want to work on
4. **Look for issues labeled** `good-first-issue` or `help-wanted` if you're not sure where to start

## Quick Setup for Contributors

Often it's as easy as forking IoTempower on GitHub and installing directly from your fork:

```bash
cd # go to home directory
my_iot_folder=iot # adjust to your preferred development folder
git clone git@github.com:<mygithubuser>/iotempower.git "$my_iot_folder"
cd "$my_iot_folder"
bash run
```

**Note:** If you have an existing IoTempower installation, you can move the repo away, move your fork in its place, and move the `.local` folder back to recreate your pre-existing installation. Make sure to update regularly.

For detailed setup instructions including forking, SSH keys, and syncing your fork, see our [Detailed Development Setup Guide](https://github.com/iotempire/iotempower/blob/master/doc/detailed-dev-setup-guide.rst).  


## Types of Contributions

### 🐛 Found a Bug?
Open an issue with:
- What you were trying to do
- What happened
- What you expected to happen
- Your hardware and software versions

### ✨ Want to Add Device Support?
See our guides for a step-by-step walkthrough:
- [Device Architecture](https://github.com/iotempire/iotempower/blob/master/doc/device-architecture.rst) - Understanding device classes and structure
- [Architecture Overview](https://github.com/iotempire/iotempower/blob/master/doc/architecture.rst) - System-level architecture
- [Device Source Code Examples](https://github.com/iotempire/iotempower/tree/master/lib/node_types/esp/src) - Browse existing device implementations

### 📚 Documentation Improvements?
We love documentation PRs! These are a fantastic way to help.

### 🎨 Have an Idea?
Open an issue to discuss it first. We're more likely to merge PRs that solve real problems than speculative features.

## Pull Request Guidelines

- Keep PRs focused on a single issue/feature
- Include tests if possible
- Update documentation if you change functionality
- Reference the issue number in your PR description
- Write meaningful commit messages that describe what your changes do and why
- Test your changes before submitting

## Code Style Guidelines

For C++ code, we follow a modified Google style guide with 4-space indentation:
```json
{ "BasedOnStyle": "Google", "IndentWidth": 4 }
```

If using VS Code/VSCodium, configure `Clang_format_style` in settings and use **Ctrl+Shift+I** to format.


## Testing Your Changes

Testing is an important part of ensuring your contributions work correctly.

- **Run compilation tests**: `iot test compile`
- **Test specific boards/devices**: `iot test compile --boards=wemos_d1_mini,esp32 --devices=laser_distance`

For more details, see the [testing documentation](https://github.com/iotempire/iotempower/blob/master/doc/testing.rst).

## Questions?

- Check the [FAQ](https://github.com/iotempire/iotempower/blob/master/doc/faq.rst)
- Ask in [GitHub Issues](https://github.com/iotempire/iotempower/issues)
- Join our [Discord community](https://discord.gg/9gq8Q9p6r3)

We'll do our best to help!

---

We appreciate your contributions to IoTempower and look forward to your active participation. Remember: **Boring is beautiful.** Quality over quantity. Documentation over task assignment. Real problems over speculative features.
