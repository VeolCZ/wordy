## Wordy

A really simply cmd tool that parses a csv files and returns only parts matching a certain pattern.

```
Usage:
    wordy <file_path> <flags>
Flags:
    -h  help
    -d  delimiter for the file, defaults to \\n, one char only
    -i  parse those that include letters 'abc'
    -c  parse those that are composed of letters 'abc'
    -p  parse those that match a pattern '#a##b' with # being wildcard
    -l  parse word of certain length e.g. 7
```
