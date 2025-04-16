FROM mcr.microsoft.com/devcontainers/cpp:ubuntu

# Install general build tools
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    && apt-get clean

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