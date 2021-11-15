#include "graphs.h"

StyleClass::StyleClass() : QObject() {}

void StyleClass::declareQML() {
	qmlRegisterType<StyleClass>("com.example.graphs", 1, 0, "Graphs");
	qmlRegisterType<StyleClass>("com.example.graphs", 1, 0, "StyleClass");
}

QString StyleClass::graphName(StyleClass::Graphs graph)
{
	switch(graph){
	case SelfieSegmentation: return "SelfieSegmentation";
	case HandTracking: return "HandTracking";
	case FaceDetection: return "FaceDetection";
	case FaceMesh: return "FaceMesh";
	case HairSegmentation: return "HairSegmentation";
	case HolisticTracking: return "HolisticTracking";
	case IrisTracking: return "IrisTracking";
	case ObjectDetection: return "ObjectDetection";
	case PoseTracking: return "PoseTracking";
	}
}

std::string StyleClass::getGraph(StyleClass::Graphs graph) {
	switch(graph){
	case SelfieSegmentation:
		return  R"(
			input_stream: "input_video"

			output_stream: "output_video"

			node {
			calculator: "FlowLimiterCalculator"
			input_stream: "input_video"
			input_stream: "FINISHED:output_video"
			input_stream_info: {
			tag_index: "FINISHED"
			back_edge: true
			}
			output_stream: "throttled_input_video"
			}

			# Subgraph that performs selfie segmentation.
			node {
			calculator: "SelfieSegmentationCpu"
			input_stream: "IMAGE:throttled_input_video"
			output_stream: "SEGMENTATION_MASK:segmentation_mask"
			}


			# Colors the selfie segmentation with the color specified in the option.
			node {
			calculator: "RecolorCalculator"
			input_stream: "IMAGE:throttled_input_video"
			input_stream: "MASK:segmentation_mask"
			output_stream: "IMAGE:output_video"
			node_options: {
			[type.googleapis.com/mediapipe.RecolorCalculatorOptions] {
			color { r: 0 g: 0 b: 255 }
			mask_channel: RED
			invert_mask: true
			adjust_with_luminance: false
			}
			}
			}

			)";
	case HandTracking:
		return R"(
		       # MediaPipe graph that performs hand detection on desktop with TensorFlow Lite
		       # on CPU.
		       # Used in the example in
		       # mediapipe/examples/desktop/hand_tracking:hand_detection_cpu.

		       # CPU image. (ImageFrame)
		       input_stream: "input_video"

		       # CPU image. (ImageFrame)
		       output_stream: "output_video"

		       # Detects palms.
		       node {
		       calculator: "PalmDetectionCpu"
		       input_stream: "IMAGE:input_video"
		       output_stream: "DETECTIONS:output_detections"
		       }

		       # Converts the detections to drawing primitives for annotation overlay.
		       node {
		       calculator: "DetectionsToRenderDataCalculator"
		       input_stream: "DETECTIONS:output_detections"
		       output_stream: "RENDER_DATA:render_data"
		       node_options: {
		       [type.googleapis.com/mediapipe.DetectionsToRenderDataCalculatorOptions] {
		       thickness: 4.0
		       color { r: 0 g: 255 b: 0 }
		       }
		       }
		       }

		       # Draws annotations and overlays them on top of the original image coming into
		       # the graph.
		       node {
		       calculator: "AnnotationOverlayCalculator"
		       input_stream: "IMAGE:input_video"
		       input_stream: "render_data"
		       output_stream: "IMAGE:output_video"
		       }


		       )";
	case FaceDetection:
		return R"(
		       # MediaPipe graph that performs face detection with TensorFlow Lite on CPU.
		       # Used in the examples in
		       # mediapipe/examples/desktop/face_detection:face_detection_cpu.

		       input_stream: "input_video"
		       output_stream: "output_video"

		       node {
			 calculator: "FlowLimiterCalculator"
			 input_stream: "input_video"
			 input_stream: "FINISHED:detections"
			 input_stream_info: {
			   tag_index: "FINISHED"
			   back_edge: true
			 }
			 output_stream: "throttled_input_video"
		       }

		       # Detects faces.
		       node {
			 calculator: "FaceDetectionFullRangeCpu"
			 input_stream: "IMAGE:throttled_input_video"
			 output_stream: "DETECTIONS:detections"
		       }

		       # Converts the detections to drawing primitives for annotation overlay.
		       node {
			 calculator: "DetectionsToRenderDataCalculator"
			 input_stream: "DETECTIONS:detections"
			 output_stream: "RENDER_DATA:render_data"
			 node_options: {
			   [type.googleapis.com/mediapipe.DetectionsToRenderDataCalculatorOptions] {
			     thickness: 4.0
			     color { r: 255 g: 0 b: 0 }
			   }
			 }
		       }

		       # Draws annotations and overlays them on top of the input images.
		       node {
			 calculator: "AnnotationOverlayCalculator"
			 input_stream: "IMAGE:throttled_input_video"
			 input_stream: "render_data"
			 output_stream: "IMAGE:output_video"
		       }
				)";
	case HairSegmentation:
		return R"(
		       # MediaPipe graph that performs hair segmentation with TensorFlow Lite on CPU.
		       # Used in the example in
		       # mediapipie/examples/desktop/hair_segmentation:hair_segmentation_cpu

		       # Images on CPU coming into and out of the graph.
		       input_stream: "input_video"
		       output_stream: "output_video"

		       # Throttles the images flowing downstream for flow control. It passes through
		       # the very first incoming image unaltered, and waits for
		       # TfLiteTensorsToSegmentationCalculator downstream in the graph to finish
		       # generating the corresponding hair mask before it passes through another
		       # image. All images that come in while waiting are dropped, limiting the number
		       # of in-flight images between this calculator and
		       # TfLiteTensorsToSegmentationCalculator to 1. This prevents the nodes in between
		       # from queuing up incoming images and data excessively, which leads to increased
		       # latency and memory usage, unwanted in real-time mobile applications. It also
		       # eliminates unnecessarily computation, e.g., a transformed image produced by
		       # ImageTransformationCalculator may get dropped downstream if the subsequent
		       # TfLiteConverterCalculator or TfLiteInferenceCalculator is still busy
		       # processing previous inputs.
		       node {
			 calculator: "FlowLimiterCalculator"
			 input_stream: "input_video"
			 input_stream: "FINISHED:hair_mask"
			 input_stream_info: {
			   tag_index: "FINISHED"
			   back_edge: true
			 }
			 output_stream: "throttled_input_video"
		       }

		       # Transforms the input image on CPU to a 512x512 image. To scale the image, by
		       # default it uses the STRETCH scale mode that maps the entire input image to the
		       # entire transformed image. As a result, image aspect ratio may be changed and
		       # objects in the image may be deformed (stretched or squeezed), but the hair
		       # segmentation model used in this graph is agnostic to that deformation.
		       node: {
			 calculator: "ImageTransformationCalculator"
			 input_stream: "IMAGE:throttled_input_video"
			 output_stream: "IMAGE:transformed_input_video"
			 node_options: {
			   [type.googleapis.com/mediapipe.ImageTransformationCalculatorOptions] {
			     output_width: 512
			     output_height: 512
			   }
			 }
		       }

		       # Caches a mask fed back from the previous round of hair segmentation, and upon
		       # the arrival of the next input image sends out the cached mask with the
		       # timestamp replaced by that of the input image, essentially generating a packet
		       # that carries the previous mask. Note that upon the arrival of the very first
		       # input image, an empty packet is sent out to jump start the feedback loop.
		       node {
			 calculator: "PreviousLoopbackCalculator"
			 input_stream: "MAIN:throttled_input_video"
			 input_stream: "LOOP:hair_mask"
			 input_stream_info: {
			   tag_index: "LOOP"
			   back_edge: true
			 }
			 output_stream: "PREV_LOOP:previous_hair_mask"
		       }

		       # Embeds the hair mask generated from the previous round of hair segmentation
		       # as the alpha channel of the current input image.
		       node {
			 calculator: "SetAlphaCalculator"
			 input_stream: "IMAGE:transformed_input_video"
			 input_stream: "ALPHA:previous_hair_mask"
			 output_stream: "IMAGE:mask_embedded_input_video"
		       }

		       # Converts the transformed input image on CPU into an image tensor stored in
		       # TfLiteTensor. The zero_center option is set to false to normalize the
		       # pixel values to [0.f, 1.f] as opposed to [-1.f, 1.f]. With the
		       # max_num_channels option set to 4, all 4 RGBA channels are contained in the
		       # image tensor.
		       node {
			 calculator: "TfLiteConverterCalculator"
			 input_stream: "IMAGE:mask_embedded_input_video"
			 output_stream: "TENSORS:image_tensor"
			 node_options: {
			   [type.googleapis.com/mediapipe.TfLiteConverterCalculatorOptions] {
			     zero_center: false
			     max_num_channels: 4
			   }
			 }
		       }

		       # Generates a single side packet containing a TensorFlow Lite op resolver that
		       # supports custom ops needed by the model used in this graph.
		       node {
			 calculator: "TfLiteCustomOpResolverCalculator"
			 output_side_packet: "op_resolver"
			 node_options: {
			   [type.googleapis.com/mediapipe.TfLiteCustomOpResolverCalculatorOptions] {
			     use_gpu: false
			   }
			 }
		       }

		       # Runs a TensorFlow Lite model on CPU that takes an image tensor and outputs a
		       # tensor representing the hair segmentation, which has the same width and height
		       # as the input image tensor.
		       node {
			 calculator: "TfLiteInferenceCalculator"
			 input_stream: "TENSORS:image_tensor"
			 output_stream: "TENSORS:segmentation_tensor"
			 input_side_packet: "CUSTOM_OP_RESOLVER:op_resolver"
			 node_options: {
			   [type.googleapis.com/mediapipe.TfLiteInferenceCalculatorOptions] {
			     model_path: "mediapipe/models/hair_segmentation.tflite"
			     use_gpu: false
			   }
			 }
		       }

		       # Decodes the segmentation tensor generated by the TensorFlow Lite model into a
		       # mask of values in [0, 255], stored in a CPU buffer. It also
		       # takes the mask generated previously as another input to improve the temporal
		       # consistency.
		       node {
			 calculator: "TfLiteTensorsToSegmentationCalculator"
			 input_stream: "TENSORS:segmentation_tensor"
			 input_stream: "PREV_MASK:previous_hair_mask"
			 output_stream: "MASK:hair_mask"
			 node_options: {
			   [type.googleapis.com/mediapipe.TfLiteTensorsToSegmentationCalculatorOptions] {
			     tensor_width: 512
			     tensor_height: 512
			     tensor_channels: 2
			     combine_with_previous_ratio: 0.9
			     output_layer_index: 1
			   }
			 }
		       }

		       # Colors the hair segmentation with the color specified in the option.
		       node {
			 calculator: "RecolorCalculator"
			 input_stream: "IMAGE:throttled_input_video"
			 input_stream: "MASK:hair_mask"
			 output_stream: "IMAGE:output_video"
			 node_options: {
			   [type.googleapis.com/mediapipe.RecolorCalculatorOptions] {
			     color { r: 0 g: 0 b: 255 }
			     mask_channel: RED
			   }
			 }
		       }

		       )";
	case HolisticTracking:
		return R"(
		       # Tracks and renders pose + hands + face landmarks.

		       # CPU image. (ImageFrame)
		       input_stream: "input_video"

		       # CPU image with rendered results. (ImageFrame)
		       output_stream: "output_video"

		       # Throttles the images flowing downstream for flow control. It passes through
		       # the very first incoming image unaltered, and waits for downstream nodes
		       # (calculators and subgraphs) in the graph to finish their tasks before it
		       # passes through another image. All images that come in while waiting are
		       # dropped, limiting the number of in-flight images in most part of the graph to
		       # 1. This prevents the downstream nodes from queuing up incoming images and data
		       # excessively, which leads to increased latency and memory usage, unwanted in
		       # real-time mobile applications. It also eliminates unnecessarily computation,
		       # e.g., the output produced by a node may get dropped downstream if the
		       # subsequent nodes are still busy processing previous inputs.
		       node {
			 calculator: "FlowLimiterCalculator"
			 input_stream: "input_video"
			 input_stream: "FINISHED:output_video"
			 input_stream_info: {
			   tag_index: "FINISHED"
			   back_edge: true
			 }
			 output_stream: "throttled_input_video"
			 node_options: {
			   [type.googleapis.com/mediapipe.FlowLimiterCalculatorOptions] {
			     max_in_flight: 1
			     max_in_queue: 1
			     # Timeout is disabled (set to 0) as first frame processing can take more
			     # than 1 second.
			     in_flight_timeout: 0
			   }
			 }
		       }

		       node {
			 calculator: "HolisticLandmarkCpu"
			 input_stream: "IMAGE:throttled_input_video"
			 output_stream: "POSE_LANDMARKS:pose_landmarks"
			 output_stream: "POSE_ROI:pose_roi"
			 output_stream: "POSE_DETECTION:pose_detection"
			 output_stream: "FACE_LANDMARKS:face_landmarks"
			 output_stream: "LEFT_HAND_LANDMARKS:left_hand_landmarks"
			 output_stream: "RIGHT_HAND_LANDMARKS:right_hand_landmarks"
		       }

		       # Gets image size.
		       node {
			 calculator: "ImagePropertiesCalculator"
			 input_stream: "IMAGE:throttled_input_video"
			 output_stream: "SIZE:image_size"
		       }

		       # Converts pose, hands and face landmarks to a render data vector.
		       node {
			 calculator: "HolisticTrackingToRenderData"
			 input_stream: "IMAGE_SIZE:image_size"
			 input_stream: "POSE_LANDMARKS:pose_landmarks"
			 input_stream: "POSE_ROI:pose_roi"
			 input_stream: "LEFT_HAND_LANDMARKS:left_hand_landmarks"
			 input_stream: "RIGHT_HAND_LANDMARKS:right_hand_landmarks"
			 input_stream: "FACE_LANDMARKS:face_landmarks"
			 output_stream: "RENDER_DATA_VECTOR:render_data_vector"
		       }

		       # Draws annotations and overlays them on top of the input images.
		       node {
			 calculator: "AnnotationOverlayCalculator"
			 input_stream: "IMAGE:throttled_input_video"
			 input_stream: "VECTOR:render_data_vector"
			 output_stream: "IMAGE:output_video"
		       }

		       )";
	case IrisTracking:
		return R"(
		       # MediaPipe graph that performs iris tracking on desktop with TensorFlow Lite
		       # on CPU.
		       # Used in the example in
		       # mediapipie/examples/desktop/iris_tracking:iris_tracking_cpu.

		       # CPU image. (ImageFrame)
		       input_stream: "input_video"

		       # CPU image. (ImageFrame)
		       output_stream: "output_video"
		       # Face landmarks with iris. (NormalizedLandmarkList)
		       output_stream: "face_landmarks_with_iris"

		       # Defines how many faces to detect. Iris tracking currently only handles one
		       # face (left and right eye), and therefore this should always be set to 1.
		       node {
			 calculator: "ConstantSidePacketCalculator"
			 output_side_packet: "PACKET:0:num_faces"
			 node_options: {
			   [type.googleapis.com/mediapipe.ConstantSidePacketCalculatorOptions]: {
			     packet { int_value: 1 }
			   }
			 }
		       }

		       # Detects faces and corresponding landmarks.
		       node {
			 calculator: "FaceLandmarkFrontCpu"
			 input_stream: "IMAGE:input_video"
			 input_side_packet: "NUM_FACES:num_faces"
			 output_stream: "LANDMARKS:multi_face_landmarks"
			 output_stream: "ROIS_FROM_LANDMARKS:face_rects_from_landmarks"
			 output_stream: "DETECTIONS:face_detections"
			 output_stream: "ROIS_FROM_DETECTIONS:face_rects_from_detections"
		       }

		       # Gets the very first and only face from "multi_face_landmarks" vector.
		       node {
			 calculator: "SplitNormalizedLandmarkListVectorCalculator"
			 input_stream: "multi_face_landmarks"
			 output_stream: "face_landmarks"
			 node_options: {
			   [type.googleapis.com/mediapipe.SplitVectorCalculatorOptions] {
			     ranges: { begin: 0 end: 1 }
			     element_only: true
			   }
			 }
		       }

		       # Gets the very first and only face rect from "face_rects_from_landmarks"
		       # vector.
		       node {
			 calculator: "SplitNormalizedRectVectorCalculator"
			 input_stream: "face_rects_from_landmarks"
			 output_stream: "face_rect"
			 node_options: {
			   [type.googleapis.com/mediapipe.SplitVectorCalculatorOptions] {
			     ranges: { begin: 0 end: 1 }
			     element_only: true
			   }
			 }
		       }

		       # Gets two landmarks which define left eye boundary.
		       node {
			 calculator: "SplitNormalizedLandmarkListCalculator"
			 input_stream: "face_landmarks"
			 output_stream: "left_eye_boundary_landmarks"
			 node_options: {
			   [type.googleapis.com/mediapipe.SplitVectorCalculatorOptions] {
			     ranges: { begin: 33 end: 34 }
			     ranges: { begin: 133 end: 134 }
			     combine_outputs: true
			   }
			 }
		       }

		       # Gets two landmarks which define right eye boundary.
		       node {
			 calculator: "SplitNormalizedLandmarkListCalculator"
			 input_stream: "face_landmarks"
			 output_stream: "right_eye_boundary_landmarks"
			 node_options: {
			   [type.googleapis.com/mediapipe.SplitVectorCalculatorOptions] {
			     ranges: { begin: 362 end: 363 }
			     ranges: { begin: 263 end: 264 }
			     combine_outputs: true
			   }
			 }
		       }

		       # Detects iris landmarks, eye contour landmarks, and corresponding rect (ROI).
		       node {
			 calculator: "IrisLandmarkLeftAndRightCpu"
			 input_stream: "IMAGE:input_video"
			 input_stream: "LEFT_EYE_BOUNDARY_LANDMARKS:left_eye_boundary_landmarks"
			 input_stream: "RIGHT_EYE_BOUNDARY_LANDMARKS:right_eye_boundary_landmarks"
			 output_stream: "LEFT_EYE_CONTOUR_LANDMARKS:left_eye_contour_landmarks"
			 output_stream: "LEFT_EYE_IRIS_LANDMARKS:left_iris_landmarks"
			 output_stream: "LEFT_EYE_ROI:left_eye_rect_from_landmarks"
			 output_stream: "RIGHT_EYE_CONTOUR_LANDMARKS:right_eye_contour_landmarks"
			 output_stream: "RIGHT_EYE_IRIS_LANDMARKS:right_iris_landmarks"
			 output_stream: "RIGHT_EYE_ROI:right_eye_rect_from_landmarks"
		       }

		       node {
			 calculator: "ConcatenateNormalizedLandmarkListCalculator"
			 input_stream: "left_eye_contour_landmarks"
			 input_stream: "right_eye_contour_landmarks"
			 output_stream: "refined_eye_landmarks"
		       }

		       node {
			 calculator: "UpdateFaceLandmarksCalculator"
			 input_stream: "NEW_EYE_LANDMARKS:refined_eye_landmarks"
			 input_stream: "FACE_LANDMARKS:face_landmarks"
			 output_stream: "UPDATED_FACE_LANDMARKS:updated_face_landmarks"
		       }

		       # Renders annotations and overlays them on top of the input images.
		       node {
			 calculator: "IrisRendererCpu"
			 input_stream: "IMAGE:input_video"
			 input_stream: "FACE_LANDMARKS:updated_face_landmarks"
			 input_stream: "EYE_LANDMARKS_LEFT:left_eye_contour_landmarks"
			 input_stream: "EYE_LANDMARKS_RIGHT:right_eye_contour_landmarks"
			 input_stream: "IRIS_LANDMARKS_LEFT:left_iris_landmarks"
			 input_stream: "IRIS_LANDMARKS_RIGHT:right_iris_landmarks"
			 input_stream: "NORM_RECT:face_rect"
			 input_stream: "LEFT_EYE_RECT:left_eye_rect_from_landmarks"
			 input_stream: "RIGHT_EYE_RECT:right_eye_rect_from_landmarks"
			 input_stream: "DETECTIONS:face_detections"
			 output_stream: "IRIS_LANDMARKS:iris_landmarks"
			 output_stream: "IMAGE:output_video"
		       }

		       node {
			 calculator: "ConcatenateNormalizedLandmarkListCalculator"
			 input_stream: "updated_face_landmarks"
			 input_stream: "iris_landmarks"
			 output_stream: "face_landmarks_with_iris"
		       }

		       )";
	case ObjectDetection:
		return R"(
		       # MediaPipe graph that performs object detection with TensorFlow Lite on CPU.
		       # Used in the examples in
		       # mediapipe/examples/desktop/object_detection:object_detection_cpu.

		       # Images on CPU coming into and out of the graph.
		       input_stream: "input_video"
		       output_stream: "output_video"

		       # Throttles the images flowing downstream for flow control. It passes through
		       # the very first incoming image unaltered, and waits for
		       # TfLiteTensorsToDetectionsCalculator downstream in the graph to finish
		       # generating the corresponding detections before it passes through another
		       # image. All images that come in while waiting are dropped, limiting the number
		       # of in-flight images between this calculator and
		       # TfLiteTensorsToDetectionsCalculator to 1. This prevents the nodes in between
		       # from queuing up incoming images and data excessively, which leads to increased
		       # latency and memory usage, unwanted in real-time mobile applications. It also
		       # eliminates unnecessarily computation, e.g., a transformed image produced by
		       # ImageTransformationCalculator may get dropped downstream if the subsequent
		       # TfLiteConverterCalculator or TfLiteInferenceCalculator is still busy
		       # processing previous inputs.
		       node {
			 calculator: "FlowLimiterCalculator"
			 input_stream: "input_video"
			 input_stream: "FINISHED:detections"
			 input_stream_info: {
			   tag_index: "FINISHED"
			   back_edge: true
			 }
			 output_stream: "throttled_input_video"
		       }

		       # Transforms the input image on CPU to a 320x320 image. To scale the image, by
		       # default it uses the STRETCH scale mode that maps the entire input image to the
		       # entire transformed image. As a result, image aspect ratio may be changed and
		       # objects in the image may be deformed (stretched or squeezed), but the object
		       # detection model used in this graph is agnostic to that deformation.
		       node: {
			 calculator: "ImageTransformationCalculator"
			 input_stream: "IMAGE:throttled_input_video"
			 output_stream: "IMAGE:transformed_input_video"
			 node_options: {
			   [type.googleapis.com/mediapipe.ImageTransformationCalculatorOptions] {
			     output_width: 320
			     output_height: 320
			   }
			 }
		       }

		       # Converts the transformed input image on CPU into an image tensor stored as a
		       # TfLiteTensor.
		       node {
			 calculator: "TfLiteConverterCalculator"
			 input_stream: "IMAGE:transformed_input_video"
			 output_stream: "TENSORS:image_tensor"
		       }

		       # Runs a TensorFlow Lite model on CPU that takes an image tensor and outputs a
		       # vector of tensors representing, for instance, detection boxes/keypoints and
		       # scores.
		       node {
			 calculator: "TfLiteInferenceCalculator"
			 input_stream: "TENSORS:image_tensor"
			 output_stream: "TENSORS:detection_tensors"
			 node_options: {
			   [type.googleapis.com/mediapipe.TfLiteInferenceCalculatorOptions] {
			     model_path: "mediapipe/models/ssdlite_object_detection.tflite"
			   }
			 }
		       }

		       # Generates a single side packet containing a vector of SSD anchors based on
		       # the specification in the options.
		       node {
			 calculator: "SsdAnchorsCalculator"
			 output_side_packet: "anchors"
			 node_options: {
			   [type.googleapis.com/mediapipe.SsdAnchorsCalculatorOptions] {
			     num_layers: 6
			     min_scale: 0.2
			     max_scale: 0.95
			     input_size_height: 320
			     input_size_width: 320
			     anchor_offset_x: 0.5
			     anchor_offset_y: 0.5
			     strides: 16
			     strides: 32
			     strides: 64
			     strides: 128
			     strides: 256
			     strides: 512
			     aspect_ratios: 1.0
			     aspect_ratios: 2.0
			     aspect_ratios: 0.5
			     aspect_ratios: 3.0
			     aspect_ratios: 0.3333
			     reduce_boxes_in_lowest_layer: true
			   }
			 }
		       }

		       # Decodes the detection tensors generated by the TensorFlow Lite model, based on
		       # the SSD anchors and the specification in the options, into a vector of
		       # detections. Each detection describes a detected object.
		       node {
			 calculator: "TfLiteTensorsToDetectionsCalculator"
			 input_stream: "TENSORS:detection_tensors"
			 input_side_packet: "ANCHORS:anchors"
			 output_stream: "DETECTIONS:detections"
			 node_options: {
			   [type.googleapis.com/mediapipe.TfLiteTensorsToDetectionsCalculatorOptions] {
			     num_classes: 91
			     num_boxes: 2034
			     num_coords: 4
			     ignore_classes: 0
			     sigmoid_score: true
			     apply_exponential_on_box_size: true
			     x_scale: 10.0
			     y_scale: 10.0
			     h_scale: 5.0
			     w_scale: 5.0
			     min_score_thresh: 0.6
			   }
			 }
		       }

		       # Performs non-max suppression to remove excessive detections.
		       node {
			 calculator: "NonMaxSuppressionCalculator"
			 input_stream: "detections"
			 output_stream: "filtered_detections"
			 node_options: {
			   [type.googleapis.com/mediapipe.NonMaxSuppressionCalculatorOptions] {
			     min_suppression_threshold: 0.4
			     max_num_detections: 3
			     overlap_type: INTERSECTION_OVER_UNION
			     return_empty_detections: true
			   }
			 }
		       }

		       # Maps detection label IDs to the corresponding label text. The label map is
		       # provided in the label_map_path option.
		       node {
			 calculator: "DetectionLabelIdToTextCalculator"
			 input_stream: "filtered_detections"
			 output_stream: "output_detections"
			 node_options: {
			   [type.googleapis.com/mediapipe.DetectionLabelIdToTextCalculatorOptions] {
			     label_map_path: "mediapipe/models/ssdlite_object_detection_labelmap.txt"
			   }
			 }
		       }

		       # Converts the detections to drawing primitives for annotation overlay.
		       node {
			 calculator: "DetectionsToRenderDataCalculator"
			 input_stream: "DETECTIONS:output_detections"
			 output_stream: "RENDER_DATA:render_data"
			 node_options: {
			   [type.googleapis.com/mediapipe.DetectionsToRenderDataCalculatorOptions] {
			     thickness: 4.0
			     color { r: 255 g: 0 b: 0 }
			   }
			 }
		       }

		       # Draws annotations and overlays them on top of the input images.
		       node {
			 calculator: "AnnotationOverlayCalculator"
			 input_stream: "IMAGE:throttled_input_video"
			 input_stream: "render_data"
			 output_stream: "IMAGE:output_video"
		       }

		       )";
	case PoseTracking:
		return R"(
		       # MediaPipe graph that performs pose tracking with TensorFlow Lite on CPU.

		       # CPU buffer. (ImageFrame)
		       input_stream: "input_video"

		       # Output image with rendered results. (ImageFrame)
		       output_stream: "output_video"
		       # Pose landmarks. (NormalizedLandmarkList)
		       output_stream: "pose_landmarks"

		       # Generates side packet to enable segmentation.
		       node {
			 calculator: "ConstantSidePacketCalculator"
			 output_side_packet: "PACKET:enable_segmentation"
			 node_options: {
			   [type.googleapis.com/mediapipe.ConstantSidePacketCalculatorOptions]: {
			     packet { bool_value: true }
			   }
			 }
		       }

		       # Throttles the images flowing downstream for flow control. It passes through
		       # the very first incoming image unaltered, and waits for downstream nodes
		       # (calculators and subgraphs) in the graph to finish their tasks before it
		       # passes through another image. All images that come in while waiting are
		       # dropped, limiting the number of in-flight images in most part of the graph to
		       # 1. This prevents the downstream nodes from queuing up incoming images and data
		       # excessively, which leads to increased latency and memory usage, unwanted in
		       # real-time mobile applications. It also eliminates unnecessarily computation,
		       # e.g., the output produced by a node may get dropped downstream if the
		       # subsequent nodes are still busy processing previous inputs.
		       node {
			 calculator: "FlowLimiterCalculator"
			 input_stream: "input_video"
			 input_stream: "FINISHED:output_video"
			 input_stream_info: {
			   tag_index: "FINISHED"
			   back_edge: true
			 }
			 output_stream: "throttled_input_video"
		       }

		       # Subgraph that detects poses and corresponding landmarks.
		       node {
			 calculator: "PoseLandmarkCpu"
			 input_side_packet: "ENABLE_SEGMENTATION:enable_segmentation"
			 input_stream: "IMAGE:throttled_input_video"
			 output_stream: "LANDMARKS:pose_landmarks"
			 output_stream: "SEGMENTATION_MASK:segmentation_mask"
			 output_stream: "DETECTION:pose_detection"
			 output_stream: "ROI_FROM_LANDMARKS:roi_from_landmarks"
		       }

		       # Subgraph that renders pose-landmark annotation onto the input image.
		       node {
			 calculator: "PoseRendererCpu"
			 input_stream: "IMAGE:throttled_input_video"
			 input_stream: "LANDMARKS:pose_landmarks"
			 input_stream: "SEGMENTATION_MASK:segmentation_mask"
			 input_stream: "DETECTION:pose_detection"
			 input_stream: "ROI:roi_from_landmarks"
			 output_stream: "IMAGE:output_video"
		       }

		       )";
	case FaceMesh:
		return R"(
		       # MediaPipe graph that performs face mesh with TensorFlow Lite on CPU.

		       # Input image. (ImageFrame)
		       input_stream: "input_video"

		       # Output image with rendered results. (ImageFrame)
		       output_stream: "output_video"
		       # Collection of detected/processed faces, each represented as a list of
		       # landmarks. (std::vector<NormalizedLandmarkList>)
		       output_stream: "multi_face_landmarks"

		       # Throttles the images flowing downstream for flow control. It passes through
		       # the very first incoming image unaltered, and waits for downstream nodes
		       # (calculators and subgraphs) in the graph to finish their tasks before it
		       # passes through another image. All images that come in while waiting are
		       # dropped, limiting the number of in-flight images in most part of the graph to
		       # 1. This prevents the downstream nodes from queuing up incoming images and data
		       # excessively, which leads to increased latency and memory usage, unwanted in
		       # real-time mobile applications. It also eliminates unnecessarily computation,
		       # e.g., the output produced by a node may get dropped downstream if the
		       # subsequent nodes are still busy processing previous inputs.
		       node {
			 calculator: "FlowLimiterCalculator"
			 input_stream: "input_video"
			 input_stream: "FINISHED:output_video"
			 input_stream_info: {
			   tag_index: "FINISHED"
			   back_edge: true
			 }
			 output_stream: "throttled_input_video"
		       }

		       # Defines side packets for further use in the graph.
		       node {
			 calculator: "ConstantSidePacketCalculator"
			 output_side_packet: "PACKET:0:num_faces"
			 output_side_packet: "PACKET:1:with_attention"
			 node_options: {
			   [type.googleapis.com/mediapipe.ConstantSidePacketCalculatorOptions]: {
			     packet { int_value: 1 }
			     packet { bool_value: true }
			   }
			 }
		       }

		       # Subgraph that detects faces and corresponding landmarks.
		       node {
			 calculator: "FaceLandmarkFrontCpu"
			 input_stream: "IMAGE:throttled_input_video"
			 input_side_packet: "NUM_FACES:num_faces"
			 input_side_packet: "WITH_ATTENTION:with_attention"
			 output_stream: "LANDMARKS:multi_face_landmarks"
			 output_stream: "ROIS_FROM_LANDMARKS:face_rects_from_landmarks"
			 output_stream: "DETECTIONS:face_detections"
			 output_stream: "ROIS_FROM_DETECTIONS:face_rects_from_detections"
		       }

		       # Subgraph that renders face-landmark annotation onto the input image.
		       node {
			 calculator: "FaceRendererCpu"
			 input_stream: "IMAGE:throttled_input_video"
			 input_stream: "LANDMARKS:multi_face_landmarks"
			 input_stream: "NORM_RECTS:face_rects_from_landmarks"
			 input_stream: "DETECTIONS:face_detections"
			 output_stream: "IMAGE:output_video"
		       }

		       )";
	default:
		return R"()";
	}
}
