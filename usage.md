# Pinch Usage Guide

`pinch` is a high-performance, memory-efficient command-line utility written in C++ for truncating large text streams and log files. By extracting the head and the tail of an input stream and discarding the middle, it allows developers and system administrators to inspect the boundaries of long outputs without overloading their terminals, storage, or system memory.

---

## Table of Contents
1. [Key Features](#key-features)
2. [Command-Line Options](#command-line-options)
3. [Core Mechanics & Memory Efficiency](#core-mechanics--memory-efficiency)
4. [Step-by-Step Examples](#step-by-step-examples)
   - [1. Basic Truncation (STDIN / Pipe)](#1-basic-truncation-stdin--pipe)
   - [2. File-Based I/O](#2-file-based-io)
   - [3. Controlling Line Counts](#3-controlling-line-counts)
   - [4. Custom Separators and Bookends](#4-custom-separators-and-bookends)
   - [5. Quiet Mode](#5-quiet-mode)
   - [6. Displaying Original Line Numbers](#6-displaying-original-line-numbers)
   - [7. Stripping ANSI Escapes and Non-Printable Binary Data](#7-stripping-ansi-escapes-and-non-printable-binary-data)
5. [Integrating Pinch into Workflows](#integrating-pinch-into-workflows)
6. [Exit Status Codes](#exit-status-codes)

---

## Key Features

- **Constant Memory Footprint:** Processes files of any size (even multi-gigabyte logs) in a single pass with a memory usage strictly proportional to the number of lines requested for display ($O(N)$ space where $N$ is the display limit).
- **Flexible Bookending:** Automatically wraps outputs in `[ SOF ]` (Start of File) and `[ EOF ]` (End of File) markers so you know exactly where the stream begins and ends.
- **Line Number Preservation:** Tracks and displays the original line numbers from the source stream, showing exactly how many lines were truncated in the middle.
- **Binary & ANSI Safety:** Can filter out non-printable ASCII codes, control sequences, and ANSI terminal escape codes (e.g., color codes) to protect your terminal output.

---

## Command-Line Options

Below is the complete set of command-line arguments supported by `pinch`.

| Short Flag | Long Flag | Argument Type | Default Value | Description |
| :---: | :--- | :---: | :--- | :--- |
| **`-h`** | `--help` | None | *N/A* | Displays the help message containing option usage and exits. |
| **`-n`** | `--number` | Integer | `20` | The maximum number of lines to display. If input is longer than this, it is truncated. |
| **`-i`** | `--input` | String | `stdin` | Path to the input file. If omitted or set to `stdin`, reads from standard input. |
| **`-o`** | `--output` | String | `stdout` | Path to the output file. If omitted or set to `stdout`, writes to standard output. |
| **`-s`** | `--separator`| String | `[... TRUNCATED ...]` | The custom string to print on the truncation line between head and tail output. |
| **`-b`** | `--bookend` | String | `[ SOF ]` | The custom string to print at the start of the output stream. |
| **`-e`** | `--endbookend`| String | `[ EOF ]` | The custom string to print at the end of the output stream. |
| **`-q`** | `--quiet` | None | *Disabled* | Enables quiet mode. Suppresses the start (`-b`) and end (`-e`) bookend markers. |
| **`-p`** | `--printable`| None | *Disabled* | Strips non-printable ASCII control characters, binary data, and ANSI escape codes. |
| **`-l`** | `--lines` | None | *Disabled* | Adds the original line numbers (1-based index) to the beginning of each displayed line. |

---

## Core Mechanics & Memory Efficiency

Unlike standard combinations of `head` and `tail` which might require reading a file twice, indexing it, or loading substantial portions of it into memory, `pinch` uses a single-pass streaming algorithm:

1. **The Head Buffer:** As lines are read from the input stream, the first $H$ lines (where $H = \lfloor N/2 \rfloor$) are stored in a fixed-size vector.
2. **The Tail Buffer:** Once the Head Buffer is full, all subsequent lines are pushed to a double-ended queue (deque) acting as a sliding window buffer of size $T = N - H$.
3. When a new line is read, it is appended to the back of the deque, and the oldest line is popped off the front.
4. Once the input stream is exhausted:
   - If the total line count is less than $N$, `pinch` outputs the head buffer immediately followed by the tail buffer, wrapped in bookends.
   - If the total line count is greater than or equal to $N$, `pinch` outputs the head buffer, then the separator line, and then the tail buffer.

This guarantees that at most $N$ lines are ever kept in memory at any point.

> [!NOTE]
> When the line limit $N$ is odd, the extra line is allocated to the tail. For example, if $N = 15$, the head buffer will contain $7$ lines and the tail buffer will contain $8$ lines.

---

## Step-by-Step Examples

### 1. Basic Truncation (STDIN / Pipe)

To process input from a terminal command, pipe the command output directly into `pinch`. By default, if the input exceeds 20 lines, it will keep the first 10 and the last 10.

```bash
# Generate 50 lines of numbers and pipe through pinch
seq 1 50 | pinch
```

**Output:**
```text
[ SOF ]
1
2
3
4
5
6
7
8
9
10
[... TRUNCATED ...]
41
42
43
44
45
46
47
48
49
50
[ EOF ]
```

---

### 2. File-Based I/O

You can specify the input file directly using `-i` or `--input` and write the results directly to an output file using `-o` or `--output`.

```bash
# Read syslog and write truncated output to brief.log
pinch -i /var/log/syslog -o ./brief.log
```

> [!IMPORTANT]
> If `pinch` fails to open the input file or cannot create/write to the output file, it will output an error message to standard error (`stderr`) indicating the system error details (e.g., `Permission denied` or `No such file or directory`) and terminate with an exit status of `1`.

---

### 3. Controlling Line Counts

You can override the default display budget of 20 lines using the `-n` or `--number` option.

If you specify an odd number, `pinch` rounds down for the head and rounds up for the tail:

```bash
# Display 7 lines of input (3 head, 4 tail)
seq 1 20 | pinch -n 7
```

**Output:**
```text
[ SOF ]
1
2
3
[... TRUNCATED ...]
17
18
19
20
[ EOF ]
```

---

### 4. Custom Separators and Bookends

If you are generating reports, markdown, or config files, you can style the separator and boundaries to match your project's syntax.

```bash
# Format the output using markdown blockquotes and comments
seq 1 30 | pinch -n 6 -b "```" -e "```" -s "<!-- Truncated Content -->"
```

**Output:**
```text
```
1
2
3
<!-- Truncated Content -->
28
29
30
```

---

### 5. Quiet Mode

If you want to feed the output of `pinch` directly to another Unix tool or script, you may want to strip the bookends and only preserve the raw lines and the separator.

```bash
# Print output without [ SOF ] or [ EOF ]
seq 1 30 | pinch -n 6 -q
```

**Output:**
```text
1
2
3
[... TRUNCATED ...]
28
29
30
```

---

### 6. Displaying Original Line Numbers

When analyzing log files, it's essential to know *where* in the original file a specific message occurred. Enabling the `-l` or `--lines` flag prepends the original 1-based line number to the output.

```bash
# Print lines with original file positions
seq 1 30 | pinch -n 8 -l -q
```

**Output:**
```text
1: 1
2: 2
3: 3
4: 4
[... TRUNCATED ...]
27: 27
28: 28
29: 29
30: 30
```

---

### 7. Stripping ANSI Escapes and Non-Printable Binary Data

If a log file contains binary garbage, terminal escape codes, or ANSI colors (like those generated by test runners or build logs), you can use the `-p` or `--printable` flag to sanitize the output.

```bash
# Strip escape sequences and binary control codes
# This will output standard text and alert you on stderr if binary data was stripped
pinch -p -i raw_terminal_output.log
```

> [!TIP]
> The stripping warning is printed to `stderr` at most once per execution to prevent cluttering your diagnostic streams.

---

## Integrating Pinch into Workflows

### View Long Docker/Process Logs
To quickly view the boot phase and shutdown phase of a Docker container without scrolling through millions of lines of logs:
```bash
docker logs my_container_name 2>&1 | pinch -n 40 -l
```

### Clean Web Requests
Clean up text files containing terminal color codes before sharing them:
```bash
curl -s "https://wttr.in/" | pinch -p -n 30
```

### Git Diff Previews
Truncate massive diff outputs to only show the start and end of changes:
```bash
git diff | pinch -n 24
```

---

## Exit Status Codes

`pinch` follows standard POSIX exit status conventions:

* **`0`**: Success. The stream was parsed, truncated if necessary, and outputted without errors.
* **`1`**: Failure. An error occurred. The specific error message will be printed to standard error (`stderr`). Common causes include:
  - Specifying a negative line count (e.g., `-n -5`).
  - Input file not found or inaccessible.
  - No permission to write to the output file.
  - Write errors on standard output (e.g., broken pipe).
