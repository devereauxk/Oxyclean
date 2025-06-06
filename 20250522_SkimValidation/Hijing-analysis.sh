forestFile=Forests/crab_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518.root
skimFile=bakovacs/skim_v1_20250602_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518.root
name=Hijing_bakovacsskim

# event cut, track cut
# forest
#root -l -b -q "skimValidation.C(\"$forestFile\", \"output/Hijing_forest_cut.root\", true, true, true)"

# skim
#root -l -b -q "skimValidation.C(\"$skimFile\", \"output/Hijing_skim_cut.root\", false, true, true)"

# plot
root -l -b -q "plotForest.C(\"output/Hijing_forest_cut.root\", \"output/Hijing_skim_cut.root\", \"plots/$name-cut\")"
