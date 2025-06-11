FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu

ENV LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8

# Install CMake, gcc, g++ and other essentials
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    gdb \
    && rm -rf /var/lib/apt/lists/*
