forestFile=Forests/crab_HiForest_250514_Starlight_SingleDiffraction_OO_5362GeV_1505PR47944.root
skimFile=bakovacs/skim_v1_20250602_HiForest_250514_Starlight_SingleDiffraction_OO_5362GeV_1505PR47944.root
name=StarlightSD_bakovacsskim

# event cut, track cut
# forest
#root -l -b -q "skimValidation.C(\"$forestFile\", \"output/StarlightSD_forest_cut.root\", true, true, true)"

# skim
root -l -b -q "skimValidation.C(\"$skimFile\", \"output/StarlightSD_skim_cut.root\", false, true, true)"

# plot
# root -l -b -q "plotForest.C(\"output/StarlightSD_forest_cut.root\", \"output/StarlightSD_skim_cut.root\", \"plots/$name-cut\")"
