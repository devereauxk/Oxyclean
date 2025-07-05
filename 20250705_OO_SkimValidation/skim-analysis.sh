#!/bin/bash
# ============================================================
# Process skim by hand
# ============================================================

source config.sh

# isForest=false, event cut, track cut
# skim
root -l -b -q "skimValidation.C(\"$skimFile\", \"output/$name-skim.root\", false, $ApplyEventSelection, $ApplyTrackSelection)"
