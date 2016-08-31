#include "vlc_video_source.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>

namespace gg
{

//-----------------------------------------------------------------------------
VideoSourceVLC::VideoSourceVLC( const std::string path )
    : _vlc_inst(nullptr)
    , _vlc_mp(nullptr)
    , m_size( 0 )
    , m_pixWidth( 0 )
    , m_pixHeight( 0 )
{
    this->mInitSource( path.c_str() );
    this->mRunSource();
    //std::cout<<"sleeping"<<std::endl;
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    //std::cout<<"done sleeping"<<std::endl;
}


//-----------------------------------------------------------------------------
VideoSourceVLC::~VideoSourceVLC()
{
    /* Stop playing */
    libvlc_media_player_stop( this->_vlc_mp );

    /* Free the media_player */
    if( _vlc_mp ) {
        libvlc_media_player_release( this->_vlc_mp );
    }

    // free engine
    if ( _vlc_inst )
        libvlc_release( this->_vlc_inst );
}


//-----------------------------------------------------------------------------
bool VideoSourceVLC::get_frame_dimensions( int & width, int & height )
{
    ///\todo mutex
    if( this->m_pixWidth==0 || this->m_pixHeight==0 ) {
        //std::cerr<<"Size not set yet"<<std::endl;
        return false;
    }
    width = this->m_pixWidth;
    height = this->m_pixHeight;
    return true;
}

//-----------------------------------------------------------------------------
bool VideoSourceVLC::get_frame( VideoFrame_BGRA & frame )
{
    ///\todo mutex

    if( this->m_pixWidth==0 || this->m_pixHeight==0 || !this->m_videoBuffer ) {
        //std::cerr<<"Size not set yet in VideoSourceVLC::get_next_frame"<<std::endl;
        return false;
    }

    //std::cout<<"m_pixWidth: "<<m_pixWidth<<std::endl;
    //std::cout<<"m_pixHeight: "<<m_pixHeight<<std::endl;
    //std::cout<<"m_size: "<<m_size<<std::endl;

    //Allocate and fill the image
    if( this->m_pixWidth*this->m_pixHeight*4 == this->m_size ) {
        frame = VideoFrame_BGRA(this->m_videoBuffer, this->m_pixWidth, this->m_pixHeight);
    }
    else
    {
        throw std::runtime_error("get_next_frame not implemented for padded images");
    }

    //std::cout<<"VideoSourceVLC::get_next_frame"<<std::endl;
    //throw std::runtime_error("get_next_frame to be implemented");
    return true;
}

bool get_frame(VideoFrame_I420 & frame)
{
    // TODO
    throw gg::VideoSourceError("get_frame(VideoFrame_I420 & frame) not implemented for VLC");
}

//-----------------------------------------------------------------------------
double VideoSourceVLC::get_frame_rate()
{
    //return libvlc_media_player_get_rate( m_mp );
    return libvlc_media_player_get_fps( this->_vlc_mp );
    //throw std::runtime_error("get_frame_rate to be implemented");
    //return 0.0;
}

//-----------------------------------------------------------------------------
void VideoSourceVLC::set_sub_frame( int x, int y, int width, int height )
{
    throw std::runtime_error("set_sub_frame not implemented");
}

//-----------------------------------------------------------------------------
void VideoSourceVLC::mInitSource( const char * path )
{
    try {
        // VLC pointers
        libvlc_media_t    *media;

        // VLC options
        char smem_options[512];

        sprintf( smem_options, "#transcode{vcodec=BGRA}:smem{video-data=%lld,video-prerender-callback=%lld,video-postrender-callback=%lld}",
                 // We are using transcode because smem only support raw audio and video formats
                 (long long int)(intptr_t)(void*) this,                               // we pass the pointer of the current object to the static callbacks
                 (long long int)(intptr_t)(void*) &VideoSourceVLC::prepareRender,  // pointer to the 1st callback called by VLC (static)
                 (long long int)(intptr_t)(void*) &VideoSourceVLC::handleStream ); // pointer to the 2nd callback called by VLC (static)

        const char * const vlc_args[] = {
            "-I", "dummy",        // Don't use any interface
            "--ignore-config",    // Don't use VLC's config
            "--extraintf=logger", // Log anything
            //"--verbose=2", // Be much more verbose then normal for debugging purpose
            //"--clock-jitter=0",
            //"--file-caching=150",
            "--no-audio",
            "--sout", smem_options    // Stream to memory
        };

        // We launch VLC
        _vlc_inst = libvlc_new( sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args );

        // If path contains a colon (:), it will be treated as a
        // URL. Else, it will be considered as a local path.
        if( std::string(path).find(":") == std::string::npos ) {
            //std::cout<<"opening file "<<path<<std::endl;
            media = libvlc_media_new_path( _vlc_inst, path );
        }
        else {
            //std::cout<<"opening url "<<path<<std::endl;
            media = libvlc_media_new_location( _vlc_inst, path );
        }

        libvlc_media_add_option( media, ":noaudio" );
        libvlc_media_add_option( media, ":no-video-title-show" );

        // Create a media player playing environement
        _vlc_mp = libvlc_media_player_new_from_media( media );
        // No need to keep the media now
        libvlc_media_release( media );

        //std::cout<<"Media open: "<<path<<std::endl;
    }
    catch( ... ) {
        throw std::runtime_error( std::string("Init device failed for the path ").append( path ) );
    }
}


//-----------------------------------------------------------------------------
void VideoSourceVLC::mRunSource()
{
    try {
        // play the media_player
        libvlc_media_player_play( _vlc_mp );
        //std::cout<<"Media playing"<<std::endl;
        return;
    }
    catch( ... )
    {
        throw std::runtime_error("Error while running source");
        //std::cerr<<"Error while running source"<<std::endl;
        //return;
    }
}

//-----------------------------------------------------------------------------
void VideoSourceVLC::prepareRender( VideoSourceVLC* p_video_data, uint8_t** pp_pixel_buffer, int size )
{
    ///\todo create mutex guard
    //std::cout<<"VideoSourceVLC::prepareRender"<<std::endl;
    // called before video rendered
    // p_video_data is the echo of the "this" pointer
    if( size != p_video_data->m_size ) {
        p_video_data->m_videoBuffer = new uint8_t[size];
        p_video_data->m_size = size;
        unsigned int width,height;
        libvlc_video_get_size( p_video_data->_vlc_mp, 0, &width, &height );
        p_video_data->m_pixWidth = width;
        p_video_data->m_pixHeight = height;
    }

    *pp_pixel_buffer = p_video_data->m_videoBuffer;
}

//-----------------------------------------------------------------------------
void VideoSourceVLC::handleStream( VideoSourceVLC* /*p_video_data*/, uint8_t* /*p_pixel_buffer*/, int /*width*/, int /*height*/,
                                   int /*pixel_pitch*/, int /*size*/, int64_t /*pts*/ )
{
    ///\todo create mutex guard
    //std::cout<<"VideoSourceVLC::handleStream"<<std::endl;
    //std::cout<<"pixel_pitch"<<pixel_pitch<<std::endl;
    //std::cout<<"size"<<size<<std::endl;
    //std::cout<<"pts"<<pts<<std::endl;
    // called after video rendered
    // p_video_data is the echo of the "this" pointer
    //p_video_data->mReadRGBFrame( p_video_data->m_videoBuffer );
    //p_video_data->m_pixWidth = width;
    //p_video_data->m_pixHeight = height;
}

}
