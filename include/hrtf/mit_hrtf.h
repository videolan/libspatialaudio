#ifndef SPATIALAUDIO_HRTF_MIT_HRTF_H
#define SPATIALAUDIO_HRTF_MIT_HRTF_H

#include "hrtf.h"

#if SPATIALAUDIO_SUPPORTS_MIT_HRTF

namespace spaudio {

    class SPAUDIO_API MIT_HRTF : public HRTF
    {
    public:
        MIT_HRTF(unsigned i_sampleRate);
        bool get(float f_azimuth, float f_elevation, float** pfHRTF);
    };

} // namespace spaudio

#endif

#endif // SPATIALAUDIO_HRTF_MIT_HRTF_H
