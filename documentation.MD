# Novel search approach
## Luc Edes (CS '26) - Winter 2024 URF

This documentation aims to provide a brief summary of some of the stuff I've worked on with Professor Mina and Amir. I won't be able to cover everything. I'm assuming this will either be read by Professor Mina or Amir, or a future URF/URA who will be working on this project.

### Background
Often times with the current stochastic approach, Professor Mina and Amir noticed that the base example itself contained lots of information about the types of traffic patterns which we're looking for, making the long stochastic search process redundant.

This project aimed to find more efficient (in terms of time, and in terms of Z3 calls) ways of generating Workloads.


### Approach
The current stochastic approach is to slowly move the Workload away from the set of bad examples towards the set of good examples.

My approach is to start with a Workload describing just a single trace (the base example) and slowly broaden around it. Note that this means that my approach guarantees that the base example will always be in the final Workload (this is not the case with the current approach).

#### Step 1: Generate the base example
#### Step 2: Generate the base example's Workload
- For each queue, for each timestep, generate: the cenq, aipg and meta-data (ecmp and dst) specs describing the base example's queue at that timestep and add it to our base Workload.
  - Optimizations:
    - If cenq stays the same over a range of timesteps, only generate one cenq spec for the range (instead of [t1, t1] cenq = c, [t2, t2] cenq = c, ... we simply have [t1, tn] cenq = c) - I believe ``normalize`` already does this for cenq=0
      - This still needs to be done for ``aipg`` and meta-data specs
    - If cenq=0 over the whole time range for a queue, no need to add aipg specs for that queue (the simplest way to describe an empty queue is simply [t1, tn] cenq = 0)
    - If aipg=0, don't add it to the Workload (this doesn't give us much information)
      - Adding both 'aipg' and 'cenq' specs is technically unnecessary. We started adding only cenq, but realized that some of the examples could benefit from adding aipg specs to better describe gaps in network traffic. If aipg=0, then it's definitely not going to be a better (easier to understand) description of the network traffic than cenq
#### Step 3: Combine (see implementation of ``Workload combine`` in ``tests.cpp``)
- Through looking at many examples, we noticed a couple spec patterns which could be manually combined into something more concise
  - Example 1:
    - [1, 1]: SUM_[q in {0, 1, }] cenq(q ,t) = 1 
    - [2, 2]: SUM_[q in {0, 1, }] cenq(q ,t) = 2
    - [3, 3]: SUM_[q in {0, 1, }] cenq(q ,t) = 3
    - This can be combined into [1, 3]: SUM_[q in {0, 1, }] cenq(q ,t) = t
      - Future improvement: The current approach doesn't allow for offsets (e.g. cenq(q, t) = t + 1). This could be added in the future if we add a `t + c` construct to the right-hand side of the equation
  - Example 2:
    - [t1, t2]: (INDIV(q1) cenq(q, t) = 0
    - [t1, t2]: (INDIV(q2) cenq(q, t) = 0
    - [t1, t2]: (INDIV(q3) cenq(q, t) = 0
    - This can be combined into [t1, t2]: (INDIV(q in {q1, q2, q3}) cenq(q, t) = 0
- This used to be done later, but because of how we broaden the Workload, it makes sense to do it now
#### Step 4: Remove specs
- Each spec is a restriction on the set of traces that the Workload describes. Removing specs hence broadens the Workload (and simplifies its description)
  - Step 4.1: Remove meta-data specs randomly
    - For some of the examples, meta-data is completely irrelevant, so we remove these first
    - We randomly pick a meta-data spec, remove it if the resulting Workload is still valid, and repeat until we can't remove any more meta-data specs (usually we end up with no meta-data specs, but we're guaranteed to end up with the minimal set of meta-data specs)
  - Step 4.2: Remove other specs in increasing order of time range length
    - We remove the spec with the smallest time range length, check if the resulting Workload is still valid, and repeat until we can't remove any more specs (we've hit the minimal Workload where each spec is necessary)
    - We prioritize removing the specs with the smallest time lengths, because these add the least information to the Workload. We want to generate the most concise, easy to understand Workload possible
      - Empirically, this way of prioritizing tends to favor keeping the `aipg` specs. However, we know that this is not always the best way to describe the network traffic. We'll see later how we bring back the `cenq` specs
  - You can see why it was a good idea to do the ``combine`` step beforehand. Each time we remove a spec, we make a Z3 call to check if we still have a valid Workload (i.e. removing that restriction did not allow bad examples into the set of traces described by the Workload). Combining reduced the total number of specs, making this step faster.
#### Step 5: Transform aipg to cenq
- This is where we bring the `cenq` specs back. Basically, we try to replace ``aipg=1`` specs with ``cenq=t`` (convince yourself why this makes sense if we have 1 packet enqueued at each timestep), obviously making a Z3 call to check if this is still a valid Workload (although in theory it makes sense, they're not exactly the same thing).
- Note that this creates a binary tree of possible Workloads with 2^n leaf Workloads, where n is the number of aipg=1 specs in the Workload. This is because we can either have aipg=1 or cenq=t for each of these specs.
  - Note that we also try to replace this with 'cenq=1'. This is because ``aipg=1`` doesn't always describe 1 packet enqueued at each timestep (the definition of ``aipg`` means that ``aipg`` for a timestep with 0 enqueues takes on the previous timestep's value)
  - Note that there were further discussions on what's better between [t1, t2]: cenq(q, t) >= t  and [t1, t2]: cenq(q, t) >= c  for some constant 1 < c < t2, as neither set of traces is a subset of the other. This is a more fundamental issue with the Workload language... (something interesting to think about going forward, but not something we need to worry about for now)

#### Step 6: For each leaf node (Workload) in the aipg-cenq binary tree, do the following:
- Step 6.1: Aggregate indivs to sums
  - This functionality is already done in the current stochastic search approach.
- Step 6.2: Broaden operations
  - See if we can get away with changing the operation from `=` to `>=` or `<=`, or '>' to '>=' or '<' to '<=' (this is a very simple operation, but it's not always valid)
- Step 6.3: Tighten constant bounds
  - Another operation taken from the current stochastic search approach.
- Step 6.4: Restrict time ranges
  - Note that the more time ranges a spec applies to, the more restrictive it is. We can try to restrict the time range of each spec to make the Workload slightly more general.
  - Note that this is one of the operations which requires the most Z3 calls (to check if the Workload is still valid each time), especially since we're doing this for 2^n different Workloads (where n is the number of aipg=1 specs in the Workload)
  - Currently, this is somewhat contradictory with ``normalize``, which was a function designed to increase the uniqueness of the Workloads we're looking for (since there are multiple different Workloads which can describe the same set of traces, this tries to aggregate the ones which are similar). The current ``normalize`` extends the time range for ``cenq`` specs with a > or >= operation, since ``cenq`` is non-decreasing. This is a good idea in and of itself, but doesn't work with what we're trying to do here. This is something we should think about going forward.

### Step 7: Return the lowest-cost Workload
- Quite simple. Call the cost function on each of our leaf Workloads and return the one with the lowest cost.
- Note that as of the time of writing, the cost function being used is still the old one (the one which was used in the current stochastic search approach).

### Results
- See the ``benchmarks`` or ``research_tests`` folders for some of the results we've gotten so far.
- Summary: In almost all cases, we get the same answer as the stochastic search approach, but in a fraction of the time and with a fraction of the Z3 calls.

### Alternative Approaches
At some point, we tried having Z3 find us the final Workload (i.e. instead of guess-and-check, we have Z3 find the Workload which describes the set of traces we're looking for). This was a good idea in theory, but in practice, it was too slow. Not a priority right now.

### Better Cost Function
Note that the current cost function doesn't really make sense for the current approach, as it takes into account the overlap with the set of good and bad traces for the stochastic search approach (we don't even have those sets here), so we're basically only looking at the number of specs in the Workload (intepretability).
This also doesn't fully capture the generality of the Workload (e.g. when broadening operations, cenq>=1 is more general than cenq=1 and should have a lower cost, but the current cost function doesn't capture this).
As of time of writing, I'm still working on a better cost function which captures this using combinatorial enumeration techniques to find the number of traces described by a given Workload.

We also considered using a sampling approach (try random traces, and check if they're in the set of traces described), and come up with a heuristic estimate of the number of traces described by the Workload.

### Future Improvements to fperf
- Professor Mina, Amir and I have come across quite a few situations while working on fperf where we wish we had additional functionality not currently available. Here are some of them:
- Time + Constant construct (see Step 3, Example 1)
- Disjunction
  - Currently, a Workload is a conjunction of TimedSpecs. It would be nice to have disjunction to be able to fully describe the set of traces we're looking for in cases where it's impossible to do so with just conjunctions.
  - As of now, the only way to guarantee completeness (i.e. that every possible trace satisfying model^base_wl^query has been found) is to re-run the search multiple times.
- Modify cenq to be able to describe the sum of enqueues over a time range not necessarily starting at 1
  - enq is a specific case of this for a single timestep

### Last-minute ideas
- Use ROBDDs to represent the set of traces described (variants to be able to handle integers)
- Use Z3 to optimize in a better way (ask Mina)