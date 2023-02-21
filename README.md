# Libctrace

## Summary

Libctrace is an experimantal project to trace all function calls.

## Author

- [Mauro Meneghin](https://github.com/m3m0m2/libctrace)

## Build

Requires GCC.

Build a small libctrace.so:

```bash
  make
```
Build the binaries to be traced with gcc arguments -finstrument-functions -rdynamic

## License

[MIT](https://choosealicense.com/licenses/mit/)
