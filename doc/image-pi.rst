Install IoTempower image onto an SD-card
========================================

- Download the Raspberry Pi image from
  `here on Google Drive <https://drive.google.com/open?id=1b0M93T2-suLFMjpmf8PLUTEGh_rKT6_6>`_
  
- Make sure the sha256-checksum of the image is correct. It should be:

  ``86358d30c919f8220f5d8c09bfd21b83616e4c8f4fdf15f9d8b6c9cf90857594``

  On Linux and MacOS, you can use ``sha256sum`` or ``shasum -a 256`` to verify
  the image, on Windows you can use
  https://raylin.wordpress.com/downloads/md5-sha-1-checksum-utility/

- Write the image to a (at least) 8GB class-10 sd-card with https://etcher.io/
  (works on Windows, MacOS, or Linux).

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
