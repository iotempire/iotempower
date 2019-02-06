Install UlnoIoT image onto an SD-card
=====================================

- Download the Raspberry Pi image from here: https://goo.gl/bVgLMr
  (you might later want to use ssh and upgrade Node-RED dashboard
  on this image, turns out it is buggy in using the charts. Do:
  ``sudo npm install -g --unsafe-perm Node-RED-dashboard``)

- Make sure the sha256-checksum of the image is correct. It should be:

  ``004485178869dfb0c49f5f9b9b11f8c4c2b9128d8ddb3dea50725e2f9a506602``

  On Linux and MacOS, you can use ``sha256sum`` or ``shasum -a 256`` to verify
  the image, on Windows you can use
  https://raylin.wordpress.com/downloads/md5-sha-1-checksum-utility/

- Write the image to a (at least) 8GB class-10 sd-card with https://etcher.io/
  (works on Windows, MacOS, or Linux).

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `First IoT Node <first-node.rst>`_.