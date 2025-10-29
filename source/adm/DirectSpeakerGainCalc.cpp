/*############################################################################*/
/*#                                                                          #*/
/*#  Calculate the gain vector to spatialise a DirectSpeaker channel.        #*/
/*#  DirectSpeakersGainCalc                                                  #*/
/*#  Copyright © 2020 Peter Stitt                                            #*/
/*#                                                                          #*/
/*#  Filename:      DirectSpeakersGainCalc.cpp                               #*/
/*#  Version:       0.1                                                      #*/
/*#  Date:          23/06/2020                                               #*/
/*#  Author(s):     Peter Stitt                                              #*/
/*#  Licence:       LGPL + proprietary                                       #*/
/*#                                                                          #*/
/*############################################################################*/

#include "adm/DirectSpeakerGainCalc.h"
#include<string>
#include <map>

namespace spaudio {
    namespace adm {

        //===================================================================================================================================
        DirectSpeakersGainCalc::DirectSpeakersGainCalc(Layout layoutWithLFE)
            : m_pointSourcePannerGainCalc(Layout::getLayoutWithoutLFE(layoutWithLFE)), m_screenEdgeLock(layoutWithLFE.getReproductionScreen(), layoutWithLFE)
        {
            m_layout = layoutWithLFE;
            m_nCh = (unsigned int)m_layout.getNumChannels();
            m_gainsPSP.resize(Layout::getLayoutWithoutLFE(layoutWithLFE).getNumChannels(), 0.);
            m_withinBounds.resize(m_nCh);
        }

        DirectSpeakersGainCalc::~DirectSpeakersGainCalc()
        {
        }

        int DirectSpeakersGainCalc::findClosestWithinBounds(const DirectSpeakerPolarPosition& direction, double tol)
        {
            // See Rec. ITU-R BS.2127-0 sec 8.5
            m_withinBounds.clear();

            // speaker coordinates
            double az = direction.azimuth;
            double el = direction.elevation;
            double d = direction.distance;

            double minAz = az;
            double minEl = el;
            double maxAz = az;
            double maxEl = el;
            double minDist = d;
            double maxDist = d;
            if (direction.bounds.hasValue())
            {
                minAz = direction.bounds.value().minAzimuth;
                minEl = direction.bounds.value().minElevation;
                maxAz = direction.bounds.value().maxAzimuth;
                maxEl = direction.bounds.value().maxElevation;
                minDist = direction.bounds.value().minDistance;
                maxDist = direction.bounds.value().maxDistance;
            }

            for (unsigned int iSpk = 0; iSpk < m_nCh; ++iSpk)
            {
                PolarPosition<double> spkDir = m_layout.getChannel(iSpk).getPolarPositionNominal();
                if ((insideAngleRange(spkDir.azimuth, minAz, maxAz, tol) || spkDir.elevation > 90. - tol) &&
                    (spkDir.elevation <= maxEl + tol && spkDir.elevation >= minEl - tol) &&
                    (spkDir.distance <= maxDist + tol && spkDir.distance >= minDist - tol))
                {
                    m_withinBounds.push_back(iSpk);
                }
            }
            if (m_withinBounds.size() == 0.)
                return -1; // No speakers found
            else if (m_withinBounds.size() == 1)
                return m_withinBounds[0];
            else if (m_withinBounds.size() > 1)
            {
                std::vector<double> distanceWithinBounds;
                CartesianPosition<double> cartDirection = PolarToCartesian(PolarPosition<double>{ direction.azimuth,direction.elevation,direction.distance });
                for (auto& t : m_withinBounds)
                {
                    CartesianPosition<double> spkCart = PolarToCartesian(m_layout.getChannel(t).getPolarPositionNominal());
                    double distance = norm(vecSubtract({ spkCart.x,spkCart.y,spkCart.z }, { cartDirection.x,cartDirection.y,cartDirection.z }));
                    distanceWithinBounds.push_back(distance);
                }
                // Find the closest
                auto distanceWithinBoundsSorted = distanceWithinBounds;
                std::sort(distanceWithinBoundsSorted.begin(), distanceWithinBoundsSorted.end());
                double smallestDistance = distanceWithinBoundsSorted[0];
                // Check all possible speakers within bounds to determine their distance
                std::vector<unsigned int> closestSpeakers;
                for (unsigned int iDist = 0; iDist < distanceWithinBounds.size(); ++iDist)
                {
                    if (distanceWithinBounds[iDist] == smallestDistance)
                        closestSpeakers.push_back(m_withinBounds[iDist]);
                }
                // If only one matches then return that index
                if (closestSpeakers.size() == 1)
                    return closestSpeakers[0];
                else if (closestSpeakers.size() == 0) // something probably went wrong...
                    return -1;
                else if (closestSpeakers.size() > 1) // no unique answer
                    return -1;
            }

            // If nothing found by this point return -1
            return -1;
        }

        void DirectSpeakersGainCalc::calculateGains(const DirectSpeakerMetadata& metadata, std::vector<double>& gains)
        {
            assert(gains.size() == m_nCh); // Gain vector length must match the number of channels

            // is the current channel an LFE
            bool isLfeChannel = isLFE(metadata);

            const std::string& nominalSpeakerLabel = GetNominalSpeakerLabel(metadata.speakerLabel);

            for (auto& g : gains)
                g = 0.f;

            if (metadata.audioPackFormatID.hasValue())
            {
                auto ituPack = ituPackNames.find(metadata.audioPackFormatID.value());
                if (ituPack != ituPackNames.end()) // if the audioPackFormat is in the list of ITU packs
                {
                    const std::string& layoutName = ituPack->second;

                    for (const MappingRule& rule : mappingRules)
                    {
                        if (MappingRuleApplies(rule, layoutName, nominalSpeakerLabel, m_layout))
                        {
                            for (auto& gain : rule.gains) {
                                int idx = m_layout.getMatchingChannelIndex(gain.first);
                                if (idx >= 0)
                                    gains[idx] = gain.second;
                            }
                            return;
                        }
                    }
                }
            }

            // Check if there are any speakers with the same label and LFE type
            const std::string& speakerLabel = GetNominalSpeakerLabel(metadata.speakerLabel);
            int idx = m_layout.getMatchingChannelIndex(speakerLabel);
            if (idx >= 0 && (m_layout.getChannel(idx).getIsLfe() == isLfeChannel))
            {
                gains[idx] = 1.;

                return;
            }

            DirectSpeakerPolarPosition direction = metadata.polarPosition;

            // Screen edge locking
            CartesianPosition<double> position = PolarToCartesian(PolarPosition<double>{ direction.azimuth,direction.elevation,direction.distance });
            position = m_screenEdgeLock.HandleVector(position, metadata.screenEdgeLock);
            PolarPosition<double> polarPosition = CartesianToPolar(position);
            direction.azimuth = polarPosition.azimuth;
            direction.elevation = polarPosition.elevation;
            direction.distance = polarPosition.distance;

            // If the channel is LFE based on frequency metadata then send to the appropriate LFE (if any exist)
            if (isLfeChannel)
            {
                idx = m_layout.getMatchingChannelIndex("LFE1");
                if (idx >= 0)
                    gains[idx] = 1.;
            }
            else
            {
                // Check for speakers within bounds
                double tol = 1e-5;
                int idxWithinBounds = findClosestWithinBounds(direction, tol);
                if (idxWithinBounds >= 0)
                {
                    gains[idxWithinBounds] = 1.;

                    return;
                }

                m_pointSourcePannerGainCalc.CalculateGains(PolarPosition<double>{ direction.azimuth,direction.elevation,direction.distance }, m_gainsPSP);
                // fill in the gains on the non-LFE channels
                int indNonLfe = 0;
                for (unsigned int i = 0; i < gains.size(); ++i)
                    if (!m_layout.getChannel(i).getIsLfe())
                        gains[i] = m_gainsPSP[indNonLfe++];
            }
        }

        bool DirectSpeakersGainCalc::MappingRuleApplies(const MappingRule& rule, const std::string& inputLayout, const std::string& speakerLabel, Layout& outputLayout)
        {
            // All conditions must be met for the rule to apply
            // "rule.speakerLabel is equal to the first (and only) speakerLabel"
            if (speakerLabel != rule.speakerLabel)
                return false;

            // "input_layout [...] is listed in rule.input_layouts, if this is listed."
            bool containsLayout = false;
            if (rule.inputLayouts.size() > 0)
            {
                for (auto& layout : rule.inputLayouts)
                    if (layout == inputLayout)
                        containsLayout = true;
                if (!containsLayout)
                    return false;
            }

            // "The name of the output loudspeaker layout, layout.name, is listed in
            // rule.output_layouts, if this is listed."
            containsLayout = false;
            if (rule.outputLayouts.size() > 0)
            {
                for (auto& layout : rule.outputLayouts)
                    if (layout == outputLayout.getLayoutName())
                        containsLayout = true;
                if (!containsLayout)
                    return false;
            }

            // All channel names listed in rule.gains exist in layout.channel_names
            for (auto& gain : rule.gains)
            {
                bool containsChannelName = false;
                for (auto& channelName : outputLayout.channelNames())
                    if (channelName == gain.first)
                        containsChannelName = true;
                if (!containsChannelName)
                    return false;
            }

            return true;
        }

    } // namespace adm
} // namespace spaudio
