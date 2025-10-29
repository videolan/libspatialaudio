/*############################################################################*/
/*#                                                                          #*/
/*#  Loudspeaker layouts.                                                    #*/
/*#                                                                          #*/
/*#  Copyright © 2020 Peter Stitt                                            #*/
/*#                                                                          #*/
/*#  Filename:      LoudspeakerLayouts.h                                     #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          23/06/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/


#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "RendererMetadata.h"
#include "Coordinates.h"
#include "ScreenCommon.h"
#include "adm/Conversions.h"

namespace spaudio {

    /** The different output layouts supported by Renderer */
    enum class OutputLayout
    {
        Stereo = 0, // 2.0 - ITU-R BS.2051-3 System A (0+2+0)
        Quad, // Channel order: FrontLeft, FrontRight, BackLeftMid, BackRightMid
        FivePointOne, // 5.1 - ITU-R BS.2051-3 System B (0+5+0)
        FivePointOnePointTwo, // 5.1.2 - ITU-R BS.2051-3 System C (2+5+0)
        FivePointOnePointFour, // 5.1.4 - ITU-R BS.2051-3 System D (4+5+0)
        FivePointOnePointFourPlusLow, // ITU-R BS.2051-3 System E (4+5+1)
        SevenPointOnePointThree, // 7.1.3 - ITU-R BS.2051-3 System F (3+7+0)
        ThirteenPointOne, // 13.1 (or 7.1.4+screens) - ITU-R BS.2051-3 System G (4+9+0)
        TwentyTwoPointTwo, // 22.2 - ITU-R BS.2051-3 System H (9+10+3)
        SevenPointOne, // 7.1 - ITU-R BS.2051-3 System I (0+7+0)
        SevenPointOnePointFour, // 7.1.4 - ITU-R BS.2051-3 System J (4+7+0)
        BEAR_9_10_5, // BEAR layout. 9+10+3 with 2 extra lower speakers
        SevenPointOnePointTwo, // 7.1.2 layout specified in IAMF v1.0.0
        ThreePointOnePointTwo, // 3.1.2 layout specified in IAMF v1.0.0
        Binaural
    };

    /** The different channel types from Table 1A in Rec. ITU-R  BS.2094-2 */
    enum class ChannelTypes
    {
        Custom = -1, // A channel not defined in Rec. ITU-R  BS.2094-2
        FrontLeft = 0, // M+030
        FrontRight, // M-030
        FrontCentre, // M+000
        LFE,
        SurroundLeft, // M+110
        SurroundRight, // M-110
        FrontLeftOfCentre, // M+022
        FrontRightOfCentre, // M-022
        BackCentre, // M+180
        SideLeft, // M+090
        SideRight, // M-090
        TopCentre, // T+000
        TopFrontLeft, // U+030
        TopFrontCentre, // U+000
        TopFrontRight, // U-030
        TopSurroundLeft, // U+110
        TopBackCentre, // U+180
        TopSurroundRight, // U-110
        TopSideLeft, // U+090
        TopSideRight, // U-090
        BottomFrontCentre, // B+000
        BottonFrontLeftMid, // B+045
        BottomFrontRightMid, // B-045
        FrontLeftWide, // M+060
        FrontRightWide, // M-060
        BackLeftMid, // M+135
        BackRightMid, // M-135
        TopBackLeftMid, // U+135
        TopBackRightMid, // U-135
        LFE1,
        LFE2,
        TopFrontLeftMid, // U+045
        TopFrontRightMid, // U-045
        FrontLeftScreen, // M+SC
        FrontRightScreen, // M-SC
        FrontLeftMid, // M+045
        FrontRightMid, // M-045
        UpperTopBackCentre, // UH+180
        BackLeft, // M+150
        BackRight, // M-150
        BottomFrontLeft, // B+030
        BottomFrontRight, // B-030
        BottomBackLeft, // B+135 (Not in Rec. ITU-R  BS.2094-2. Used for BEAR layout)
        BottomBackRight, // B-135 (Not in Rec. ITU-R  BS.2094-2. Used for BEAR layout)
        ACN0, ACN1, ACN2, ACN3, ACN4, ACN5, ACN6, ACN7, ACN8,
        ACN9, ACN10, ACN11, ACN12, ACN13, ACN14, ACN15, ACN16
    };

    namespace bs2094 {
        /** List of labels for audio channels from Rec. ITU-R BS.2094-2 Table 1A. Includes */
        static const std::vector<std::string> channelLabels = {
            "M+030", "M-030", "M+000", "LFE", "M+110", "M-110",
            "M+022", "M-022", "M+180", "M+090", "M-090", "T+000",
            "U+030", "U+000", "U-030", "U+110", "U+180", "U-110",
            "U+090", "U-090", "B+000", "B+045", "B-045", "M+060", "M-060",
            "M+135", "M-135", "U+135", "U-135", "LFE1", "LFE2",
            "U+045", "U-045", "M+SC", "M-SC", "M+045", "M-045",
            "UH+180", "M+150", "M-150", "B+030", "B-030",
            "B+135", // Not in Rec. ITU-R BS.2094-1. Used in BEAR 9+10+3 plus 2 layout.
            "B-135", // Not in Rec. ITU-R BS.2094-1. Used in BEAR 9+10+3 plus 2 layout.
            "" /* empty to indicate no appropriate channel name */
        };

        /** Directions of audio channels from Rec. ITU-R BS.2094-2 Table 1A. */
        static const std::vector<PolarPosition<double>> positions = {
            PolarPosition<double>{30.,0.,1.}, // FrontLeft - M+030
            PolarPosition<double>{-30.,0.,1.}, // FrontRight - M-030
            PolarPosition<double>{0.,0.,1.}, // FrontCentre - M+000
            PolarPosition<double>{0.,-30.,1.}, // LFE - LFE
            PolarPosition<double>{110.,0.,1.}, // SurroundLeft - M+110
            PolarPosition<double>{-110.,0.,1.}, // SurroundRight - M-110
            PolarPosition<double>{22.5,0.,1.}, // FrontLeftOfCentre - M+022
            PolarPosition<double>{-22.5,0.,1.}, // FrontRightOfCentre - M-022
            PolarPosition<double>{180.,0.,1.}, // BackCentre - M+180
            PolarPosition<double>{90.,0.,1.}, // SideLeft - M+090
            PolarPosition<double>{-90.,0.,1.}, // SideRight - M-090
            PolarPosition<double>{0.,90.,1.}, // TopCentre - T+000
            PolarPosition<double>{30.,30.,1.}, // TopFrontLeft - U+030
            PolarPosition<double>{0.,30.,1.}, // TopFrontCentre - U+000
            PolarPosition<double>{-30.,30.,1.}, // TopFrontRight - U-030
            PolarPosition<double>{110.,30.,1.}, // TopSurroundLeft - U+110
            PolarPosition<double>{180.,30.,1.}, // TopBackCentre - U+180
            PolarPosition<double>{-110.,30.,1.}, // TopSurroundRight - U-110
            PolarPosition<double>{90.,30.,1.}, // TopSideLeft - U+090
            PolarPosition<double>{-90.,30.,1.}, // TopSideRight - U-090
            PolarPosition<double>{0.,-30.,1.}, // BottomFrontCentre - B+000
            PolarPosition<double>{45.,-30.,1.}, // BottonFrontLeftMid - B+045
            PolarPosition<double>{-45.,-30.,1.}, // BottomFrontRightMid - B-045
            PolarPosition<double>{60.,0.,1.}, // FrontLeftWide - M+060
            PolarPosition<double>{-60.,0.,1.}, // FrontRightWide - M-060
            PolarPosition<double>{135.,0.,1.}, // BackLeftMid - M+135
            PolarPosition<double>{-135.,0.,1.}, // BackRightMid - M-135
            PolarPosition<double>{135.,30.,1.}, // TopBackLeftMid - U+135
            PolarPosition<double>{-135.,30.,1.}, // TopBackRightMid - U-135
            PolarPosition<double>{45.,-30.,1.}, // LFE1 - LFE1
            PolarPosition<double>{-45.,-30.,1.}, // LFE2 - LFE2
            PolarPosition<double>{45.,30.,1.}, // TopFrontLeftMid - U+045
            PolarPosition<double>{-45.,30.,1.}, // TopFrontRightMid - U-045
            PolarPosition<double>{25.,0.,1.}, // FrontLeftScreen - M+SC
            PolarPosition<double>{-25.,0.,1.}, // FrontRightScreen - M-SC
            PolarPosition<double>{45.,0.,1.}, // FrontLeftMid - M+045
            PolarPosition<double>{-45.,0.,1.}, // FrontRightMid - M-045
            PolarPosition<double>{180.,45.,1.}, // UpperTopBackCentre - UH+180
            PolarPosition<double>{150.,0.,1.}, // BackLeft - M+150
            PolarPosition<double>{-150.,0.,1.}, // BackRight - M-150
            PolarPosition<double>{30.,-30.,1.}, // BottomFrontLeft - B+030
            PolarPosition<double>{-30.,-30.,1.}, // BottomFrontRight - B-030
            PolarPosition<double>{135.,-30.,1.}, // BottomBackLeft - B+135
            PolarPosition<double>{-135.,-30.,1.} // BottomBackRight - B-135
        };
    } // namespace bs2094

    /** If the the speaker label is in the format "urn:itu:bs:2051:[0-9]:speaker:X+YYY then
     *  return the X+YYY portion. Otherwise, returns the original input.
     * @param label		String containing the label.
     * @return			String of the nominal speaker label (X+YYY portion of input).
     */
    static inline const std::string& GetNominalSpeakerLabel(const std::string& label)
    {
        auto& channelLabels = bs2094::channelLabels;
        for (size_t i = 0; i < channelLabels.size(); ++i)
            if (stringContains(label, channelLabels[i]) && channelLabels[i] != "LFE")
                return channelLabels[i];

        // Rename the LFE channels, if requried.
        // See Rec. ITU-R BS.2127-0 sec 8.3
        if (stringContains(label, "LFER"))
            return channelLabels[30];
        else if (stringContains(label, "LFE") || stringContains(label, "LFEL"))
            return channelLabels[29];

        return channelLabels.back();
    }

    // Information about a speaker channel (as opposed to an audio channel, which could be Object, HOA, etc.)
    class Channel
    {
    public:
        /** Default constructor. Creates a FrontCentre channel */
        Channel();

        /** Constructor for a custom channel type */
        Channel(std::string& channelName, PolarPosition<double> position, PolarPosition<double> positionNominal, bool channelLfe);

        /** Constructor to initialise from a specific channel type */
        Channel(ChannelTypes channelType);

        /** Constructor to intialise froma specific channel type but with a custom position. */
        Channel(ChannelTypes channelType, PolarPosition<double> position);

        /** Constructor to initialise from a one of the channel names in bs2094::channelLabels.
         * If an unknown string is passed then the channel will be set to a Custom type and its position
         * to 0deg azimuth and 0deg elevation (i.e. directly to the front).
         */
        Channel(const std::string& channelName);
        ~Channel();

        /** Get the name of the channel. */
        const std::string& getChannelName() const;

        /** Get the channel's type. */
        ChannelTypes getChannelType() const;

        /** Get the polar position of the channel. This is the actual position of the channel. */
        const PolarPosition<double>& getPolarPosition() const;

        /** Get the nominal position of the channel based on its type. */
        const PolarPosition<double>& getPolarPositionNominal() const;

        /** Set the Channel's polar position. */
        void setPolarPosition(const PolarPosition<double>& polarPosition);

        /** Set the Channels nominal polar position. */
        void setPolarPositionNominal(const PolarPosition<double>& polarPosNominal);

        /** Returns true if the channel is an LFE. */
        bool getIsLfe() const;

    private:
        bool isChannelLFE();

        std::string name;
        ChannelTypes channelType = ChannelTypes::Custom;
        // Real loudspeaker position
        PolarPosition<double> polarPosition;
        // Nominal loudspeaker position from ITU-R BS.2051-2
        PolarPosition<double> polarPositionNominal;
        bool isLFE = false;
    };

    // Class used to store the layout information
    class Layout
    {
    public:
        Layout();
        Layout(std::string layoutName, std::vector<Channel> layoutChannels, bool layoutHasLfe);
        Layout(std::string layoutName, std::vector<Channel> layoutChannels, bool layoutHasLfe, Screen screen);
        Layout(OutputLayout layoutType);
        Layout(std::string& layoutName);

        /** Get the name of the layout.
         * @return String containing the layout name.
         */
        const std::string& getLayoutName() const;

        /** Get a vector of the channels in the layout.
         * @return Vector of the Channels in the layout.
         */
        const std::vector<Channel>& getChannels() const;

        /** Returns a reference to the specified channel.
         * @param iCh Index of the channel.
         * @return A reference to the channel.
         */
        const Channel& getChannel(size_t iCh) const;
        Channel& getChannel(size_t iCh);

        /** Returns the number of channels in the layout. */
        size_t getNumChannels() const;

        /** If the layout contains an LFE channel then this returns true.
         * @return Returns true if the layout contains an LFE channel.
         */
        bool hasLfe() const;

        /** Get the optionally set reproduction screen.
         * @return Reference to the optional screen.
         */
        const Optional<Screen>& getReproductionScreen() const;

        /** Set the Layout's reproduction screen. */
        void setReproductionScreen(const Screen& screen);

        /** If the channel name matches one of the channels in the Layout then return
         *  its index. If not, return -1.
         * @param channelName	String containing the name of the channel.
         * @return				The index of the channel in the layout. If it is not present, returns -1.
         */
        int getMatchingChannelIndex(const std::string& channelName);

        /** Returns a list of the channel names in order. */
        std::vector<std::string> channelNames() const;

        /** Returns true if the layout contains the specified channel.
         * @param channelName Name of the channel to check for.
         * @return Returns true if channelName is present in the layout
         */
        bool containsChannel(const std::string& channelName) const;

        /** Returns a version of the input layout without any LFE channels.
         * @param layout	Input layout.
         * @return			Copy of the input layout with any LFE channels removed.
         */
        static Layout getLayoutWithoutLFE(Layout layout);

        static const std::vector<Layout>& getSpeakerLayouts();

        /** Get the speakerLayout that matches the given name. If no match then returns empty.
         * @param layoutName	String containing the name of the desired speaker layout.
         * @return				The speaker layout matching the name. Returns an empty layout if none is found.
         */
        static Layout getMatchingLayout(std::string layoutName);

        /** Get the speakerLayout that matches the given name.
         * @param layoutName	The type of the loudspeaker layout
         * @return				The speaker layout matching the name.
         */
        static Layout getMatchingLayout(OutputLayout layoutType);

    private:
        std::string name;
        std::vector<Channel> channels;
        bool hasLFE = false;

        Optional<Screen> reproductionScreen = Optional<Screen>();
    };

    /** Check if the input DirectSpeakerMetadata is for an LFE channel.
    * @param metadata	Metadata to check.
    * @return			Returns true if the input metadata refers to an LFE channel.
    */
    static inline bool isLFE(const DirectSpeakerMetadata& metadata)
    {
        // See Rec. ITU-R BS.2127-1 sec. 6.3
        if (metadata.channelFrequency.lowPass.hasValue())
            if (metadata.channelFrequency.lowPass.value() <= 120.)
                return true;

        const std::string& nominalLabel = GetNominalSpeakerLabel(metadata.speakerLabel);
        if (stringContains(nominalLabel, "LFE1") || stringContains(nominalLabel, "LFE2"))
        {
            return true;
        }
        return false;
    }

    /** Check the loudspeaker positions are within the valid ranges. See Rec. ITU-R BS.2127-1 Sec. 3.1.
     * @param layout Loudspeaker layout to check.
     * @return Returns true if all loudspeakers are in the valid range.
     */
    static inline bool checkLayoutAngles(const Layout& layout)
    {
        /** Angular ranges for each loudspeaker for all loudspeaker layouts.
        Where possible, ranges are taken from Rec. ITU-R BS.2051-3. If the layout is not
        in Rec. ITU-R BS.2051-3 then the nearest equivalent is used.
        */
        struct ChannelRanges
        {
            std::pair<double, double> azRange;
            std::pair<double, double> elRange;
        };
        static const std::map<std::string, std::map<std::string, ChannelRanges>> speakerRanges = {
            // Stereo - BS.2051-3 System A 0+2+0
            {"0+2+0",{ {"M+030",{{30., 30.}, {0.,0.}}},
            {"M-030",{{-30., -30.}, {0.,0.}} }}},
            // Quad
            {"0+4+0",{ {"M+045",{{45., 45.}, {0.,0.}}},
            {"M-045",{{-45., -45.}, {0.,0.}}},
            {"M+135",{{135., 135.}, {0.,0.}}},
            {"M-135",{{-135., -135.}, {0.,0.}}}
            }},
            // 5.1 - BS.2051-3 System B 0+5+0
            {"0+5+0",{ {"M+030",{{30., 30.}, {0.,0.}}},
            {"M-030",{{-30., -30.}, {0.,0.}}},
            {"M+000",{{0., 0.}, {0.,0.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"M+110",{{100., 120.}, {0.,15.}}},
            {"M-110",{{-120., -100.}, {0.,15.}}}
            }},
            // 5.1.2 - BS.2051-3 System C 2+5+0
            {"2+5+0",{ {"M+030",{{30., 30.}, {0.,0.}}},
            {"M-030",{{-30., -30.}, {0.,0.}}},
            {"M+000",{{0., 0.}, {0.,0.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"M+110",{{100., 120.}, {0.,15.}}},
            {"M-110",{{-120., -100.}, {0.,15.}}},
            {"U+030",{{30., 45.}, {30.,55.}}},
            {"U-030",{{-45., -30.}, {30.,55.}}}
            }},
            // 5.1.4 - BS.2051-3 System D 4+5+0
            {"4+5+0",{ {"M+030",{{30., 30.}, {0.,0.}}},
            {"M-030",{{-30., -30.}, {0.,0.}}},
            {"M+000",{{0., 0.}, {0.,0.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"M+110",{{100., 120.}, {0.,15.}}},
            {"M-110",{{-120., -100.}, {0.,15.}}},
            {"U+030",{{30., 45.}, {30.,55.}}},
            {"U-030",{{-45., -30.}, {30.,55.}}},
            {"U+110",{{110., 135.}, {30.,55.}}},
            {"U-110",{{-135., -110.}, {30.,55.}}}
            }},
            // BS.2051-3 System E 4+5+1
            {"4+5+1",{ {"M+030",{{30., 30.}, {0.,0.}}},
            {"M-030",{{-30., -30.}, {0.,0.}}},
            {"M+000",{{0., 0.}, {0.,0.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"M+110",{{100., 120.}, {0.,15.}}},
            {"M-110",{{-120., -100.}, {0.,15.}}},
            {"U+030",{{30., 45.}, {30.,55.}}},
            {"U-030",{{-45., -30.}, {30.,55.}}},
            {"U+110",{{110., 135.}, {30.,55.}}},
            {"U-110",{{-135., -110.}, {30.,55.}}},
            {"B+000",{{0., 0.}, {-30., -15.}}}
            }},
            // BS.2051-3 System F 3+7+0
            {"3+7+0",{ {"M+000",{{0.,0.}, {0.,0.}}},
            {"M+030",{{30., 30.}, {0.,0.}}},
            {"M-030",{{-30., -30.}, {0.,0.}}},
            {"U+045",{{30., 45.}, {30.,45.}}},
            {"U-045",{{-45., -30.}, {30.,45.}}},
            {"M+090",{{60., 150.}, {0.,0.}}},
            {"M-090",{{-150., -60.}, {0.,0.}}},
            {"M+135",{{60., 150.}, {0.,0.}}},
            {"M-135",{{-150., -60.}, {0.,0.}}},
            {"UH+180",{{180., 180}, {45.,90.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"LFE2",{{-180., 180.}, {-90.,90.}}}
            }},
            // BS.2051-3 System G 4+9+0 - Note: doesn't include the screen speakers because these are defined as in Rec. ITU-R BS.2127-1 Sec. 7.3.9
            {"4+9+0",{ {"M+030",{{30., 45.}, {0.,0.}}},
            {"M-030",{{-45., -30.}, {0.,0.}}},
            {"M+000",{{0., 0.}, {0.,0.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"M+090",{{85., 110.}, {0.,0.}}},
            {"M-090",{{-110., -85.}, {0.,0.}}},
            {"M+135",{{120., 150.}, {0.,0.}}},
            {"M-135",{{-150., -120.}, {0.,0.}}},
            {"U+045",{{30., 45.}, {30.,55.}}},
            {"U-045",{{-45., -30.}, {30.,55.}}},
            {"U+135",{{100., 150.}, {30.,55.}}},
            {"U-135",{{-150., -100.}, {30.,55.}}},
            {"M+SC",{{5., 25.}, {0.,0.}} },
            {"M-SC",{{-25., -5.}, {0.,0.}}}
            }},
            // BS.2051-3 System H 9+10+3
            {"9+10+3",{ {"M+060",{{45., 60.}, {0.,5.}}},
            {"M-060",{{-60., -45.}, {0.,5.}}},
            {"M+000",{{0., 0.}, {0.,5.}}},
            {"LFE1",{{30., 90.}, {-30.,-15.}}},
            {"M+135",{{110., 135.}, {0.,15.}}},
            {"M-135",{{-135., -110.}, {0.,15.}}},
            {"M+030",{{22.5, 30.}, {0.,5.}}},
            {"M-030",{{-30., -22.5}, {0.,5.}}},
            {"M+180",{{180., 180.}, {0.,15.}}},
            {"LFE2",{{-90., -30.}, {-30.,-15.}}},
            {"M+090",{{90., 90.}, {0.,15.}}},
            {"M-090",{{-90., -90.}, {0.,15.}}},
            {"U+045",{{45., 60.}, {30.,45.}}},
            {"U-045",{{-60., -45.}, {30.,45.}}},
            {"U+000",{{0., 0.}, {30.,45.}}},
            {"T+000",{{-180., 180.}, {90.,90.}}},
            {"U+135",{{110., 135.}, {30.,45.}}},
            {"U-135",{{-135., -110.}, {30.,45.}}},
            {"U+090",{{90., 90.}, {30.,45.}}},
            {"U-090",{{-90., -90.}, {30.,45.}}},
            {"U+180",{{180., 180.}, {30.,45.}}},
            {"B+000",{{0., 0.}, {-30.,-15.}}},
            {"B+045",{{45., 60.}, {-30.,-15.}}},
            {"B-045",{{-60., -45.}, {-30.,-15.}}}
            }},
            // 7.1 - BS.2051-3 System I 0+7+0
            { "0+7+0",{ {"M+030",{{30., 45.}, {0.,0.}}},
            {"M-030",{{-45., -30.}, {0.,0.}}},
            {"M+000",{{0., 0.}, {0.,0.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"M+090",{{85., 110.}, {0.,0.}}},
            {"M-090",{{-110., -85.}, {0.,0.}}},
            {"M+135",{{120., 150.}, {0.,0.}}},
            {"M-135",{{-150., -120.}, {0.,0.}}}
            }},
            // 7.1.4 - BS.2051-3 System J 4+7+0
            { "4+7+0",{ {"M+030",{{30., 45.}, {0.,0.}}},
            {"M-030",{{-45., -30.}, {0.,0.}}},
            {"M+000",{{0., 0.}, {0.,0.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"M+090",{{85., 110.}, {0.,0.}}},
            {"M-090",{{-110., -85.}, {0.,0.}}},
            {"M+135",{{120., 150.}, {0.,0.}}},
            {"M-135",{{-150., -120.}, {0.,0.}}},
            {"U+045",{{30., 45.}, {30.,55.}}},
            {"U-045",{{-45., -30.}, {30.,55.}}},
            {"U+135",{{100., 150.}, {30.,55.}}},
            {"U-135",{{-150., -100.}, {30.,55.}}}
            }},
            // 7.1.2 - IAMF v1.0
            { "2+7+0",{ {"M+030",{{30., 45.}, {0.,0.}}},
            {"M-030",{{-45., -30.}, {0.,0.}}},
            {"M+000",{{0., 0.}, {0.,0.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"M+090",{{85., 110.}, {0.,0.}}},
            {"M-090",{{-110., -85.}, {0.,0.}}},
            {"M+135",{{120., 150.}, {0.,0.}}},
            {"M-135",{{-150., -120.}, {0.,0.}}},
            {"U+045",{{30., 45.}, {30.,55.}}},
            {"U-045",{{-45., -30.}, {30.,55.}}}
            }},
            // 3.1.2 - IAMF v1.0
            { "2+3+0",{ {"M+030",{{30., 45.}, {0.,0.}}},
            {"M-030",{{-45., -30.}, {0.,0.}}},
            {"M+000",{{0., 0.}, {0.,0.}}},
            {"LFE1",{{-180., 180.}, {-90.,90.}}},
            {"U+045",{{30., 45.}, {30.,55.}}},
            {"U-045",{{-45., -30.}, {30.,55.}}},
            }},
            // BEAR: 9+10+3 plus 2 bottom layer speakers
            { "9+10+5",{ {"M+060",{{45., 60.}, {0.,5.}}},
            {"M-060",{{-60., -45.}, {0.,5.}}},
            {"M+000",{{0., 0.}, {0.,5.}}},
            {"M+135",{{110., 135.}, {0.,15.}}},
            {"M-135",{{-135., -110.}, {0.,15.}}},
            {"M+030",{{22.5, 30.}, {0.,5.}}},
            {"M-030",{{-22.5,-30.}, {0.,5.}}},
            {"M+180",{{180., 180.}, {0.,15.}}},
            {"M+090",{{90., 90.}, {0.,15.}}},
            {"M-090",{{-90., -90.}, {0.,15.}}},
            {"U+045",{{45., 60.}, {30.,45.}}},
            {"U-045",{{-60., -45.}, {30.,45.}}},
            {"U+000",{{0., 0.}, {30.,45.}}},
            {"T+000",{{-180., 180.}, {90.,90.}}},
            {"U+135",{{110., 135.}, {30.,45.}}},
            {"U-135",{{-135., -110.}, {30.,45.}}},
            {"U+090",{{90., 90.}, {30.,45.}}},
            {"U-090",{{-90., -90.}, {30.,45.}}},
            {"U+180",{{180., 180.}, {30.,45.}}},
            {"B+000",{{0., 0.}, {-30.,-15.}}},
            {"B+045",{{45., 60.}, {-30.,-15.}}},
            {"B-045",{{-60., -45.}, {-30.,-15.}}},
            {"B+135",{{110., 135.}, {-30.,-15.}}},
            {"B-135",{{-135., -110.}, {-30.,-15.}}} }}
        };

        auto it = speakerRanges.find(layout.getLayoutName());
        double tol = 1e-6;
        if (it != speakerRanges.end())
        {
            const auto& layoutRanges = it->second;

            for (auto& channel : layout.getChannels())
            {
                std::pair<double, double> azRange, elRange;
                auto& nominalLabel = GetNominalSpeakerLabel(channel.getChannelName());
                auto ranges = layoutRanges.find(nominalLabel);
                if (ranges != layoutRanges.end())
                {
                    elRange = ranges->second.elRange;

                    if (channel.getChannelName() == "M+SC" || channel.getChannelName() == "M-SC")
                    {
                        // "the absolute azimuth of both M+SC and M-SC loudspeakers must either be between 5 deg and 25 deg or
                        // between 35° or 60°"
                        std::pair<double, double> azRange1 = ranges->second.azRange;
                        std::pair<double, double> azRange2 = { 35., 60. };
                        elRange = { 0., 0. };
                        double absAz = std::abs(channel.getPolarPosition().azimuth);
                        if (!(insideAngleRange(channel.getPolarPosition().azimuth, azRange1.first, azRange1.second, tol) || insideAngleRange(absAz, azRange2.first, azRange2.second, tol)
                            || !insideAngleRange(channel.getPolarPosition().elevation, elRange.first, elRange.second)))
                            return false;
                    }
                    else
                    {
                        azRange = ranges->second.azRange;
                        if (!insideAngleRange(channel.getPolarPosition().azimuth, azRange.first, azRange.second, tol)
                            || !insideAngleRange(channel.getPolarPosition().elevation, elRange.first, elRange.second, tol))
                            return false;
                    }
                }
                else
                    return false; // Input layout contains a speaker not in the predefined layout
            }
            return true;
        }
        return false;
    }

} // namespace spaudio
