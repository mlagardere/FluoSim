[region.Number] 3
[region.Geometries] 43 15 0 255 0 255 136 254 139 241 137 232 138 224 143 218 148 206 140 199 141 204 165 194 169 178 163 178 157 166 159 162 163 157 160 156 154 146 150 130 134 130 126 135 117 134 109 124 104 116 104 107 107 94 116 75 102 72 96 64 93 63 83 60 82 60 76 54 69 53 52 57 50 57 44 53 42 49 44 44 43 26 29 22 14 17 5 57 62 25 82 35 89 36 100 46 108 44 112 50 117 55 123 56 126 61 133 60 139 65 139 70 142 71 145 67 150 67 153 75 165 83 168 83 169 87 182 100 192 113 196 121 195 130 209 134 221 145 218 148 206 140 199 141 204 165 194 169 178 163 178 157 166 159 162 163 157 160 156 154 146 150 130 134 130 126 135 117 134 109 124 104 116 104 107 107 94 116 75 102 72 96 64 93 63 83 60 82 60 76 54 69 53 52 57 50 57 44 57 31 60 27 100 92 68 91.9819 68.5708 91.9276 69.1393 91.8374 69.7033 91.7115 70.2603 91.5506 70.8083 91.3553 71.345 91.1263 71.8682 90.8646 72.3758 90.5713 72.8658 90.2474 73.3362 89.8944 73.7851 89.5136 74.2107 89.1066 74.6113 88.675 74.9853 88.2205 75.3312 87.745 75.6475 87.2504 75.9331 86.7387 76.1867 86.212 76.4073 85.6723 76.5941 85.1218 76.7463 84.5628 76.8633 83.9975 76.9445 83.4282 76.9898 82.8572 76.9989 82.2868 76.9717 81.7192 76.9084 81.1567 76.8092 80.6017 76.6746 80.0564 76.505 79.5229 76.3012 79.0034 76.0639 78.5 75.7942 78.0147 75.4931 77.5495 75.1619 77.1063 74.8018 76.6867 74.4142 76.2926 74.0009 75.9255 73.5634 75.5869 73.1035 75.2782 72.6231 75.0005 72.124 74.755 71.6084 74.5428 71.0782 74.3646 70.5356 74.2211 69.9828 74.1131 69.422 74.0408 68.8555 74.0045 68.2856 74.0045 67.7144 74.0408 67.1445 74.1131 66.578 74.2211 66.0172 74.3646 65.4644 74.5428 64.9218 74.755 64.3916 75.0005 63.876 75.2782 63.3769 75.5869 62.8965 75.9255 62.4366 76.2926 61.9991 76.6867 61.5858 77.1063 61.1983 77.5495 60.8381 78.0147 60.5069 78.5 60.2058 79.0034 59.9361 79.5229 59.6988 80.0564 59.495 80.6017 59.3254 81.1567 59.1908 81.7192 59.0916 82.2868 59.0283 82.8572 59.0011 83.4282 59.0102 83.9975 59.0555 84.5628 59.1367 85.1218 59.2537 85.6723 59.4059 86.212 59.5927 86.7387 59.8133 87.2504 60.0669 87.745 60.3525 88.2205 60.6688 88.675 61.0147 89.1066 61.3887 89.5136 61.7893 89.8944 62.2149 90.2474 62.6638 90.5713 63.1342 90.8646 63.6242 91.1263 64.1318 91.3553 64.655 91.5506 65.1917 91.7115 65.7397 91.8374 66.2967 91.9276 66.8607 91.9819 67.4292 92 68 
[region.areCompartments] TRUE TRUE FALSE 
[particleSystemParams.N_particles] 100000
[dynamicParams.D_outsideContact] 0.3
[dynamicParams.D_insideContact_ratio] 1
[dynamicParams.k_on] 0.15
[dynamicParams.k_off] 0.015
[dynamicParams.D_trapped] 0.04
[dynamicParams.crossingProbability_outIn] 0.25
[dynamicParams.immobileFraction] 0
[dynamicParams.isFixed] FALSE
[photoPhysicsParams.k_on_fluo] 0
[photoPhysicsParams.k_off_fluo] 0
[renderingParams.image_intensity] 1
[renderingParams.areRegionsVisible] TRUE
[renderingParams.isBackgroundImageVisible_inGeom] TRUE
[renderingParams.isBackgroundImageVisible_inLiveAndExp] FALSE
[renderingParams.spot_size] 0.171
[renderingParams.isSpotIntensity_inPhotonsPerSec] FALSE
[renderingParams.spot_intensity] 1000
[renderingParams.ADCounts_perPhoton] 1
[renderingParams.rendering_mode] RENDERING_POINTS
[renderingParams.particle_color] PARTICLE_WHITE
[renderingParams.backgroundImage_path]  Resources/Examples/FRAP-in.tif
[simulationParams.dt_sim] 0.1
[simulationParams.current_plane] 0
[simulationParams.current_time] 0
[simulationParams.pixel_size] 0.16
[liveExperimentParams.experimentType] PHOTOBLEACHING_LIVE_EXPERIMENT
[liveExperimentParams.measured_RgnName] Region2
[liveExperimentParams.bleached_RgnName] Region2
[liveExperimentParams.photoActivated_RgnName] Region0
[experimentParams.experimentType] FRAP_EXPERIMENT
[experimentParams.acquisitionType] STREAM_ACQUISITION
[experimentParams.N_planes] 1000
[experimentParams.N_frap] 25
[experimentParams.dN_frap] 4
[experimentParams.k_off_frap] 4.25
[experimentParams.N_photoActivation] 100
[experimentParams.dN_photoActivation] 100
[experimentParams.k_on_photoActivation] 100
[experimentParams.N_drug] 100
[experimentParams.drug_affectedParam] DOUT_DRUGPARAM
[experimentParams.drug_newValue] 0
[experimentParams.SRI_pointingAccuracy] 0.1
[experimentParams.SRI_detectionIntensity] 1.5259e-005
[experimentParams.SRI_zoom] 1
[experimentParams.N_presequ] 0
[experimentParams.N_repetition] 1
[experimentParams.acquisitionPeriod] 0.25
[experimentParams.measuredRegionIdxs_v] Region2 
[experimentParams.bleachedRegionIdxs_v] Region2 
[experimentParams.file_destination] SimulationOutputs
[experimentParams.recording_mode] ALL_IN_ONE_RECORDING
[experimentParams.is_fcsUsingGaussianBeam] FALSE
[experimentParams.fcs_beamSigma] 0
[experimentParams.fcs_beamKoff] 0
[experimentParams.fcs_maxIntensity] 0
[simulationState.simulator_mode] GEOMETRY_MODE
[simulationState.simulationStarted] FALSE
[simulationState.simulationPaused] FALSE
[simulationState.simulationEnded] TRUE
[simulationState.isSingleShot] FALSE
[simulationState.isRendering] TRUE
[simulationState.isParameterWidgetVisible] TRUE
[simulationState.isGraphicVisible] TRUE