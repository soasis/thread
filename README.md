# `ztd.thread`

This is an experimental library meant to provide some additional functionality to C11 threads. The background for this comes from the serious issues C++ suffered while trying to improve their threading APIs while suffering backwards-compatibility and structure upgrade problems:

- [https://wg21.link/p2019 (most mature)](https://wg21.link/p2019)
- [https://wg21.link/p3072](https://wg21.link/p3072)
- [https://wg21.link/p3022](https://wg21.link/p3022)

C++ is attempting to use templates and other techniques to try and save on effort. This library takes a different approach, instead using an enumeration with 65,536 reserved values [`0`, `65'535`] for the standard, and 2,147,418,112 reserved values [`65'636`, `2'147'483'647`] for implementation-defined attributes. Each value of the enumeration corresponds to a structure that describes something for the thread to use.

You can find the [the library documentation here (https://ztdthread.rtfd.io)](https://ztdthread.rtfd.io/).
