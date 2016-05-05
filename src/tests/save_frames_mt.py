#!/usr/bin/python

# imports
import pygiftgrab
import time
from datetime import timedelta
import argparse
import sys
from epiphan_thread import EpiphanThread


def error(message):
    sys.stderr.write("error: %s\n" % message)
    sys.exit(1)

# default parameters
frame_rate = 60
recording_duration = 1  # min
storage_type = pygiftgrab.Storage.File_H265
lap = 30
sub_frame = False

# argument parser
parser = argparse.ArgumentParser()
parser.add_argument("--storage-type", type=str, help="xvid OR h265")
parser.add_argument("--sub-frame", action="store_true",
                    help="whether to save only the ROI")
parser.add_argument("--filepath-prefix", type=str,
                    help="prefix to filepath where videostream is saved")
parser.add_argument("--duration", type=float, help="how long to record (sec.)")
args = parser.parse_args()
if args.storage_type:
    if args.storage_type == "xvid":
        storage_type = pygiftgrab.Storage.File_XviD
    elif args.storage_type == "h265":
        storage_type = pygiftgrab.Storage.File_H265
    else:
        error(args.storage_type + ' not recognised')
sub_frame = args.sub_frame
if args.duration:
    recording_duration = args.duration / 60
file_path_prefix = ''
if args.filepath_prefix:
    file_path_prefix = args.filepath_prefix + '-'

# report header
print str(sys.argv) + ' ( ' + str(args) + ' )'
print 'Recording video stream from Epiphan SDI + DVI ports to ' + str(storage_type)

try:
    # actual work
    start = time.time()
    sdi_thread = EpiphanThread(device_type=pygiftgrab.Device.DVI2PCIeDuo_SDI,
                               sub_frame=sub_frame, frame_rate=frame_rate,
                               recording_duration=recording_duration,
                               storage_type=storage_type,
                               file_path_prefix=file_path_prefix)
    dvi_thread = EpiphanThread(device_type=pygiftgrab.Device.DVI2PCIeDuo_DVI,
                               sub_frame=sub_frame, frame_rate=frame_rate,
                               recording_duration=recording_duration,
                               storage_type=storage_type,
                               file_path_prefix=file_path_prefix)
    sdi_thread.start()
    dvi_thread.start()
    sdi_thread.join()
    dvi_thread.join()
    elapsed = (time.time() - start)

    # report results
    num_frames = int(recording_duration * 60 * frame_rate)
    total_num_frames = 2 * num_frames
    real_frame_rate = num_frames / elapsed
    print 'TOTAL Epiphan frames, total time, total expected time, ' + \
          'real framerate, expected framerate, SDI w x h, DVI w x h, ' + \
          str(total_num_frames) + ', ' + \
          str(timedelta(seconds=elapsed)) + ', ' + \
          str(timedelta(minutes=recording_duration)) + ', ' + \
          str(real_frame_rate) + ', ' + \
          str(frame_rate) + ', ' + \
          ('(sub-frame)' if sub_frame else '')
    sys.stdout.flush()

except (RuntimeError, IOError) as e:
    print e.message
