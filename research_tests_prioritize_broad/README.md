First tests prioritizing broad time ranges
The goal was to get ```fq_codel``` test to produce ```aipg``` specs

This was accomplished. However, the other tests also produced ```aipg``` specs... although these were more broad, they were slightly less interpretable than before.

Solutions I want to try:
1. Spec-specific mutations (if we detect a range of aipg = 1, we turn them into cenq's). Applying this process would turn the ```aipg``` specs into ```cenq``` specs as they were before this new approach for the other tests, but would keep the ```aipg``` specs for the ```fq_codel``` test...
2. Try both a this approach (prioritize broad time ranges) and another one (likely randomly removing, from original Workload either w/ or w/out ```aipg``` specs), and compare cost at the end (and obviously choose the best)