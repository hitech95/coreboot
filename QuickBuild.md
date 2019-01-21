Quick start documentation
===

# 1 Introduction: Build an UEFI Image

This is a quick documentation on how build a custom image of Coreboot for your chromebook.

> Please note that the following steps are for Ubuntu, so if you use another disto some steps may differ.

Further we'll build an UEFI image for `peppy`. Change the build target to the one you want to build.
> NOTE: The available targets are listed in the [Supported Devices](https://mrchromebox.tech/#devices) page.

# 2 Payloads
Coreboot in itself is "only" minimal code for initializing a mainboard with peripherals. After the initialization, it jumps to a payload. 

> For more informations see the [Payloads](https://www.coreboot.org/Payloads) Documentation on Coreboot 

## 2.1 Tianocore
The Payload for UEFI images is [Tianocore](https://www.tianocore.org/).

## 2.2 SeaBios
[SeaBIOS](https://seabios.org/SeaBIOS) is an open-source implementation of the standard bootstrap callback layer implemented by an x86 BIOS.
Generally this is used to boot legacy OSs.

# 3 Install Dependencies

## 3.1 Coreboot
```
apt-get install git build-essential gnat flex bison libncurses5-dev wget zlib1g-dev
```

## 3.2 Tianocore payload

If you want to build the Tianocore Payload you must install also those dependencies:

```
apt-get install uuid-dev nasm iasl
```

# 4 Download sources
## 4.1 Download Coreboot by MrChromebox

We have to download the sources, the `master` branch is bleeding edge. For this reason you should use the latest stable branch.

```
git clone https://github.com/MrChromebox/coreboot.git
cd coreboot
git checkout 2019.01.13
```
>NOTE: While writing this document the latest stable branch is 2019.01.13

## 4.2 Download from Coreboot repository
```
git clone https://review.coreboot.org/coreboot
cd coreboot
```
> NOTE: The mainline Coreboot don't have all the configuration files and the build scripts. You have to procead by [hands](https://doc.coreboot.org/lessons/lesson1.html)

## 4.3 Enable Submodules
Coreboot use `git submodules` to manage some internal dependencies. On older version of `git` the submodules are not downloaded automatically; for this reason you might have to run:
```
git submodules update --init checkout
```

At this point we are ready to proceed.

> NOTE: You may encounter errors if the submodules on the github are out of date. In this case they will be downloaded from [Gerrit](https://review.coreboot.org). 
To download a specific commit you have to type the `sha1` of the required commit in the **search bar**, then you have to **click** on **download** and use the `pull` command inside the `3rdparty` directory.

# 5 Build the Toolchain
Even if it's possible to use the system toolchain this is not raccomanded.
You can also specify the CPU core count with `CPUS=4` to speedup the build precess. The example is for a 4 core CPU.
```
make crossgcc-i386 CPUS=4
```
> NOTE: At this moment even if the target have the x86_64 arch the toolchain must be compiled for i386.

# 6 Extract binary blobs

Unfortunately to have a working machine we have to include a few [blobs](https://en.wikipedia.org/wiki/Binary_blob) from Intel.
You can get those blobs from a compatible **downloaded** firmware, from a your firmware **backup** or from your chromebook machine.

To extract the firmware we need the `cbfstool`.

> NOTE: Right now there isn't a open source solution to replace those blobs. For more info see [Binary Situation](https://www.coreboot.org/Binary_situation)

## 6.1 Build CBFSTool
The process should be quick, in case you can specify the CPU core count with the `-j` [parameter](https://www.gnu.org/software/make/manual/make.html#Parallel).
```
cd ./util/cbfstool
make
cd..
cd..
```

## 6.3 Make a Backup
If you don't have the binary files and you don't have a firmware file you can extract it from your current firmware.

>NOTE: You need the flashrom binary, ChromeOS already provide this executable. If you're running a linux distro you can download and install it.

```
./flashrom -r peppy.rom
```

## 6.4 Extract binaries from a firmware
This step might be different for you, this strongly depends on your target platform.
Check in the `config` directory for the file `.config.{your_target}.uefi` for the correct path and the needed blobs.

For `peppy` we need the following binaries
 * `mrc.bin` [DRAM Init binary](https://doc.coreboot.org/northbridge/intel/haswell/mrc.bin.html)
 * `cpu_microcode_blob.bin` CPU Microcode
 * `vbt.bin` Board specific binary used to initialize the video
 * `IntelGopDriver.efi` Board specific binary used to initialize the video, this file with vbt.nin makes the VBIOS
Optional binaries:
 * `er.RW.flat` The [EC](https://www.chromium.org/chromium-os/ec-development) firmware
 * `me.bin` [Intel Management Engine](https://www.coreboot.org/Intel_Management_Engine)

> NOTE: The EC firmware can be compiled and than copied into this repository.
> Is also possible to not include the EC firmware into the image by changing the config.
> Add a comment with a `#` on lines that start with `CONFIG_EC_GOOGLE_`

> NOTE: When reading the flash memory the area where the Intel ME firmware is stored is read as 'OxFF' so is not possible to extract the Intel ME firmware directly from the flash backup.

We are extracting some of the necessary binaries from a **backup** firmware file: `peppy.rom`
```
cbfstool peppy.rom extract -n mrc.bin -f mrc.bin
cbfstool peppy.rom extract -n cpu_microcode_blob.bin -f cpu_microcode_blob.bin
cbfstool peppy.rom extract -n vbt.bin -f vbt.bin
```

Now we have to copy all the extracted files in the right location. For `peppy`:

```
mkdir -p blobs/soc/hsw/intel/book
cp mrc.bin blobs/soc/hsw/intel/book/
cp vbt.bin blobs/soc/hsw/intel/book/
cp cpu_microcode_blob.bin blobs/soc/hsw/intel/
```

## 6.5 Extract Intel ME
**TODO**

## 6.6 Extract GOP UEFI
**TODO**

# 7 Build Coreboot
Now you can build your coreboot firmware.

You can build an image for a specified platform using:

```
./build-uefi.sh peppy
```

If you want to build the images for all the platforms you can simply specify:

```
./build-uefi.sh
```

# 8 Flash the image
Once the compilation is finished you can now flash the image:
```
./flashrom -i BIOS -w {your_image_file.rom}
```
