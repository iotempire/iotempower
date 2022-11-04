Install IoTempower image onto an SD-card
========================================

REMARK: The transitioning to docker is still not completed. (Any volunteers?)
The 64 bit image has been updated and is now based on `DietPi <https://dietpi.com>`__,
if you want to run updates or make changes, please check out their documentation
(maybe one day IoTempower gateway functionality can be directly integrated).
Make sure to resize the image at one point with the DietPi tools
(DietPi-Drive_Manager -> / -> Resize).

- Download the 64 Bit Raspberry Pi (only Pi 4 and probably Pi 3) image:
  `iotgateway-pi64.img.xz <https://drive.google.com/file/d/1zz-bvu_x7rynVBTDSdJqs3SnXT4-cLyP>`_ 
  from Google Drive.

  The old 32 bit version for pi 1-2 and zero (needs to be updated) is
  `here on Google Drive <https://drive.google.com/open?id=1b0M93T2-suLFMjpmf8PLUTEGh_rKT6_6>`_,
  sha256sum: ``f3be2ba597f2b885eb573a8c8af14e7cf788d614d765ae99ae0223c4f887454a``
  
- Make sure the sha256-checksum of the image is correct. It should be:
  ``fdf47f3ae35a7bdebe01597bc5a7715c68d0d34a31be41b1491eb3eff6a9e919``

  On Linux and MacOS, you can use ``sha256sum`` or ``shasum -a 256`` to verify
  the image, on Windows you can use
  https://raylin.wordpress.com/downloads/md5-sha-1-checksum-utility/

- Write the image to an (at least) 8GB class-10 sd-card with 
  `Etcher <https://www.balena.io/etcher/>`_
  (works on Windows, MacOS, or Linux).

Top: `ToC <index-doc.rst>`_, Previous: `Installation <installation.rst>`_,
Next: `First IoT Node <first-node.rst>`_.
