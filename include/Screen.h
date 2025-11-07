/*############################################################################*/
/*#                                                                          #*/
/*#  Screen scaling and screen edge lock handling                            #*/
/*#								                                             #*/
/*#                                                                          #*/
/*#  Filename:      Screen.h												 #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          30/10/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#pragma once

#include "SpatialaudioAPI.h"
#include "adm/Conversions.h"
#include "ScreenCommon.h"
#include "LoudspeakerLayouts.h"
#include "Coordinates.h"
#include "Tools.h"

namespace spaudio {

    /** The the position of the source from a position relative to the reference screen to a position
     *  relative to the reproduction screen.
    */
    class SPAUDIO_API ScreenScaleHandler {
    public:
        ScreenScaleHandler(const Optional<Screen>& reproductionScreen, const Layout& layout);
        ~ScreenScaleHandler();

        /** Scales a position depending on the reproduction screen and the reference screen. See Rec. ITU-R BS.2127-0 sec. 7.3.3 pg 40 for more details
         * @param position			Cartesian position to be scaled.
         * @param screenRef			If true then applies the scaling, otherwise return the original position unmodified.
         * @param referenceScreen	Reference screen to use for the scaling.
         * @param cartesian			Flag if the Cartesian rendering path is to be used.
         * @return					Returns the modified position.
         */
        CartesianPosition<double> handle(CartesianPosition<double> position, bool screenRef, const Screen& referenceScreen, bool cartesian);

    private:
        Layout m_layout;
        // The reproduction screen
        Optional<Screen> m_repScreen;
        // The internal representation of the screens
        PolarEdges m_repPolarEdges, m_refPolarEdges;

        /** Scale the position for the polar/egocentric path.
         * @param position	The position to scale.
         * @return			The scaled postion.
         */
        CartesianPosition<double> ScalePosition(CartesianPosition<double> position);

        /** Scale the azimuth and elevation based on the polar edges of the screen.
         * @param az	The azimuth to scale in degrees.
         * @param el	The elevation to scale in degrees.
         * @return		Pair containing scaled az and el.
         */
        std::pair<double, double> ScaleAzEl(double az, double el);
    };

    /** Apply screen edge locking to supplied position based on the reproduction screen and (if cartesian == true) the layout. */
    class SPAUDIO_API ScreenEdgeLockHandler
    {
    public:
        ScreenEdgeLockHandler(const Optional<Screen>& reproductionScreen, const Layout& layout);
        ~ScreenEdgeLockHandler();

        /** Apply screen edge locking to a position. See Rec. ITU-R BS.2127-1 sec. 7.3.4 pg. 43.
         * @param position			The position to process.
         * @param screenEdgeLock	Screen edge lock metadata.
         * @param cartesian			If true then uses cartesian/allocentric processing. Otherwise uses polar/egocentric method.
         * @return					The modified position.
         */
        CartesianPosition<double> HandleVector(CartesianPosition<double> position, ScreenEdgeLock screenEdgeLock, bool cartesian = false);

        /** Apply screen edge locking to an azimuth and elevation.
         * @param azimuth			The azimuth in degrees to process.
         * @param elevation			The elevation in degrees to process.
         * @param screenEdgeLock	ScreenEdgeLock metadata.
         * @return					The modified directions in degrees.
         */
        std::pair<double, double> HandleAzEl(double azimuth, double elevation, ScreenEdgeLock screenEdgeLock);

    private:
        Layout m_layout;
        Optional<Screen> m_reproductionScreen;
        PolarEdges m_repPolarEdges;
    };

} // namespace spaudio
