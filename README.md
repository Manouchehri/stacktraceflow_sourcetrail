# Introduction

`stacktraceflow` combines the power of [Valgrind](https://valgrind.org/) and
[Sourcetrail](https://www.sourcetrail.com/) to visualize internal interactions
of software written in any language, as long as it's compiled to ELF/DWARF
format.

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
    /home/glon/Programs/tmp/build/stacktraceflow --translate %{SOURCE_FILE_PATH} %{DATABASE_FILE_PATH}
5. In the 'Files & Directories to index' field add the `stacktraceflow_record`
   directory that was created during `stacktraceflow --run ...`
6. Click 'Next', then in the newly opened window click 'Create'
7. You will see a dialog prompting you to start indexing. Click 'Start'

