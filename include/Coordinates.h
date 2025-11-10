/*############################################################################*/
/*#                                                                          #*/
/*#  Cartesian and polar coordinates                                         #*/
/*#								                                             #*/
/*#                                                                          #*/
/*#  Filename:      Coordinates.h                                            #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          23/06/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#ifndef SPATIALAUDIO_COORDINATES_H
#define SPATIALAUDIO_COORDINATES_H

namespace spaudio {

    /** Structure holding a spherical-polar position. */
    template<typename T>
    struct PolarPosition
    {
        T azimuth = 0.0;
        T elevation = 0.0;
        T distance = 1.0;
    };
    template<typename T>
    inline bool operator==(const PolarPosition<T>& lhs, const PolarPosition<T>& rhs)
    {
        return lhs.azimuth == rhs.azimuth && lhs.elevation == rhs.elevation && lhs.distance == rhs.distance;
    }

    /** Structure holding a cartesian position. */
    template<typename T>
    struct CartesianPosition
    {
        T x = 1.0;
        T y = 0.0;
        T z = 0.0;
    };
    template<typename T>
    inline bool operator==(const CartesianPosition<T>& lhs, const CartesianPosition<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }
    template<typename T>
    inline CartesianPosition<T> operator+(const CartesianPosition<T>& lhs, const CartesianPosition<T>& rhs)
    {
        return CartesianPosition<T>{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }
    template<typename T>
    inline CartesianPosition<T> operator-(const CartesianPosition<T>& lhs, const CartesianPosition<T>& rhs)
    {
        return CartesianPosition<T>{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    }

} // namespace spaudio

#endif // SPATIALAUDIO_COORDINATES_H
