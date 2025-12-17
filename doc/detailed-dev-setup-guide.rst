================================
Detailed Development Setup Guide
================================

This guide provides comprehensive instructions for setting up a development environment for contributing to IoTempower.

Prerequisites
=============

1. **Have IoTempower Installed**:
  
   - Follow the `IoTempower installation instructions <installation.rst>`_, for example, using the default ``curl`` command:
    
   .. code-block:: bash

      curl -L https://now.iotempower.us | bash -s -- --default

   - Verify the installation was successful by running the ``iot`` command in a (new) terminal
   - Use ``bash run`` inside the ``/iot`` dir to fix any install issues

Setting Up Your Development Fork
=================================

2. **Backup the IoTempower install directory** (Optional): 
  
   - The **default** installation directory location = ``/iot``
   - The following steps assume you installed IoTempower in the default location
   - Refer to the **manual installation** steps to set a custom install dir location and adjust these instructions according to your configuration
  
   - Potentially use the ``mv`` command to **move** the contents of ``iot`` to a new **backup** called ``iot.bak``:
  
   .. code-block:: bash

      mv iot iot.bak    

3. **Fork IoTempower**: 
  
   - Open the IoTempower Repository on GitHub in a browser: https://github.com/iotempire/iotempower
   - Find the ``Fork`` button just below the page header, between ``Star`` and ``Watch`` buttons
   - Click ``Fork`` to create a new ``Fork`` of the ``main`` branch and follow prompts
   - Please consider supporting IoTempower with a ``Star``
   - Use ``Watch`` to receive dev updates by email

Sync & Clone Your Fork
-----------------------
  
After forking, clone your fork to your local machine:

.. code-block:: bash

   git clone git@github.com:<your-GitHub-username>/iotempower.git

- Make sure you have an ``SSH Key`` on your local device linked to your GitHub account at `GitHub Keys <https://github.com/settings/keys>`_. 
  - Use ``ssh-keygen`` to generate one if needed.
- Make sure your local git is configured; learn more here: `Setting your username in Git <https://docs.github.com/en/get-started/getting-started-with-git/setting-your-username-in-git>`_
  - Use ``git config --list`` to check if your git ``user.name`` and ``user.email`` are set 
  - If not set, use ``git config user.name "Your Name"`` and ``git config user.email "your@email.com"``
  - Use the ``--global`` flag if you'd like this configuration to apply system-wide

- Keep your fork in **sync** with the main IoTempower repository by pulling changes regularly
  - Open your forked repo in a browser and select ``Sync fork`` 
  - Then run a ``pull`` in your local repo
  - Or use ``iot upgrade`` to update your ``iot`` install automagically

4. **Move ``.local`` To Your Fork** (If applicable): 
  
   - Finally, copy the ``iot.bak/.local`` dir to your newly cloned fork:
  
   .. code-block:: bash

      cp -a iot.bak/.local iot/

   - Verify the installation was successful by running the ``iot`` command in a (new) terminal
   - Use ``bash run`` inside the ``/iot`` dir to fix any install issues
   - Your own forked IoTempower is ready for your contributions!  


Making Contributions
====================

Basic Steps
-----------

1. **Get Ready**: Open the forked ``/iot`` directory in your IDE of choice
    
   - The IoTempower team recommends `VS-Codium <https://vscodium.com/>`_ or `VS-Code <https://code.visualstudio.com/>`_
   - Use ``Pull`` to keep your (re)\ ``synced`` fork updated with the main IoTempower repo on your local fork clone
  
2. **Make changes**:
    
   - ``stage changes`` & ``commit`` your ``changes`` to your fork on GitHub
  
3. **Submit a Pull Request**: 
  
   - When ready, click ``Contribute`` and select ``Open pull request``
   - This takes you to a page with a summary of your ``commits``
   - Fill in relevant information about your ``Pull Request`` (PR)
   - Consider using one of the provided templates
   - **Describe Your Changes**: make sure to provide a description of what your changes do and mention any relevant issue numbers
   - You can continue pushing new ``commits`` and they will automatically be part of the current PR
    
4. **Get Reviewed**: 
  
   - Once submitted, your pull request will be reviewed by the core team and possibly other contributors.
   - After the dev team reviews and accepts your changes your ``Pull Request`` will be ``merged`` into the ``main`` IoTempower branch directly.

Additional Notes
----------------

- **Follow the Style Guides**: Please adhere to the code style guidelines outlined below.
- **Write Meaningful Commit Messages**: Describe what your changes do and why.
- **Test**: Please test new features to ensure they work as expected (see Testing section below).
- **Update the Docs**: If you're changing anything that requires a change in documentation, please update it.

Code Style Guidelines
=====================

For consistency and readability, we enforce a specific coding style for our **C++** files. 

Our project adheres to a modified Google style guide, with specific modifications for indent width.

Configure Your Development Environment
---------------------------------------

**Visual Studio Code Users**: If you are using Visual Studio Code or VSCodium, you can easily adjust your code formatting settings to match our style. 

- Look for ``Clang_format_style`` in the settings (you can use ``Ctrl`` + ``,`` to open settings) and set it to:

  .. code-block:: json

     { "BasedOnStyle": "Google", "IndentWidth": 4 }

- Use the formatting shortcut **Ctrl+Shift+I** on *Linux* to automatically *format* your code.
- Learn more about configuring this feature here: https://code.visualstudio.com/docs/cpp/cpp-ide

**Other Environments**: If you use a different IDE or text editor, please configure it to use the Google C++ style guide as a base, but set the indentation width to 4 spaces.

**Before Submitting a Pull Request**:

- Ensure your contributions are properly formatted according to these guidelines. 
- Please review your changes for any unintended formatting alterations before committing. 
- This helps maintain the quality and consistency of the codebase.


Top: `ToC <index-doc.rst>`_, Next: `CONTRIBUTING <../.github/CONTRIBUTING.md>`_
