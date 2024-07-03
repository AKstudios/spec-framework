# spec-framework

This is a basic Linux framework for the SpEC II board containing various C++ modules that can be used to build Linux application compiled using GCC 7.5 linario toolchain.

---
### To use:
- This framework repo can be imported as a submodule in an application codebase so it can be maintained and updated separately. To do this, run:

```
git submodule add https://git-out.gss.anl.gov/spec_2/spec-framework.git
```

---

### Supported features:
- Config file parser - A tool to parse configuration files
- Sleeper - Interruptable sleeper. Replaces `usleep()` with `select()` and has multi-threaded support
- One-shot timer - A rollover-safe one-shot and repeating timer
- Logger - logs messages to syslog or external file
- UDP sockets - Server and client for manging UDP sockets over IPv4 and IPv6
- TCP/IP sockets - Server and client for manging TCP/IP sockets over IPv4 and IPv6
- SLAC - Wrapper over Qualcomm's SLAC implementation

---


