/*############################################################################*/
/*#                                                                          #*/
/*#  Ambisonic C++ Library                                                   #*/
/*#  Copyright © 2007 Aristotel Digenis                                      #*/
/*#  Copyright © 2017 Videolabs                                              #*/
/*#                                                                          #*/
/*#  Filename:      AmbisonicCommons.h                                       #*/
/*#  Version:       0.2                                                      #*/
/*#  Date:          19/05/2007                                               #*/
/*#  Author(s):     Aristotel Digenis, Peter Stitt                           #*/
/*#  Licence:       LGPL                                                     #*/
/*#                                                                          #*/
/*############################################################################*/


#ifndef SPATIALAUDIO_AMBISONIC_COMMONS_H
#define SPATIALAUDIO_AMBISONIC_COMMONS_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <memory.h>

#include "SpatialaudioAPI.h"
#include "Coordinates.h"

namespace spaudio {

#define DEFAULT_ORDER    1
#define DEFAULT_HEIGHT    true
#define DEFAULT_BFORMAT_SAMPLECOUNT    512
#define DEFAULT_SAMPLERATE 44100
#define DEFAULT_BLOCKSIZE 512
#define DEFAULT_HRTFSET_DIFFUSED false


    //TODO
    enum BFormatChannels3D
    {
        kW,
        kY, kZ, kX,
        kV, kT, kR, kS, kU,
        kQ, kO, kM, kK, kL, kN, kP,
        kNumOfBformatChannels3D
    };

    /*enum BFormatChannels2D
    {
        kW,
        kX, kY,
        kU, kV,
        kP, kQ,
        kNumOfBformatChannels2D
    };*/

    /** Convert degrees to radians.
     * @param fDegrees  Input angle in degrees.
     * @return          Output angle in radians.
     */
    SPAUDIO_API float DegreesToRadians(float fDegrees);

    /** Convert radians to degrees.
     * @param fRadians  Input angle in radians.
     * @return          Output angle in degrees.
     */
    SPAUDIO_API float RadiansToDegrees(float fRadians);

    /** Get the number of BFormat components for a given BFormat configuration.
     * @param nOrder    Ambisonic order.
     * @param b3D       True if the signal is 3D.
     * @return          The number of ambisonic components
     */
    SPAUDIO_API unsigned OrderToComponents(unsigned nOrder, bool b3D);

    /** Returns the index component of a BFormat stream where components of a given
     *  configuration start. For example, in a BFormat stream, the components of a
     *  2nd order 3D configuration, would start at index 4.
     * @param nOrder    Ambisonic order.
     * @param b3D       True if the signal is 3D.
     * @return          Index of the first component of a particular order.
     */
    SPAUDIO_API unsigned OrderToComponentPosition(unsigned nOrder, bool b3D);

    /** Get the recommended minimum speakers needed to decode a BFormat stream of
     *  a given configuration.
     * @param nOrder    Ambisonic order.
     * @param b3D       True if the signal is 3D.
     * @return          Recommended minimum number of speakers.
     */
    SPAUDIO_API unsigned OrderToSpeakers(unsigned nOrder, bool b3D);

    /** Get the label for a given index component in a BFormat stream.
     * @param nComponent    Index of the component
     * @param b3D           True if the signal is 3D.
     * @return              The label of the specified component.
     */
    SPAUDIO_API char ComponentToChannelLabel(unsigned nComponent, bool b3D);

    /** Get the order that the channel corresponds to. E.g. Channel 0 belongs to order 0, 1-to-3 belong to order 1, 4-to-8 belong to order 2
     * @param nComponent    Index of the component
     * @param b3D           True if the signal is 3D.
     * @return              The order of the specified component.
     */
    SPAUDIO_API unsigned ComponentPositionToOrder(unsigned nComponent, bool b3D);

    /** Get the AmbiX channel index for a spherical harmonic of specified order and degree.
     * @param order     Order of the signal such that order <= maxOrder.
     * @param degree    Degree of the signal such that -order <= degree <= order.
     * @param b3D       True if the component index is for a 3D signal.
     * @return          AmbiX component index.
     */
    SPAUDIO_API unsigned OrderAndDegreeToComponent(int order, int degree, bool b3D);

    /** Get the spherical harmonic order and degree for a specified AmbiX channel index.
     * @param order     Order of the signal such that order <= maxOrder.
     * @param degree    Degree of the signal such that -order <= degree <= order.
     * @param b3D       True if the component index is for a 3D signal.
     * @return          AmbiX component index.
     */
    SPAUDIO_API void ComponentToOrderAndDegree(int nComponent, bool b3D, int& order, int& degree);

    /** Returns the gain to convert an N3D signal to an SN3D one.
     * @param order     Order of the signal such that order <= maxOrder.
     * @return          Conversion gain 1/sqrt(2*order + 1)
     */
    template<typename T> SPAUDIO_API
    T N3dToSn3dFactor(int order);

    /** Returns the gain to convert an SN3D signal to an N3D one.
     * @param order     Order of the signal such that order <= maxOrder.
     * @return          Conversion gain sqrt(2*order + 1)
     */
    template<typename T> SPAUDIO_API
    T Sn3dToN3dFactor(int order);

    /** Returs the gain to convert a FuMa normalised signal to an SN3D one.
     * @param order     Order of the signal such that order <= 3.
     * @param degree    Degree of the signal such that -order <= degree <= order.
     * @return          Conversion gain
     */
    template<typename T> SPAUDIO_API
    T FuMaToSn3dFactor(int order, int degree);

} // namespace spaudio

#endif // SPATIALAUDIO_AMBISONIC_COMMONS_H
