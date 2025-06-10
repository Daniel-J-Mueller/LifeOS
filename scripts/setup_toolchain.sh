#!/usr/bin/env bash
set -e

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root or with sudo" >&2
  exit 1
fi

apt-get update
apt-get install -y \
  build-essential \
  gcc-i686-linux-gnu:amd64 g++-i686-linux-gnu:amd64 \
  gcc-x86-64-linux-gnu:amd64 g++-x86-64-linux-gnu:amd64 \
  qemu-system-x86

echo "Toolchain installation complete."
