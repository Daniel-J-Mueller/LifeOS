#!/usr/bin/env bash
set -e

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root or with sudo" >&2
  exit 1
fi

apt-get update

# Core build tools (gcc/g++)
apt-get install -y --no-install-recommends build-essential

# QEMU and KVM for running x86 binaries
apt-get install -y --no-install-recommends qemu-system-x86 qemu-kvm

echo "Toolchain installation complete."