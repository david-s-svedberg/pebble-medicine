FROM mcr.microsoft.com/vscode/devcontainers/base:0-ubuntu-18.04
LABEL MAINTAINER="David Svedberg"

ENV DEBIAN_FRONTEND=noninteractive
# Install dependencies
RUN apt-get update \
    && apt-get -y install --no-install-recommends \
    build-essential \
    curl \
    python2.7-dev \
    python-pip \
    libfreetype6 \
    libc6-dev-i386 \

   #
   # Clean up
   && apt-get autoremove -y \
   && apt-get clean -y \
   && rm -rf /var/lib/apt/lists/*
ENV DEBIAN_FRONTEND=dialog

# Install Pebble Tool
ENV PEBBLE_TOOL_VERSION pebble-sdk-4.5-linux64
ENV PEBBLE_PATH /root/pebble-dev
ENV PEBBLE_HOME $PEBBLE_PATH/$PEBBLE_TOOL_VERSION
ENV PATH $PEBBLE_HOME/bin:$PATH

RUN mkdir -p $PEBBLE_PATH
RUN curl -sSL https://developer.rebble.io/s3.amazonaws.com/assets.getpebble.com/pebble-tool/$PEBBLE_TOOL_VERSION.tar.bz2 \
		| tar -v -C $PEBBLE_PATH -xj

WORKDIR $PEBBLE_HOME

RUN pip install --upgrade setuptools
RUN pip install wheel

RUN /bin/bash -c " \
    pip install virtualenv \
    && virtualenv .env \
    && source .env/bin/activate \
    && pip install -r requirements.txt \
    && deactivate \
    "
RUN apt-get update \
    && apt-get -y install --no-install-recommends \
    libsdl1.2debian libfdt1 libpixman-1-0 npm

RUN /bin/bash -c " \
    mkdir -p /root/.pebble-sdk \
    && touch /root/.pebble-sdk/NO_TRACKING \
    "

# Install Pebble SDK
RUN yes | pebble sdk install https://github.com/aveao/PebbleArchive/raw/master/SDKCores/sdk-core-4.3.tar.bz2

RUN apt-get install -y nodejs