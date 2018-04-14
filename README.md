# OpenCLDemo
Demo of running OpenCL for Android phones both with or without OpenCL library in stock ROM.

This demo directly supports Google Pixel 2, which is running Android Oreo stock ROM 
but does not have OpenCL libraries. It has been tested to work on Nexus 5, Nexus 6P (both don't have OpenCL libraries). 
The demo also works on Redmi Note 4X (nikel, MTK) which has ARM Mali T880 GPU.

OpenCL Configurations for Phones having no OpenCL libraries:
- [x] Pixel 2 stock ROM is Android 8.1. The code in this repo is configured for that. Just download 
the code and `./gradlew iDeb`, you are good to go.
- [x] I tested Nexus 5 running LineageOS 15.1 (based on Android 8.1), 
just push the [libOpenCL.so](https://github.com/csarron/PhoneVendorBlobs/blob/master/msm8974/vendor/lib/libOpenCL.so) 
to your phone. You can do it by first enable adb root in the developer options, then `adb root; adb remount;`,
`adb push libOpenCL.so /system/lib/`, and you also need to append `libOpenCL.so` to the file `/system/etc/public.libraries.txt`, 
reboot your phone. It should work now. 

For Nexus 5, you can ignore [jniLibs](app/src/main/jniLibs), 
those are only for non rooted phones(in this repo for Pixel 2 only).


I also made it work on stock ROMs for Nexus 5 and Nexus 6P.
For more on how to get OpenCL work on Android Phones with/without stock ROMs 
(especially with newer Androids like Nougat or Oreo), I'll put the details in my blog. Stay tuned.

Modified from [Sony Example](https://developer.sonymobile.com/downloads/code-example-module/opencl-code-example/)
