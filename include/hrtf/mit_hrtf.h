#ifndef MIT_HRTF_H
#define MIT_HRTF_H

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

#endif // MIT_HRTF_H
