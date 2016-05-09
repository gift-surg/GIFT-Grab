import threading
import pygiftgrab


class EpiphanThread(threading.Thread):
    def __init__(self, device_type, sub_frame, frame_rate,
                 recording_duration, storage_type, file_path_prefix, lap=20):
        self.num_frames = int(recording_duration * 60 * frame_rate)

        self.device_type = device_type
        self.device = pygiftgrab.Factory.connect(self.device_type)
        if sub_frame:
            self.device.set_sub_frame(660, 160, 678, 688)

        self.target = pygiftgrab.Factory.writer(storage_type)
        if storage_type == pygiftgrab.Storage.File_H265:
            extension = '.mp4'
        elif storage_type == pygiftgrab.Storage.File_XviD:
            extension = '.avi'
        file_path = file_path_prefix + \
                    str(format(recording_duration, '.2f')) + \
                    '-min-mt-py-rec-from-' + str(device_type) + \
                    extension
        print 'Video will be saved in: ' + file_path
        self.target.init(file_path, frame_rate)

        self.lap = lap

        threading.Thread.__init__(self)

    def run(self):
        try:
            frame = pygiftgrab.VideoFrame_BGRA(False)

            for i in range(1, self.num_frames + 1):
                if i % self.lap == 0:
                    print str(self.device_type) + \
                          " frame # " + str(i) + \
                          " / " + str(self.num_frames)
                self.device.get_frame(frame)
                self.target.append(frame)

            self.target.finalise()
            pygiftgrab.Factory.disconnect(self.device_type)
        except (RuntimeError, IOError) as e:
            print e.message
            return
