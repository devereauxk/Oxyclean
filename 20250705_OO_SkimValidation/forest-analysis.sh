#!/bin/bash
# ============================================================
# Process forest by hand
# ============================================================

source config.sh

# isForest=true, event cut, track cut
root -l -b -q "skimValidation.C(\"$forestFile\", \"output/$name-forest.root\", true, $ApplyEventSelection, $ApplyTrackSelection)"
