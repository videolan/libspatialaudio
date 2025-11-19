/*############################################################################*/
/*#                                                                          #*/
/*#  Loudspeaker layouts.		                                             #*/
/*#								                                             #*/
/*#  Copyright © 2020 Peter Stitt                                            #*/
/*#                                                                          #*/
/*#  Filename:      AdmLayout.h	                                             #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          23/06/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#include "LoudspeakerLayouts.h"

namespace spaudio {

    Channel::Channel() : Channel(ChannelTypes::FrontCentre)
    {
    }

    Channel::Channel(std::string& channelName, PolarPosition<double> position, PolarPosition<double> positionNominal, bool channelLfe)
        : name(channelName), channelType(ChannelTypes::Custom), polarPosition(position), polarPositionNominal(positionNominal), isLFE(channelLfe)
    {
    }

    Channel::Channel(ChannelTypes channelType)
    {
        int index = static_cast<int>(channelType);
        if (index > -1 && static_cast<size_t>(index) < bs2094::channelLabels.size())
        {
            name = bs2094::channelLabels[index];
            this->channelType = channelType;
            if (static_cast<size_t>(index) < bs2094::positions.size())
            {
                polarPosition = bs2094::positions[index];
                polarPositionNominal = bs2094::positions[index];
            }
            else //
            {

            }
            isLFE = isChannelLFE();
        }
        else
        {
            throw std::runtime_error("Channel type not complete. Please add label and position");
        }
    }

    Channel::Channel(ChannelTypes channelType, PolarPosition<double> position) : Channel(channelType)
    {
        // Set the polar position to the custom value
        polarPosition = position;
    }

    Channel::Channel(const std::string& channelName)
    {
        name = channelName;

        // Find the channel in the labels vector
        auto it = std::find(bs2094::channelLabels.begin(), bs2094::channelLabels.end(), channelName);

        if (it != bs2094::channelLabels.end())
        {
            auto index = std::distance(bs2094::channelLabels.begin(), it);
            channelType = static_cast<ChannelTypes>(index);
            polarPosition = bs2094::positions[index];
            polarPositionNominal = bs2094::positions[index];
            isLFE = isChannelLFE();
        }
        else
        {
            throw std::runtime_error("Unknown channel: " + channelName);
        }
    }
    Channel::~Channel() {};

    bool Channel::isChannelLFE()
    {
        const std::string& nominalLabel = GetNominalSpeakerLabel(name);
        return (stringContains(nominalLabel, "LFE1") || stringContains(nominalLabel, "LFE2"));
    };

    const std::string& Channel::getChannelName() const
    {
        return name;
    }

    ChannelTypes Channel::getChannelType() const
    {
        return channelType;
    }

    const PolarPosition<double>& Channel::getPolarPosition() const
    {
        return polarPosition;
    }

    const PolarPosition<double>& Channel::getPolarPositionNominal() const
    {
        return polarPositionNominal;
    }

    void Channel::setPolarPosition(const PolarPosition<double>& polarPos)
    {
        polarPosition = polarPos;
    }

    void Channel::setPolarPositionNominal(const PolarPosition<double>& polarPosNominal)
    {
        polarPositionNominal = polarPosNominal;
    }

    bool Channel::getIsLfe() const
    {
        return isLFE;
    }


    Layout::Layout()
    {
    }

    Layout::Layout(std::string layoutName, std::vector<Channel> layoutChannels, bool layoutHasLfe)
        : name(layoutName), channels(layoutChannels), hasLFE(layoutHasLfe)
    {
    }

    Layout::Layout(std::string layoutName, std::vector<Channel> layoutChannels, bool layoutHasLfe, Screen screen)
        : name(layoutName), channels(layoutChannels), hasLFE(layoutHasLfe), reproductionScreen(screen)
    {
    }

    Layout::Layout(OutputLayout layoutType)
    {
        std::string layoutName;
        switch (layoutType)
        {
        case OutputLayout::Stereo:
            layoutName = "0+2+0";
            break;
        case OutputLayout::Quad:
            layoutName = "0+4+0";
            break;
        case OutputLayout::FivePointOne:
            layoutName = "0+5+0";
            break;
        case OutputLayout::FivePointOnePointTwo:
            layoutName = "2+5+0";
            break;
        case OutputLayout::FivePointOnePointFour:
            layoutName = "4+5+0";
            break;
        case OutputLayout::FivePointOnePointFourPlusLow:
            layoutName = "4+5+1";
            break;
        case OutputLayout::SevenPointOnePointThree:
            layoutName = "3+7+0";
            break;
        case OutputLayout::ThirteenPointOne:
            layoutName = "4+9+0";
            break;
        case OutputLayout::TwentyTwoPointTwo:
            layoutName = "9+10+3";
            break;
        case OutputLayout::SevenPointOne:
            layoutName = "0+7+0";
            break;
        case OutputLayout::SevenPointOnePointFour:
            layoutName = "4+7+0";
            break;
        case OutputLayout::BEAR_9_10_5:
            layoutName = "9+10+5";
            break;
        case OutputLayout::SevenPointOnePointTwo:
            layoutName = "2+7+0";
            break;
        case OutputLayout::ThreePointOnePointTwo:
            layoutName = "2+3+0";
            break;
        case OutputLayout::Binaural:
            throw std::runtime_error("Binaural is not a supported loudspeaker layout.");
            break;
        default:
            throw std::runtime_error("Layout is not in the list of presets! Please add it.");
            break;
        }
        *this = getMatchingLayout(layoutName);
    }

    Layout::Layout(std::string& layoutName)
    {
        *this = getMatchingLayout(layoutName);
    }

    const std::string& Layout::getLayoutName() const
    {
        return name;
    }

    const std::vector<Channel>& Layout::getChannels() const
    {
        return channels;
    }

    const Channel& Layout::getChannel(size_t iCh) const
    {
        assert(iCh < channels.size());
        return channels[iCh];
    }

    Channel& Layout::getChannel(size_t iCh)
    {
        assert(iCh < channels.size());
        return channels[iCh];
    }

    size_t Layout::getNumChannels() const
    {
        return channels.size();
    }

    bool Layout::hasLfe() const
    {
        return hasLFE;
    }

    const Optional<Screen>& Layout::getReproductionScreen() const
    {
        return reproductionScreen;
    }

    void Layout::setReproductionScreen(const Screen& screen)
    {
        reproductionScreen = screen;
    }

    int Layout::getMatchingChannelIndex(const std::string& channelName)
    {
        unsigned int nChannels = (unsigned int)getNumChannels();
        for (unsigned int iCh = 0; iCh < nChannels; ++iCh)
        {
            if (channelName.compare(channels[iCh].getChannelName()) == 0)
                return iCh;
        }
        return -1; // if no matching channel names are found
    }

    std::vector<std::string> Layout::channelNames() const
    {
        std::vector<std::string> channelNames;
        for (unsigned int iCh = 0; iCh < getNumChannels(); ++iCh)
            channelNames.push_back(channels[iCh].getChannelName());

        return channelNames;
    }

    bool Layout::containsChannel(const std::string& channelName) const
    {
        for (unsigned int iCh = 0; iCh < getNumChannels(); ++iCh)
            if (channels[iCh].getChannelName() == channelName)
                return true;
        return false;
    }

    Layout Layout::getLayoutWithoutLFE(Layout layout)
    {
        Layout layoutNoLFE = layout;
        unsigned int nCh = (unsigned int)layout.getNumChannels();
        layoutNoLFE.channels.resize(0);
        for (unsigned int iCh = 0; iCh < nCh; ++iCh)
            if (!layout.channels[iCh].getIsLfe())
                layoutNoLFE.channels.push_back(layout.channels[iCh]);
        layoutNoLFE.hasLFE = false;

        return layoutNoLFE;
    }

    const std::vector<Layout>& Layout::getSpeakerLayouts()
    {
        /** Predefined speaker layouts. */
        static const std::vector<Layout> speakerLayouts = {
            // Stereo - BS.2051-3 System A 0+2+0
            Layout{ "0+2+0",std::vector<Channel>{ Channel{"M+030"}, Channel{"M-030"} }, false},
            // Quad - note: Not defined in ITU-R BS.2051-3
            Layout{ "0+4+0",std::vector<Channel>{ Channel("M+045"), Channel("M-045"), Channel("M+135"), Channel("M-135")}, false},
            // 5.1 - BS.2051-3 System B 0+5+0
            Layout{ "0+5+0",std::vector<Channel>{ Channel("M+030"), Channel("M-030"), Channel("M+000"), Channel("LFE1"), Channel("M+110"), Channel("M-110")}, true},
            // 5.1.2 - BS.2051-3 System C 2+5+0
            Layout{ "2+5+0",std::vector<Channel>{ Channel("M+030"), Channel("M-030"), Channel("M+000"), Channel("LFE1"),Channel("M+110"), Channel("M-110"),
            Channel("U+030"), Channel("U-030")}, true},
            // 5.1.4 - BS.2051-3 System D 4+5+0
            Layout{ "4+5+0",std::vector<Channel>{ Channel("M+030"), Channel("M-030"), Channel("M+000"), Channel("LFE1"), Channel("M+110"), Channel("M-110"),
            Channel("U+030"), Channel("U-030"), Channel("U+110"), Channel("U-110")}, true},
            // 5.1.4+top - BS.2051-3 System E 4+5+1
            Layout{"4+5+1",std::vector<Channel>{ Channel("M+030"),Channel("M-030"),Channel("M+000"),Channel("LFE1"),Channel("M+110"),Channel("M-110"),
            Channel("U+030"),Channel("U-030"),Channel("U+110"),Channel("U-110"),Channel("B+000")}, true},
            // 7.1.3 - BS.2051-3 System F 3+7+0
            Layout{"3+7+0",std::vector<Channel>{ Channel("M+000"),Channel("M+030"),Channel("M-030"),Channel("U+045"),Channel("U-045"),Channel("M+090"),Channel("M-090"),
            Channel("M+135"),Channel("M-135"),Channel("UH+180"),Channel("LFE1"),Channel("LFE2")}, true},
            // 13.1 - BS.2051-3 System G 4+9+0
            Layout{"4+9+0",std::vector<Channel>{ Channel("M+030"),Channel("M-030"),Channel("M+000"),Channel("LFE1"),Channel("M+090"),Channel("M-090"),Channel("M+135"),
            Channel("M-135"),Channel("U+045"),Channel("U-045"),Channel("U+135"),Channel("U-135"), Channel("M+SC"), Channel("M-SC")}, true},
            // 22.2 BS.2051-3 System H 9+10+3
            Layout{"9+10+3",std::vector<Channel>{ Channel("M+060"),Channel("M-060"),Channel("M+000"),Channel("LFE1"),Channel("M+135"),Channel("M-135"),Channel("M+030"),Channel("M-030"),
            Channel("M+180"),Channel("LFE2"),Channel("M+090"),Channel("M-090"),Channel("U+045"),Channel("U-045"),Channel("U+000"),Channel("T+000"),Channel("U+135"),Channel("U-135"),
            Channel("U+090"),Channel("U-090"),Channel("U+180"),Channel("B+000"),Channel("B+045"),Channel("B-045")}, true},
            // 7.1 - BS.2051-3 System I 0+7+0
            Layout{"0+7+0",std::vector<Channel>{ Channel("M+030"),Channel("M-030"),Channel("M+000"),Channel("LFE1"),Channel("M+090"),Channel("M-090"),Channel("M+135"),Channel("M-135")}, true},
            // 7.1.4 - BS.2051-3 System J 4+7+0
            Layout{"4+7+0",std::vector<Channel>{ Channel("M+030"),Channel("M-030"),Channel("M+000"),Channel("LFE1"),Channel("M+090"),Channel("M-090"),
            Channel("M+135"),Channel("M-135"),Channel("U+045"),Channel("U-045"),Channel("U+135"),Channel("U-135")}, true},
            // 7.1.2 - IAMF v1.0.0-errata
            Layout{"2+7+0",std::vector<Channel>{ Channel("M+030"),Channel("M-030"),Channel("M+000"),Channel("LFE1"),Channel("M+090"),Channel("M-090"),Channel("M+135"),Channel("M-135"),
            Channel("U+045"),Channel("U-045")}, true },
            // 3.1.2 - IAMF v1.0.0-errata
            Layout{"2+3+0",std::vector<Channel>{ Channel("M+030"),Channel("M-030"),Channel("M+000"),Channel("LFE1"),Channel("U+045"),Channel("U-045")}, true },
            // EBU Tech 3369 (BEAR) 9+10+5 - 9+10+3 with LFE1 & LFE2 removed and B+135 & B-135 added
            Layout{"9+10+5",std::vector<Channel>{ Channel("M+060"),Channel("M-060"),Channel("M+000"),Channel("M+135"),Channel("M-135"),Channel("M+030"),Channel("M-030"),Channel("M+180"),
            Channel("M+090"),Channel("M-090"),Channel("U+045"),Channel("U-045"),Channel("U+000"),Channel("T+000"),Channel("U+135"),Channel("U-135"),Channel("U+090"),Channel("U-090"),
            Channel("U+180"),Channel("B+000"),Channel("B+045"),Channel("B-045"),Channel("B+135"),Channel("B-135")}, true },
        };
        return speakerLayouts;
    }

    Layout Layout::getMatchingLayout(std::string layoutName)
    {
        auto& speakerLayouts = getSpeakerLayouts();

        unsigned int nLayouts = (unsigned int)speakerLayouts.size();
        for (unsigned int iLayout = 0; iLayout < nLayouts; ++iLayout)
        {
            if (layoutName.compare(speakerLayouts[iLayout].name) == 0)
                return speakerLayouts[iLayout];
        }
        return {}; // if no matching channel names are found
    }

    Layout Layout::getMatchingLayout(OutputLayout layoutType)
    {
        auto& speakerLayouts = getSpeakerLayouts();
        int ind = static_cast<int>(layoutType);
        return speakerLayouts[ind];
    }

} // namespace spaudio
