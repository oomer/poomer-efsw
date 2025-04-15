# poomer-efsw [Work in progress]
Prototype cross-platform command line file monitoring and queuing

## Usage

poomer-efsw --watchdir:/tmp
poomer-efsw --watchdir:/tmp --ext:zip --ext:bsz


# Build 
```
workdir/
├── build_engine_sdk/
├── oom/
├── efsw/
└── poomer-efsw/
```

## MacOS
Install Cmake to /Applications
```
curl -LO https://github.com/Kitware/CMake/releases/download/v3.31.6/cmake-3.31.6-macos-universal.dmg
open cmake-3.31.6-macos-universal.dmg
```

Download SDK for your OS and drag bella_engine_sdk into your workdir. On Windows rename unzipped folder by removing version ie bella_engine_sdk-24.6.0 -> bella_engine_sdk

- [bella_engine_sdk MacOS](https://downloads.bellarender.com/bella_engine_sdk-24.6.0.dmg)
- [bella_engine_sdk Linux](https://downloads.bellarender.com/bella_engine_sdk-24.6.0.tar.gz)
- [bella_engine_sdk Win](https://downloads.bellarender.com/bella_engine_sdk-24.6.0.zip)

```
mkdir workdir
cd workdir
git clone https://github.com/SpartanJ/efsw.git
mkdir -p efsw/build
cd efsw/build
/Applications/CMake.app/Contents/bin/cmake ..
make -j4
cd ../..
git clone https://github.com/oomer/oom.git
git clone https://github.com/oomer/poomer-efsw.git
cd poomer-efsw
make all -j4
```
