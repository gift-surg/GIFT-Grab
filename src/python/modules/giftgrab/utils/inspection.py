from subprocess import check_output


def frame_rate(file_path):
    """Use ``ffprobe`` to inspect frame rate of video file

    @param file_path
    @return
    @throw OSError if `file_path` does not exist, or if
    ``ffprobe`` cannot be run (e.g. not installed)
    @throw ValueError if frame rate cannot be inferred from
    ``ffprobe`` output
    """
    ffprobe_output = check_output(['ffprobe',
                                   '-v', 'error',
                                   '-of', 'flat=s=_',
                                   '-show_entries', 'stream=r_frame_rate',
                                   file_path])
    frame_rate_str = ffprobe_output.split('=')[1].strip()[1:-1].split('/')
    fr = -1
    if len(frame_rate_str) == 1:
        fr = float(frame_rate_str[0])
    elif len(frame_rate_str) == 2:
        fr = float(frame_rate_str[0])/float(frame_rate_str[1])
    else:
        raise ValueError('Frame rate cannot be inferred from ffprobe output')

    return fr


def num_frames(file_path):
    """Use ``ffprobe`` to inspect no. of frames in video file

    @param file_path
    @return
    @throw OSError if `file_path` does not exist, or if
    ``ffprobe`` cannot be run (e.g. not installed)
    @throw ValueError if frame count cannot be inferred from
    ``ffprobe`` output
    """
    ffprobe_output = check_output(['ffprobe',
                                   '-v', 'error',
                                   '-count_frames',
                                   '-of', 'default=nokey=1:noprint_wrappers=1',
                                   '-show_entries', 'stream=nb_read_frames',
                                   file_path])
    return int(ffprobe_output)


def duration(file_path):
    """Use ``ffprobe`` to inspect duration of video file

    @param file_path
    @return
    @throw OSError if `file_path` does not exist, or if
    ``ffprobe`` cannot be run (e.g. not installed)
    @throw ValueError if duration cannot be inferred from
    ``ffprobe`` output
    """
    ffprobe_output = check_output(['ffprobe',
                                   '-v', 'error',
                                   '-of', 'flat=s=_',
                                   '-show_entries', 'format=duration',
                                   file_path])
    duration_str = ffprobe_output.split('=')[1].strip()[1:-1]
    dur = -1
    if duration_str is not None:
        dur = float(duration_str)
    else:
        raise ValueError('Duration cannot be inferred from ffprobe output')

    return dur


def resolution(file_path):
    """Use ``ffprobe`` to inspect resolution of video file

    @param file_path
    @return width, height
    @throw OSError if `file_path` does not exist, or if
    ``ffprobe`` cannot be run (e.g. not installed)
    @throw ValueError if resolution cannot be inferred from
    ``ffprobe`` output
    """
    width = -1
    height = -1
    ffprobe_commands = [ ['ffprobe',
                          '-v', 'error',
                          '-of', 'flat=s=_',
                          '-show_entries', 'stream=width',
                          file_path],
                         ['ffprobe',
                          '-v', 'error',
                          '-of', 'flat=s=_',
                          '-show_entries', 'stream=height',
                          file_path] ]
    res = []
    for ffprobe_command in ffprobe_commands:
        ffprobe_output = check_output(ffprobe_command)
        resolution_str = ffprobe_output.split('=')[1].strip()
        if resolution_str is not None:
            res.append(float(resolution_str))
        else:
            raise ValueError('Resolution cannot be inferred from ffprobe output')
    width = res[0]
    height = res[1]

    return width, height


def codec(file_path):
    """Use ``ffprobe`` to inspect codec of video file

    @param file_path
    @return width, height
    @throw OSError if `file_path` does not exist, or if
    ``ffprobe`` cannot be run (e.g. not installed)
    @throw ValueError if codec cannot be inferred from
    ``ffprobe`` output
    """
    ffprobe_output = check_output(['ffprobe',
                                   '-v', 'error',
                                   '-of', 'flat=s=_',
                                   '-show_entries', 'stream=codec_name',
                                   file_path])
    codec_str = ffprobe_output.split('=')[1].strip().strip('"')
    if codec_str is not None:
        return codec_str
    else:
        raise ValueError('Codec cannot be inferred from ffprobe output')
