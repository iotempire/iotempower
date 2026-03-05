Legacy and Obsoleted Tools
==========================

This page documents tools that were previously part of IoTempower but have
been removed or are no longer maintained. They are preserved here for
historical reference and for users who may have installed them independently.


CloudCmd (Web-based File Manager)
----------------------------------

**Status: Removed from IoTempower**

CloudCmd was a web-based file manager bundled with the IoTempower Raspberry Pi
image. It allowed browsing and editing files directly from a browser at URLs
like ``https://iotgateway/cloudcmd/fs/home/iot/iot-test``.

**Why it was removed:**
CloudCmd became difficult to install reliably and is no longer a supported
component of IoTempower. The install process was fragile and not worth
maintaining as a default tool.

**Alternatives:**
Use SSH to access and manage files on the gateway directly:

.. code-block:: bash

   ssh iot@iotgateway

(Use ``iot@192.168.12.1`` if hostname resolution does not work.
The default password is ``iotempire``.)

Once logged in, you can use the ``micro`` editor to edit files:

.. code-block:: bash

   micro ~/iot-test/node1/setup.cpp

The ``iot menu`` command (run inside the IoTempower environment) provides an
interactive menu for common operations like creating nodes and deploying
firmware — it is available on all supported platforms.

Users who installed CloudCmd manually and still rely on the
``cloudcmd_starter`` / ``cloudcmd_stop`` helper scripts can continue using
them; those scripts remain available.


Tilde Text Editor
-----------------

**Status: Removed from IoTempower convenience tools**

The `Tilde <https://os.ghalkes.nl/tilde>`__ text editor was previously
installed as part of the ``--convenience`` toolset. It has been removed
because its build process is broken in current environments.

**Alternative:** Use ``micro``, which is installed as part of the
``--convenience`` toolset and provides a similar beginner-friendly terminal
editing experience.


Top: `ToC <index-doc.rst>`_
