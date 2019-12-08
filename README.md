# Introduction

`stacktraceflow` combines the power of [Valgrind](https://valgrind.org/) and
[Sourcetrail](https://www.sourcetrail.com/) to quickly produce interactive
callgraph of software written in any language, as long as it's compiled to
ELF/DWARF format.

The basic workflow is this:
* You run your software as you always do, but prepending `stacktraceflow --run`
  on the command line.
* Afterwards you open the generated data in Sourcetrail, which shows interactive
  callgraph of all the functions that were called during the execution,
  including those in external libraries. It also shows you the corresponding
  parts of the source code as you navigate the callgraph.

# Demo

[7m video hosted on Streamable](https://streamable.com/gvn9o)

As an example, it shows how to build visualisation for the
[parity-ethereum](https://github.com/paritytech/parity-ethereum/) project,
written in Rust.

Forward to 1m15s to see the visualisation.

Forward to 3m30s to see how to quickly generate data.

# Building

```
$ git clone https://github.com/Baranowski/stacktraceflow_sourcetrail.git
$ cd stacktraceflow_sourcetrail
$ git submodule update --init
$ mkdir build
$ cd build
$ cmake ..
$ make
```

# Usage

## Recording data

To get started, run:

    stacktraceflow --run PROGRAM [ ARG1 [ ARG2 [ .. ] ] ]

This command will record execution of PROGRAM with the given arguments and
write the data to the (possibly newly created) `stacktraceflow_record`
subdirectory.

For best results, PROGRAM should be debug version of your program. I.e., it
should contain debug symbols, and ideally also be built without
optimizations.

This command may be ran multiple times. The recording will not get overwritten.

## Visualization

Once you have enough data recorded, it's time to import it into
[Sourcetrail](https://github.com/CoatiSoftware/Sourcetrail/releases). To do so:

1. Open Sourcetrail as you always do and create a new project (or edit an
   existing one).
2. Click 'Add source group'
3. On the left sidebar of the newly opened window choose 'Custom', then
   'Custom Command Source Group' and click 'Next'.
4. In the 'Custom Command' field enter:
    `<STACKTRACEFLOW_DIR>/stacktraceflow --translate %{SOURCE_FILE_PATH} %{DATABASE_FILE_PATH}`
5. In the 'Files & Directories to index' field add the `stacktraceflow_record`
   directory that was created during `stacktraceflow --run ...`
6. Click 'Next', then in the newly opened window click 'Create'
7. You will see a dialog prompting you to start indexing. Click 'Start'

You can also take a look at [SourcetrailDB README](https://github.com/Baranowski/SourcetrailDB#integrating-with-sourcetrail)
for some clues.

# Limitations

* This has been tested on Linux only so far. In principle, it should be
  reasonably easy to support also MacOS and Windows, but I don't own those
  systems and so couldn't test.
* Currently only single process can be monitored. If your program forks, the
  children will be ignored. (This does not apply to threads - all threads are
  monitored.)
* Mapping symbols to source code locations works only on the level of line
  numbers. If you want to click a function definition in Sourcetrail's Code
  View, you have to click on the first character of the first line of
  definition.

