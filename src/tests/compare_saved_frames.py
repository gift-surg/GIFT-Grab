#!/usr/bin/python

import sys
import cv2
import inspect

def synopsis():
	print inspect.getfile(inspect.currentframe()) + '   <video-1>   <video-2> '

if len(sys.argv) < 3:
	synopsis()
else:
	filename_1 = str(sys.argv[1])
	filename_2 = str(sys.argv[2])
	print 'Comparing ' + filename_1 + ' vs ' + filename_2

	video_1 = cv2.VideoCapture(filename_1)
	if not video_1.isOpened():
		print 'File ' + filename_1 + ' could not be opened'
		exit(-1)

	video_2 = cv2.VideoCapture(filename_2)
	if not video_2.isOpened():
		print 'File ' + filename_2 + ' could not be opened'
		exit(-1)

	CV_CAP_PROP_FRAME_COUNT = 7
	num_frames = video_1.get(CV_CAP_PROP_FRAME_COUNT)
	if num_frames != video_2.get(CV_CAP_PROP_FRAME_COUNT):
		print 'Mismatch between the number of frames'
		exit(-1)
	num_frames = int(num_frames)

	CV_CAP_PROP_FRAME_WIDTH = 3
	CV_CAP_PROP_FRAME_HEIGHT = 4
	width = video_1.get(CV_CAP_PROP_FRAME_WIDTH)
	height = video_1.get(CV_CAP_PROP_FRAME_HEIGHT)
	if width != video_2.get(CV_CAP_PROP_FRAME_WIDTH) or \
	   height != video_2.get(CV_CAP_PROP_FRAME_HEIGHT):
	   print 'Video frame dimension mismatch'
	   exit(-1)
	width = int(width)
	height = int(height)

	print str(num_frames) + '   ' + str(width) + ' x ' + str(height) + ' frames'

# cap = cv2.VideoCapture()

# while(True):
#     # Capture frame-by-frame
#     ret, frame = cap.read()

#     # Our operations on the frame come here
#     gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

#     # Display the resulting frame
#     cv2.imshow('frame',gray)
#     if cv2.waitKey(1) & 0xFF == ord('q'):
#         break

# # When everything done, release the capture
# cap.release()
# cv2.destroyAllWindows()
