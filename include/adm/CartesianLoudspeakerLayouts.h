/*############################################################################*/
/*#                                                                          #*/
/*#  Positions of speakers in layouts using ADM Cartesian coordinates        #*/
/*#								                                             #*/
/*#  Copyright © 2025 Peter Stitt                                            #*/
/*#                                                                          #*/
/*#  Filename:      CartesianLoudspeakerLayouts.h                            #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          10/09/2025                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#ifndef SPATIALAUDIO_ADM_CARTESIAN_LOUDSPEAKER_LAYOUTS_H
#define SPATIALAUDIO_ADM_CARTESIAN_LOUDSPEAKER_LAYOUTS_H

namespace spaudio {
    namespace adm {

        /**
        Cartesian speaker coordinates defined in Rec. ITU-R BS.2127-1 Sec. 11.2
        */
        const std::map<std::string, std::map<std::string, CartesianPosition<double>>> alloPositions = {
            // Stereo - BS.2051-3 System A 0+2+0
            {"0+2+0",{ {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}} }},
            // 5.1 - BS.2051-3 System B 0+5+0
            {"0+5+0",{ {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"M+000",{0.,1.,0.}},
            {"M+110",{-1.,-1.,0.}},
            {"M-110",{1.,-1.,0.}},
            {"LFE1",{-1.,1.,-1.}}
            }},
            // 5.1.2 - BS.2051-3 System C 2+5+0
            {"2+5+0",{ {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"M+000",{0.,1.,0.}},
            {"M+110",{-1.,-1.,0.}},
            {"M-110",{1.,-1.,0.}},
            {"U+030",{-1.,1.,1.}},
            {"U-030",{1.,1.,1.}},
            {"LFE1",{-1.,1.,-1.}}
            }},
            // 5.1.4 - BS.2051-3 System D 4+5+0
            {"4+5+0",{ {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"M+000",{0.,1.,0.}},
            {"M+110",{-1.,-1.,0.}},
            {"M-110",{1.,-1.,0.}},
            {"U+030",{-1.,1.,1.}},
            {"U-030",{1.,1.,1.}},
            {"U+110",{-1.,-1.,1.}},
            {"U-110",{1.,-1.,1.}},
            {"LFE1",{-1.,1.,-1.}}
            }},
            // BS.2051-3 System E 4+5+1
            {"4+5+1",{ {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"M+000",{0.,1.,0.}},
            {"M+110",{-1.,-1.,0.}},
            {"M-110",{1.,-1.,0.}},
            {"U+030",{-1.,1.,1.}},
            {"U-030",{1.,1.,1.}},
            {"U+110",{-1.,-1.,1.}},
            {"U-110",{1.,-1.,1.}},
            {"B+000",{0.,1.,-1.}},
            {"LFE1",{-1.,1.,-1.}}
            }},
            // BS.2051-3 System F 3+7+0
            {"3+7+0",{ {"M+000",{0.,1.,0.}},
            {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"U+045",{-1.,1.,1.}},
            {"U-045",{1.,1.,1.}},
            {"M+090",{-1.,0.,0.}},
            {"M-090",{1.,0.,0.}},
            {"M+135",{-1.,-1.,0.}},
            {"M-135",{1.,-1.,0.}},
            {"UH+180",{0.,-1.,1.}},
            {"LFE1",{-1.,1.,-1.}},
            {"LFE2",{1.,1.,-1.}}
            }},
            // BS.2051-3 System G 4+9+0 - Note: doesn't include the screen speakers because these are defined as in Rec. ITU-R BS.2127-1 Sec. 7.3.9
            {"4+9+0",{ {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"M+000",{0.,1.,0.}},
            {"M+090",{-1.,0.,0.}},
            {"M-090",{1.,0.,0.}},
            {"M+135",{-1.,-1.,0.}},
            {"M-135",{1.,-1.,0.}},
            {"U+045",{-1.,1.,1.}},
            {"U-045",{1.,1.,1.}},
            {"U+135",{-1.,-1.,1.}},
            {"U-135",{1.,-1.,1.}},
            {"LFE1",{-1.,1.,-1.}},
            {"LFE2",{1.,1.,-1.}}
            }},
            // BS.2051-3 System H 9+10+3
            {"9+10+3",{ {"M+060",{-1.,0.414214,0.}},
            {"M-060",{1.,0.414214,0.}},
            {"M+000",{0.,1.,0.}},
            {"M+135",{-1.,-1.,0.}},
            {"M-135",{1.,-1.,0.}},
            {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"M+180",{0.,-1.,0.}},
            {"M+090",{-1.,0.,0.}},
            {"M-090",{1.,0.,0.}},
            {"U+045",{-1.,1.,1.}},
            {"U-045",{1.,1.,1.}},
            {"U+000",{0.,1.,1.}},
            {"T+000",{0.,0.,1.}},
            {"U+135",{-1.,-1.,1.}},
            {"U-135",{1.,-1.,1.}},
            {"U+090",{-1.,0.,1.}},
            {"U-090",{1.,0.,1.}},
            {"U+180",{0.,-1.,1.}},
            {"B+000",{0.,1.,-1.}},
            {"B+045",{-1.,1.,-1.}},
            {"B-045",{1.,1.,-1.}},
            {"LFE1",{-1.,1.,-1.}},
            {"LFE2",{1.,1.,-1.}}
            }},
            // 7.1 - BS.2051-3 System I 0+7+0
            { "0+7+0",{ {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"M+000",{0.,1.,0.}},
            {"M+090",{-1.,0.,0.}},
            {"M-090",{1.,0.,0.}},
            {"M+135",{-1.,-1.,0.}},
            {"M-135",{1.,-1.,0.}},
            {"LFE1",{-1.,1.,-1.}}
            }},
            // 7.1.4 - BS.2051-3 System J 4+7+0
            { "4+7+0",{ {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"M+000",{0.,1.,0.}},
            {"M+090",{-1.,0.,0.}},
            {"M-090",{1.,0.,0.}},
            {"M+135",{-1.,-1.,0.}},
            {"M-135",{1.,-1.,0.}},
            {"U+045",{-1.,1.,1.}},
            {"U-045",{1.,1.,1.}},
            {"U+135",{-1.,-1.,1.}},
            {"U-135",{1.,-1.,1.}},
            {"LFE1",{-1.,1.,-1.}}
            }},
            // BEAR 9+10+5
            {"9+10+5", { {"M+060",{-1.,0.414214,0.}},
            {"M-060",{1.,0.414214,0.}},
            {"M+000",{0.,1.,0.}},
            {"M+135",{-1.,-1.,0.}},
            {"M-135",{1.,-1.,0.}},
            {"M+030",{-1.,1.,0.}},
            {"M-030",{1.,1.,0.}},
            {"M+180",{0.,-1.,0.}},
            {"M+090",{-1.,0.,0.}},
            {"M-090",{1.,0.,0.}},
            {"U+045",{-1.,1.,1.}},
            {"U-045",{1.,1.,1.}},
            {"U+000",{0.,1.,1.}},
            {"T+000",{0.,0.,1.}},
            {"U+135",{-1.,-1.,1.}},
            {"U-135",{1.,-1.,1.}},
            {"U+090",{-1.,0.,1.}},
            {"U-090",{1.,0.,1.}},
            {"U+180",{0.,-1.,1.}},
            {"B+000",{0.,1.,-1.}},
            {"B+045",{-1.,1.,-1.}},
            {"B-045",{1.,1.,-1.}},
            {"B+135",{-1.,-1.,-1.}},
            {"B-135",{1.,-1.,-1.}},
            {"LFE1",{-1.,1.,-1.}},
            {"LFE2",{1.,1.,-1.}} }}
        };

        /** Returns the cartesian/allocentric positions of specified layout as specified in Rec. ITU-R BS.2127-1 Sec. 7.3.9
         * @param layout Layout of the speaker array
         * @return Positions of the speakers in cartesian/allocentric format. If the layout is not supported by (i.e. not defined
         * in the tables in section 11.2 then an empty vector is returned.
         */
        static inline std::vector<CartesianPosition<double>> positionsForLayout(const Layout& layout)
        {
            std::vector<CartesianPosition<double>> layoutPositions;

            auto it = alloPositions.find(layout.getLayoutName());
            if (it != alloPositions.end())
            {
                const auto& positions = it->second;

                for (auto& channel : layout.getChannels())
                    if (channel.getChannelName() == "M+SC" || channel.getChannelName() == "M-SC")
                        layoutPositions.push_back(PointPolarToCart(channel.getPolarPosition()));
                    else
                        layoutPositions.push_back(positions.at(channel.getChannelName()));
            }
            return layoutPositions;
        }

    }//namespace adm
} // namespace spaudio

#endif // SPATIALAUDIO_ADM_CARTESIAN_LOUDSPEAKER_LAYOUTS_H
