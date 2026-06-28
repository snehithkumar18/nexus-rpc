FROM gcr.io/oss-fuzz-base/base-builder:v1
RUN apt-get update && apt-get install -y make cmake

COPY . $SRC/nexus_rpc
WORKDIR $SRC/nexus_rpc
COPY .clusterfuzzlite/build.sh $SRC/build.sh
chmod +x $SRC/build.sh
