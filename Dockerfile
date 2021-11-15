FROM rabits/qt:5.12-desktop

USER root

RUN apt-get update && apt-get install -y --no-install-recommends \
        libopencv-core-dev \
        libopencv-imgproc-dev \
	libqt5multimedia5 \
	libpulse-dev

COPY . /MediapipeExample/
