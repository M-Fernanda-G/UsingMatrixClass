# Custom Memory Allocator Profiling Assignment

## Purpose

Your group will study how memory allocator design affects program
performance. You will compare regular global `new` with overloaded global
`new` backed by every provided version of CustomMemoryAllocator.

Your conclusions must be supported by repeatable measurements. A result from
one workload or machine must not be presented as proof that an allocator is
always faster.

## Required allocator versions

Profile every provided version

- v1.0.0
- v2.0.0
- v3.0.0
- v4.0.0
- v5.0.0
- v6.0.0
- v7.0.0

Use a separate Release build directory for each version (tag).  Create a repository for group results in the course organization.

## Required workload pairs

Profile every regular and overloaded pair in UsingMatrixClass

- Matrix array
- Uniform matrix nodes
- Nested matrix stress
- Linked list

The programs in each pair use a common workload header. Their intended
difference is regular global `new` compared with overloaded global `new`.
The overloaded program initializes CustomMemoryAllocator. This changes global
allocation for the caller and its linked dependencies.

## Required profiling runs

For every allocator version and workload pair, run both the regular and
overloaded programs at all three scales.

### Short runs

- Select parameters that make each run last between 3 and 5 minutes
- Complete 5 regular runs
- Complete 5 overloaded runs with identical parameters

### Medium runs

- Select parameters that make each run last between 2 and 3 hours
- Complete 5 regular runs
- Complete 5 overloaded runs with identical parameters

### Long runs

- Select parameters that make each run last longer than 6 hours
- Complete 5 regular runs
- Complete 5 overloaded runs with identical parameters

Use calibration runs to select workload parameters. Calibration runs do not
count as measured runs. A measured run must execute the real workload for its
entire duration. Sleeping does not count as workload time.

## Fair experiments

Keep each regular and overloaded comparison as similar as possible.

- Use the same workload arguments and repetition counts
- Use the same random seed
- Use the same source revision and compiler options
- Use Release builds
- Run pair comparisons on the same machine.  It is understandable that different students have different hardware. Make sure your profile results do not aggregate results from different machines. 
- Avoid running unrelated profiling jobs at the same time unless it is part of the intentional profiling.
- Preserve matching program checksums
- Record failed or interrupted runs
- Do not remove inconvenient measurements without explanation

Record the following environment information

- Processor model and core count
- Installed memory
- Operating system and version
- Compiler and standard library versions
- Allocator tag and source commit
- Workload arguments and repetition count
- Timing and profiling tools
- Other important work running on the machine

Hardware may vary when experiments use different computers. Record every
variation and keep results from different environments identifiable. Explain
when processor, memory, operating system, or compiler differences appear to
affect the results.

Consider experiments conducted while the operating system is largely inactive
and while it is heavily loaded. Do not aggregate mixed environments. If you perform this comparison, control and
describe the background workload. Discuss how scheduling, memory pressure,
and competing processes affect elapsed time and measurement variation.

## Measurements and statistics

Record elapsed cpu time in seconds for every run. You may also collect peak memory use, page faults, context switches, and allocation
counts when supported by your tools.

Make sure you include the following descriptive statistics:

- Minimum
- Maximum
- Arithmetic average
- Sample standard deviation

Preserve all raw measurements. Your report must make it possible to trace a
statistic back to its measured runs.

## Tables and charts

Create tables that summarize

- Environments differences
- Workload parameters
- Timing data
- Descriptive statistics
- Regular and overloaded comparisons
- Results for versions v1.0.0 through v7.0.0
- Results for your improved allocator

Create meaningful charts that compare regular `new` with overloaded `new`.
Charts should show how performance changes across allocator versions,
workloads, and run lengths.

Useful charts include

- Mean elapsed time with standard deviation error bars
- Percent improvement for each allocator version
- Version progression from v1.0.0 through v7.0.0
- All five observations for important comparisons
- Comparison among regular `new`, v7.0.0, and your improved allocator

Use consistent colors, units, ordering, and scales. Explain both improvements
and regressions. Do not design a chart that hides unfavorable results.

Every table and graph must use a LaTeX caption and label. Refer to each table
and graph from the report text. Axes must identify their quantity and unit.

## Allocator version analysis

Explain how every allocator version functions and what improvement it adds.
Read the implementation rather than repeating only the README description.

For each version discuss

- Free memory data structures
- Allocation search strategy
- Block splitting and reuse
- Deallocation and adjacent block merging
- Alignment and allocator metadata
- Canary checks
- Integration with `new`, `new[]`, `delete`, and `delete[]`
- Expected advantages and caveats
- Possible performance or memory costs


Relate these choices to general concepts learned in hardware and operating
systems courses. Appropriate topics include

- CPU cache and memory access locality
- Main memory access cost
- Virtual memory and paging
- Operating system memory requests
- Process scheduling and system load
- Thread synchronization
- Fragmentation
- Memory use compared with speed

Keep the hardware discussion understandable and relevant to the measured
programs. Do not add advanced hardware terminology without explaining why it
matters to the experiment.

## Improved allocator repository

Create a new repository in the course organization. Begin with v7.0.0 and
implement a meaningful allocator improvement.

The improved allocator must

- Remain a drop in replacement for CustomMemoryAllocator
- Support `new`, `new[]`, `delete`, and `delete[]`
- Work for allocations made by the caller and its dependencies
- Be fetchable by another CMake project
- Have an immutable version tag
- Include unit and integration tests
- Include brief build and use instructions
- Remain reusable by projects other than UsingMatrixClass
- Include CMAKE MACRO directives that profile allocator behaviors (misses, searching, reuse). Find meaningful things to measure and profile. Macros must be able to turn off metric counting to avoid interfering with timing results. Note that runs rely on random seeds for reproducibility.

Compare the improved
allocator with regular `new` and the original v7.0.0 allocator. Demonstrate an
empirical advantage in at least one reasonable scenario. Clearly identify any
workload where the change causes a regression.

## README questions

Review every question in the UsingMatrixClass README. Provide meaningful
technical responses in the report. Group related questions into appropriate
subsections.

Responses should use

- Allocator source code
- Results collected by your group
- Hardware and operating system concepts
- Credible cited sources

## IEEE conference report

Write a report of at least 20 substantive pages with the official IEEE
conference paper LaTeX template. References and appendices do not count toward
the minimum. Extra words that do not add information do not count as useful
content.

Use BibTeX for citations. Cite credible sources for technical claims. Every
bibliography entry must be cited in the report.

Use appropriate sections and subsections. The report should include

- Abstract
- Introduction and research questions
- Background
- Explanation of allocator versions
- Experimental methodology
- Hardware and software environments
- Workload descriptions
- Statistical results
- Charts and comparisons
- Hardware and operating system discussion
- Improved allocator design
- Improved allocator results
- Responses to README questions
- Threats to validity
- Reproducibility
- Conclusions
- Group member commitments and contributions
- BibTeX references
- Supporting appendices

Use compact itemized lists when they make important information easier to
review. Lists must not replace technical explanation.

If you do not know LaTeX, learn it and use ShareLaTeX or Overleaf throughout
the project. Do not wait until the end of the assignment to begin assembling
the report.

## Group member commitments

Every group must include a report section describing the commitment of each
member. Include

- Work each member agreed to complete
- Experiments each member conducted
- Code and analysis each member produced
- Report and presentation sections each member prepared
- Work each member actually completed

Every member should review and approve this section before submission. Each
member remains responsible for understanding the full project and answering
questions during the presentation.

## Beamer poster

Create a presentation poster in LaTeX with Beamer and the Beamer poster
package. Submit the LaTeX source and compiled PDF.

The poster should present

- Research questions
- Experimental design
- Allocator version progression
- Most meaningful charts
- Improved allocator design
- Important improvements and regressions
- Conclusions
- Group members

The poster should be visual, readable, and concise. It should not reproduce
the full report.

## Beamer slideshow

Create a separate slideshow using LaTeX Beamer. Submit the LaTeX source and
compiled PDF.

The slideshow should guide the class through

- Motivation and hypotheses
- Profiling methodology
- Allocator version changes
- Results across the three time scales
- Effects of hardware, operating system, and system load
- Improved allocator design and evidence
- Limitations and conclusions

Use readable charts and limited text on each slide. Every group member must
participate in presenting the findings and answering questions.

## Repository contents

Submit a repository containing

- Improved allocator source and public headers
- CMake build configuration
- Tests
- Final version tag
- Experiment scripts
- Raw measurement data
- Statistics and chart generation files
- IEEE report source and PDF
- BibTeX database
- Beamer poster source and PDF
- Beamer slideshow source and PDF
- README with build, test, and reproduction instructions

Keep results traceable to allocator version, workload, parameters, machine,
operating system, and run number. Do not fabricate, duplicate, or silently
remove measurements.

## Final checklist

- All seven allocator versions were tested
- All four workload pairs were tested
- Five runs were completed for every required comparison and time scale
- Raw results and parameters were preserved
- Minimum, maximum, average, and sample standard deviation were calculated
- Tables and charts use LaTeX captions and labels
- Hardware and operating system variations were recorded and discussed
- Every allocator version was explained
- Every README question was addressed
- The improved allocator is reusable, fetchable, tested, and versioned
- The report has at least 20 substantive IEEE format pages
- Citations use BibTeX
- Group member commitments and contributions are documented
- The Beamer poster is complete
- The Beamer slideshow is complete

# Grading

The following will be criteria used for grading the report, poster, slideshow, and presentation:

* Technical Depth
* Poster Appearance (sleek but informative)
* Slideshow Appearance
* Presentation (both group and individual)
* Participation and contribution by all members. 
* Meaningful statistical analysis
* CustomMemoryAllocator Improvement
* Acknowledgement of edge cases and limitations