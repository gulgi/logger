# logger
Basic logging functionality (C++).

Windows & C++11(ish) only, for now.
Missing most features. But works! (on my computer ^_^)

## Usage

```cpp
Logger log;
log.log("one 1");
log.log("two %", 2);
log.log("three %", 3.0f);
log.log("%% 4", "fou", 'r');
log.log("percent \%");
```

When ``Logger`` is created it creates a log-file in ``log``, creates a file there (using current time), and spawns a low priority thread that checks if there is things to print every 0.2 seconds or so.
And I chould add settings for most of this. :)

## TODO
Lots! :)


/gulgi