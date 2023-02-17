
# Mediapipe integration to Qt application example

**NOTE** Also checkout https://github.com/rajkundu/libmp-example

Example on how to integrate mediapipe as a dynamic library into Qt applicaton on Linux.

Resulting application:

 [![](images/results.gif)]()

Mediapipe examples implemented are:
 * SelfieSegmentation
 * HandTracking
 * FaceDetection
 * FaceMesh
 * HairSegmentation
 * HolisticTracking
 * IrisTracking
 * ObjectDetection
 * PoseTracking


## Guide
Guide assumes Ubuntu 18.04. Project was built using Qt 5.12, I don't know if it works on higher or lower versions.

1. In order to build the mediapipe as .so, you need to clone my fork of mediapipe and checkout the linux-lib-example branch: 

`git clone https://github.com/asprecic/mediapipe.git
&& cd mediapipe
&& git checkout linux-lib-example`

2. You can build mediapipe using the provided Dockerfile or if you want to do it natively, follow mediapipe's guide on how to build it.
My forked repo contains additional example under examples/desktop/libexample which you have to build like this:

`bazel build --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/libexample:libexample.so`

3. You can find the built libexample.so under bazel-bin/mediapipe/examples/desktop/libexample/libexample.so.
Copy libexample.so to EXAMPLE_LIB_PATH/lib. Sadly, header files don't get generated so you will have to
manually copy example.h to EXAMPLE_LIB_PATH/include (location of example.h is mediapipe/examples/desktop/libexample/example.h).
At this point, you have libexample.so and coresponding header file example.h which you can use to integrate into any application.
4. Create user.pri and specify EXAMPLE_LIB_PATH
5. Create mediapipe directory wherever the final binary of build is, and copy mediapipe/modules and mediapipe/models to it.
In these folders, .tflite models can be found, without which example can't run. (poller will not return packets)
6. Install dependencies `apt install libopencv-core-dev libopencv-imgproc-dev libpulse-dev`
6. Build the Qt project and try the example.

Forked Mediapipe repo disables unused opencv dependencies as they bloat the dependencies (if all enabled, about 230 dependencies in total, including dumb dependencies like mysqlclient, sqlite and ffmpeg, which are not needed at all for these examples). If you need to work with videoio or use any other opencv dependencies, you can enable them for linking in mediapipe/third_party/opencv_linux.BUILD.

## License
License is MIT, do whatever you want with this.
If you found this example helpful, I would appreciate feedback that you found it useful,
via github stars or anything.

## Contributing
If you have any improvements to this example, please do suggest them
and share them with the community.
I accept PR's and any additions to this example (eg. Windows and MacOS integration)
