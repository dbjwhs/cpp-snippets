# C++ Development Environment Docker Container

This repository contains everything you need to set up a modern C++20 development environment using Docker on macOS.

## Features

- Ubuntu 22.04 base
- Modern C++20 development environment
- GCC-13 compiler with full C++20 support (including `<format>`)
- Clang-16 as an alternative compiler
- CMake 3.30.0
- Visual Studio Code (code-server)
- SSH server for remote access
- CURL development libraries
- {fmt} library as a fallback for C++20 format, this was need for some of my projects

## Prerequisites

- Docker Desktop for Mac
- Git
- SSH client

## Getting Started

### Build and start the container (stop if you are re-building)

```bash
docker-compose stop
docker-compose build
docker-compose up -d
```

Create a persistent development workspace

```bash
mkdir -p ~/cppsnip-dev
```

This will:
- Build the Docker image based on the Dockerfile
- Start the container in detached mode
- Mount the `~/cppsnip-dev` directory to `/cppsnip-dev` in the container
- Expose port 8080 for VS Code and port 2222 for SSH

### Access Visual Studio Code

Open your browser and navigate to:

```
http://localhost:8080
```

Login using the password: `<add your password here>` that you changed in `dockerfile`

### SSH Access

To connect to the container via SSH without host key verification warnings:

```bash
ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null root@localhost -p 2222
```

Password: `yourpassword`

For convenience, add this to your SSH config (`~/.ssh/config`):

```
Host docker-cpp
    HostName localhost
    Port 2222
    User root
    StrictHostKeyChecking no
    UserKnownHostsFile /dev/null
```

Then connect simply with:

```bash
ssh docker-cpp
```

## Using the Development Environment

### Compilers

The container has both GCC-13 and Clang-16 installed:

- GCC-13 is set as the default (with full C++20 support)
- Use `g++` for C++ compilation with GCC
- Use `clang++` for C++ compilation with Clang

### CMake

The environment includes CMake 3.30.0:

```bash
cd /path/to/your/project
mkdir build && cd build
cmake ..
make
```

For C++20 features, add to your CMakeLists.txt:

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### GitHub Integration

To clone repositories from GitHub, either:

1. Mount your SSH keys when starting the container:

Add to your docker-compose.yml:
```yaml
volumes:
  - ~/cppsnip-dev:/cppsnip-dev
  - ~/.ssh:/root/.ssh:ro
```

2. Generate a new SSH key in the container:
```bash
ssh-keygen -t ed25519 -C "your_email@example.com"
```
Then add the public key to your GitHub account.

## Persistence

Files are persisted in these locations:

- `/cppsnip-dev` in the container maps to `~/cppsnip-dev` on your host
- Any SSH keys mounted from your host remain unchanged

## Customization

### Change the Password

To change the VS Code or SSH password, edit these lines in the Dockerfile:

```dockerfile
# VS Code password
echo "password: yourpassword" >> ~/.config/code-server/config.yaml

# SSH password
echo 'root:yourpassword' | chpasswd
```

### Add More Development Tools

Add additional packages in the initial apt-get install section:

```dockerfile
RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    git \
    # Add more packages here
```

## Troubleshooting

### SSH Connection Issues

If you have trouble connecting via SSH, check:

1. The SSH service is running:
```bash
docker exec -it $(docker ps -q --filter "name=cpp-dev") service ssh status
```

2. If needed, restart the SSH service:
```bash
docker exec -it $(docker ps -q --filter "name=cpp-dev") service ssh restart
```

### VS Code Cannot Access Files

If VS Code can't access files, check volume mounting:

```bash
docker inspect $(docker ps -q --filter "name=cpp-dev") | grep Mounts -A 10
```

### Container Not Starting

Check logs:

```bash
docker-compose logs
```

## License

This project is licensed under the MIT License
