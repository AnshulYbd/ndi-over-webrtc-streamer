# Building the source
mkdir winBuild && cd winBuild
cmake .. && cmake --build . --config Debug

on Linux
mkdir linuxBuild && cd linuxBuild
cmake .. && cmake --build .


# Streaming H264 and opus

This example streams H264 and opus(not tested )<sup id="a1">[1](#f1)</sup> samples to the connected browser client.

## Start the example signaling server

```sh
cd <checkoutbasedir>\app\signaling-server-nodejs>
npm install
npm start
```

## Start a web server

open index.html in chrome ( tested only in chrome browser)

## Start the streamer
Run app either from VS2019 or from command prompt

Arguments:

- `-a` Directory with OPUS samples (default: *../../../../examples/streamer/samples/opus/*).
- `-b` Directory with H264 samples (default: *../../../../examples/streamer/samples/h264/*).
- `-d` Signaling server IP address (default: 127.0.0.1).
- `-p` Signaling server port (default: 8000).
- `-v` Enable debug logs.
- `-h` Print this help and exit.


Press START Button on chrome web page
Things to check
NDI camera output H264
Camer name src.p_ndi_name = "NDIPTZ2 (Chan_1, 192.168.208.50)"; #Fileparser.cpp line number 60 change and recompile or create commandline argument parameter args

## Generating H264 and Opus samples
Refer libdatachannel streamer example for more details.