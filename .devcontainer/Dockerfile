FROM mcr.microsoft.com/devcontainers/cpp:ubuntu

# Add LLVM repository for latest Clang version
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    echo "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-16 main" >> /etc/apt/sources.list && \
    apt-get update

# Install general build tools and latest Clang
RUN apt-get update && apt-get install -y \
    build-essential \
    clang-format \
    clang-16 \
    lldb-16 \
    lld-16 \
    libc++-16-dev \
    libc++abi-16-dev \
    && apt-get clean

# Create symbolic links for the latest Clang version
RUN ln -sf /usr/bin/clang-16 /usr/bin/clang && \
    ln -sf /usr/bin/clang++-16 /usr/bin/clang++

# Install CMake 4.0.1
RUN wget https://github.com/Kitware/CMake/releases/download/v4.0.1/cmake-4.0.1-linux-x86_64.sh && \
    chmod +x cmake-4.0.1-linux-x86_64.sh && \
    ./cmake-4.0.1-linux-x86_64.sh --skip-license --prefix=/usr/local && \
    rm cmake-4.0.1-linux-x86_64.sh

RUN mkdir -p /opt && \
    cd /opt && \
    wget -O gurobi12.0.1_linux64.tar.gz https://packages.gurobi.com/12.0/gurobi12.0.1_linux64.tar.gz && \
    tar -xvf gurobi12.0.1_linux64.tar.gz && \
    rm gurobi12.0.1_linux64.tar.gz

# Set environment variables
ENV GUROBI_HOME="/opt/gurobi1201/linux64"
ENV PATH="${PATH}:${GUROBI_HOME}/bin"
ENV LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${GUROBI_HOME}/lib"