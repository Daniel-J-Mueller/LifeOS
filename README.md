# LifeOS

LifeOS is an emulatable operating system designed to host complex intelligence frameworks. The project aims for minimalist outputs and indirect human control, allowing autonomous operation with optional human intervention. Visual feedback is kept informative and relevant to the active user. Each human machine interface (HMI) is viewed as an input modality to a broader intelligent system that reacts in the best interest of the user and OS.

The project is built from the ground up with a focus on careful planning before implementation to avoid common OS pitfalls. All components, from the bootloader to user interaction, will be developed in stages with an emphasis on testability and modularity.

Design documents live under the `Documentation` directory. The `scripts` folder contains utilities for setting up the build environment. Run `sudo ./scripts/setup_toolchain.sh` to install the cross compiler and emulator packages required for development. After the toolchain is ready, execute `./scripts/build_image.sh` to create a bootable image. See `Documentation/build_environment/build_and_run.txt` for instructions on running the image under QEMU.

Implementation code lives under the `src` directory. The layout mirrors the OS
components described in the architecture documentation:

- `src/boot`   contains the bootloader.
- `src/kernel` holds the kernel and its subsystems.
- `src/hal`    houses the hardware abstraction layer.
- `src/drivers` stores device drivers.

Refer to `Documentation/source_layout.txt` for a full description of the source
tree.
