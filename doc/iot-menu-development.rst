IoT Menu Development Roadmap
============================

This document consolidates the requirements from GitHub issues #89 and #101
into a unified development plan for the IoT menu system.

Background
----------

The current ``iot menu`` is implemented in JavaScript (using terminal-kit)
in ``bin/user_menu.js``. While functional, there is a desire to:

1. **Migrate to Python** - As per issue #89, the JavaScript implementation
   should be replaced with a Python-based solution for better integration
   with the existing IoTempower Python ecosystem.

2. **Auto-launch on Environment Start** - As per issue #101, the menu
   should automatically appear when the IoT environment is activated.

Implementation Decision: Use Textual
------------------------------------

After evaluating multiple options (prompt_toolkit, golang-based tview, etc.),
**Textual** (https://github.com/Textualize/textual) has been selected as the
framework for the new IoT menu implementation.

Reasons for choosing Textual:

- **Already in use**: The ``lib/ap_configurator`` module already uses Textual,
  providing a consistent UI experience across the project.

- **Modern Python TUI framework**: Textual provides a rich, modern terminal
  user interface with support for widgets, layouts, and styling via CSS.

- **Active development**: Textual is actively maintained with good documentation.

- **SBC-friendly**: Textual has been designed with consideration for
  single-board computers like Raspberry Pi.

- **Markdown support**: Through Frogmouth integration, Textual can display
  documentation directly in the terminal.

Consolidated Requirements
-------------------------

From Issue #89 - New IoT Menu:

- Replace the JavaScript-based menu with a Python/Textual implementation
- Provide command-line access via simple shell escape
- Create a general TUI (Text UI) affecting all commands
- Ensure performance is acceptable on Raspberry Pi and Pi Zero
- Consider integration with documentation viewing (Frogmouth)

From Issue #101 - Auto-launch:

- The IoT menu should automatically pop up when the IoT environment starts
- This provides an immediate entry point for users
- Should still allow command-line access for advanced users

Implementation Approach
-----------------------

1. Create a new Textual-based menu application in ``lib/iot_menu/``
2. Reuse patterns from the existing ``lib/ap_configurator/`` implementation
3. Integrate with existing shell commands (deploy, adopt, upgrade, etc.)
4. Configure auto-launch behavior in the environment activation scripts
5. Provide option to disable auto-launch for power users

Related Files
-------------

- Current JavaScript menu: ``bin/user_menu.js``
- Legacy Python prototype: ``bin/user_menu-python.py``
- Menu launcher script: ``bin/iot_menu``
- Existing Textual implementation: ``lib/ap_configurator/``
- Python requirements: ``lib/iot_installs/requirements.txt`` (Textual already included)

References
----------

- GitHub Issue #89: https://github.com/iotempire/iotempower/issues/89
- GitHub Issue #101: https://github.com/iotempire/iotempower/issues/101
- Textual Documentation: https://textual.textualize.io/
- Frogmouth (Markdown viewer): https://github.com/textualize/frogmouth


Top: `ToC <index-doc.rst>`_, Previous: `Tool Support <tool-support.rst>`_
