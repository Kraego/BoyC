FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu

# Install CMake, gcc, g++ and other essentials
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*
