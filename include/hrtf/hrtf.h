#ifndef SPATIALAUDIO_HRTF_HRTF_H
#define SPATIALAUDIO_HRTF_HRTF_H

#include "../SpatialaudioAPI.h"

namespace spaudio {

    class SPAUDIO_API HRTF
    {
    public:
        HRTF(unsigned i_sampleRate)
            : i_sampleRate(i_sampleRate), i_len(0)
        {
        }
        virtual ~HRTF() = default;

        virtual bool get(float f_azimuth, float f_elevation, float** pfHRTF) = 0;

        bool isLoaded() { return i_len != 0; }
        unsigned getHRTFLen() { return i_len; }

    protected:
        unsigned i_sampleRate;
        unsigned i_len;
    };

} // namespace spaudio

#endif // SPATIALAUDIO_HRTF_HRTF_H
