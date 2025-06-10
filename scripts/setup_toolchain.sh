#!/usr/bin/env bash
set -e

install_pkgs() {
  if ! apt-get install -y --no-install-recommends "$@"; then
    return 1
  fi
}

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root or with sudo" >&2
  exit 1
fi

apt-get update

# Core build tools
install_pkgs build-essential

# Attempt to install cross compilers forcing the amd64 variants first. Some
# Ubuntu installations report these packages as virtual when multiarch pulls in
# i386 alternatives. Falling back to the default package names improves
# compatibility across releases.
cross_pkgs=(
  gcc-i686-linux-gnu:amd64 g++-i686-linux-gnu:amd64
  gcc-x86-64-linux-gnu:amd64 g++-x86-64-linux-gnu:amd64
)
if ! install_pkgs "${cross_pkgs[@]}"; then
  echo "Falling back to default cross compiler package names" >&2
  install_pkgs gcc-i686-linux-gnu g++-i686-linux-gnu \
               gcc-x86-64-linux-gnu g++-x86-64-linux-gnu
fi

install_pkgs qemu-system-x86

echo "Toolchain installation complete."
