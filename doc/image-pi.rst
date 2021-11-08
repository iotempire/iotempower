Install IoTempower image onto an SD-card
========================================

ATTENTION: The raspberry pi image is currently outdated as we are using
transitioning to docker. Please stay tuned and be active on the issues
or in the forum if you want the re-implemantation of this feature
coming back.

- Download the Raspberry Pi image from - this version is outdated!!!
  `here on Google Drive <https://drive.google.com/open?id=1b0M93T2-suLFMjpmf8PLUTEGh_rKT6_6>`_
  
- Make sure the sha256-checksum of the image is correct. It should be:

  ``f3be2ba597f2b885eb573a8c8af14e7cf788d614d765ae99ae0223c4f887454a``

  On Linux and MacOS, you can use ``sha256sum`` or ``shasum -a 256`` to verify
  the image, on Windows you can use
  https://raylin.wordpress.com/downloads/md5-sha-1-checksum-utility/

- Write the image to a (at least) 8GB class-10 sd-card with https://etcher.io/
  (works on Windows, MacOS, or Linux).

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
