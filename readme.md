# Pinch

Pinch is a utility for cutting the middle out of log/text files or redirected input via stdin if it exceeds the default of 20 lines by outputting the first 10 lines and the last 10 lines. It uses buffered reading to ensure that even if the input exceeds the default of 20 lines, it doesn't load the entire file into memory.

## Command-Line Arguments

| Flag | Argument | Description | Default Value |
| :---: | :---: | :--- | :--- |
| **`-n`** | `<integer>` | Overrides the maximum number of lines to display. | `20` |
| **`-i`** | `<filename>`| Takes input from a specified file instead of reading from STDIN. | `stdin` |
| **`-o`** | `<filename>`| Redirects output to a specified file instead of writing to STDOUT. | `stdout` |
| **`-s`** | `<string>`  | Overrides the middle truncation separator text. | `[... TRUNCATED ...]` |
| **`-b`** | `<string>`  | Overrides the Start-of-File bookend marker. | `[ SOF ]` |
| **`-e`** | `<string>`  | Overrides the End-of-File bookend marker. | `[ EOF ]` |
| **`-q`** | *None*      | Quiet mode. Suppresses the `-b` and `-e` markers completely (outputs only data and the `-s` separator). | Disabled |
| **`-p`** | *None*      | Printable only. Strips non-printable control characters, escape sequences, and binary data. Triggers a warning. | Disabled (Binary Safe) |
| **`-l`** | *None*      | Line numbers. Adds the original 1-based line numbers to the output. | Disabled |

---

## Detailed Documentation

For a comprehensive guide on usage, memory efficiency, and complex examples, see:
- [usage.md](file:///home/nikole/java-workspace2/pinch/usage.md)
- [pinch.1](file:///home/nikole/java-workspace2/pinch/pinch.1) (Man page)

To view the man page locally:
```bash
man ./pinch.1
```

## License

This project is licensed under the terms of the GNU General Public License version 3 (GPL-3.0). See the [LICENSE](file:///home/nikole/java-workspace2/pinch/LICENSE) file for the full license text.

