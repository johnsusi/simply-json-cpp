FROM ubuntu:18.04 as ubuntu1804

RUN \
  apt-get update -y && \
  apt-get install -y --no-install-recommends \
    apt-transport-https \
    build-essential \
    ca-certificates \
    gnupg \
     \
    software-properties-common \
    wget && \
  wget -O - \
    https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null \
    | gpg --dearmor - \
    | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
  wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key \
    | apt-key add - && \
  apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
  apt-add-repository 'deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-12 main' && \
  add-apt-repository ppa:ubuntu-toolchain-r/test && \
  apt-get update -y && \
  apt-get install -y --no-install-recommends clang-12 cmake g++-10 ninja-build



WORKDIR /usr/src/simply-json

COPY . ./

RUN make clean all BUILD_DIR=build/gcc-debug     BUILD_TYPE=Debug   CXX=g++-10
RUN make clean all BUILD_DIR=build/gcc-release   BUILD_TYPE=Release CXX=g++-10
RUN make clean all BUILD_DIR=build/clang-debug   BUILD_TYPE=Debug   CXX=clang++-12
RUN make clean all BUILD_DIR=build/clang-release BUILD_TYPE=Release CXX=clang++-12
