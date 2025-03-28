Install IoTempower image onto an SD-card
========================================

REMARK: The transitioning to docker is still not completed. (Any volunteers?)
The 64 bit image has been updated and is now based on `DietPi <https://dietpi.com>`__,
if you want to run updates or make changes, please check out their documentation
(maybe one day IoTempower gateway functionality can be directly integrated).
Make sure to resize the image at one point with the DietPi tools
(DietPi-Drive_Manager -> / -> Resize).

- Download the 64 Bit Raspberry Pi (only Pi 4 and probably Pi 3) image:
  `iotgateway2024_v2.img.xz <https://drive.google.com/file/d/1PMG5RvH36KjrTJvraiiEV34nhFETPCpk>`_ 
  from Google Drive.

  The old 32 bit version for pi 1-2 and zero (needs to be updated) is
  `here on Google Drive <https://drive.google.com/open?id=1b0M93T2-suLFMjpmf8PLUTEGh_rKT6_6>`_,
  sha256sum: ``f3be2ba597f2b885eb573a8c8af14e7cf788d614d765ae99ae0223c4f887454a``
  
- Make sure the sha256-checksum of the 64 Bit image is correct. It should be:
  ``5d2fc222a6a303e23ed049485b515271394d3e66a46a151e4bcba99257a34482``

  On Linux and MacOS, you can use ``sha256sum`` or ``shasum -a 256`` to verify
  the image, on Windows you can use
  https://raylin.wordpress.com/downloads/md5-sha-1-checksum-utility/

- Write the image to an (at least) 8GB class-10 sd-card with 
  `Etcher <https://www.balena.io/etcher/>`_
  (works on Windows, MacOS, or Linux) and don't forget to expand the filesystem later on.

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
