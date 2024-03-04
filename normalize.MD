What does the ```normalize``` function do?

- Merges consecutive timeline entries if they have the same set of specifications (wl_spec_t)
- Clear workload if ```total_time``` is 0
- Checks if the size of all_specs exceeds a certain max_size.

normalize(time_range_t time_range)

- Zero propagation:
    - Separates zero and non-zero Comp specs
    - Iterative process...

regenerate_spec_set:

- Merge adjacent / overlapping specs

TimedSpec::normalize:

- Extends non-decreasing Comp operations (contradiction with restricting time ranges)

Comp::normalize:

- Checks for comparisons between metrics
    - Checks for subsets / supersets
    - QSum is always on the left
    - 