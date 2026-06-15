# Pinch is a utility for cutting the middle out of log/text files or redirected input via stdin if it exceeds the default of 20 lines by outputting the first 10 lines and the last 10 lines. It uses buffered reading to ensure that even if the input exceeds the default of 20 lines| it doesn't load the entire file into memory.

Flag|Argument|Description|Default Value
----|----|----|-----
-n|<integer>|Overrides the maximum number of lines to display.|20
-i|<filename>|Takes input from a specified file instead of reading from STDIN.|STDIN
-o|<filename>|Redirects output to a specified file instead of writing to STDOUT.|STDOUT
-s|<string>|Overrides the middle truncation separator text.|[... TRUNCATED ...]
-b|<string>|Overrides the Start-of-File bookend marker.|[ SOF ]
-e|<string>|Overrides the End-of-File bookend marker.|[ EOF ]
-q|None|Quiet mode. Suppresses the -b and -e markers completely (outputs only data and the -s separator).|Disabled
-v|None|"Printable only. Strips non-printable control characters| escape sequences| and binary data. Triggers a warning."|Disabled (Binary Safe)

