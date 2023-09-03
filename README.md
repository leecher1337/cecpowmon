What is this?
=============
This little program watches the power status of a 
[HDMI CEC](https://de.wikipedia.org/wiki/Consumer_Electronics_Control)-capable
device on the HDMI port for changes and executes a shell script upon change.

Background:
I recently got a RasPi4 with official 7" touch screen in a case and wanted to 
make this my living-room media control center. I attached a video beamer to its
HDMI port (would also work for a TV set) and installed [KODI](https://kodi.tv/).
The aim is to automatically restart Kodi and execute it on the video projector
screen, as soon as I turn on the device and restart it on the internal touch 
screen when the device gets turned off again.

The program has one major advantage over 
[libcec-daemon](https://github.com/matthewbauer/libcec-daemon), which can do
the same thing:
The libcec-daemon relies on libcec, which is only capable of accessing the cec
interface with one application, so as soon as the libcec-daemon monitors the 
CEC-interface, no other applications (like KODI) can use the CEC interface 
anymore, which is not desirable, as i.e. KODI also implements remote control 
capabilities in order to be able to control KODI with the remote that comes 
with the HDMI  device.

Requirements
============
[CEC kernel support](https://docs.kernel.org/driver-api/media/cec-core.html)
i.e., on raspi, the KMS/DRM driver driver (vc4-kms-v3d) is capable of this,
the firmware kms driver (vc4-fkms-v3d) is NOT!

Compiling
=========
Simply use `make`.

If make is not available, you can just compile it with GCC:
`gcc -o cecpowmon cecpowmon.c`

Usage
=====
cecpowmon <Script to execute>

Program watches CEC traffic and if TV is turned on or off, scripts gets executed.
Parameter passed on activation: CEC_MSG_REQUEST_ACTIVE_SOURCE
Parameter passed on standby: CEC_MSG_STANDBY

Contact/Issue tracker
=====================
https://github.com/leecher1337/cecpowmon

