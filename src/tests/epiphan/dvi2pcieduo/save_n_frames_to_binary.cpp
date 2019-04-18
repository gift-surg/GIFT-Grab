#include <cstdlib>
#include <thread>
#include <chrono>
#include <string>

#include "iobservable.h"
#include "iobserver.h"
#include "videosourcefactory.h"
#include "device.h"
#include "io_utils.h"


using namespace gg;
using namespace std;


class BgraFrameSaver : public IObserver
{
private:
    size_t _max_num_to_save;
    size_t _num_saved;

public:
    BgraFrameSaver(size_t max_num_frames);

    ~BgraFrameSaver();

public:
    void update(VideoFrame &frame);
};

int main()
{
    VideoSourceFactory &sfac = VideoSourceFactory::get_instance();
    IVideoSource *epiphan = sfac.get_device(DVI2PCIeDuo_DVI, BGRA);
    BgraFrameSaver saver(3);

    epiphan->attach(saver);

    this_thread::sleep_for(std::chrono::seconds(10));  // operate pipeline for this duration

    epiphan->detach(saver);

    return EXIT_SUCCESS;
}

BgraFrameSaver::BgraFrameSaver(size_t max_num_frames)
    : _max_num_to_save(max_num_frames)
    , _num_saved(0)
{
    // nop
}

BgraFrameSaver::~BgraFrameSaver()
{
    // nop
}

void BgraFrameSaver::update(VideoFrame &frame)
{
    if (_num_saved >= _max_num_to_save)
        return;

    string filename = "snapshot-";
    filename.append(to_string(_num_saved));
    filename.append(".bin");

    save_binary(frame, filename);

    _num_saved++;
}
