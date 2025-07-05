#!/bin/bash
# ============================================================
# Plot skim and forest comparison
# ============================================================

source config.sh

root -l -b -q "plotComparison.C(\"output/$name-forest.root\", \"output/$name-skim.root\", \"plots/$name\")"
