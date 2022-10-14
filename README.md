## Prerequisites for building

  wget http://archive.ubuntu.com/ubuntu/pool/main/o/openssl/libssl1.1_1.1.1f-1ubuntu2.16_amd64.deb
  
  dpkg -i libssl1.1_1.1.1f-1ubuntu2.16_amd64.deb
  
  sudo ln -s <PATH>/ndi-over-webrtc-streamer/3rdParty/libs/ndi/libndi-x86_64.so.5 /usr/lib/x86_64-linux-gnu/libndi.so.5
  
  sudo ln -s <PATH>/ndi-over-webrtc-streamer/3rdParty/libs/libdc/libdatachannel.so /usr/lib/x86_64-linux-gnu/libdatachannel.so.0.17.10
  
  NDI camera should output H264 without split nonIDR frames.

  Camer name hardcoded in #Fileparser.cpp line #48 src.p_ndi_name = "NDIPTZ2 (Chan_1, 192.168.208.50)"; 

## Building the source on Windows

  mkdir winBuild && cd winBuild
  
  cmake .. && cmake --build . --config Debug

## Building the source on Linux

  mkdir linuxBuild && cd linuxBuild
  
  cmake .. && cmake --build .

# Streaming H264 and opus
This example streams H264 and opus(not tested )<sup id="a1">[1](#f1)</sup> samples to the connected browser client.

## Start the example signaling server

```sh
cd <PATH>\app\signaling-server-nodejs>
npm install
npm start
```

## How to stream video steps
  1. Run node npm start to run server
  2. Launch or Refresh web page index.html in chrome by double clicking it, before every start Refresh webpage is required.
  3. Run the streamer app
  4. Press START Button on chrome web page  
Streamer app read the frames from ndi camera and rtp packetized it and send it to chrome browser after handshaking of SDP and Answer.

Arguments:
- `-d` Signaling server IP address (default: 127.0.0.1).
- `-p` Signaling server port (default: 8000).
- `-v` Enable debug logs.
- `-h` Print this help and exit.


## Generating H264 and Opus samples
Refer libdatachannel streamer example for more details.
