forestFile=Forests/crab_HiForest_250514_Starlight_DoubleDiffraction_OO_5362GeV_1505PR47944.root
skimFile=bakovacs/skim_v1_20250602_HiForest_250514_Starlight_DoubleDiffraction_OO_5362GeV_1505PR47944.root
name=StarlightDD_bakovacsskim

# event cut, track cut
# forest
#root -l -b -q "skimValidation.C(\"$forestFile\", \"output/StarlightDD_forest_cut.root\", true, true, true)"

# skim
root -l -b -q "skimValidation.C(\"$skimFile\", \"output/StarlightDD_skim_cut.root\", false, true, true)"

# plot
# root -l -b -q "plotForest.C(\"output/StarlightDD_forest_cut.root\", \"output/StarlightDD_skim_cut.root\", \"plots/$name-cut\")"
